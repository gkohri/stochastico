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

#ifndef SDM_DISCRIMINATOR_H
#define SDM_DISCRIMINATOR_H

#include <vector>
#include <map>

#include "sdm/data_store.h"
#include "sdm/noir-space.h"
#include "sdm/model.h"
#include "sdm/training_data.h"
#include "rng/random.h"

namespace sdm{

/*
 * The discriminator is specialized to a specific color (class). It learns to
 * discriminate points belonging to its color from points belonging to other
 * colors
 */
class Discriminator {
 public:
    Discriminator( const int &principal_color = 0 ): 
        trainingData( principal_color ), models(),
        boundary(0), rand(0),
        numPrincipalColor(0.0), 
        numOtherColor(0.0),threshold(1.0),
        lowerFrac(0.0), upperFrac(0.1), beta(1.0),
        principalColor( principal_color ), numUnfinished(0) {}

    virtual ~Discriminator(){
        clear();
    }

    /*
     * Retrieve the color on which this discriminator has specialized.
     */
    int get_principal_color(){
        return principalColor;
    }

    /*
     * Set the random number generator to use.
     */
    void set_random( rng::Random *random ){
        rand = random;
        trainingData.set_random( rand );
    }

    /*
     * Set the boundaries for the learning stage
     */
    void set_boundary( const NoirSpace *boundary ){
        this->boundary = boundary;
    }

    /*
     * Set the lower fraction for use in the learning stage
     */
    void set_lower_fraction( const double &lower_fraction ){
        lowerFrac = lower_fraction;
    }

    /*
     * Set the upper fraction for use in the learning stage
     */
    void set_upper_fraction( const double &upper_fraction ){
        upperFrac = upper_fraction;
    }

    /*
     * Set the enrichment factor for use in the learning stage
     */
    void set_beta( const double &beta_value ){
        beta = beta_value;
    }

    /*
     * Add some training data to be used in the learning stage.
     */
    void add_training_data( DataStore *data );

    /*
     * This method creates models using the least covered point as the model's
     * nexus
     */
    void create_models_lc( const int &num_models, const int &num_attempts );

    /*
     * Determine the probability that the specified point is a member of
     * the class specialized by this discriminator.
     */
    double test( const Point *point );

    /*
     * Removes all the data and all the models
     */
    void clear();

    /*
     * Checks the data for consistency before training begins
     */
    void check_data_consistency();
    
 private:
    TrainingData trainingData;
    std::vector<Model*> models;
    const NoirSpace *boundary;
    rng::Random  *rand;
    double  numPrincipalColor;
    double  numOtherColor;
    double  threshold;
    double  lowerFrac;
    double  upperFrac;
    double  beta;
    int     principalColor;
    int     numUnfinished;

    void training_data_prob_distribution();

    Discriminator(const Discriminator&) = delete;
    Discriminator& operator=(const Discriminator&) = delete;

};

}   // end namespace sdm

#endif   // SDM_DISCRIMINATOR_H
