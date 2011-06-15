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


#include "noir/point.h"

#include <cmath>
#include <cstdlib>

namespace noir {

Point::Point( const NoirSpace *noirSpace) :
              noirSpace(noirSpace),
              nominals(0), ordinals(0), intervals(0), reals(0) {

    nominals  = new int[noirSpace->nominal];
    ordinals  = new double[noirSpace->ordinal];
    intervals = new double[noirSpace->interval];
    reals     = new double[noirSpace->real];

    for ( int n = 0; n < noirSpace->nominal; ++n ){
        nominals[n] = 0;
    }

    for ( int o = 0; o < noirSpace->ordinal; ++o ){
        ordinals[o] = 0;
    }

    for ( int i = 0; i < noirSpace->interval; ++i ){
        intervals[i] = 0.0;
    }

    for ( int r = 0; r < noirSpace->real; ++r ){
        reals[r] = 0.0;
    }
}

Point::~Point() {
    delete[] nominals;
    delete[] ordinals;
    delete[] intervals;
    delete[] reals;
}

}  // namespace noir
