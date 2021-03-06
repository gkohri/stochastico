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

#ifndef SDM_DATA_MANAGER_H
#define SDM_DATA_MANAGER_H

#include <set>
#include <string>
#include <vector>

#include "noir/noir_space.h"
#include "noir/orthotope.h"
#include "rng/random.h"
#include "sdm/data_store.h"
#include "sdm/model.h"
#include "sdm/nominal_scale.h"
#include "sdm/training_data.h"
#include "util/misc.h"
#include "util/properties.h"

namespace sdm {

/*
 * A class to manage the data.
 */
class DataManager {
 public:
    DataManager():trainingData(), testData(), trialData(), enclosure(0), 
                  folds(),
                  delimiter(util::Delimiters::COMMA), colors(), noirSpace(0),
                  skipLines(), nominalFields(), ordinalFields(), 
                  intervalFields(),
                  realFields(), nominalValues(), ordinalValues(),
                  numFields(2), idField(-1), colorField(1) {}

    virtual ~DataManager();

    /*
     * Initializes the data manager using the specified parameters
     */
    void init( const util::Properties &parameters );

    /*
     * Loads the training data from a file
     */
    void load_training_data( const std::string &filename );

    /*
     * Loads the test data from a file
     */
    void load_test_data( const std::string &filename );

    /*
     * Loads the trial data from a file
     */
    void load_trial_data( const std::string &filename );


    /*
     * Partitions the training data into the specified number of folds
     */
    void partition_training_data( const int &num_folds,
                                            rng::Random *rand = NULL );

    /*
     * If the training data has been partitioned, this returns the number
     * of folds, otherwise it returns 0.
     */
    size_t get_num_folds() const {
        return folds.size();
    }

    /*
     * Retrieve the specified partition
     */
    DataStore* get_partition( unsigned partition ) {
        if ( partition < folds.size() )
            return folds[partition];
        else
            return NULL;
    }

    /*
     * Retrieve the test data
     */
    DataStore* get_test_data() {
        return &testData;
    }

    /*
     * Retrieve the trial data
     */
    DataStore* get_trial_data() {
        return &trialData;
    }

    /*
     * Returns the number of colors (classes) found in the training data.
     */
    int get_num_colors() const {
        return colors.size();
    }

    /*
     * Returns the hyper-rectangle which encloses all the data.
     */
    const noir::Orthotope* get_enclosure() {
        return enclosure;
    }

    /*
     * Checks whether or not test data has been loaded by this DataManager
     */
    bool has_test_data(){
        return !testData.is_empty();
    }

    /*
     * Checks whether or not trial data has been loaded by this DataManager
     */
    bool has_trial_data(){
        return !trialData.is_empty();
    }

 private:
    DataStore trainingData;
    DataStore testData;
    DataStore trialData;
    noir::Orthotope *enclosure;
    std::vector<DataStore*> folds;
    std::string delimiter;
    NominalScale colors;
    noir::NoirSpace *noirSpace;
    std::set<int> skipLines;
    std::vector<int> nominalFields;
    std::vector<int> ordinalFields;
    std::vector<int> intervalFields;
    std::vector<int> realFields;
    std::vector<NominalScale*> nominalValues;
    std::vector<NominalScale*> ordinalValues;
    std::vector<double> intervalPeriods;
    size_t numFields;
    int idField;
    int colorField;

    void load_data( const std::string &filename, DataStore &dataStore );

    template<typename ValueType>
    inline int parse_single_value(const util::Properties &parameters,
                                  const std::string &name, ValueType &value);
    template<typename ValueType>
    inline int parse_field_index(const util::Properties &parameters,
                                  const std::string &name, ValueType &value);

    template<typename Container, typename ContentType>
    int parse_values( const util::Properties &parameters,
                      const std::string &name, 
                      Container &values );

    template<typename Container, typename ContentType>
    int parse_field_indecies( const util::Properties &parameters,
                              const std::string &name, 
                              Container &values );

    void read_ordinal_values(const util::Properties &parameters);
    void read_interval_periods(const util::Properties &parameters);
};

}   // namespace sdm

#endif   // SDM_DATA_MANAGER_H
