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

#ifndef UTIL_OPTIONS_H
#define UTIL_OPTIONS_H

//
//   Purpose:  This option table is used for conveniently defining
//             options which can be given on the command line to the
//             current program.
//

#include <string>
#include <vector>

namespace util {

class Option {
 public:
        // A constant indicating that the option requires an argument
        static const bool VALUE_REQUIRED    = true;

        // A constant indicating that the option doe not require an argument
        static const bool NO_VALUE_REQUIRED = false;

        /*
         * This friend method reads in all the command line arguements
         */
        friend void get_command_line_options( int, char **, 
                                              std::vector<Option*>& );

        /*
         * This friend method prints out a usage message
         */
        friend void usage( char *, std::vector<Option*>& );

        Option( std::string op_name, std::string def_val, bool value_req ) : 
                    name( op_name ), value(""),
                    default_value( def_val ), value_required( value_req ) {}

        virtual ~Option() {}


        /*
         * retrieves the name of the option
         */
        std::string get_name() const {
            return name;
        }

        /*
         * retrieves the value of the option
         */
        std::string get_value() const {
            return value;
        }

        /*
         * retrieves the default value of the option
         */
        std::string get_default_value() const {
            return default_value;
        }

        /*
         * checks whether a value is required for this option
         */
        bool is_value_required() const {
            return value_required;
        }

 private:
        std::string  name;
        std::string  value;
        std::string  default_value;
        bool value_required;
};

}
#endif // UTIL_OPTIONS_H
