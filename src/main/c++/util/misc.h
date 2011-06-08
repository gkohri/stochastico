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

#ifndef UTIL_MISC_H
#define UTIL_MISC_H

/**
 *
 */

#include <util/misc.h>

namespace util {

/*
 * Common delimiters for use with CSV and Property files
 */
struct Delimiters{
    static const std::string COLON;
    static const std::string COMMA;
    static const std::string EQUALS;
    static const std::string PIPE;
    static const std::string QUOTE;
    static const std::string SEMI_COLON;
    static const std::string SPACE;
    static const std::string TAB;
    static const std::string WHITE_SPACE;
};

}  // namespace util

#endif  // UTIL_MISC_H
