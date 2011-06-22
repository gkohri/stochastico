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
 *
 */

#include "config.h"

#include <cstdlib>
#include <new>
#include <string>
#include <cmath>
#include <vector>

#include "sdm/sdmachine.h"
#include "sdm/data_manager.h"
#include "util/options.h"
#include "util/properties.h"
#include "util/functions.h"
#include "util/invalid_input_error.h"
#include "util/io_error.h"
#include "util/timer.h"

using std::string;
using std::vector;

using sdm::SDMachine;
using sdm::DataManager;
using util::Option;
using util::Properties;
using util::to_string;
using util::Timer;

/*
 * The main routine. Its purpose is to orchastrate the application.
 */
int main( int argc, char * argv[] ) {
    // Define the command line options

    Option version( "-version", "P", Option::NO_VALUE_REQUIRED );
    Option param_file( "-parameters", "", Option::VALUE_REQUIRED );

    vector<Option*> prog_options;
    prog_options.push_back( &version );
    prog_options.push_back( &param_file );

    // retrieve the command line options

    get_command_line_options( argc, argv, prog_options );

    if ( !version.get_value().empty() ) {
        fprintf(stderr,"%s version %s\n", APPNAME, VERSION);
        return 0;
    }

    if ( param_file.get_value().empty() ) {
        fprintf(stderr,"%s\n", "Parameter file not found!");
        usage( argv[0], prog_options );
    }

    Properties parameters;

    parameters.load( param_file.get_value() );

    // Read the data
    fprintf(stderr,"\n----%s----\n\n","reading data" );

    if ( !parameters.contains_property( string("Data::Training::Filename") ) ) {
        string msg =
            "Property \"Data::Training::Filename\" not found in file: " +
                    param_file.get_value();
        throw util::InvalidInputError( __FILE__, __LINE__, msg );
    }


    DataManager dataManager;

    dataManager.init( parameters );

    fprintf(stderr,"\n----%s----\n\n","loading the training data" );
    dataManager.load_training_data(
            parameters.get_property( string("Data::Training::Filename") ) );

    fprintf(stderr,"\n----%s----\n\n","loading the test data" );
    dataManager.load_test_data(
            parameters.get_property( string("Data::Testing::Filename") ) );

    // Initialize the stochastic discrimination machine
    fprintf(stderr,"----%s----\n\n","initializing SDM" );

    SDMachine sdm;

    sdm.init( parameters );

    // Learn the training data
    fprintf(stderr,"----%s----\n\n","learning" );

    double realTime;
    double cpuTime;
    Timer timer;

    sdm.learn( dataManager );

    timer.elapsed( realTime, cpuTime );

    fprintf(stderr,"%s %.4f s\n", "real time: ", realTime);
    fprintf(stderr,"%s %.4f s\n", "cpu time: ", cpuTime);
    fprintf(stderr,"%s %.4f\n", "speed up: ", cpuTime/realTime);

    fprintf(stderr,"\n----%s----\n\n","loading the trial data" );

    dataManager.load_trial_data(
            parameters.get_property( string("Data::Trial::Filename") ) );

    sdm.process_trial_data( dataManager );
}
