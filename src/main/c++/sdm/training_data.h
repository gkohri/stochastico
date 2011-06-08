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

#ifndef SDM_TRAINING_DATA_H
#define SDM_TRAINING_DATA_H

#include <map>
#include <set>

#include "sdm/covered_point.h"
#include "sdm/data_store.h"
#include "rng/random.h"

namespace sdm {

/*
 * Training Data is used during the learning stage.
 */
class TrainingData{
public:

    TrainingData ( const int &principal_color = 0): data(), pcData(), 
                   pcOrderedData(),
                   principalColor( principal_color ),numPrincipalColor(0),
                   numOtherColor(0),rand(NULL) {}

    virtual ~TrainingData ();

    /*
     * Set a pointer to a random number generator.
     */
    void set_random( rng::Random *random ){
        rand = random;
    }

    /*
     * Retrieve the number of data points in the training data
     */
    inline size_t size() const {
        return data.size();
    }

    //
    // Iterators for iterating over the data.
    //
    typedef std::vector<CoveredPoint *>::iterator iterator;
    typedef std::vector<CoveredPoint *>::const_iterator const_iterator;

    /*
     * Retrieve an iterator starting at the beginning of the data
     */
    inline iterator begin() {
        return data.begin();
    }

    /*
     * Retrieve an iterator to the end of the data
     */
    inline iterator end() {
        return data.end();
    }

    /*
     * Retrieve a constant iterator starting at the beginning of the data
     */
    inline const_iterator begin() const {
        return data.begin();
    }

    /*
     * Retrieve a constant iterator to the end of the data
     */
    inline const_iterator end() const {
        return data.end();
    }

    /*
     * Add a covered point to the training data
     */
    void add( CoveredPoint *const p );

    /*
     * Get the number data points belonging to the principal color (class)
     */
    int get_num_principal_color() const {
        return numPrincipalColor;
    }

    /*
     * Get the number of data points belonging to the other colors (classes)
     */
    int get_num_other_color() const {
        return numOtherColor;
    }

    /*
     * Retrieve the number of data points
     */
    int get_num_data_points() const {
        return static_cast<int>(data.size());
    }

    
    /*
     * Get the least covered data point in this container
     */
    CoveredPoint* get_least_covered();

    /*
     * Get the rank-th least covered data point in this container
     */
    CoveredPoint* get_least_covered(int rank);

    /*
     * Get a random data point from this container
     */
    CoveredPoint* get_random_point();

    /*
     * Create a list of nearest neighbors to each point
     */
    void find_nn();

    /*
     * Get the nearest neighbor point to the specified point
     */
    CoveredPoint* get_nn(CoveredPoint *cp);

    /*
     * Sort the points in order of their coverge.
     */
    void reorder();

    /*
     * Removes all points and clears all registers and counters.
     */
    void clear();

private:
    std::vector<CoveredPoint *> data;
    std::vector<CoveredPoint *> pcData;
    std::multiset<CoveredPoint *, CoveredPoint::CompareCoverage> pcOrderedData;
    std::map<CoveredPoint *,CoveredPoint *> nn;
    int principalColor;
    int numPrincipalColor;
    int numOtherColor;
    rng::Random *rand;

    TrainingData(const TrainingData&) = delete;
    TrainingData& operator=(const TrainingData&) = delete;

};

}   // end namespace SD

#endif  // SDM_TRAINING_DATA_H
