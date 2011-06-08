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

namespace sdm{

using std::string;
using std::vector;
using std::numeric_limits;

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

void Discriminator::create_models_lc( const int &num_models, 
                                      const int &num_attempts ){

    check_data_consistency();
    trainingData.find_nn() ;

    trainingData.reorder();

    vector<CoveredPoint *>::iterator pit;

    numUnfinished = 0;

    double avg_cov = 0.0;
    int rank = 0;
    bool not_finished = false;
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
        Model *model = new Model(principalColor);

        //Make max_attempts to find a new model
        int t;
        for ( t = 0; t < num_attempts; t++ ){

            //Expand the model by adding a new hyper-rectangle
            model->expand( *boundary, least_covered, lc_nn, rand, lpf, upf );

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
                break;
            }

            double ratio_diff = model_pc/numPrincipalColor - 
                                                model_oc/numOtherColor;

            //Check for richness
            if ( ratio_diff >= beta ){

                //Check for uniformity only if we are not using the least
                //covered point
                if (!test_cov || avg_mod_cov < avg_cov || models.size() == 0){
                    model->accepted( numPrincipalColor, numOtherColor );
                    not_finished = false;
                    double avg_cov_m = 0.0;
                    for (pit = trainingData.begin(); pit < trainingData.end(); 
                                                                    ++pit){
                        if ( (*pit)->get_color() == principalColor ) {
                            double inc = model->predict((*pit)->get_point());
                            (*pit)->increment_coverage(inc);
                            double cov =  (*pit)->get_coverage()*norm;
                            avg_cov_m += cov;
                        }
                    }
                    avg_cov_m /= model_pc;
                    if ( models.size() == 0 ) {
                        avg_cov = avg_cov_m;
                    } else {
                        avg_cov += (avg_cov - avg_cov_m)*norm;
                    }
                    models.push_back( model );
                    trainingData.reorder();
                    if (test_cov) rank = 0;
                    break;
                } 
            }

            // make the next space a bit bigger
            upf *= 1.10;
            lpf *= 1.10;

            // make the last space a bit thicker
            model->thicken( *boundary, rand, 0.8);
        }

        //if could not find an enriched model after max tries, delete the
        //   model and try again with a different point.
        if ( not_finished ){
            delete model;
            if ( t == num_attempts ){
                ++numUnfinished;
            }
            ++rank;
        }
    }

    training_data_prob_distribution();
};

void Discriminator::training_data_prob_distribution(){

    double avg_pc = 0.0;
    double avg_oc = 0.0;
    double avg2_pc = 0.0;
    double avg2_oc = 0.0;
    double npc = 0.0;
    double opc = 0.0;

    vector<CoveredPoint *>::iterator pit;
    for ( pit = trainingData.begin(); 
          pit < trainingData.end(); ++pit ){
        if ( (*pit)->get_color() == principalColor ) {
            npc = test( (*pit)->get_point() );
            avg_pc += npc;
            avg2_pc += (npc*npc);
        } else {
            opc = test( (*pit)->get_point() );
            avg_oc += opc;
            avg2_oc += (opc*opc);
        }
    }

    avg_pc /= numPrincipalColor;
    avg2_pc /= numPrincipalColor;
    avg_oc /= numOtherColor;
    avg2_oc /= numOtherColor;

    double sf_pc = numPrincipalColor/(numPrincipalColor - 1.0);
    double sf_oc = numOtherColor/(numOtherColor - 1.0);
    double sig_pc = sqrt(sf_pc*(avg2_pc - avg_pc*avg_pc));
    double sig_oc = sqrt(sf_oc*(avg2_oc - avg_oc*avg_oc));

    if ( avg_oc == 0.0 )
        threshold = 1.0;
    else
        threshold = (avg_oc+2.0*sig_oc + avg_pc - 2.0*sig_pc)/2.0;

    fprintf(stdout, 
   "%s %d\n%s %d\n%s %d\n%s %d\n%s %10.3e %s %10.3e %s %10.3e %s %10.3e\n\n", 
                   "number of models:", static_cast<int>(models.size()),
                   "number exceeded:", numUnfinished,
                   "number PC:", static_cast<int>(numPrincipalColor),
                   "number YC:", static_cast<int>(numOtherColor),
                   "avg Y_PC: ", avg_pc,
                   "sigma: ", sig_pc,
                   "avg Y_OC: ", avg_oc,
                   "sigma: ", sig_oc );

};


double Discriminator::test( const Point* point ){
    vector<Model*>::const_iterator mit;

    unsigned num_models = models.size();
    double prediction = 0.0;
    for (unsigned m = 0; m < num_models; ++m){
        prediction += models[m]->predict( point );
    }
    prediction /= static_cast<double>(num_models);

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
