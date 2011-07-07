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

#ifndef SDM_SD_MACHINE_H
#define SDM_SD_MACHINE_H

#include <vector>
#include <map>

#include "sdm/data_store.h"
#include "sdm/model.h"
#include "sdm/discriminator.h"
#include "sdm/data_manager.h"
#include "rng/random.h"
#include "stat/multi_scorecard.h"
#include "util/properties.h"

namespace sdm {

/*
 * The Stochastic Discrimination Machine organizes the work to be done in
 * the learning and recognition phases.
 */
class SDMachine {
 public:
    explicit SDMachine() : discriminators(), uniform(0), numModels(100), 
                           numFolds(8), numAttempts(100), lowerFrac(0.0), 
                           upperFrac(0.1), enrichmentLevel(0.1) {}

    virtual ~SDMachine();

    /*
     * Initialize the SDMachine usint the specified parameters.
     */
    void init( util::Properties &parameters );

    /*
     * Learn the training data managed by the DataManager
     */
    void learn( DataManager &dataManager );

    /*
     * Prepare the specified Discrimator by adding data from the specified
     * DataManager while skipping the specified fold.
     */
    void ready_discriminator( Discriminator *dis, DataManager &dataManager, 
                              const int &skip_fold );

    /*
     * Process the trial data held by the data manager, if any.
     */
    void process_trial_data( DataManager &dataManager );

    /*
     * Enumerate the supported learning algorithms.
     */
    enum LearningAlgorithms {LeastCovered, RandomPoints};

 private:
    std::vector<Discriminator*> discriminators;
    std::vector<stat::MultiScorecard*> learning_results;
    rng::Random *uniform;
    util::Properties *sdmParameters;
    ModelTypes::Types modelTypes;
    int numModels;
    int numFolds;
    int numAttempts;
    double lowerFrac;
    double upperFrac;
    double enrichmentLevel;
    LearningAlgorithms learningAlgorithm;

    rng::Random* initialize_uniform_rng( const util::Properties &props );
    void clear_learning_results();
    void create_discriminators( DataManager &dataManager );
    void ready_discriminators( DataManager &dataManager, const int &part );
    void train_discriminators();

    stat::MultiScorecard* test( DataStore &testData );

    void process( DataStore &trialData );

    void folded_learning( DataManager &dataManager );
    void simple_learning( DataManager &dataManager );
    template<typename ValueType>
    inline void set_value(const std::string &name, ValueType &value);
    inline void set_parameter(const std::string &name, std::string &parameter);

};

}   // end namespace sdm

#endif   // SDM_SD_MACHINE_H
