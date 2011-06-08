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

#include "sdm/training_data.h"

#include <set>
#include <cstdio>
#include <limits>

#include "sdm/covered_point.h"
#include "sdm/data_store.h"
#include "rng/random.h"

namespace sdm {

using std::map;
using std::multiset;
using std::pair;
using std::vector;

TrainingData::~TrainingData (){
    vector<CoveredPoint*>::iterator pit;
    for ( pit = data.begin(); pit != data.end(); ++pit){
        delete *pit;
    }
    data.clear();
}


void TrainingData::add( CoveredPoint *const p ) {
    data.push_back( p );
    if ( principalColor == p->get_color() ){
        pcData.push_back( p );
        pcOrderedData.insert(p);
        ++numPrincipalColor;
    } else {
        ++numOtherColor;
    }
}

CoveredPoint* TrainingData::get_least_covered(int rank) {

    if ( rank == 0 ) return *(pcOrderedData.begin());

    int numit = 0;
    multiset<CoveredPoint*,CoveredPoint::CompareCoverage>::iterator pit;
    for ( pit = pcOrderedData.begin(); pit != pcOrderedData.end(); ++pit){
        if ( numit == rank ) break;
        ++numit;
    }
    
    return *pit;
}

CoveredPoint* TrainingData::get_random_point() {
    int randomPoint = rand->next_int( static_cast<int>(pcData.size()) );
    return pcData[randomPoint];
}


void TrainingData::reorder(){
    pcOrderedData.clear();
    vector<CoveredPoint*>::iterator pit;
    for ( pit = pcData.begin(); pit != pcData.end(); ++pit){
        pcOrderedData.insert(*pit);
    }
}

void TrainingData::find_nn(){
    vector<CoveredPoint*>::iterator ipit;
    vector<CoveredPoint*>::iterator jpit;
    for ( ipit = pcData.begin(); ipit != pcData.end(); ++ipit){
        double dist = std::numeric_limits<double>::max();
        CoveredPoint *nnpoint = 0;
        for ( jpit = pcData.begin(); jpit != pcData.end(); ++jpit){
            if ( jpit == ipit ) continue;
            double ijdist = (*ipit)->distance(*jpit);
            if ( ijdist < dist ) {
                dist = ijdist;
                nnpoint = *jpit;
            }
        }
        nn[*ipit] = nnpoint;
    }
}

CoveredPoint* TrainingData::get_nn(CoveredPoint *cp) {
    return nn[cp];
}

void TrainingData::clear(){

    vector<CoveredPoint*>::iterator pit;
    for ( pit = data.begin(); pit != data.end(); ++pit){
        delete *pit;
    }

    data.clear();
    pcData.clear();
    nn.clear();
    pcOrderedData.clear();

    numPrincipalColor = 0;
    numOtherColor = 0;
}

}   // end namespace sdm
