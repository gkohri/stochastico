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

#ifndef RNG_RANMAR_H
#define RNG_RANMAR_H

#include <rng/random.h>

namespace rng {

/**
 *   Universal random number generator proposed by Marsaglia, Zaman and Tsang.
 *   Ranmar is a portable 24 bit, random 
 *   number generator that returns identical random numbers on all 
 *   machines with at least 24 bits in the mantissa of the floating
 *   point representation.  A detailed description can be found in:
 *   G. Marsaglia, A. Zaman and  W. Tsang, 
 *   "Toward a universal random number generator," Statistics & Probability 
 *   Letters, vol. 9, pp. 35-39 (1990).
 *   The present implementation follows
 *   F. James, "A review of pseudorandom number generators", 
 *   Comput. Phys. Commun., 60, pp. 329-344 (1990).
 *   
 *   This implementation was evaluated using version 3.31.0 of the 
 *   Dieharder suite of statistical tests for random number generators 
 *   and successfully passed each test.
 */
class Ranmar: public virtual Random {
 public:

    /**
     * Constructor
     *
     * Note: Each pair (seed1,seed2) yields a unique, uncorrelated sequence of
     *       of random numbers.  Given the allowed ranges, this admits
     *       942,377,568 uncorrelated random sequences.
     *
     * @param seed1 an integer in the range 0<= seed1 <= 31328. If seed1 is 
     *        outside this range it will be wrapped using modular arithmatic
     * @param seed2 an integer in the range 0<= seed2 <= 30081. If seed2 is 
     *        outside this range it will be wrapped using modular arithmatic
     *
     * @param fib an integer swith for choosing the Fibonacci lags. fib = 0
     *        uses Fibonacci lags of 97 and 33 yielding a period of
     *        2^144 (10^43). fib = 1 uses lags of 607 and 273 
     *        yielding a period of 2^654 (10^196).
     *   
     *
     */
    Ranmar(const int& seed1, const int& seed2, const int& fib = 0):Random() {

        if ( fib == 1 ) {
            LAG_P = 607;
            LAG_Q = 273;
        } else {
            LAG_P   = 97;
            LAG_Q   = 33;
        }

        int ij = seed1;
        int kl = seed2;

        const int LAG_PP1 = LAG_P + 1;

        u = new double[LAG_PP1];
        for (int ii = 0; ii < LAG_PP1; ii++) {
            u[ii] = 0.0;
        }

        if (ij < 0) {
            ij = -ij;
        }

        if (ij > 31328) {
            ij = ij % 31328;
        }

        if (kl < 0) {
            kl = -kl;
        }

        if (kl > 30081) {
            kl = kl % 30081;
        }

        int i = ((ij/177) % 177) + 2;
        int j = (ij % 177) + 2;
        int k = ((kl/169) % 178) + 1;
        int l = kl % 169;

        for (int ii = 1; ii < LAG_PP1; ++ii) {
            double s = 0.0;
            double t = 0.5;
            for (int jj = 0; jj < 24; ++jj) {
                int m = (((i*j) % 179)*k) % 179;
                i = j;
                j = k;
                k = m;
                l = (53*l+1) % 169;
                if (((l*m) % 64) >= 32) s += t;
                t *= 0.5;
            }
            u[ii] = s;
        }

        c  =   362436.0/16777216.0;
        cd = -7654321.0/16777216.0;
        cm = 16777213.0/16777216.0;
        ip = LAG_P;
        iq = LAG_Q;
    };

    virtual ~Ranmar() {
        delete[] u;
    }

    /**
     * Generate a random number uniformly in the range [0.0,1.0]
     */
    double next() {

        double uni = u[ip] - u[iq];

        if (uni < 0.0) uni += 1.0;
        u[ip] = uni;

        --ip;
        if ( ip == 0 ) ip = LAG_P;

        --iq;
        if ( iq == 0 ) iq = LAG_P;

        c += cd;
        if ( c < 0.0 ) c += cm;

        uni -= c;
        if ( uni < 0.0 ) uni += 1.0;

        return( uni );
    }

    /**
     * Generate the a random integer uniformly in the range:
     *          [0,n-1] if n>0, or
     *          [n+1,0] if n<0
     */
    int next_int(const int &n) {
        return static_cast<int>( static_cast<double>(n)*next() );
    }

    /**
     * Generate a random, unsigned integer uniformly in the range [0,2^32-1]
     */
    unsigned next_uint() {
        unsigned int r1 = (unsigned int) (next()*16777215.0);
        unsigned int r2 = (unsigned int) (next()*16777215.0);
        
        return ( r1 | (r2 << 24 ) );
    }

 private:
    int LAG_P;
    int LAG_Q;

    int ip; 
    int iq;
    double c;
    double cd;
    double cm;
    double *u;

    Ranmar(const Ranmar&);
    Ranmar& operator=(const Ranmar&);
};

}  // namespace rng

#endif   // END RNG_RANMAR_H
