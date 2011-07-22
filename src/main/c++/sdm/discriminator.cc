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


#include "sdm/discriminator.h"

#include <cmath>
#include <limits>
#include <string>
#include <vector>

#include "util/functions.h"
#include "util/csv.h"
#include "util/invalid_input_error.h"
#include "sdm/orthotope_model.h"
#include "sdm/ball_model.h"
#include "stat/accumulator.h"

namespace sdm{

using std::string;
using std::vector;
using std::numeric_limits;

using stat::Accumulator;
using util::CSVReader;
using util::to_string;

void Discriminator::add_training_data( DataStore *data ){

    DataStore::const_iterator pit;
    for ( pit = data->begin(); pit < data->end(); ++pit ){
        CoveredPoint *cp = new CoveredPoint(*pit);
        trainingData.add( cp );
    }

};

void Discriminator::check_data_consistency(){

    if ( trainingData.get_num_principal_color() == 0 ){
        throw util::InvalidInputError( __FILE__, __LINE__,
            "Invalid training data. No principal colors present!" );
    }

    if ( trainingData.get_num_other_color() == 0 ){
        throw util::InvalidInputError( __FILE__, __LINE__,
            "Invalid training data. No other colors present!" );
    }

    numPrincipalColor = static_cast<double>(
                                    trainingData.get_num_principal_color() );
    numOtherColor = static_cast<double>(trainingData.get_num_other_color());
}

void Discriminator::create_models_rc( const int &num_models, 
                                      const int &num_spaces ){
    check_data_consistency();
    trainingData.find_nn() ;

    vector<CoveredPoint *>::iterator pit;

    numUnfinished = 0;
    numBroken = 0;

    double avg_cov = 0.0;
    double norm = 1.0;
    for ( int m = 0; m < num_models; m++ ){
        double lpf = lowerFrac;
        double upf = upperFrac;

        if ( models.size() > 0 ) {
            norm = 1.0/static_cast<double>(models.size());
        }

        //Generate a random model
        Model *model = modelFactory->get_model(principalColor, 
                                           numPrincipalColor, numOtherColor);

        bool not_finished = true;
        //Add points to the model
        int t;
        for ( t = 0; t < num_spaces; t++ ){
            CoveredPoint *nexus = trainingData.get_random_point();
            CoveredPoint *nn = trainingData.get_nn(nexus);
            model->expand( *boundary, nexus, nn, rand, lpf, upf );

            if ( !(model->covers(nexus)) ) {
                fprintf(stderr,"nexus not covered!\n");
            }
            if ( !(model->covers(nn)) ) {
                double dist = nexus->get_data_point()->noirSpace->norm(
                                        nexus->get_data_point(),
                                        nn->get_data_point() );
                fprintf(stderr,"c: %d  neighbor not covered! %10.3e\n",
                                    principalColor,dist);
            }

            //Check the average coverage of points in this model

            model->clear_checked_points();

            double avg_mod_cov = 0.0;
            double num_mod_cov = 0.0;
            for (pit = trainingData.begin(); pit < trainingData.end(); ++pit){
                if ( model->check_point( *pit ) ) {
                        avg_mod_cov += (*pit)->get_coverage()*norm;
                        num_mod_cov += 1.0;
                }
            }
            avg_mod_cov /=  num_mod_cov;

            double model_pc = 
                        static_cast<double>(model->get_num_principal_color());
            double model_oc = 
                        static_cast<double>(model->get_num_other_color());

            //If we have to many points in the model, then break and start over
            if ( model_pc == numPrincipalColor ) {
/*
                double model_total = model_pc + model_oc;
                fprintf(stderr,
                    "c: %d t:  %d  ns:  %d total: %f  %f  %f %f %f \n",
                principalColor, t, model->get_num_elements(),
                model_total, numPrincipalColor,numOtherColor,
                    avg_mod_cov, avg_cov);
*/
                ++numBroken;
                break;
            }


            double ratio_diff = model_pc/numPrincipalColor - 
                                                model_oc/numOtherColor;

            //Check for richness
            if ( ratio_diff >= enrichmentLevel ){
                double delta = avg_mod_cov - avg_cov;
                if (delta < 0.1 || models.size() == 0){
                    not_finished = false;
                    double avg_cov_m = 0.0;
                    for (pit = trainingData.begin(); pit < trainingData.end(); 
                                                                    ++pit){
                        if ( (*pit)->get_color() == principalColor ) {
                            if ( model->covers(*pit) ) {
                                (*pit)->increment_coverage(1.0);
                                double cov =  (*pit)->get_coverage()*norm;
                                avg_cov_m += cov;
                            }
                        }
                    }
                    avg_cov_m /= model_pc;
                    if ( models.size() == 0 ) {
                        avg_cov = avg_cov_m;
                    } else {
                        avg_cov += (avg_cov_m - avg_cov)/
                                        static_cast<double>(models.size()+1);
                    }
                    models.push_back( model );
                    break;
                }
            }
        }

        //if no enriched model could be created, delete the current model
        //and try again with different points.
        if ( not_finished ){
            delete model;
            if ( t == num_spaces ){
                ++numUnfinished;
            }
        }
    }

    training_data_prob_distribution();
};


void Discriminator::create_models_lc( const int &num_models, 
                                      const int &num_spaces ){

    check_data_consistency();
    trainingData.find_nn() ;

    trainingData.reorder();

    vector<CoveredPoint *>::iterator pit;

    numUnfinished = 0;
    numBroken = 0;

    double avg_cov = 0.0;
    int rank = 0;
    for ( int m = 0; m < num_models; m++ ){

        double lpf = lowerFrac;
        double upf = upperFrac;

        //Get one of the least covered points in the training set
        //But Don't use the same point twice in a row
        CoveredPoint *least_covered = trainingData.get_least_covered(rank);
        CoveredPoint *lc_nn = trainingData.get_nn(least_covered) ;

        bool test_cov = false;
        if ( rank > 0 ) {
            test_cov = true;
        }

        double norm = 1.0;
        if ( models.size() > 0 ) {
            norm = 1.0/static_cast<double>(models.size());
        }

        //Generate a random model
        Model *model = modelFactory->get_model(principalColor, 
                                             numPrincipalColor, numOtherColor);

        //Make max_attempts to find a new model
        int t;
        bool not_finished = true;
        for ( t = 0; t < num_spaces; t++ ){

            //Expand the model by adding a new hyper-rectangle
            model->expand( *boundary, least_covered, lc_nn, rand, lpf, upf );

            if ( !(model->covers(least_covered)) ) {
                fprintf(stderr,"nexus not covered!\n");
            }
            if ( least_covered == 0) {
                fprintf(stderr,"nexus does not exist!\n");
            }
            if ( lc_nn == 0) {
                fprintf(stderr,"neighbor does not exist!\n");
            }
            if ( !(model->covers(lc_nn)) ) {
                double dist = least_covered->get_data_point()->noirSpace->norm(
                                        least_covered->get_data_point(),
                                        lc_nn->get_data_point() );
                fprintf(stderr,"c: %d  neighbor not covered! %10.3e\n",
                                    principalColor,dist);
            }


            model->clear_checked_points();

            //Check the average coverage of points in this model
            double avg_mod_cov = 0.0;
            if ( test_cov ) {
                double num_mod_cov = 0.0;
                for (pit = trainingData.begin(); pit < trainingData.end(); 
                                                                       ++pit){
                    if ( model->check_point( *pit ) ) {
                        avg_mod_cov += (*pit)->get_coverage()*norm;
                        num_mod_cov += 1.0;
                    }
                }
                avg_mod_cov /=  num_mod_cov;
            } else {
                for (pit = trainingData.begin(); pit < trainingData.end(); 
                                                                      ++pit){
                    model->check_point( *pit );
                }
            }

            double model_pc = 
                        static_cast<double>(model->get_num_principal_color());
            double model_oc = 
                        static_cast<double>(model->get_num_other_color());

            //If we have to many points in the model, then break and start over
            if ( model_pc == numPrincipalColor ) {
                ++numBroken;
                break;
            }

            double ratio_diff = model_pc/numPrincipalColor - 
                                                model_oc/numOtherColor;

            //Check for richness
            if ( ratio_diff >= enrichmentLevel ){

                //Check for uniformity only if we are not using the least
                //covered point
                if (!test_cov || avg_mod_cov < avg_cov || models.size() == 0){
                    not_finished = false;
                    double avg_cov_m = 0.0;
                    for (pit = trainingData.begin(); pit < trainingData.end(); 
                                                                    ++pit){
                        if ( (*pit)->get_color() == principalColor ) {
                            double inc = 
                                    model->characteristic(
                                                (*pit)->get_data_point());
                            (*pit)->increment_coverage(inc);
                            double cov =  (*pit)->get_coverage()*norm;
                            avg_cov_m += cov;
                        }
                    }
                    avg_cov_m /= model_pc;
                    if ( models.size() == 0 ) {
                        avg_cov = avg_cov_m;
                    } else {
                        avg_cov += (avg_cov_m - avg_cov)/
                                        static_cast<double>(models.size()+1);
                    }
                    models.push_back( model );
                    trainingData.reorder();
                    if (test_cov) rank = 0;
                    break;
                } 
            }

            // make the next space a bit bigger
            if ( t < 10 ) {
                upf *= 1.10;
                lpf *= 1.10;
            }

            // make the last space a bit thicker
            model->thicken( *boundary, rand, 0.8);
        }

        //if could not find an enriched model after max tries, delete the
        //   model and try again with a different point.
        if ( not_finished ){
            delete model;
            if ( t == num_spaces ){
                ++numUnfinished;
            }
            ++rank;
            if ( rank == numPrincipalColor ) {
                rank = 0;
            }
        }
    }

    training_data_prob_distribution();
};

void Discriminator::training_data_prob_distribution(){

    Accumulator apc;
    Accumulator aoc;

    vector<CoveredPoint *>::iterator pit;
    for ( pit = trainingData.begin(); 
          pit < trainingData.end(); ++pit ){
        if ( (*pit)->get_color() == principalColor ) {
            double npc = test( (*pit)->get_data_point() );
            apc.gather(npc);
        } else {
            double opc = test( (*pit)->get_data_point() );
            aoc.gather(opc);
        }
    }

    Accumulator pc_cover;
    Accumulator oc_cover;
    Accumulator subspaces;
    unsigned num_models = models.size();
    for (unsigned m = 0; m < num_models; ++m){
        double avg_mod_pc_cover = 0.0;
        double avg_mod_oc_cover = 0.0;
        for ( pit = trainingData.begin(); 
                pit < trainingData.end(); ++pit ){
            if ( models[m]->covers(*pit) ) {
                if ( (*pit)->get_color() == principalColor ) {
                    ++avg_mod_pc_cover;
                } else {
                    ++avg_mod_oc_cover;
                }
            }
        }
        pc_cover.gather(avg_mod_pc_cover);
        oc_cover.gather(avg_mod_oc_cover);
        subspaces.gather(models[m]->get_num_elements());
    }

    fprintf(stdout, "%s  %d\n%s %d\n%s %d\n%s  %d\n%s %d\n%s %d\n%s %10.3e  %s %10.3e\n%s  %10.3e  %s %10.3e\n%s %10.3e\n%s  %10.3e\n%s  %10.3e\n\n", 
                   "principal color:", principalColor,
                   "number of models:", static_cast<int>(models.size()),
                   "number exceeded:", numUnfinished,
                   "number broken:", numBroken,
                   "number PC:", static_cast<int>(numPrincipalColor),
                   "number YC:", static_cast<int>(numOtherColor),
                   "avg Y_PC: ", apc.mean(),
                   "deviation: ", apc.deviation_population(),
                   "avg Y_OC: ", aoc.mean(),
                   "deviation: ", aoc.deviation_population(),
                   "avg. PC covered per model: ", pc_cover.mean(),
                   "avg. OC covered per model: ", oc_cover.mean(),
                   "avg. num. subspaces per model: ", subspaces.mean() );

};


double Discriminator::test( const DataPoint* point ){
    vector<Model*>::const_iterator mit;

    unsigned num_models = models.size();
    double prediction = 0.0;
    for (unsigned m = 0; m < num_models; ++m){
        prediction += models[m]->characteristic( point );
    }

    if ( num_models > 0 ) {
        prediction /= static_cast<double>(num_models);
    }

    return prediction;
}

void Discriminator::clear(){
    trainingData.clear();
    vector<Model*>::const_iterator mit;
    for (mit = models.begin(); mit != models.end(); ++mit){
        delete *mit;
    }
    models.clear();
}

}
