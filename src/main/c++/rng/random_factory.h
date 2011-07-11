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

#ifndef RNG_RANDOM_FACTORY_H
#define RNG_RANDOM_FACTORY_H

#include <cstddef>
#include <mutex>

#include "rng/random.h"


namespace rng {

/**
 * A thread safe factory class for creating random number generators for
 * multiple threads running in parallel.  Calls to get_rng() return one of
 * 942 million possible random number generators each of which has a period
 * greater than 10^45. The random number generators are returned in sequence 
 * to ensure repeatability of trials.
 */
class RandomFactory {
 public:
    /**
     * The set_seed() method must be called before the first 
     * call to get_instance(), afterwards it  will have no effect. A second or
     * subsequent call to set_seed has not effect.
     */
    static void set_seed( const unsigned &useed);

    /**
     * Get an instance of the RandomFactory.
     */
    static RandomFactory* get_instance();

    /**
     * Get an new random number generator. The random number generators
     * returned by this call are not thread safe. Each thread should call this
     * method to get its own random number generator.
     *
     * The calling thread is responsible for handling the resources allocated
     * by this call.  When the random number generator is no longer needed, it
     * should be explicitly deleted.
     */
    Random* get_rng();

 private:

    static RandomFactory *instance;
    static unsigned seed;

    const int numIJseeds;
    const int numKLseeds;
    int currentIJ;
    int currentKL;
    int *ijSeeds;
    int *klSeeds;

    static std::mutex synchronize;

    RandomFactory();
    virtual ~RandomFactory();

    RandomFactory(const RandomFactory&);
    RandomFactory& operator=(const RandomFactory&);


    static RandomFactory* create_instance();
    static void schedule_for_destruction(void (*fun)());
    static void destroy();
};

}  // namespace rng

#endif  // END RNG_RANDOM_FACTORY_H
