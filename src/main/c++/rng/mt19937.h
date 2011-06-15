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

#ifndef RNG_MT_19937_H
#define RNG_MT_19937_H

#include <math.h>
#include <limits.h>
#include <assert.h>

namespace rng {

/** The Mersenne Twister random number generator.

        This is a C++ class to hold Matsumoto's Mersenne Twister random number
        generator.

Comments from Matsumoto's original program:

   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT 
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

typedef unsigned ulong;

class MTwist: public virtual Random {
 public:

//  Constructor

    explicit MTwist(ulong seed = 4357): N(624),
                                        M(397),
                                        MATRIX_A(0x9908b0dfUL),
                                        UPPER_MASK(0x80000000UL),
                                        LOWER_MASK(0x7fffffffUL) {
        mti = N+1;

        mt    = new ulong[N];
        mag01 = new ulong[2];

        mag01[0] = 0x0UL;
        mag01[1] = MATRIX_A;

        init(seed);
    }

//  Destructor

    ~MTwist() {
        delete[] mt;
        delete[] mag01;
    }


//  Generate the next random integer uniformly in the closed interval [0,2^32-1].

    ulong next_uint() {
        ulong y;

        // generate N words at one time

        if (mti >= N) {
            int kk;

            // if init() has not been called,
            // a default initial seed is used

            if (mti == N+1) init(5489UL);

            for (kk = 0; kk < N-M; kk++) {
                y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
                mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
            }

            for (; kk < N-1; kk++) {
                y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
                mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
            }

            y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
            mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

            mti = 0;
        }

        // Tempering

        y = mt[mti++];
        y ^=  (y >> 11);
        y ^=  (y << 7)  & 0x9d2c5680UL;
        y ^=  (y << 15) & 0xefc60000UL;
        y ^=  (y >> 18);

        return y;
    }

/*
 Generate the next  random integer uniformly in the closed interval 
    [0,n-1] if n > 0 and
    [n+1,0] if n < 0.
*/

    int next_int(const int &n) {
        return static_cast<int>( static_cast<double>(n)*next() );
    }

// Generate the next random number in the closed interval [0,1].

    double next() {
        return ( static_cast<double>(next_uint())*(1.0/4294967295.0) );
    }

// Generate the next random number in the open interval (0,1)

    double next_open() {
        return ( (next() + 0.5)*(1.0/4294967296.0) );
    }


// Matsumoto and Nishimura's initialization routine from 2002/1/26
// See Knuth TAOCP Vol2. 3rd Ed. P.106 for the multiplier.

    void init(ulong seed = 4357) {
        mt[0]= seed & 0xffffffffUL;
        for (mti = 1; mti < N; mti++) {
            mt[mti] = (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
            mt[mti] &= 0xffffffffUL;
        }
    }

// Initialize by an array with array-length.  init_key is the array for
// initializing keys and key_length is its length

    void init_by_array(ulong *init_key, int key_length) {
        int i, j, k;

        init(19650218UL);

        i = 1;
        j = 0;
        k = (N > key_length ? N : key_length);

        for ( ; k; k-- ) {
            mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
                                               + init_key[j] + j;  // non linear
            mt[i] &= 0xffffffffUL;       // for WORDSIZE > 32 machines
            i++;
            j++;
            if ( i >= N ) {
                mt[0] = mt[N-1];
                i = 1;
            }
            if (j >= key_length) j=0;
        }

        for (k = N-1; k; k--) {
            mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL)) - i;
            // non linear
            mt[i] &= 0xffffffffUL;      // for WORDSIZE > 32 machines
            i++;
            if ( i >= N ) {
                mt[0] = mt[N-1];
                i = 1;
            }
        }

        // MSB is 1; assuring non-zero initial array
        mt[0] = 0x80000000UL;
    }


// File an array with random numbers

    void fill(double* begin, double* end, double a = 0.0, double b = 1.0) {
        double diff = b - a;

        while (begin < end) *begin++ = a + diff*next();
    }

    void fill(ulong *begin, ulong *end, ulong a = 0, ulong b = UINT_MAX) {
        assert(b >= a);

        if (b == a) {
            while ( begin < end ) *begin++ = a;
        } else {
            ulong diff =  b - a;

            int bshift = 32 - (static_cast<int>(ceil(
                                log(static_cast<double>(diff))/log(2.0))));

            while ( begin < end ) {
                ulong rnum = next_uint() >> bshift;
                if ( rnum <= diff) *begin++ = a + rnum;
            }
        }
    }

 private:
    const int N;               // length of state vector
    const int M;               // M-parameter
    const ulong MATRIX_A;     // constant vector
    const ulong UPPER_MASK;   // most significant w-r bits
    const ulong LOWER_MASK;   // least significant r bits

    int       mti;         // mti==N+1 means mt[N] is not initialized
    ulong *mt;            // the array for the state vector
    ulong *mag01;         // mag01 $ [x] = x * MATRIX_A$  for x=0,1

    MTwist(const MTwist&) = delete;
    MTwist& operator=(const MTwist&) = delete;

};

}  // namespace rng

#endif  // RNG_MT_19937_H
