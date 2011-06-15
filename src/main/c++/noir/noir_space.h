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

#ifndef NOIR_NOIR_SPACE_H
#define NOIR_NOIR_SPACE_H

#include "noir/norm.h"

namespace noir {

class Point;
/*
 * A Noir Space is Banach space formed by the Cartesian product of Nominal,
 * Ordinal, periodic Interval and Real sets, together with a suitable metric.
 */
class NoirSpace {
 public:
    NoirSpace( const int nominal_dimensions,
                    const int ordinal_dimensions,
                    const int interval_dimensions,
                    const int real_dimensions ):
                            nominal(nominal_dimensions),
                            ordinal(ordinal_dimensions),
                            interval(interval_dimensions),
                            real(real_dimensions) {}

    virtual ~NoirSpace() {}

    NoirSpace(const NoirSpace &that) : nominal(that.nominal),
                                                 ordinal(that.ordinal),
                                                 interval(that.interval),
                                                 real(that.real) {}

    NoirSpace& operator=(const NoirSpace &that){
        return *(new NoirSpace(that));
    }


    const int nominal;
    const int ordinal;
    const int interval;
    const int real;

    Norm norm;

};

}   // end namespace noir

#endif   // NOIR_NOIR_SPACE_H
