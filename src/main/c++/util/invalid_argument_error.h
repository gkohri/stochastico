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

#ifndef UTIL_INVALID_ARGUMENT_ERROR_H
#define UTIL_INVALID_ARGUMENT_ERROR_H

#include <cstdio>
#include <stdexcept>
#include <string>


namespace util {

/**
 * A wrapper class around the std::invalid_argument exception to provide the
 * filename and line number at which the exception occured.
 */

class InvalidArgumentError : public std::range_error {
 public:

    InvalidArgumentError( const char *file, const int &line, 
                              const std::string &msg): 
                                    std::range_error(msg) {

        snprintf(msg_,MSG_SIZE,"[%s:%d] %s\n", file,line, msg.c_str() );
    }

    virtual ~InvalidArgumentError() throw() {}

    const char* what() const throw() {
        return msg_;
    }

 private:
    static const int MSG_SIZE = 640;
    char msg_[MSG_SIZE];
};

}   // namespace util

#endif   // UTIL_INVALID_ARGUMENT_ERROR_H
