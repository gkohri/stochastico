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


#include "noir/noir_space.h"

#include <cmath>
#include <limits>
#include <set>

namespace noir {

using std::set;

NoirSpace::NoirSpace( const NoirDimensions *noir_dimensions ):
                            dimensions(noir_dimensions),
                            ordinal_boundaries(0),
                            interval_boundaries(0),
                            real_boundaries(0),
                            diameter(-1),
                            allowed_nominals(0) {

    allowed_nominals = new set<int>[dimensions->nominal];
    for ( int n = 0; n < dimensions->nominal; ++n ) {
        allowed_nominals[n].clear();
    }

    ordinal_boundaries = new double*[dimensions->ordinal];
    for ( int o = 0; o < dimensions->ordinal; ++o ) {
        ordinal_boundaries[o] = new double[2];
        ordinal_boundaries[o][0] = -std::numeric_limits<double>::max();
        ordinal_boundaries[o][1] =  std::numeric_limits<double>::max();
    }

    interval_boundaries = new double*[dimensions->interval];
    interval_periods = new double[dimensions->interval];
    for ( int i = 0; i < dimensions->interval; ++i ) {
        interval_boundaries[i] = new double[2];
        interval_boundaries[i][0] = -std::numeric_limits<double>::max();
        interval_boundaries[i][1] =  std::numeric_limits<double>::max();
        interval_periods[i] = std::numeric_limits<double>::max();
    }


    real_boundaries = new double*[dimensions->real];
    for ( int r = 0; r < dimensions->real; ++r ) {
        real_boundaries[r] = new double[2];
        real_boundaries[r][0] = -std::numeric_limits<double>::max();
        real_boundaries[r][1] =  std::numeric_limits<double>::max();
    }
}

NoirSpace::~NoirSpace() {
    delete[] allowed_nominals;

    for ( int o = 0; o < dimensions->ordinal; o++ ) {
        delete[] ordinal_boundaries[o];
    }
    delete[] ordinal_boundaries;

    for ( int i = 0; i < dimensions->interval; i++ ) {
        delete[] interval_boundaries[i];
    }
    delete[] interval_boundaries;
    delete[] interval_periods;

    for ( int r = 0; r < dimensions->real; r++ ) {
        delete[] real_boundaries[r];
    }
    delete[] real_boundaries;
}

bool NoirSpace::in_closure( const Point *point ) const {

    bool in_closure = true;

    // Check the real cooordinates

    const double* reals = point->get_real_coordinates();
    for ( int r = 0; r < dimensions->real; ++r ) {
        if ( isnan(reals[r]) ) continue;
        if ( reals[r] < real_boundaries[r][0] ||
             reals[r] > real_boundaries[r][1]   ) {
            in_closure = false;
            break;
        }
    }

    if ( !in_closure ) return false;

    // Check the interval cooordinates

    const double* intervals = point->get_interval_coordinates();
    for ( int i = 0; i < dimensions->interval; ++i ) {
        if ( isnan(intervals[i]) ) continue;
        double lower = interval_boundaries[i][0];
        double upper = interval_boundaries[i][1];
        double value = intervals[i];
        if ( upper < lower ) {
            if ( !(lower <= value || value <= upper ) ) {
                in_closure = false;
                break;
            }
        } else {
            if ( !(lower <= value && value <= upper ) ) {
                in_closure = false;
                break;
            }
        }
    }

    if ( !in_closure ) return false;

    // Check the ordinal cooordinates

    const double* ordinals = point->get_ordinal_coordinates();
    for ( int o = 0; o < dimensions->ordinal; ++o ) {
        if ( isnan(ordinals[o]) ) continue;
        if ( ordinals[o] < ordinal_boundaries[o][0] ||
             ordinals[o] > ordinal_boundaries[o][1]   ) {
            in_closure = false;
            break;
        }
    }

    if ( !in_closure ) return false;

    // Check the nominal cooordinates
    const int* nominals = point->get_nominal_coordinates();
    for ( int n = 0; n < dimensions->nominal; ++n ) {
        if ( nominals[n] == -1 ) continue;
        if ( allowed_nominals[n].find(nominals[n]) ==
                                    allowed_nominals[n].end() ) {
            in_closure = false;
            break;
        }
    }

    return in_closure;
}

double NoirSpace::get_diameter() {
    if (diameter == -1) {

        diameter = 0.0;
        // The reals
        for ( int r = 0; r < dimensions->real; ++r ) {
            diameter += fabs(real_boundaries[r][1] - real_boundaries[r][0]);
        }

        // The intervals
        for ( int i = 0; i < dimensions->interval; ++i ) {
            double diff = fabs(interval_boundaries[i][1] - 
                                                interval_boundaries[i][0]);
            if (diff > 0.5*interval_periods[i]) diff = 0.5*interval_periods[i];
            diameter += diff;
        }

        // The ordinals
        for ( int o = 0; o < dimensions->ordinal; ++o ) {
            diameter += fabs(ordinal_boundaries[o][1] - 
                                                 ordinal_boundaries[o][0]);
        }

        // The nominals
        for ( int n = 0; n < dimensions->nominal; ++n ) {
            diameter += allowed_nominals[n].size() == 0 ? 0.0 : 1.0;
        }
    }

    return diameter;
}

}  // namespace noir
