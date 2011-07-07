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

#ifndef STAT_MULTI_SCORECARD_H
#define STAT_MULTI_SCORECARD_H

#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <map>
#include <utility>
#include <vector>

namespace stat {

/*
 * A score card version of ROC.
 */
class MultiScorecard {
 public:
    explicit MultiScorecard(const int &num_classes) : numClasses(num_classes),
                   all(), correct(num_classes), 
                   wrong(num_classes), examples(num_classes), 
                   fp(num_classes),
                   modified(0), m_(0) {}


    virtual ~MultiScorecard() {}

    template <typename InputIterator>
    void record_results( const int &target_class, InputIterator results) {
        std::vector<double> scores(numClasses);
        for (size_t c = 0; c < numClasses; c++ ){
            scores[c] = *results;
            ++results;
        }
        
        all.push_back(std::pair<std::vector<double>,int>(scores,target_class));
        modified = true;
    }

    void clear() {
        all.clear();
        modified = false;
    }

    double accuracy();

    double error_rate(const int &class_id);

    double error_rate();

    double sensitivity(const int &class_id);

    double avg_sensitivity();

    double precision(const int &class_id);

    double avg_precision();

    double false_discovery_rate(const int &class_id);

    double avg_false_discovery_rate();

    double m() {
        calculate_m();
        return m_;
    }


 private:
    const size_t numClasses;
    std::vector<std::pair<std::vector<double>,int>> all;
    std::vector<double> correct;
    std::vector<double> wrong;
    std::vector<double> examples;
    std::vector<double> fp;
    bool modified;
    double m_;

    MultiScorecard(const MultiScorecard&) = delete;
    MultiScorecard& operator=(const MultiScorecard&) = delete;

    void calculate_m();
    void process();

    double ahat( const int &i, const int &j);
    double auc( std::vector<std::pair<double,int>> scores );

};

}   // end namespace stat

#endif   // STAT_MULTI_SCORECARD_H
