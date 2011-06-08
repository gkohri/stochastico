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


#include "sdm/model.h"

#include <cmath>
#include <limits>
#include <set>
#include <vector>

#include "rng/random.h"
#include "sdm/noir-dimensions.h"

namespace sdm {

using std::numeric_limits;
using std::set;
using std::vector;

using rng::Random;
using sdm::NoirDimensions;

void Model::expand( const NoirSpace &region, CoveredPoint *nexus, 
                    CoveredPoint *nn, Random *rand,
                    const double &lp, const double &up ){

    const NoirDimensions *dimensions = nexus->get_noir_dimension();
    NoirSpace *ns = new NoirSpace( dimensions );

    double upper = 0.0;
    double lower = 0.0;

// Select the Real Dimensions

    const double* reals = nexus->get_real_coordinates();
    const double* nn_reals = 0;
    if ( nn != 0 ) nn_reals = nn->get_real_coordinates();

    for ( int r = 0; r < dimensions->real; r++ ){

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

        ns->set_real_boundaries( r, rectLower, rectUpper );
    }

// Select the Interval Dimensions

    const double* intervals = nexus->get_interval_coordinates();

    for ( int i = 0; i < dimensions->interval; i++ ){

        double coordinate = intervals[i];
        if ( isnan( coordinate ) ) continue;

        region.get_interval_boundaries( i, lower, upper );

        double radius = (upper - lower)*0.5;

        double zu = radius*(lp + (up-lp)*rand->next());
        double rectUpper = coordinate + zu;

        if ( rectUpper > upper ) rectUpper = upper;

        double zl = radius*(lp + (up-lp)*rand->next());
        double rectLower = coordinate - zl;

        if ( rectLower < lower ) rectLower = lower;

        ns->set_interval_boundaries( i, rectLower, rectUpper );
    }


// Select the Ordinal Dimensions

    int iupper = 0.0;
    int ilower = 0.0;

    const int* ordinals = nexus->get_ordinal_coordinates();

    for ( int o = 0; o < dimensions->ordinal; o++ ){

        int coordinate = ordinals[o];
        if ( coordinate == -1 ) continue;

        region.get_ordinal_boundaries( o, ilower, iupper );

        int radius = (iupper - ilower)/2;
        if ( radius == 0 ) radius = 1;

        int rectUpper = coordinate;
        if ( rand->next() < up ) rectUpper += radius;

        if ( rectUpper > iupper ) rectUpper = iupper;

        int rectLower = coordinate;
        if ( rand->next() < up ) rectLower -= radius;

        if ( rectLower < ilower ) rectLower = ilower;

        ns->set_ordinal_boundaries( o, rectLower, rectUpper );
    }


// Select the Nominal Dimensions

    const int* nominals = nexus->get_nominal_coordinates();

    for ( int n = 0; n < dimensions->nominal; ++n ){
        int coordinate = nominals[n];
        if ( coordinate == -1 ) continue;

        const set<int> allowed = region.get_nominals(n);

        int max_allowable = static_cast<int>(allowed.size());

        for ( int nn = 0; nn < max_allowable; ++nn) {
            if ( rand->next() > up) continue;
            ns->add_nominal( n, nn );
        }
    }

// Store the resulting space

    spaces.push_back( ns );
}

void Model::thicken(const NoirSpace &region, Random *rand, const double &frac){

    NoirSpace *ns = spaces.back();

    double upper = 0.0;
    double lower = 0.0;
    for ( int d = 0; d < region.dimensions->real; d++ ){
        if ( rand->next() > frac ) continue;

        region.get_real_boundaries( d, lower, upper );

        ns->set_real_boundaries( d, lower, upper );
    }
}

bool Model::check_point( const CoveredPoint *p ){
    vector<NoirSpace*>::const_iterator nsit;

    bool covered = false;
    for ( nsit = spaces.begin(); nsit != spaces.end(); ++nsit ) {
        if ( (*nsit)->in_closure( p->get_point() ) ){
            if ( p->get_color() == principalColor ){
                numPrincipalColor++;
                covered = true;
            } else {
                numOtherColor++;
            }
            break;
        }
    }

    return covered;
}

bool Model::covers( const CoveredPoint *p ) const {
    vector<NoirSpace*>::const_iterator nsit;

    bool is_covered = false;
    for ( nsit = spaces.begin(); nsit != spaces.end(); ++nsit ) {
        if ( (*nsit)->in_closure( p->get_point() ) ){
            is_covered = true;
            break;
        }
    }

    return is_covered;
}

void Model::clear_checked_points(){
    numPrincipalColor = 0;
    numOtherColor = 0;
}

void Model::accepted( const double &numPC, const double &numOC ){
    fracPrincipal = static_cast<double>(numPrincipalColor)/numPC;
    fracOther     = static_cast<double>(numOtherColor)/numOC;
    probNorm     = 1.0/(fracPrincipal - fracOther);
}

double Model::predict( const Point *p ){
    vector<NoirSpace*>::const_iterator nsit;

    double characteristic = 0.0;
    for ( nsit = spaces.begin(); nsit != spaces.end(); ++nsit ) {
        if ( (*nsit)->in_closure(p) ){
            characteristic = 1.0;
            break;
        }
    }

    return ( (characteristic - fracOther)*probNorm );
}

}  // namespace sdm
