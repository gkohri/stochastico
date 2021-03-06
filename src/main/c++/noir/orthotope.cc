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


#include "noir/orthotope.h"

#include <cmath>
#include <limits>
#include <set>

namespace noir {

using std::set;

Orthotope::Orthotope( const NoirSpace *noir_space ):
                            noirSpace(noir_space),
                            ordinal_boundaries(0),
                            interval_boundaries(0),
                            real_boundaries(0),
                            allowed_nominals(0) {

    allowed_nominals = new set<int>[noirSpace->nominal];
    for ( int n = 0; n < noirSpace->nominal; ++n ) {
        allowed_nominals[n].clear();
    }

    ordinal_boundaries = new double*[noirSpace->ordinal];
    for ( int o = 0; o < noirSpace->ordinal; ++o ) {
        ordinal_boundaries[o] = new double[2];
        ordinal_boundaries[o][0] = -std::numeric_limits<double>::max();
        ordinal_boundaries[o][1] =  std::numeric_limits<double>::max();
    }

    interval_boundaries = new double*[noirSpace->interval];
    for ( int i = 0; i < noirSpace->interval; ++i ) {
        interval_boundaries[i] = new double[2];
        interval_boundaries[i][0] = -std::numeric_limits<double>::max();
        interval_boundaries[i][1] =  std::numeric_limits<double>::max();
    }


    real_boundaries = new double*[noirSpace->real];
    for ( int r = 0; r < noirSpace->real; ++r ) {
        real_boundaries[r] = new double[2];
        real_boundaries[r][0] = -std::numeric_limits<double>::max();
        real_boundaries[r][1] =  std::numeric_limits<double>::max();
    }
}

Orthotope::~Orthotope() {
    delete[] allowed_nominals;

    for ( int o = 0; o < noirSpace->ordinal; o++ ) {
        delete[] ordinal_boundaries[o];
    }
    delete[] ordinal_boundaries;

    for ( int i = 0; i < noirSpace->interval; i++ ) {
        delete[] interval_boundaries[i];
    }
    delete[] interval_boundaries;

    for ( int r = 0; r < noirSpace->real; r++ ) {
        delete[] real_boundaries[r];
    }
    delete[] real_boundaries;
}

bool Orthotope::in_closure( const Point *point ) const {

    bool in_closure = true;

    // Check the real cooordinates

    const double* reals = point->get_real_coordinates();
    for ( int r = 0; r < noirSpace->real; ++r ) {
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
    for ( int i = 0; i < noirSpace->interval; ++i ) {
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
    for ( int o = 0; o < noirSpace->ordinal; ++o ) {
        if ( ordinals[o] == -1) continue;
        if ( ordinals[o] < ordinal_boundaries[o][0] ||
             ordinals[o] > ordinal_boundaries[o][1]   ) {
            in_closure = false;
            break;
        }
    }

    if ( !in_closure ) return false;

    // Check the nominal cooordinates
    const int* nominals = point->get_nominal_coordinates();
    for ( int n = 0; n < noirSpace->nominal; ++n ) {
        if ( nominals[n] == -1 ) continue;
        if ( allowed_nominals[n].find(nominals[n]) ==
                                    allowed_nominals[n].end() ) {
            in_closure = false;
            break;
        }
    }

    return in_closure;
}

}  // namespace noir
