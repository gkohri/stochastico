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

#ifndef STAT_MULTI_ROC_H
#define STAT_MULTI_ROC_H

#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <map>
#include <utility>
#include <vector>

namespace stat {

/**
 * A multiple class version of ROC (Receiver Operating Charteristic).
 * Unlike the two class case, the outcome of classifying an arbitrary
 * instance is not a single numerical value, but a set of numerical
 * values, one for each of the possible classes.
 */
class MultiROC {
 public:
/**
 * Create a new instance capable of handling the specified number of classes.
 */
    explicit MultiROC(const int &num_classes) : numClasses(num_classes),
                   all(), correct(num_classes), 
                   wrong(num_classes), examples(num_classes), 
                   fp(num_classes),
                   modified(0), m_(0) {}


    virtual ~MultiROC() {}

/**
 * Retrieves the number of classes this instance can handle.
 */
    int get_num_classes() const {
        return numClasses;
    }

/**
 * Record the results of a classification experiment.
 * 
 * @param target_class the class to which this instance belongs
 * @param results an iterator over the scores for each class. The scores
 *        should always be in the same order.
 */
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

    /**
     * Erases all previously recorded scores and resets all counters.
     */
    void clear() {
        all.clear();
        modified = false;
    }

    /**
     * Calculate the overall accuracy of the classifier. The highest score 
     * is used
     * to determine the predicted class, which is then compared to the
     * class from which the instance came.
     */
    double accuracy();

    /**
     * Calculate the overall error rate of the classifier. This is just 
     * 1.0 - accuracy.
     */
    double error_rate();

    /**
     * Calculate the error rate for the specified class.
     */
    double error_rate(const int &class_id);

    /**
     * Calculate the sensitivity for the specified class.
     *
     * The sensitivity is defined as the number of correctly identified
     * instances of the specified class divided by the number of 
     * recorded instances of the specified class.
     *
     */
    double sensitivity(const int &class_id);

    /**
     * Calculate the average sensitivity over all classes
     */
    double avg_sensitivity();

    /**
     * Calculate the precision for the specified class.
     *
     * The precision is defined as the number of correctly identified
     * instances of the specified class divided by the number of 
     * correctly identified instances plus the number of instances incorrectly
     * attributed to the specified class.
     *
     */
    double precision(const int &class_id);

    /**
     * Calculate the average precision over all classes
     */
    double avg_precision();

    /**
     * Calculate the false discovery rate for the specified class.
     *
     * The false discovery rate is defined as the number of 
     * the number of instances incorrectly attributed to the specified class
     * divided by the number of 
     * correctly identified instances plus the number of instances incorrectly
     * attributed to the specified class.
     */
    double false_discovery_rate(const int &class_id);

    /**
     * Calculate the average precision over all classes
     */
    double avg_false_discovery_rate();

    /**
     * Calculate M, the generalization of AUC to multiple classes.
     *
     * M is defined as the average over all p(i|j), where
     * p(i|j) is the probability that an arbitrary instance of class j will
     * have a smaller estimation of belonging to class i than an arbitrary
     * instance of class i.
     */
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

    MultiROC(const MultiROC&) = delete;
    MultiROC& operator=(const MultiROC&) = delete;

    void calculate_m();
    void process();

    double ahat( const int &i, const int &j);
    double auc( std::vector<std::pair<double,int>> scores );

};

}   // end namespace stat

#endif   // STAT_MULTI_ROC_H
