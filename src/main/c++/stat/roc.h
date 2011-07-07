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

#ifndef STAT_ROC_H
#define STAT_ROC_H

#include <cmath>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

namespace stat {

/**
 * The ROC (Receiver Operating Charteristic) class holds and analysizes the 
 * outcomes from a binary classification task. By convention, the two classes
 * are generically designated "positive" and "negative". The result of
 * classifying an arbitrary instance of either class is a numerical value
 * known as a "score".
 * The scores are real numbers, whereby larger scores are associated with
 * the positive class and smaller scores are assocatiated with the negative
 * class. The scores may have an interpretation as a probability, or they may
 * be unbounded, or they may be binary (e.g, {0, 1}), thereby indidcating
 * absolute certaintity on the side of the classifier.
 * 
 * ROC can compute the optimal threshold for dividing scores
 * into positive and negative classes. Using the optimal threshold the
 * standard ROC (Reciever Operating Characteristic) properties are determined.
 *
 * Throughout the documentation the following notation is used:
 *
 *     TP = True Positive - the number of positive instances whose score is
 *          above the threshold
 *     TN = True Negative - the number of negative instances whose score is
 *          below the threshold
 *     FP = False Positive - the number of negative instances whose score is
 *          above the threshold
 *     FN = False Negative - the number of positve instances whose score is
 *          below the threshold
 *
 *     NP = Number of Positive instances
 *     NN = Number of Negative instances
 *     NT = Number of Total instances
 */
class ROC {
 public:
    ROC() : all(), modified(0), tp(0), 
                  tn(0), fp(0), fn(0), n(0), num_positive(0), num_negative(0),
                  threshold(-std::numeric_limits<double>::max()),
                  auc_(0) {}


    virtual ~ROC() {}

    /**
     * Records the score of a positive instance.
     *
     * @param score the score for a positive instance.
     */
    void record_positive( const double &score ) {
        all.push_back( std::pair<double,int>(score, 1) );
        ++num_positive;
        modified = true;
    }

    /**
     * Records the score of a negative instance.
     *
     * @param score the score for a negative instance.
     */
    void record_negative( const double &score ) {
        all.push_back( std::pair<double,int>(score, 0) );
        ++num_negative;
        modified = true;
    }

    /**
     * Erases all previously recorded scores and resets all counters.
     */
    void clear() {
        all.clear();
        modified = false;
        tp = 0.0;
        fp = 0.0;
        num_positive = 0.0;
        num_negative = 0.0;
        threshold = -std::numeric_limits<double>::max();
    }

    /**
     * Determine the optimal threshold for separating the positive and
     * and negative examples.  The optimial threshold maximizes the
     * classifier's accuracy.
     */
    double optimal_threshold() {
        if ( modified ) optimize_roc();
        return threshold;
    }

    /**
     * Determines the accuracy of the classifier using the optimal
     * threshold for separating the positive and negative examples.
     */
    double accuracy(){
        if ( modified ) optimize_roc();
        return ((tp + tn)/n);
    }

    /**
     * Determines the error rate of the classifier using the optimal
     * threshold for separating the positive and negative examples.
     */
    double error_rate(){
        if ( modified ) optimize_roc();
        return ((fp + fn)/n);
    }

    /**
     * The specificity is defined as: TN/NN
     */
    double specificity() {
        if ( modified ) optimize_roc();
        return (tn/num_negative);
    }

    /**
     * The sensitivity is defined as: TP/NP
     * 
     * The sensitivity is als known as: "recall" or "true positive rate".
     */
    double sensitivity() {
        if ( modified ) optimize_roc();
        return (tp/num_positive);
    }

    /**
     * The false positive rate is defined as: FP/NN
     * 
     * The false positive rate is als known as: "fall-out" 
     */
    double false_positive_rate() {
        if ( modified ) optimize_roc();
        return ( fp/num_negative );
    }

    /**
     * The precision is defined as: TP/(TP+FP)
     * 
     * The precision is als known as: "positive predictive value" 
     */
    double precision() {
        if ( modified ) optimize_roc();
        return ( tp/(tp + fp) );
    }

    /**
     * The negative predictive value is defined as: TN/(TN+FN)
     * 
     */
    double negative_predictive_value(){
        if ( modified ) optimize_roc();
        return ( tn/(tn + fn) );
    }

    /**
     * The false discovery rate value is defined as: FP/(FP+TP)
     * 
     */
    double false_discovery_rate(){
        if ( modified ) optimize_roc();
        return ( fp/ (fp + tp) );
    }

    /**
     * The f1 score is defined as: 
     *      2*precision*recall/(precision + recall)
     * 
     */
    double f1_score() {
        if ( modified ) optimize_roc();
        double precision_ = precision();
        double recall_ = sensitivity();
        return ( (2.0*precision_*recall_ )/(precision_+recall_) );
    }

    /**
     * Matthews correlation coefficient
     */
    double mcc() {
        if ( modified ) optimize_roc();
        double denom = (tp + fp)*(tp + fn)*(tn + fp)*(tn + fn);

        return (tp*tn - fp*fn)/sqrt(denom);
    }


    /**
     * Calculates the area under the curve (AUC).
     * 
     * NOTE: In contrast to the other ROC properties, the AUC is 
     * independent of the threshold used to separate the classes.
     */
    double auc() {
        if ( modified ) optimize_roc();
        return auc_;
    }


 private:
    std::vector<std::pair<double,int>> all;
    bool modified;
    double tp;
    double tn;
    double fp;
    double fn;
    double n;
    double num_positive;
    double num_negative;
    double threshold;
    double auc_;

    ROC(const ROC&) = delete;
    ROC& operator=(const ROC&) = delete;

    void optimize_roc();

};

}   // end namespace stat

#endif   // STAT_ROC_H
