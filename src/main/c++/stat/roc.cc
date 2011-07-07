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

#include "roc.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <limits>
#include <utility>
#include <vector>

namespace stat {

using std::binary_function;
using std::pair;
using std::vector;

struct LessFirstOnly :
      public binary_function<pair<double,int>, pair<double,int>, bool> {
    inline bool
    operator()(const pair<double, int>& x, const pair<double, int>& y) {
        return (x.first < y.first);
    }
};

void ROC::optimize_roc() {
    int tp_ = 0;
    int fp_ = 0;
    int num_acc = 0;
    int n_ = static_cast<int>(all.size());

    make_heap( all.begin(), all.end(), LessFirstOnly() );
    sort_heap( all.begin(), all.end(), LessFirstOnly() );
    
    vector<pair<double,int>>::reverse_iterator rit = all.rbegin();
    vector<pair<double,int>>::reverse_iterator rlast = all.rend();

    int rank = static_cast<int>(all.size());
    int index0 = static_cast<int>(num_negative);
    int s = 0;
    for ( ; rit != rlast; ++rit ) {
        if ( (*rit).second ) {
            ++tp_;
        } else {
            s  += (rank - index0);
            --index0;
            ++fp_;
        }
        int num_acc_ = tp_ + n_ - fp_;
        if ( num_acc_ > num_acc ){
            num_acc = num_acc_;
            tp  = static_cast<double>(tp_);
            fp  = static_cast<double>(fp_);
            threshold = (*rit).first;
        }
        --rank;
    }

    tn = num_negative - fp;
    fn = num_positive - tp;
    n = static_cast<double>(all.size());

    auc_ = static_cast<double>(s)/(num_negative*num_positive);

    modified = false;
}


}   // end namespace stat

