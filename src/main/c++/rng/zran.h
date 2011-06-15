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

#ifndef RNG_ZRAN_H
#define RNG_ZRAN_H

#include "rng/random.h"

/**  The Zran random number generator class.

     Zran is a 32-bit random number generator proposed by 
     G. Marsaglia and A. Zaman, Comp. in Phys., vol. 8 (1994) 117.
     The period is 2**125 and it works on all 32-bit machines. 

*/

namespace rng {

class Zran : public virtual Random {
 public:

/// Generates the next random number in the sequence

    unsigned  next_uint() {
        int s;

        if (y > x + c) {
            s = y - (x + c);
            c = 0u;
        } else {
            s = (x + c) - y - 18;
            c = 1u;
        }
        x = y;
        y = z;
        return ((z = s) + (n = 69069u*n + 1013904243u));
    }

/** Generates the next random number in the sequence and 
    returns its double precision representation. */

    double next() {
        return ( static_cast<double>(next_uint())*(1.0/4294967295.0) );
    }


    int next_int(const int &n) {
        return static_cast<int>( static_cast<double>(n)*next() );
    }


/// Resets the random number generator

    void reset(unsigned  seed) {
        x = 521288629u;
        y = 362436069u;
        z = 1613801u;
        c = 1u;
        n = seed;
    }

/// default constructor

    Zran(): n(1131199209u), x(521288629u), y(362436069u), z(1613801u), c(1u) {}

/// normal constructor

    Zran( const unsigned  &nn,
          const unsigned  &xx = 521288629u, 
          const unsigned  &yy = 362436069u, 
          const unsigned  &zz = 1613801u   ):n(nn), x(xx), y(yy), z(zz), c(1u) {}

/// destructor

    virtual ~Zran() {}

 private:
    unsigned  n, x, y, z, c;
};

}   // namespace rng

#endif  // RNG_ZRAN_H
