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

#include "noir/noir_space.h"
#include "noir/orthotope.h"
#include "rng/random.h"

namespace sdm {

using std::numeric_limits;
using std::set;
using std::vector;

using noir::NoirSpace;
using noir::ClosedSpace;
using noir::Orthotope;
using rng::Random;


bool Model::check_point( const CoveredPoint *p ){
    vector<ClosedSpace*>::const_iterator nsit;

    bool covered = false;
    for ( nsit = spaces.begin(); nsit != spaces.end(); ++nsit ) {
        if ( (*nsit)->in_closure( p->get_data_point() ) ){
            if ( p->get_color() == principalColor ){
                ++numPrincipalColor;
                covered = true;
            } else {
                ++numOtherColor;
            }
            break;
        }
    }

    return covered;
}

bool Model::covers( const CoveredPoint *p ) const {
    vector<ClosedSpace*>::const_iterator nsit;

    bool is_covered = false;
    for ( nsit = spaces.begin(); nsit != spaces.end(); ++nsit ) {
        if ( (*nsit)->in_closure( p->get_data_point() ) ){
            is_covered = true;
            break;
        }
    }

    return is_covered;
}

void Model::clear_checked_points(){
    numPrincipalColor = 0.0;
    numOtherColor = 0.0;
}

double Model::characteristic( const DataPoint *p ){
    vector<ClosedSpace*>::const_iterator nsit;

    double characteristic = 0.0;
    for ( nsit = spaces.begin(); nsit != spaces.end(); ++nsit ) {
        if ( (*nsit)->in_closure(p) ){
            characteristic = 1.0;
            break;
        }
    }

    double fracOther = numOtherColor/totalOtherColors;
    return ( (characteristic - fracOther)/
              (numPrincipalColor/totalPrincipalColors - fracOther) );
}

}  // namespace sdm
