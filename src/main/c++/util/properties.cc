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


#include <cerrno>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>

#include "util/properties.h"
#include "util/functions.h"
#include "util/invalid_input_error.h"
#include "util/io_error.h"

namespace util {

using std::string;
using std::map;

using util::Properties;
using util::trim;
using util::InvalidInputError;
using util::IOError;

void Properties::load( const string& props_filename ) {

    FILE *file = fopen( props_filename.c_str(), "r" );
    int errsv = errno;

    if ( file == NULL ) {
        string msg = "ERROR: " + string(strerror(errsv)) + " '" 
                                            + props_filename + "'";
        throw IOError(__FILE__, __LINE__, msg );
    }

    map<string,string>::iterator it;

    char    *line = NULL;
    size_t  len = 0;
    ssize_t read;

    while ( (read = getline(&line,&len,file)) != -1 ) {

            if ( !line ) continue;

            // Skip if this is a comment line
            if ( line[0] == '#' || line[0] == '!' ) {
                free(line);
                line = NULL;
                len = 0;
                continue;
            }

            string natural = line;

            // Skip if this is a blank line
            size_t first_non_white = natural.find_first_not_of( " \t\v\f\n\r" );

            if ( first_non_white == string::npos ) {
                free(line);
                line = NULL;
                len = 0;
                continue;
            }

            size_t sep = natural.find( '=' );

            if ( sep == string::npos ) {
                string msg = "Invalid file. Line contains no separator: " +
                                                natural;
                free(line);
                throw InvalidInputError( __FILE__, __LINE__, msg );
            }

            string proto_key = natural.substr( 0, sep );
            string key = trim( proto_key );

            if ( key.empty() ) {
                string msg = "Invalid file. Empty key! ";
                free(line);
                throw InvalidInputError( __FILE__, __LINE__, msg );
            }

            string value = trim( natural.substr( sep + 1, 
                                  ( natural.length() - sep + 1 ) ) );

            string unquoted_value = trim_quotes( value );

            set_property( key, unquoted_value );

            free(line);
            line = NULL;
            len = 0;
    }

    fclose(file);
};

}   // namespace util
