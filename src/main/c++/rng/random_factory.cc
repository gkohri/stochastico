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


#include "rng/random_factory.h"

#include <cstdlib>
#include <stdexcept>

#include "rng/random.h"
#include "rng/ranmar.h"
#include "rng/zran.h"

namespace rng {

using std::runtime_error;
using std::mutex;

RandomFactory* RandomFactory::instance = 0;
unsigned RandomFactory::rfSeed = 0;
mutex RandomFactory::rfMutex;

void RandomFactory::set_seed( const unsigned seed) {
    if ( RandomFactory::rfSeed == 0 ) {
        rfMutex.lock();
        if ( RandomFactory::rfSeed == 0 ) {
            RandomFactory::rfSeed = seed;
        }
        rfMutex.unlock();
    }
}


RandomFactory* RandomFactory::get_instance() {
    if ( RandomFactory::instance == 0 ){
        rfMutex.lock();
        if ( RandomFactory::instance == 0 ){
            RandomFactory::instance = create_instance();
            schedule_for_destruction( RandomFactory::destroy );
        }
        rfMutex.unlock(); 
    }
    return RandomFactory::instance;
}

RandomFactory* RandomFactory::create_instance() {
    return new RandomFactory();
}

void RandomFactory::destroy(){
    if ( RandomFactory::instance != 0 ){
        delete RandomFactory::instance;
        RandomFactory::instance = 0;
    }
}

void RandomFactory::schedule_for_destruction(void (*fun)()){
    if ( atexit( fun ) != 0  ) {
        throw runtime_error( "atexit failed for PRNG factory !");
    }
}

RandomFactory::RandomFactory():numIJseeds(31329), numKLseeds(30082),
                                         currentIJ(0), currentKL(0) {

    // Generate lists of all possible seeds for Ranmar

    ijSeeds = new int[numIJseeds];
    klSeeds = new int[numKLseeds];

    for ( int i = 0; i < numIJseeds; ++i ){
        ijSeeds[i] = i;
    }

    for ( int i = 0; i < numKLseeds; ++i ){
        klSeeds[i] = i;
    }

    // Shuffle the lists using Knuth's in-place shuffle algorithm

    if ( RandomFactory::rfSeed == 0 ) RandomFactory::rfSeed = 868051;
    Zran zran(RandomFactory::rfSeed);

    for ( int i = numIJseeds-1; i > 0; --i ){
        int k = zran.next_int(i);
        int temp = ijSeeds[i];
        ijSeeds[i] = ijSeeds[k];
        ijSeeds[k] = temp;
    }


    for ( int j = numKLseeds-1; j > 0; --j ){
        int k = zran.next_int(j);
        int temp = klSeeds[j]; 
        klSeeds[j] = klSeeds[k]; 
        klSeeds[k] = temp;
    }
}

RandomFactory::~RandomFactory() {
    delete[] ijSeeds;
    delete[] klSeeds;
}

Random* RandomFactory::get_rng(){
    rfMutex.lock();

    int ij = ijSeeds[currentIJ];
    int kl = klSeeds[currentKL];
    Random *rng = new Ranmar(ij, kl);

    if ( ++currentKL == numKLseeds ){
        currentKL = 0;
        if ( ++currentIJ == numIJseeds ){
            throw runtime_error( "Exhausted all possible parallel RNGs!");
        }
    }

    rfMutex.unlock();
    return rng;
}


}  // namespace rng
