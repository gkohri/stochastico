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

namespace stat {

/*
 * Receiver Operating Characteristic
 */
class ROC {
 public:
    ROC():tp(0), tn(0), fp(0), fn(0) {}

    virtual ~ROC() {}

    void record_true_positive()  { ++tp; }
    void record_true_negative()  { ++tn; }
    void record_false_positive() { ++fp; }
    void record_false_negative() { ++fn; }

    double true_positive()  { return  static_cast<double>(tp); }
    double true_negative()  { return  static_cast<double>(tp); }
    double false_positive() { return  static_cast<double>(tp); }
    double false_negative() { return  static_cast<double>(tp); }

    double sensitivity(){
        int64_t denom = tp + fn;
        if ( denom > 0 )
            return static_cast<double>(tp)/static_cast<double>(denom);
        else
            return 0.0;
    }

    double recall(){
        return sensitivity();
    }

    double false_positive_rate(){
        int64_t denom = fp + tn;
        if ( denom > 0 )
            return static_cast<double>(fp)/static_cast<double>(denom);
        else
            return 0.0;
    }

    double fall_out(){
        return false_positive_rate();
    }

    double accuracy(){
        int64_t denom = tp + tn + fp + fn;
        if ( denom > 0 )
            return static_cast<double>(tp + tn)/static_cast<double>(denom);
        else
            return 0.0;
    }

    double error_rate(){
        int64_t denom = tp + tn + fp + fn;
        if ( denom > 0ll )
            return static_cast<double>(fp + fn)/static_cast<double>(denom);
        else
            return 0.0;
    }

    double specificity(){
        int64_t denom = fp + tn;
        if ( denom > 0 )
            return static_cast<double>(tn)/static_cast<double>(denom);
        else
            return 0.0;
    }

    double precision(){
        int64_t denom = tp + fp;
        if ( denom > 0 )
            return static_cast<double>(tp)/static_cast<double>(denom);
        else
            return 0.0;
    }

    double negative_predictive_value(){
        int64_t denom = tn + fn;
        if ( denom > 0 )
            return static_cast<double>(tn)/static_cast<double>(tn + fn);
        else
            return 0.0;
        
    }

    double false_discovery_rate(){
        int64_t denom = fp + tp;
        if ( denom > 0 )
            return static_cast<double>(fp)/static_cast<double>(denom);
        else
            return 0.0;
    }

    double f1_score() {
        double denom = precision() + recall();
        if ( denom > 0 )
            return ( (2.0*precision()*recall() )/denom );
        else
            return 0.0;
    }

    double auc() {
        if ( (tp + fn) > 0 && (tn + fp) > 0 ) {
            double dtp =  static_cast<double>(tp);
            double dtn =  static_cast<double>(tn);
            double dfn =  static_cast<double>(fn);
            double dfp =  static_cast<double>(fp);
            return ( (dtp*dtn - dtp*dfp/2.0 + dfn*dtn/2.0)/
                                         ((dtp + dfn)*(dtn + dfp)) );
        } else {
            return 0.0;
        }
    }

    /*
     * Matthews correlation coefficient
     */
    double mcc() {

        double dtp =  static_cast<double>(tp);
        double dtn =  static_cast<double>(tn);
        double dfn =  static_cast<double>(fn);
        double dfp =  static_cast<double>(fp);

        double denom = (dtp + dfp)*(dtp + dfn)*(dtn + dfp)*(dtn + dfn);

        if ( denom > 1.0e-6 ) {
            return (dtp*dtn - dfp*dfn)/sqrt(denom);
        } else {
            return 0.0;
        }
    }

    void clear() {
        tp = 0;
        tn = 0;
        fp = 0;
        fn = 0;
    }

 private:
    int64_t tp;
    int64_t tn;
    int64_t fp;
    int64_t fn;
};

}   // end namespace stat

#endif   // STAT_ROC_H
