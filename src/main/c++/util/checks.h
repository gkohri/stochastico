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

#ifndef UTIL_CHECKS_H
#define UTIL_CHECKS_H

#ifdef PERFORM_CHECKS

#define check_equality(a,b) assert(((a) == (b)))
#define check_bounds(a,b) assert(((a) > 0) && ((a) < (b)))

#else

#define check_equality(a,b) (static_cast<void> (0))
#define check_bounds(a,b) (static_cast<void> (0))

#endif   // PERFORM_CHECKS

#endif   // UTIL_CHECKS_H
