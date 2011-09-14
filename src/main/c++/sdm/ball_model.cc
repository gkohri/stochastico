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


#include "sdm/ball_model.h"

#include <cmath>
#include <cstdio>
#include <limits>
#include <set>
#include <vector>

#include "noir/ball.h"
#include "noir/noir_space.h"
#include "noir/ball.h"
#include "rng/random.h"

namespace sdm {

using std::numeric_limits;
using std::set;
using std::vector;

using noir::Ball;
using noir::ClosedSpace;
using noir::NoirSpace;
using noir::Orthotope;
using rng::Random;

void BallModel::expand( const Orthotope &region, CoveredPoint *nexus, 
                    CoveredPoint *nn, Random *rand,
                    const double &lp, const double &up ){

    const NoirSpace *noirSpace = nexus->get_noir_space();

// Determine the radius of the Ball

    double diameter = static_cast<double>(noirSpace->nominal +
                                          noirSpace->ordinal +
                                          noirSpace->interval +
                                          noirSpace->real);

    double nn_dist = noirSpace->norm(nexus->get_data_point(),
                                     nn->get_data_point() );

    double radius = diameter*(lp + (up-lp)*static_cast<double>(rand->next()));

    radius = radius < nn_dist ? nn_dist : radius;

    //fprintf(stderr,"radius: %10.3e  %10.3e\n",radius,nn_dist);

    Ball *ball = new Ball( noirSpace, radius );

// Select the Real Dimensions

    const double* reals = nexus->get_real_coordinates();
    const double* nn_reals = 0;
    if ( nn != 0 ) nn_reals = nn->get_real_coordinates();

    for ( int r = 0; r < noirSpace->real; r++ ){

        double coordinate = reals[r];
        double between = coordinate;
        if ( !isnan( coordinate ) && !isnan( nn_reals[r] ) ) {
            double diff = nn_reals[r] - coordinate;
            between += diff*static_cast<double>(rand->next());
            //between = (nn_reals[r] + coordinate)*0.5;
        } else {
            between = numeric_limits<double>::quiet_NaN();
        }

        ball->set_real_coordinate( r, between );
    }

// Select the Interval Dimensions

    const double* intervals = nexus->get_interval_coordinates();
    const double* nn_intervals = 0;
    if ( nn != 0 ) nn_intervals = nn->get_interval_coordinates();

    for ( int i = 0; i < noirSpace->interval; i++ ){

        double coordinate = intervals[i];
        double between = coordinate;
        if ( !isnan( coordinate ) && !isnan( nn_intervals[i] ) ) {
            double diff = nn_intervals[i] - coordinate;
            between += diff*static_cast<double>(rand->next());
        } else {
            between = numeric_limits<double>::quiet_NaN();
        }

        ball->set_interval_coordinate( i, between );
    }


// Select the Ordinal Dimensions

    const double* ordinals = nexus->get_ordinal_coordinates();
    const double* nn_ordinals = 0;
    if ( nn != 0 ) nn_ordinals = nn->get_ordinal_coordinates();

    for ( int o = 0; o < noirSpace->ordinal; o++ ){

        double coordinate = ordinals[o];
        double between = coordinate;
        if ( !isnan( coordinate ) && !isnan( nn_ordinals[o] ) ){
            double diff = nn_ordinals[o] - coordinate;
            between += diff*static_cast<double>(rand->next());
        } else {
            between = numeric_limits<double>::quiet_NaN();
        }

        ball->set_ordinal_coordinate( o, between );
    }


// Select the Nominal Dimensions

    const int* nominals = nexus->get_nominal_coordinates();
    const int* nn_nominals = 0;
    if ( nn != 0 ) nn_nominals = nn->get_nominal_coordinates();

    for ( int n = 0; n < noirSpace->nominal; ++n ){
        int coordinate = nominals[n];

        const set<int> allowed = region.get_nominals(n);

        int max_allowable = static_cast<int>(allowed.size()) - 2;


        if ( coordinate != -1 && nn_nominals[n] != -1 ){
            ball->add_nominal( n, coordinate );
            ball->add_nominal( n, nn_nominals[n] );
        }

        for ( int nn = 0; nn < max_allowable; ++nn) {
            if ( static_cast<double>(rand->next()) > up ) continue;
            ball->add_nominal( n, nn );
        }
    }

// Store the resulting space

/*
    double nn_ball_dist = noirSpace->norm(ball,
                                     nn->get_data_point() );
    double nexus_ball_dist = noirSpace->norm(ball,
                                     nexus->get_data_point() );
    fprintf(stderr,"c: %d radius: %10.3e  %10.3e  %10.3e  %10.3e\n",
                    get_principal_color(),
                                              radius,nn_dist,
                                              nexus_ball_dist,
                                              nn_ball_dist);
*/

    spaces.push_back( ball );
}

void BallModel::thicken(const Orthotope &region, Random *rand, 
                              const double &frac){

    Ball *ball = dynamic_cast<Ball*>(spaces.back());
    double radius = ball->get_radius()/frac;

    ball->set_radius(radius);
}


}  // namespace sdm
