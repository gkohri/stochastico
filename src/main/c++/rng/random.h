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

#ifndef RNG_RANDOM_H
#define RNG_RANDOM_H

namespace rng {

/* 
    An interface for psuedo random number generators.
*/
class Random {
 public:

    /**
     * Generate the next random number uniformly in the range [0,1]
     */
    virtual double next(void) = 0;

    /**
     * Generate the next random number uniformly in the range [0,2^32]
     */
    virtual unsigned next_uint(void) = 0;

    /**
     * Generate the next random integer uniformly in the range:
     *          [0,n-1] if n>0, or
     *          [n+1,0] if n<0
     */
    virtual int next_int(const int &n) = 0;

    virtual ~Random() {}
};

}  // namespace rng

#endif  // END RNG_RANDOM_H
