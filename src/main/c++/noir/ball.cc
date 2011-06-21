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


#include "noir/ball.h"

#include <cmath>
#include <cstdio>
#include <limits>
#include <set>

namespace noir {

using std::set;

Ball::Ball( const NoirSpace *noir_space, const double &radius ) : 
            Point(noir_space), radius(radius) {
    allowed_nominals = new set<int>[noirSpace->nominal];
    for ( int n = 0; n < noirSpace->nominal; ++n ) {
        allowed_nominals[n].clear();
    }
}

Ball::~Ball() {
    delete[] allowed_nominals;
}

bool Ball::in_closure( const Point *point ) const {

    double dist = noirSpace->norm( this, point );

    int const *p_nominals = point->get_nominal_coordinates();
    for ( int n = 0; n < noirSpace->nominal; ++n ) {
        if ( allowed_nominals[n].find(p_nominals[n]) !=
                                        allowed_nominals[n].end() ) {
            dist -= 1.0;
        }
    }

    double epsilon = 1.0e-3;
    if ( dist - radius > epsilon )
        return false;
    else
        return true;
}


}  // namespace noir
