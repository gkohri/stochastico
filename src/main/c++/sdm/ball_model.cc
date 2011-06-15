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
#include <limits>
#include <set>
#include <vector>

#include "noir/ball.h"
#include "noir/noir_space.h"
#include "noir/orthotope.h"
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
    Orthotope *orthotope = new Orthotope( noirSpace );

    double upper = 0.0;
    double lower = 0.0;

// Select the Real Dimensions

    const double* reals = nexus->get_real_coordinates();
    const double* nn_reals = 0;
    if ( nn != 0 ) nn_reals = nn->get_real_coordinates();

    for ( int r = 0; r < noirSpace->real; r++ ){

        double coordinate = reals[r];
        if ( isnan( coordinate ) ) continue;

        double nn_diff = 0.0;
        if ( nn_reals != 0 && !isnan( nn_reals[r] ) )
            nn_diff = nn_reals[r] - coordinate;

        region.get_real_boundaries( r, lower, upper );

        double radius = (upper - lower)*0.5;

        double zu = radius*(lp + (up-lp)*rand->next());
        double rectUpper = coordinate + zu;
        if ( nn_diff > 0.0 ) rectUpper += nn_diff;

        if ( rectUpper > upper ) rectUpper = upper;

        double zl = radius*(lp + (up-lp)*rand->next());
        double rectLower = coordinate - zl;
        if ( nn_diff < 0.0 ) rectLower += nn_diff;

        if ( rectLower < lower ) rectLower = lower;

        orthotope->set_real_boundaries( r, rectLower, rectUpper );
    }

// Select the Interval Dimensions

    const double* intervals = nexus->get_interval_coordinates();
    const double* nn_intervals = 0;
    if ( nn != 0 ) nn_intervals = nn->get_interval_coordinates();

    for ( int i = 0; i < noirSpace->interval; i++ ){

        double coordinate = intervals[i];
        if ( isnan( coordinate ) ) continue;

        double nn_diff = 0.0;
        if ( nn_intervals != 0 && !isnan( nn_intervals[i] ) )
            nn_diff = nn_intervals[i] - coordinate;

        region.get_interval_boundaries( i, lower, upper );

        double radius = 0.5;

        double zu = radius*(lp + (up-lp)*rand->next());
        double rectUpper = coordinate + zu;
        if ( nn_diff > 0.0 ) rectUpper += nn_diff;

        if ( rectUpper > 1.0 ) 
            rectUpper = rectUpper - 1.0;
        else if ( rectUpper > upper )
            rectUpper = upper;

        double zl = radius*(lp + (up-lp)*rand->next());
        double rectLower = coordinate - zl;
        if ( nn_diff < 0.0 ) rectLower += nn_diff;

        if ( rectLower < 0.0 ) 
            rectLower = 1.0+lower;
        else if ( rectLower < lower )
            rectLower = lower;

        orthotope->set_interval_boundaries( i, rectLower, rectUpper );
    }


// Select the Ordinal Dimensions

    const double* ordinals = nexus->get_ordinal_coordinates();
    const double* nn_ordinals = 0;
    if ( nn != 0 ) nn_ordinals = nn->get_ordinal_coordinates();

    for ( int o = 0; o < noirSpace->ordinal; o++ ){

        double coordinate = ordinals[o];
        if ( isnan( coordinate ) ) continue;

        double nn_diff = 0.0;
        if ( nn_ordinals != 0 && !isnan( nn_ordinals[o] ) )
            nn_diff = nn_ordinals[o] - coordinate;

        region.get_ordinal_boundaries( o, lower, upper );

        double radius = (upper - lower)/2.0;

        double rectUpper = coordinate;
        if ( rand->next() < up ) rectUpper += radius;
        if ( nn_diff > 0.0 ) rectUpper += nn_diff;

        if ( rectUpper > upper ) rectUpper = upper;

        double rectLower = coordinate;
        if ( rand->next() < up ) rectLower -= radius;
        if ( nn_diff < 0.0 ) rectLower += nn_diff;

        if ( rectLower < lower ) rectLower = lower;

        orthotope->set_ordinal_boundaries( o, rectLower, rectUpper );
    }


// Select the Nominal Dimensions

    const int* nominals = nexus->get_nominal_coordinates();
    const int* nn_nominals = 0;
    if ( nn != 0 ) nn_nominals = nn->get_nominal_coordinates();

    for ( int n = 0; n < noirSpace->nominal; ++n ){
        int coordinate = nominals[n];
        if ( coordinate == -1 ) continue;

        const set<int> allowed = region.get_nominals(n);
/*
        // Odd, it seems to work better using a random selection of nominals

        ns->add_nominal( n, coordinate );

        if ( nn_nominals != 0 && ! nn_nominals[n] != -1 )
            ns->add_nominal( n, nn_nominals[n] );

        int max_allowable = static_cast<int>(allowed.size()) - 2;
*/
        int max_allowable = static_cast<int>(allowed.size());

        for ( int nn = 0; nn < max_allowable; ++nn) {
            if ( rand->next() > up ) continue;
            orthotope->add_nominal( n, nn );
        }
    }

// Store the resulting space

    spaces.push_back( orthotope );
}

void BallModel::thicken(const Orthotope &region, Random *rand, 
                              const double &frac){

    Orthotope *orthotope = dynamic_cast<Orthotope*>(spaces.back());

    double upper = 0.0;
    double lower = 0.0;
    for ( int d = 0; d < region.noirSpace->real; d++ ){
        if ( rand->next() > frac ) continue;

        region.get_real_boundaries( d, lower, upper );

        orthotope->set_real_boundaries( d, lower, upper );
    }
}


}  // namespace sdm
