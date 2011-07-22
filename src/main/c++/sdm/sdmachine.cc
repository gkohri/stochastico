/*
 *  Copyright 2011 The Stochastico Project
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "sdm/sdmachine.h"

#include <limits>
#include <string>
#include <vector>

#include <pthread.h>
#include <stdio.h>

#include "noir/orthotope.h"
#include "sdm/discriminator.h"
#include "sdm/data_point.h"
#include "sdm/model.h"
#include "sdm/ball_model.h"
#include "sdm/orthotope_model.h"
#include "rng/random.h"
#include "rng/random_factory.h"
#include "rng/ranmar.h"
#include "stat/accumulator.h"
#include "stat/multi_class_roc.h"
#include "util/functions.h"
#include "util/properties.h"
#include "util/invalid_input_error.h"


namespace sdm {

using std::string;
using std::vector;

using noir::Orthotope;
using rng::Random;
using rng::RandomFactory;
using stat::Accumulator;
using stat::MultiClassROC;
using util::to_numeric;
using util::Properties;

SDMachine::~SDMachine() {
    delete uniform;

    vector<Discriminator*>::const_iterator dit;
    for (dit = discriminators.begin(); dit != discriminators.end(); ++dit) {
        delete *dit;
    }
    discriminators.clear();

    vector<MultiClassROC*>::const_iterator rit;
    for (rit = learning_results.begin(); rit != learning_results.end(); ++rit) {
        delete *rit;
    }
    learning_results.clear();
}


void SDMachine::set_parameter(const string &name, string &parameter ) {
    parameter = sdmParameters->get_property( name );

    if ( parameter.empty() ) {
        fprintf(stderr,"%s: '%s' %s \n", "Warning: ", name.c_str(),
             " is missing in parameters file! Using default value: " );
    }
}

template<typename ValueType>
void SDMachine::set_value(const string &name, ValueType &value ) {
    string value_str = sdmParameters->get_property( name );

    if ( value_str.empty() ) {
        fprintf(stderr,"%s: '%s' %s \n", "Warning: ", name.c_str(),
             " is missing in parameters file!" );
        throw util::InvalidInputError( __FILE__, __LINE__,
                        name + " is missing in parameters file!" );
    } else {
        to_numeric(value_str, value);
    }
}

void SDMachine::init( Properties &parameters ) {

    sdmParameters = &parameters;

    uniform = initialize_uniform_rng( parameters );

    set_value( "SDM::Learning::NumberOfModels", numModels );
    set_value( "SDM::Learning::NumberOfFolds", numFolds );
    set_value( "SDM::Model::FeatureSpace::UpperFraction", upperFrac );
    set_value( "SDM::Model::FeatureSpace::LowerFraction", lowerFrac );
    set_value( "SDM::Learning::MaximumNumberOfSubspaces", numAttempts );
    set_value( "SDM::Learning::EnrichmentLevel", enrichmentLevel );

    string subspaceTypes;
    set_parameter( "SDM::Model::SubspaceTypes", subspaceTypes );

    if ( subspaceTypes.compare( "Balls" ) == 0 ){
        modelTypes = ModelTypes::Ball;
    } else if ( subspaceTypes.compare( "Orthotopes" ) == 0 ){
        modelTypes = ModelTypes::Orthotope;
    } else {
        throw util::InvalidInputError( __FILE__, __LINE__,
                        "Unknown subspace type: " + subspaceTypes);
    }

    string learning_algorithms;
    set_parameter( "SDM::Learning::Algorithm", learning_algorithms );

    if ( learning_algorithms.compare( "LeastCovered" ) == 0 ){
        learningAlgorithm = LeastCovered;
    } else if ( learning_algorithms.compare( "RandomPoints" ) == 0 ){
        learningAlgorithm = RandomPoints;
    } else {
        throw util::InvalidInputError( __FILE__, __LINE__,
                      "Unknown learning algorithm: " + learning_algorithms);
    }


}

void SDMachine::learn( DataManager &dataManager ) {
    if ( dataManager.has_test_data() ) {
        numFolds = 1;  // overrides any numFolds setting in the input file
        simple_learning( dataManager );
    } else {
        folded_learning( dataManager );
    }
}


/*
 This struct and the following function are just hacks to speed-up the learning
 processes.  This needs to be redone to create a proper multi-threaded
 program.
*/
struct thread_data {
    SDMachine *sdm;
    Discriminator *dis;
    DataManager *dm;
    DataStore *dataStore;
    int dis_id;
    int *fold;
};

extern "C" void* prepare_discriminators( void *arg ) {
    thread_data* td = (thread_data*) arg;
    td->sdm->ready_discriminator( td->dis, *(td->dm), *(td->fold) );
    return NULL;
}


