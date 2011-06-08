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


#include "sdm/point.h"

#include <cmath>
#include <cstdlib>

namespace sdm {


Point::Point( const int& id, const int& color,
              const NoirDimensions *dimensions): 
              dimensions(dimensions), id(id), color(color), 
              nominals(0), ordinals(0), intervals(0), reals(0) {

    nominals  = new int[dimensions->nominal];
    ordinals  = new int[dimensions->ordinal];
    intervals = new double[dimensions->interval];
    reals     = new double[dimensions->real];

    for ( int n = 0; n < dimensions->nominal; ++n ){
        nominals[n] = 0;
    }

    for ( int o = 0; o < dimensions->ordinal; ++o ){
        ordinals[o] = 0;
    }

    for ( int i = 0; i < dimensions->interval; ++i ){
        intervals[i] = 0.0;
    }

    for ( int r = 0; r < dimensions->real; ++r ){
        reals[r] = 0.0;
    }
}


Point::~Point() {
    delete[] nominals;
    delete[] ordinals;
    delete[] intervals;
    delete[] reals;
}

double Point::distance( const Point *p) const {
    double dist = 0.0;

    double const *p_reals = p->get_real_coordinates();
    for ( int r = 0; r < dimensions->real; ++r ){
        if ( isnan(reals[r]) || isnan(p_reals[r]) ) continue; 
        dist += fabs(reals[r] - p_reals[r]);
    }

    double const *p_intervals = p->get_interval_coordinates();
    for ( int i = 0; i < dimensions->interval; ++i ){
        if ( isnan(intervals[i]) || isnan(p_intervals[i]) ) continue; 
        dist += fabs(intervals[i] - p_intervals[i]);
    }

    int const *p_ordinals = p->get_ordinal_coordinates();
    for ( int o = 0; o < dimensions->ordinal; ++o ){
        dist += static_cast<double>(abs(ordinals[o] - p_ordinals[o]));
    }

    int const *p_nominals = p->get_nominal_coordinates();
    for ( int n = 0; n < dimensions->nominal; ++n ){
        dist += ( nominals[n] == p_nominals[n] ? 1.0 : 0.0 );
    }

    return dist;
}

}  // namespace sdm
