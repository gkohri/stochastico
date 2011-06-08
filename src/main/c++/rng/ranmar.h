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

/*
 *   Universal random number generator proposed by Marsaglia and Zaman
 *   in report FSU-SCRI-87-50. Returns random numbers in the range
 *   [0,1]. It returns identical numbers on all machines with at least 
 *   24 bits in the mantissa of the floating point representation.
*/

#include "rng/random.h"
#include "util/invalid_argument_error.h"

namespace rng {

class Ranmar: public virtual Random {
 public:

    /*
     * Constructor
     *
     * The input values should be in the ranges:    
     *
     *               0<= IJ <= 31328
     *               0<= KL <= 30081
     *
     * To get the standard values in the Marsaglia-Zaman paper,
     * put IJ=1802, KL= 9373.
     *
     * Note: Each pair (IJ,KL) yields a unique, uncorrelated sequence of
     *       of random numbers.  Essentially, Ranmar is capable of
     *       producing 942,377,568 uncorrelated random sequences.
     *
    */
    explicit Ranmar(const int &ij = 1802, const int &kl = 9373):Random(),
                    LAG_P(607), LAG_Q(273) {
        int i, j, k, l, ii, jj, m;
        double s, t;

        const int LAG_PP1 = LAG_P + 1;

        u = new double[LAG_PP1];
        for (ii = 0; ii < LAG_PP1; ii++) {
            u[ii] = 0.0;
        }

        if (ij < 0 || ij > 31328)
            throw util::InvalidArgumentError(__FILE__,__LINE__,
                                                "ij is out of range!");

        if (kl < 0 || kl > 30081)
            throw util::InvalidArgumentError(__FILE__,__LINE__,
                                                "kl is out of range!");


        i = ((ij/177) % 177) + 2;
        j = (ij % 177) + 2;
        k = ((kl/169) % 178) + 1;
        l = kl % 169;

        for (ii = 1; ii < LAG_PP1; ii++) {
            s = 0.0;
            t = 0.5;
            for (jj = 1; jj < 25; jj++) {
                m = (((i*j) % 179)*k) % 179;
                i = j;
                j = k;
                k = m;
                l = (53*l+1) % 169;
                if (((l*m) % 64) >= 32) s = s + t;
                t = 0.5*t;
            }
            u[ii] = s;
        }

        c  =   362436.0/16777216.0;
        cd =  7654321.0/16777216.0;
        cm = 16777213.0/16777216.0;
        ip = LAG_P;
        iq = LAG_Q;
    };

    virtual ~Ranmar() {
        delete[] u;
    }

    /**
     * Generate the next random number uniformly in the range [0,1]
     */
    double next() {
        double uni;

        uni = u[ip] - u[iq];

        if (uni < 0.0) uni = uni + 1.0;
        u[ip] = uni;

        ip = ip - 1;
        if ( ip == 0 ) ip = LAG_P;

        iq = iq - 1;
        if ( iq == 0 ) iq = LAG_P;

        c = c - cd;
        if ( c < 0.0 ) c = c + cm;

        uni = uni - c;
        if ( uni < 0.0 ) uni = uni + 1.0;

        return( uni );
    }

    /**
     * Generate the next random integer uniformly in the range:
     *          [0,n-1] if n>0, or
     *          [n+1,0] if n<0
     */
    int next_int(const int &n) {
        return static_cast<int>( static_cast<double>(n)*next() );
    }

    /**
     * Generate the next random integer uniformly over the full 32 bit range
     */
    unsigned next_uint() {
        return static_cast<unsigned>( 4294967296.999999*next() );
    }

 private:
    //  static const int LAG_P   = 97;
    //  static const int LAG_Q   = 33;

    //static const int LAG_P = 607;
    //static const int LAG_Q = 273;
    const int LAG_P;
    const int LAG_Q;


    int ip; 
    int iq;
    double c;
    double cd;
    double cm;
    double *u;

    Ranmar(const Ranmar&) = delete;
    Ranmar& operator=(const Ranmar&) = delete;
};

}  // namespace rng

#endif   // END RNG_RANMAR_H