void SDMachine::simple_learning( DataManager &dataManager ) {

    dataManager.partition_training_data( 1, uniform );

    create_discriminators( dataManager );

    int one = 1;
    pthread_t *tid = new pthread_t[discriminators.size()];
    thread_data *td = new thread_data[discriminators.size()];
    for ( size_t d = 0; d < discriminators.size(); d++ ) {
            td[d].sdm = this;
            td[d].dis = discriminators[d];
            td[d].dm = &dataManager;
            td[d].fold = &one;
            pthread_create( &tid[d], NULL, 
                            prepare_discriminators, &td[d] );
            //prepare_discriminators( td );
    }

    for ( size_t d = 0; d < discriminators.size(); d++ ) {
        pthread_join( tid[d], NULL );
    }

    delete[] tid;
    delete[] td;

    MultiClassROC *result = test( *(dataManager.get_test_data()) );

    fprintf(stdout,"\n%s%.4f\n%s%.4f\n%s%.4f\n%s%.4f\n%s%.4f\n\n",
                       "accuracy: ",    result->accuracy(),
                       "error rate: ",  result->error_rate(),
                       "M: ",  result->m(),
                       "sensitivity: ", result->avg_sensitivity(),
                       "fdr: ", result->avg_false_discovery_rate() );

}

void SDMachine::folded_learning( DataManager &dataManager ) {

    //randomly partition training data into numFolds parts

    dataManager.partition_training_data( numFolds, uniform );

    //clear_learning_results();

    //create discriminators
    create_discriminators( dataManager );

    Accumulator error_accumulator;
    Accumulator accuracy_accumulator;
    Accumulator auc_accumulator;

    pthread_t *tid = new pthread_t[discriminators.size()];
    thread_data *td = new thread_data[discriminators.size()];
    for ( int f = 0; f < numFolds; f++ ) {
        for ( size_t d = 0; d < discriminators.size(); d++ ) {
            td[d].sdm = this;
            td[d].dis = discriminators[d];
            td[d].dm = &dataManager;
            td[d].fold = &f;
            pthread_create( &tid[d], NULL, 
                            prepare_discriminators, &td[d] );
        }
        for ( size_t d = 0; d < discriminators.size(); d++ ) {
            pthread_join( tid[d], NULL );
        }

        MultiClassROC *result = test(*(dataManager.get_partition(f)));
        error_accumulator.gather( result->error_rate() );
        accuracy_accumulator.gather( result->accuracy() );
        auc_accumulator.gather( result->m() );

        fprintf(stdout,"\n%s%.4f\n%s%.4f\n%s%.4f\n%s%.4f\n%s%.4f\n\n",
                       "accuracy: ",    result->accuracy(),
                       "error rate: ",  result->error_rate(),
                       "sensitivity: ", result->avg_sensitivity(),
                       "fdr: ", result->avg_false_discovery_rate(),
                       "m: ", result->m() );

        delete result;
    }

    fprintf(stdout,
                "\n%s\t\t%.4f\n%s\t%.4f\n%s\t\t%.4f\n%s\t%.4f\n%s\t\t%.4f\n\n",
                   "avg. accuracy: ",    accuracy_accumulator.mean(),
                   "avg. error rate: ",  error_accumulator.mean(),
                   "deviation: ", accuracy_accumulator.deviation_population(),
                   "standard error: ", error_accumulator.standard_error(),
                   "avg. m: ", auc_accumulator.mean() );

    delete[] tid;
    delete[] td;
}


void SDMachine::create_discriminators(DataManager &dataManager) {

    const int num_colors = dataManager.get_num_colors();
    const Orthotope *enclosure = dataManager.get_enclosure();

    RandomFactory *prf = RandomFactory::get_instance();

    for ( int c = 0; c < num_colors; c++ ) {
        Discriminator *dis = new Discriminator( c );

        dis->set_random( prf->get_rng() );
        dis->set_boundary( enclosure );
        dis->set_lower_fraction( lowerFrac );
        dis->set_upper_fraction( upperFrac );
        dis->set_enrichment_level( enrichmentLevel );
        if ( modelTypes == ModelTypes::Ball ) {
            dis->set_model_factory( new BallModelFactory() );
        } else if ( modelTypes == ModelTypes::Orthotope ) {
            dis->set_model_factory( new OrthotopeModelFactory() );
        }

        discriminators.push_back( dis );   
    }
}

void SDMachine::ready_discriminator(Discriminator *dis, 
                                    DataManager &dataManager, 
                                    const int &skip_fold) {
    dis->clear();
    for ( int fold = 0; fold < numFolds; fold++ ) {
        if ( fold == skip_fold ) continue;
        dis->add_training_data( dataManager.get_partition( fold ) );
    }
    if ( learningAlgorithm == LeastCovered )
        dis->create_models_lc( numModels, numAttempts);
    else if ( learningAlgorithm == RandomPoints )
        dis->create_models_rc( numModels, numAttempts);
}

void SDMachine::ready_discriminators(DataManager &dataManager,
                                      const int &part) {

    DataStore *trainingData = dataManager.get_partition( part );

    for ( size_t d = 0; d < discriminators.size(); d++ ) {
        discriminators[d]->add_training_data( trainingData );
    }

};

/*
 Again, here are some hacks to speed-up the learning. This will be redone
 after the program's basic correctness has been verified.
*/

