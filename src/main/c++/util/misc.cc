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


#include <string>

#include <util/misc.h>

namespace util {

using std::string;

const string Delimiters::COLON       = ":";
const string Delimiters::COMMA       = ",";
const string Delimiters::EQUALS      = "=";
const string Delimiters::PIPE        = "|";
const string Delimiters::QUOTE       = "\"";
const string Delimiters::SEMI_COLON  = ";";
const string Delimiters::SPACE       = " ";
const string Delimiters::TAB         = "\t";
const string Delimiters::WHITE_SPACE = " \t\v\f\n\r";

}
