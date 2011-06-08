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

#ifndef UTIL_NUMBER_FORMAT_ERROR_H
#define UTIL_NUMBER_FORMAT_ERROR_H

#include <cxxabi.h>

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <typeinfo>

namespace util {

/*
* Thrown when an error occurs while attempting to convert a string to a
* number.
*/

class NumberFormatError : public std::range_error {
 public:
    NumberFormatError(const char *file, const int  &line,
                          const char *number, const char *type_name ):
                          std::range_error(" "),
                          file(file),
                          line(&line),
                          number(number),
                          type_name(type_name) {}

    virtual ~NumberFormatError() throw() {}

    const char* what() const throw() {
        int status;
        char *real_name =  __cxxabiv1::__cxa_demangle(type_name,
                                                    NULL, NULL, &status);

        std::string filename = file;
        size_t separator = filename.find_last_of("/\\");
        if ( separator != filename.npos ) {
            filename = filename.substr(separator+1);
        }
 
        char *msg_ = new char[MSG_SIZE];
        snprintf(msg_,MSG_SIZE,"[%s:%d] %s %s %s\n",filename.c_str(),
                        *line,number, " is not of type: ", real_name );
        free(real_name);
        return msg_;
    }

 private:
    static const int MSG_SIZE = 640;
    const char *file;
    const int  *line;
    const char *number;
    const char *type_name;
};



}   // namespace util

#endif  // UTIL_NUMBER_FORMAT_ERROR_H
