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

#ifndef SDM_NOIR_DIMENSIONS_H
#define SDM_NOIR_DIMENSIONS_H

#include <string>

namespace sdm {

/*
 * The Noir dimension of a Noir space specifies the number of Nominal,
 * Ordinal, periodic Interval and Real dimensions belonging to the Noir Space.
 */
class NoirDimensions {
 public:
    NoirDimensions( const int nominal_dimensions,
                    const int ordinal_dimensions,
                    const int interval_dimensions,
                    const int real_dimensions ):
                            nominal(nominal_dimensions),
                            ordinal(ordinal_dimensions),
                            interval(interval_dimensions),
                            real(real_dimensions) {}

    virtual ~NoirDimensions() {}

    const int nominal;
    const int ordinal;
    const int interval;
    const int real;
};

}   // end namespace SD

#endif   // SDM_NOIR_DIMENSIONS_H