static  double **prediction;
extern "C" void* test_discriminators( void *arg ) {
    thread_data* td = (thread_data*) arg;
    unsigned num_tests = td->dataStore->size();
    for (unsigned t = 0; t < num_tests; ++t) {
        DataPoint *point = (*(td->dataStore))[t];
        prediction[t][td->dis_id] = td->dis->test( point );
    }
    return NULL;
}

void SDMachine::clear_learning_results() {
    vector<MultiClassROC*>::const_iterator rit;
    for (rit = learning_results.begin(); rit != learning_results.end(); ++rit) {
        delete *rit;
    }
    learning_results.clear();
}


MultiClassROC* SDMachine::test( DataStore &test_data ) {
    vector<Discriminator*>::const_iterator dit;
    DataStore::const_iterator pit;

    MultiClassROC *roc;
    if (discriminators.size() == 1) {
        roc = new MultiClassROC( discriminators.size() + 1);
    } else {
        roc = new MultiClassROC( discriminators.size() );
    }

    int real_color = 0;
    int predicted_color = 0;

    double threshold = -std::numeric_limits<double>::max();
    if (discriminators.size() == 1) threshold = 0.5;

    unsigned num_dis = discriminators.size();
    unsigned num_tests = test_data.size();
    prediction = new double*[num_tests];
    for (unsigned t = 0; t < num_tests; ++t) {
        prediction[t] = new double[num_dis];
    }

    pthread_t *tid = new pthread_t[discriminators.size()];
    thread_data *td = new thread_data[discriminators.size()];
    for (unsigned d = 0; d < num_dis; ++d) {
            td[d].sdm = this;
            td[d].dis = discriminators[d];
            td[d].dis_id = d;
            td[d].dataStore = &test_data;
            pthread_create( &tid[d], NULL, 
                            test_discriminators, &td[d] );
    }
    for ( size_t d = 0; d < discriminators.size(); d++ ) {
        pthread_join( tid[d], NULL );
    }

    for (unsigned td = 0; td < num_tests; ++td) {
        predicted_color = 0;
        real_color = test_data[td]->get_color();
        if (discriminators.size() == 1) {
            double ds1[2];
            ds1[real_color] = prediction[td][0];
            ds1[real_color^1] = threshold;
            roc->record_results(real_color, ds1);
        }

        roc->record_results(real_color, prediction[td]);
    }

    for (unsigned td = 0; td < num_tests; ++td) {
        delete[] prediction[td];
    }
    delete[] prediction;
    delete[] tid;
    delete[] td;

    return roc;
};

void SDMachine::process_trial_data( DataManager &dataManager ) {
    if ( dataManager.has_trial_data() ) {
        process( *dataManager.get_trial_data() );
    }
}

void SDMachine::process( DataStore &trial_data ) {
    vector<Discriminator*>::const_iterator dit;
    DataStore::const_iterator pit;

    double threshold = -std::numeric_limits<double>::max();
    if (discriminators.size() == 1) threshold = 0.5;

    unsigned num_dis = discriminators.size();
    unsigned num_trials = trial_data.size();
    prediction = new double*[num_trials];
    for (unsigned td = 0; td < num_trials; ++td) {
        prediction[td] = new double[num_dis];
    }

    pthread_t *tid = new pthread_t[discriminators.size()];
    thread_data *td = new thread_data[discriminators.size()];
    for (unsigned d = 0; d < num_dis; ++d) {
            td[d].sdm = this;
            td[d].dis = discriminators[d];
            td[d].dis_id = d;
            td[d].dataStore = &trial_data;
            pthread_create( &tid[d], NULL, 
                            test_discriminators, &td[d] );
    }
    for ( size_t d = 0; d < discriminators.size(); d++ ) {
        pthread_join( tid[d], NULL );
    }

    for (unsigned td = 0; td < num_trials; ++td) {

        fprintf(stdout,"%d", trial_data[td]->get_id());

        for (unsigned d = 0; d < num_dis; ++d) {
            fprintf(stdout,",%.6f", prediction[td][d]);
        }
        fprintf(stdout,"\n");
    }

    for (unsigned t = 0; t < num_trials; ++t) {
        delete[] prediction[t];
    }
    delete[] prediction;
    delete[] tid;
    delete[] td;

};


Random* SDMachine::initialize_uniform_rng( const Properties &props ) {
    string rng = props.get_property( "SDM::Random::Seed" );
    if ( rng.empty() ) {
        throw util::InvalidInputError( __FILE__, __LINE__,
                        "SDM::Random::Seed is missing in parameters file!" );
    } else {
        string seed_string = props.get_property( "SDM::Random::Seed" );
        if ( seed_string.empty() ) {
            throw util::InvalidInputError( __FILE__, __LINE__,
                        "Random Seed is missing in parameters file!" );
        }
        unsigned seed;
        to_numeric(seed_string, seed );
        RandomFactory::set_seed(seed);
    }

    return RandomFactory::get_instance()->get_rng();
};


} // end namespace SD
