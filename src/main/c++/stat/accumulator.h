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

#ifndef STAT_ACCUMULATOR_H
#define STAT_ACCUMULATOR_H

#include <cmath>
#include <cstdint>

namespace stat {

/**
 * A class for accumulating the mean and variance on-line using
 * Welford's algorithm. 
 *
 * This implementation trades speed for numerical stability.
 * The storage requirements are constant and do not depend upon the number
 * values accumulated.
 */
class Accumulator {
 public:
    Accumulator():cN(0), cSum(0), cMean(0), cM2(0) {}

    virtual ~Accumulator() {}

    /**
     * Add one new data value to the accumulator
     */
    inline void gather( double const &x ) {
        ++cN;
        double delta = x - cMean;
        cMean += delta/cN;
        cM2 += delta*(x - cMean);
        cSum += x;
    }

    /**
     * Add a range of data values to the accumulator
     */
    template <typename InputIterator>
    inline void gather(InputIterator begin, InputIterator end)
    {
        double l_n = 0.0;
        double l_sum = 0.0;
        double l_mean = 0.0;
        double l_m2 = 0.0;
        double delta = 0.0;
        for (InputIterator it = begin; it != end; ++it) {
            ++l_n;
            delta = *it - l_mean;
            l_mean += delta/l_n;
            l_m2 += delta*(*it - l_mean);
            l_sum += *it;
        }

        delta = l_mean - cMean;

        cMean = (cN*cMean + l_n*l_mean)/(cN + l_n);
    
        cM2 += l_m2 + delta*delta*((cN*l_n)/(cN+l_n));

        cN += l_n;

        cSum += l_sum;
    }


    /**
     * Returns the number of values collected thus far
     */
    inline double number() {return cN;}

    /**
     * Returns the sum of all values collected.
     */
    inline double sum() {return cSum;}

    /**
     * Returns the mean of all values collected.
     */
    inline double mean() {return cMean;}

    /**
     * Returns the standard error in the mean.
     */
    inline double standard_error() {return sqrt(cM2/((cN-1.0)*cN));}

    /**
     * Calculates the variance assuming the values collected
     * amount to all values that can be collected.
     * 
     * If the values collecte do not cover the entirety of the population,
     * this method will yield a biased sample variance.
     */
    inline double variance_population() {return cM2/cN;}

    /**
     * Calculates the variance assuming the values collected so far 
     * amount to a sub-set of all the values that can possibly be collected.
     * 
     * This is known as the unbiased sample variance.
     */
    inline double variance_sample() {return cM2/(cN-1.0);}

    /**
     * This is a convenience method as the deviation is the
     * square root of the variance.
     */
    inline double deviation_population() {return sqrt(cM2/cN);}

    /**
     * This is a convenience method as the deviation is the
     * square root of the variance.
     */
    inline double deviation_sample() {return sqrt(cM2/(cN-1.0));}

 private:
    double cN;
    double cSum;
    double cMean;
    double cM2;

    Accumulator(const Accumulator&) = delete;
    Accumulator& operator=(const Accumulator&) = delete;
};

}   // end namespace stat

#endif   // STAT_ACCUMULATOR_H
