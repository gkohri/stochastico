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

#ifndef UTIL_FUNCTIONS_H
#define UTIL_FUNCTIONS_H

#include <sstream>
#include <string>
#include <typeinfo>

#include "util/misc.h"
#include "util/number_format_error.h"

namespace util {

/**
 * A simple function for tokenizing a string based upon a set of delimiters.
 * By default, any white-space character can be used a delimiter: " \t\f\v\n\r"
 * Note: The type "ContainerT" must be a container for the string type.
 */
template <class ContainerT>
inline void tokenize(const std::string& str, ContainerT& tokens, 
                     const std::string& delimiters = Delimiters::WHITE_SPACE ) {
    std::string::size_type pos, lastPos = 0;
    while (true) {
        pos = str.find_first_of(delimiters, lastPos);
        if (pos == std::string::npos) {
            pos = str.length();
            tokens.push_back( std::string(str.data()+lastPos, (pos-lastPos) ) );

            break;
        } else {
            tokens.push_back( std::string(str.data()+lastPos, (pos-lastPos) ));
        }
        lastPos = pos + 1;
    }
};


static std::stringstream numeric_interpreter__;
static std::stringstream converter__;

/**
 *  Converts a string to type Target
 */
template <typename Target> 
inline void to_numeric(std::string const &s, Target &value) {
    numeric_interpreter__.clear();

    numeric_interpreter__ << s;

    if ( (numeric_interpreter__ >> value).fail()  ||
        !(numeric_interpreter__ >> std::ws).eof()   ) {
            throw NumberFormatError( __FILE__, __LINE__, s.c_str(), 
                                        typeid(value).name());
    }
};

/**
 *   Converts a type to a string
 */
template<typename Source>
inline std::string to_string( Source arg ){
    converter__.clear();

    std::string result;

    converter__ << arg;
    converter__ >> result;

    return result;
}

/**
 * Trims white spaces surrounding strings
 */
inline std::string trim( const std::string &s )
{
    size_t last_non_white = s.find_last_not_of( Delimiters::WHITE_SPACE );
    if ( last_non_white == std::string::npos ) return std::string();

    size_t first_non_white = s.find_first_not_of( Delimiters::WHITE_SPACE );
    if ( first_non_white == std::string::npos ) first_non_white = 0;

    return s.substr( first_non_white, ( last_non_white - first_non_white + 1 ));
};

/**
 * Trims quotes surrounding strings
 */
inline std::string trim_quotes( const std::string &s )
{
    std::string::const_iterator bit = s.begin();
    std::string::const_iterator eit = s.end();
    if ( *bit == '"' && *(--eit) == '"' ){
        return std::string( ++bit, eit );
    } else {
        return s;
    }
};

}  // namespace util

#endif  // UTIL_FUNCTIONS_H
