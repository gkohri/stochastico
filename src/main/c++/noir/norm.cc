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

#include "noir/norm.h"

#include <cmath>
#include <cstdlib>

#include "noir/point.h"

namespace noir {

double Norm::operator()(const Point* x, const Point* y) const {
    double dist = 0.0;

    const NoirSpace *const noirSpace = x->noirSpace;

    double const *x_reals = x->get_real_coordinates();
    double const *y_reals = y->get_real_coordinates();
    for ( int r = 0; r < noirSpace->real; ++r ){
        if ( isnan(x_reals[r]) || isnan(y_reals[r]) ) continue; 
        dist += fabs(x_reals[r] - y_reals[r]);
    }

    double const *x_intervals = x->get_interval_coordinates();
    double const *y_intervals = y->get_interval_coordinates();
    for ( int i = 0; i < noirSpace->interval; ++i ){
        if ( isnan(x_intervals[i]) || isnan(y_intervals[i]) ) continue;
        double sin_arg = sin(M_PI*(x_intervals[i] - y_intervals[i]));
        dist += fabs(sin_arg);
    }

    double const *x_ordinals = x->get_ordinal_coordinates();
    double const *y_ordinals = y->get_ordinal_coordinates();
    for ( int o = 0; o < noirSpace->ordinal; ++o ){
        if ( x_ordinals[o] == -1 || y_ordinals[o] == -1 ) continue;
        dist += fabs(x_ordinals[o] - y_ordinals[o]);
    }

    int const *x_nominals = x->get_nominal_coordinates();
    int const *y_nominals = y->get_nominal_coordinates();
    for ( int n = 0; n < noirSpace->nominal; ++n ){
        if ( x_nominals[n] == -1 || y_nominals[n] == -1 ) continue;
        dist += ( x_nominals[n] == y_nominals[n] ? 0.0 : 1.0 );
    }

    return dist;
}

double Norm::operator()(const Point* x) const {
    double dist = 0.0;

    const NoirSpace *const noirSpace = x->noirSpace;

    double const *x_reals = x->get_real_coordinates();
    for ( int r = 0; r < noirSpace->real; ++r ){
        if ( isnan(x_reals[r]) ) continue; 
        dist += fabs(x_reals[r]);
    }

    double const *x_intervals = x->get_interval_coordinates();
    for ( int i = 0; i < noirSpace->interval; ++i ){
        if ( isnan(x_intervals[i]) ) continue;
        dist += fabs( sin(M_PI*x_intervals[i]) );
    }

    double const *x_ordinals = x->get_ordinal_coordinates();
    for ( int o = 0; o < noirSpace->ordinal; ++o ){
        if ( x_ordinals[o] == -1 ) continue;
        dist += fabs(x_ordinals[o]);
    }

    int const *x_nominals = x->get_nominal_coordinates();
    for ( int n = 0; n < noirSpace->nominal; ++n ){
        if ( x_nominals[n] != -1 ) ++dist;
    }

    return dist;
}

}  // namespace noir
