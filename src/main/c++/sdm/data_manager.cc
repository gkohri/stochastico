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


#include "sdm/data_manager.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include <set>
#include <string>
#include <vector>

#include "noir/noir_space.h"
#include "noir/orthotope.h"
#include "rng/random.h"
#include "sdm/data_point.h"
#include "util/functions.h"
#include "util/csv.h"
#include "util/invalid_input_error.h"
#include "util/properties.h"

namespace sdm {

using std::set;
using std::string;
using std::vector;
using std::numeric_limits;

using noir::NoirSpace;
using noir::Orthotope;
using rng::Random;
using util::CSVReader;
using util::to_numeric;
using util::tokenize;
using util::to_string;
using util::trim;
using util::Properties;

DataManager::~DataManager(){
    if ( folds.size() > 1 ) {
        for ( unsigned f = 0; f < folds.size(); f++ ) {
            delete folds[f];
        }
    }
    for ( unsigned n = 0; n < nominalValues.size(); n++ ) {
        delete nominalValues[n];
    }
    delete enclosure;
    delete noirSpace;
}

void DataManager::init( const Properties &parameters ) {
    string delimiter_param( "Data::Fields::Deliminator" );
    if ( parameters.contains_property(delimiter_param) ) {
        delimiter = parameters.get_property( delimiter_param );
    }


    parse_single_value(parameters, "Data::Fields::NumberOf", numFields );
    parse_field_index(parameters, "Data::Fields::ID", idField );
    parse_field_index(parameters, "Data::Fields::Class", colorField );

    vector<int>::iterator vit;

    parse_values<set<int>,int>(parameters, "Data::Lines::Skip", skipLines );

    parse_values<vector<int>,int>(parameters, "Data::Fields::Real", 
                                  realFields );

    //In the input file the indicies start with 1, but in C++ they start with 0
    for ( vit = realFields.begin(); vit != realFields.end(); ++vit ) {
        --(*vit);
    }

    parse_values<vector<int>,int>(parameters, "Data::Fields::Interval", 
                                  intervalFields );

    read_interval_periods(parameters);

    //In the input file the indicies start with 1, but in C++ they start with 0
    for ( vit = intervalFields.begin(); vit != intervalFields.end(); ++vit ) {
        --(*vit);
    }


    parse_values<vector<int>,int>(parameters, "Data::Fields::Ordinal", 
                                  ordinalFields );

    read_ordinal_values(parameters);

    //In the input file the indicies start with 1, but in C++ they start with 0
    for ( vit = ordinalFields.begin(); vit != ordinalFields.end(); ++vit ) {
        --(*vit);
    }



    parse_values<vector<int>,int>(parameters, "Data::Fields::Nominal", 
                                  nominalFields );

    //In the data file the indicies start with 1, but in C++ they start with 0
    for ( vit = nominalFields.begin(); vit != nominalFields.end(); ++vit ) {
        --(*vit);
    }

}

template<typename ValueType>
int DataManager::parse_field_index( const Properties &parameters,
                                    const string &name, ValueType &value ) {
    int info = parse_single_value( parameters, name, value );
    if ( info == 0) --value;  // Field numbering in input file starts with 1, 
                              // but C arrays start with 0
    return info;
}

template<typename ValueType>
int DataManager::parse_single_value( const Properties &parameters,
                                    const string &name, ValueType &value ) {
    int status = 1;
    if ( parameters.contains_property(name) ) {
        string value_str = parameters.get_property( name );
        if (!value_str.empty() && value_str.compare( "none" ) != 0) {
            to_numeric( value_str, value );
            status = 0;
        }
    }

    return status;
}

template<typename Container, typename ContentType>
int DataManager::parse_values( const Properties &parameters,
                               const string &name, 
                               Container &values ) {
    if ( !parameters.contains_property(name) ) return 1;

    string value_str = parameters.get_property( name );
    if ( value_str.empty() || value_str.compare( "none" ) == 0) {
        return 1;
    }

    vector<string> parts;
    tokenize( value_str, parts, "," );

    for (unsigned p = 0; p < parts.size(); ++p) {
        size_t dash_index = parts[p].find( '-' );
        if ( dash_index == string::npos ) {
            ContentType value;
            to_numeric( parts[p], value);
            values.insert(values.end(), value );
        } else {
            string start_str = parts[p].substr(0,dash_index);
            string end_str = parts[p].substr(dash_index+1);

            ContentType start;
            to_numeric(start_str, start);
            ContentType end;
            to_numeric(end_str, end);
            if ( end < start ) {
                throw util::InvalidInputError(__FILE__,__LINE__,
                                        "End is less than start!"); 
            }
            for (ContentType i = start; i != end+1; ++i) {
                values.insert(values.end(), i );
            }
        }
    }
    
    return 0;
}

void DataManager::read_ordinal_values(const Properties &parameters) {
    for (unsigned of = 0; of < ordinalFields.size(); ++of ){
        int fieldID = ordinalFields[of];
        string value_prop = "Data::Fields::Ordinal::" + to_string(fieldID);
        if ( parameters.contains_property(value_prop) ) {
            string value_str = parameters.get_property(value_prop);

            vector<string> values;
            tokenize( value_str, values, "," );

            NominalScale *ns = new NominalScale();

            for( unsigned v = 0; v < values.size(); ++v ) {
                ns->mark( trim(values[v]) );
            }

            ordinalValues.push_back(ns);
        } else {
            string msg = "'" + value_prop + "' not found in parameters file!";
            throw util::InvalidInputError(__FILE__,__LINE__, msg );
        }
    }
}

void DataManager::read_interval_periods(const Properties &parameters) {
    for (unsigned i = 0; i < intervalFields.size(); ++i ){
        int fieldID = intervalFields[i];
        string value_prop = "Data::Fields::Period::" + to_string(fieldID);
        if ( parameters.contains_property(value_prop) ) {
            string value_str = parameters.get_property(value_prop);

            double period;
            to_numeric(value_str, period);

            intervalPeriods.push_back(period);
        } else {
            string msg = "'" + value_prop + "' not found in parameters file!";
            throw util::InvalidInputError(__FILE__,__LINE__, msg );
        }
    }
}

void DataManager::load_training_data( const string &filename ) {
    load_data( filename, trainingData );
}

void DataManager::load_test_data( const string &filename ) {
    if ( !filename.empty() ) load_data( filename, testData );
}

void DataManager::load_trial_data( const string &filename ) {
    if ( !filename.empty() ) load_data( filename, trialData );
}


void DataManager::load_data( const string &filename, DataStore &dataStore ) {

    CSVReader csvReader( filename );

    csvReader.set_field_delimiter( delimiter );

    vector<string> fields;

    int nominal_dimensions = nominalFields.size();
    int ordinal_dimensions = ordinalFields.size();
    int interval_dimensions = intervalFields.size();
    int real_dimensions = realFields.size();

    for ( int n = 0; n < nominal_dimensions; n++ ) {
        NominalScale *ns = new NominalScale();
        nominalValues.push_back(ns);
    }

    double **real_min_max = new double*[real_dimensions];
    for ( int r = 0; r < real_dimensions; r++ ) {
        real_min_max[r] = new double[2];
        real_min_max[r][0] =  numeric_limits<double>::max();
        real_min_max[r][1] = -numeric_limits<double>::max();
    }

    noirSpace = new NoirSpace( nominal_dimensions, ordinal_dimensions,
                               interval_dimensions, real_dimensions );

    int id = 0;
    double value = 0.0;
    int ivalue = 0;
    int line = 1;
    while ( csvReader.has_more_lines() ) {
        csvReader.next_line( fields );

        if ( skipLines.find(line) != skipLines.end() ) {
            ++line;
            fields.clear();
            continue;
        }

        if ( fields.size() != numFields ) {
            throw util::InvalidInputError(__FILE__,__LINE__,
                    "Expected " + to_string(numFields) +
                     " fields, but found " + to_string(fields.size()) +
                     " fields at line: " + to_string(line) +
                                    ". Wrong file?" );
        }

        int color = colors.transcribe( fields[colorField] );
        if ( color == -1 ) {
            color = colors.mark( fields[colorField] );
        }

        if ( idField > -1 ) {
            to_numeric( fields[idField], id );
        } else {
            id++;
        }

        DataPoint *point = new DataPoint( id, color, noirSpace);

        // Get the nominal valued features
        for ( int n = 0; n < nominal_dimensions; ++n ) {
            string value_str = trim( fields[nominalFields[n]] );

            // Treat missing nominal values as any other nominal
            // values rather than assigning them a maker like  NaN

            ivalue = nominalValues[n]->mark( value_str );

            point->set_nominal_coordinate( n, ivalue );
        }

        // Get the ordinal valued features
        for ( int o = 0; o < ordinal_dimensions; ++o ) {
            string value_str = trim( fields[ordinalFields[o]] );

            if ( value_str.empty()             ||
                 value_str.compare( "?" ) == 0 ||
                 value_str.compare( "*" ) == 0    ) {
                ordinalValues[o]->mark( value_str );
            }

            int value = ordinalValues[o]->transcribe( value_str );

            point->set_ordinal_coordinate( o, static_cast<double>(value) );
        }

        // Get the interval valued features
        for ( int i = 0; i < interval_dimensions; ++i ) {
            string value_str = fields[intervalFields[i]];
            if ( value_str.empty()             ||
                 value_str.compare( "?" ) == 0 ||
                 value_str.compare( "*" ) == 0    ) {
                value = numeric_limits<double>::quiet_NaN();
            } else {
                to_numeric( value_str, value );
            }

            // normalize all periods to run between 0 and 1
            while ( value < 0.0 ) {
                value += intervalPeriods[i];
            }
            value = fmod(value, intervalPeriods[i]);
            value /= intervalPeriods[i];

            point->set_interval_coordinate( i, value );
        }

        // Get the real valued features
        for ( int r = 0; r < real_dimensions; ++r ) {
            string value_str = fields[realFields[r]];
            if ( value_str.empty()             ||
                 value_str.compare( "?" ) == 0 ||
                 value_str.compare( "*" ) == 0    ) {
                value = numeric_limits<double>::quiet_NaN();
            } else {
                to_numeric( value_str, value );
            }
            point->set_real_coordinate( r, value );
            if ( value < real_min_max[r][0] ) real_min_max[r][0] = value;
            if ( value > real_min_max[r][1] ) real_min_max[r][1] = value;
        }

        dataStore.add( point );

        ++line;
        fields.clear();
    }

    double lambda = 1.01;
    double ilambda = 0.99;
    if ( enclosure == 0 ) {
        enclosure = new Orthotope( noirSpace );

        for ( int n = 0; n < noirSpace->nominal; n++ ) {
            int max_nominal = nominalValues[n]->size();
            for ( int nn = 0; nn < max_nominal; nn++ ) {
                enclosure->add_nominal(n, nn);
            }
        }

        for ( int o = 0; o < noirSpace->ordinal; o++ ) {
            //int max = ordinalValues[o]->size() + 1;
            //double dmax = static_cast<double>(max);
            enclosure->set_ordinal_boundaries(o, 0.0, 1.0 );
        }

        for ( int i = 0; i < noirSpace->interval; i++ ) {
            enclosure->set_interval_boundaries(i, 0.0, 1.0 );
        }


        for ( int r = 0; r < noirSpace->real; r++ ) {
            double min =  real_min_max[r][0];
            double max =  real_min_max[r][1];

            if ( min > 0.0 ) {
                min *= ilambda;
            } else {
                min *= lambda;
            }
            real_min_max[r][0] = min;

            if ( max > 0.0 ) {
                min *= lambda;
            } else {
                max *= ilambda;
            }
            real_min_max[r][1] = max;

            enclosure->set_real_boundaries(r, 0.0, 1.0 );
        }
    }

    // Normalize the data. The interval dimensions are already normalized.

    DataStore::iterator dit;
    for (dit = dataStore.begin(); dit != dataStore.end(); ++dit) {
        DataPoint *dataPoint = *dit;
        for ( int r = 0; r < noirSpace->real; r++ ) {
            double min =  real_min_max[r][0];
            double max =  real_min_max[r][1];
            double rc = dataPoint->get_real_coordinate(r) - min;
            rc /= (max - min);
            dataPoint ->set_real_coordinate(r, rc);
        }
        for ( int o = 0; o < noirSpace->ordinal; o++ ) {
            double oc = dataPoint->get_ordinal_coordinate(o);
            oc /= static_cast<double>(ordinalValues[o]->size());
            dataPoint ->set_ordinal_coordinate(o, oc);
        }
    }


    // clean up our temporary storage
    for ( int c = 0; c < noirSpace->real; c++ ) {
        delete[] real_min_max[c];
    }
    delete[] real_min_max;
};

void DataManager::partition_training_data( const int &num_folds,
                                           Random *rand ) {
    if ( num_folds < 2 ) {
        folds.push_back( &trainingData );
        return;
    }

    for ( int f = 0; f < num_folds; f++ ) {
        folds.push_back( new DataStore() );
    }

    int num_colors =  colors.size();

    vector<vector<int>> color(num_colors);

    for ( size_t td = 0; td < trainingData.size(); ++td ) {
        color[trainingData[td]->get_color()].push_back(td);
    }

    for ( int c = 0; c < num_colors ; ++c ) {
        int num_data = color[c].size();
        for ( int cd = 0; cd < num_data ; ++cd ) {
            int fold = cd % num_folds;
            folds[fold]->add( trainingData[color[c].at(cd)] );
        }
    }
}

}  // namespace sdm
