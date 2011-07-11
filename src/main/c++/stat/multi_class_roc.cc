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

#include "multi_class_roc.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <limits>
#include <utility>
#include <vector>

namespace stat {

using std::binary_function;
using std::numeric_limits;
using std::pair;
using std::vector;

/**
 * A pair comparison functional that only takes into account the first
 * member of the pair.  In cases where the ordering on the second member of
 * the pair doesn't matter, this might be more efficient.
 */
struct LessFirstOnly :
      public binary_function<pair<double,int>, pair<double,int>, bool> {
    inline bool
    operator()(const pair<double, int>& x, const pair<double, int>& y) {
        return (x.first < y.first);
    }
};

void MultiClassROC::process() {
    for ( size_t c = 0; c < numClasses; ++c ){
        correct[c] = 0.0;
        wrong[c] = 0.0;
        fp[c] = 0.0;
        examples[c] = 0.0;
    }
    size_t num_scores = all.size();
    for ( size_t s = 0; s < num_scores; ++s ){
        double max = -std::numeric_limits<double>::max();
        vector<double> scores = all[s].first;
        int real  = all[s].second;
        int best = -1;
        for ( size_t c = 0; c < numClasses; ++c ){
            if ( scores[c] > max ) {
                best = static_cast<int>(c);
                max = scores[c];
            }
        }
        if ( best == real ) {
            ++correct[real];
        } else {
            ++fp[best];
            ++wrong[real];
        }
        ++examples[real];
    }

    modified = false;
}

double MultiClassROC::accuracy(){
    if ( modified ) process();
    double t_correct = 0.0;
    double total = 0.0;
    for ( size_t c = 0; c < numClasses; ++c ){
        t_correct += correct[c];
        total += examples[c];
    }
    return (t_correct/total);
}


double MultiClassROC::error_rate(){
    if ( modified ) process();
    double t_wrong = 0.0;
    double t_total = 0.0;
    for ( size_t c = 0; c < numClasses; ++c ){
        t_wrong += wrong[c];
        t_total += examples[c];
    }
    return (t_wrong/t_total);
}

double MultiClassROC::error_rate(const int &class_id){
    if ( modified ) process();
    return (wrong[class_id]/examples[class_id]);
}


double MultiClassROC::sensitivity(const int &class_id){
    if ( modified ) process();
    return (correct[class_id]/examples[class_id]);
}

double MultiClassROC::avg_sensitivity(){
    if ( modified ) process();
    double sensitivity = 0.0;
    for ( size_t c = 0; c < numClasses; ++c ){
        if (examples[c] != 0.0 )
            sensitivity += correct[c]/examples[c];
    }
    return (sensitivity/static_cast<double>(numClasses));
}

double MultiClassROC::precision(const int &class_id){
    if ( modified ) process();
    return (correct[class_id]/(correct[class_id] + fp[class_id]));
}

double MultiClassROC::avg_precision(){
    if ( modified ) process();
    double precision = 0.0;
    for ( size_t c = 0; c < numClasses; ++c ){
        precision += correct[c]/(correct[c] + fp[c]);
    }
    return (precision/static_cast<double>(numClasses));
}

double MultiClassROC::false_discovery_rate(const int &class_id){
    if ( modified ) process();
    return (fp[class_id]/(correct[class_id] + fp[class_id]));
}

double MultiClassROC::avg_false_discovery_rate(){
    if ( modified ) process();
    double fdr = 0.0;
    for ( size_t c = 0; c < numClasses; ++c ){
        double denom = correct[c] + fp[c];
        if ( denom != 0.0 )
            fdr += fp[c]/denom;
    }
    return (fdr/static_cast<double>(numClasses));
}

void MultiClassROC::calculate_m() {

    m_ = 0.0;

    for ( int i = 0; i < static_cast<int>(numClasses); ++i ) {
        for ( int j = i+1; j < static_cast<int>(numClasses); ++j ) {
            m_ += ahat(i,j);
        }
    }

    m_ = 2.0*m_/static_cast<double>(numClasses*(numClasses-1));
}

double MultiClassROC::ahat( const int &i, const int &j){

    vector<pair<double,int>> i_scores;
    vector<pair<double,int>> j_scores;

    int num_results = all.size();
    for ( int r = 0; r < num_results; ++r ) {
        if ( all[r].second == i ) {
            i_scores.push_back( pair<double,int>(all[r].first[i], 0) );
            j_scores.push_back( pair<double,int>(all[r].first[j], 1) );
        } else if ( all[r].second == j ) {
            i_scores.push_back( pair<double,int>(all[r].first[i], 1) );
            j_scores.push_back( pair<double,int>(all[r].first[j], 0) );
        }
    }

    return ( ( auc(i_scores) +  auc(j_scores) )*0.5 );
}


double MultiClassROC::auc( vector<pair<double,int>> scores ){

    make_heap( scores.begin(), scores.end(), LessFirstOnly() );
    sort_heap( scores.begin(), scores.end(), LessFirstOnly() );

    int num_scores = static_cast<int>(scores.size());

    int index_0 = 0;
    double rank_sum = 0.0;
    double num_0 = 0.0;
    double num_1 = 0.0;
    for ( int s = 0, rank = 1; s < num_scores; ++s,++rank ) {
        if ( scores[s].second == 0 ) {
            ++index_0;
            rank_sum += static_cast<double>(rank - index_0);
            ++num_0;
        } else {
            ++num_1;
        }
    }

    if ( num_0 == 0 || num_1 == 0 ) {
        return 0.0;
    } else {
        return (rank_sum/(num_0*num_1));
    }
}

}   // end namespace stat
