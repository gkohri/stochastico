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

#ifndef NOIR_NORM_H
#define NOIR_NORM_H

namespace noir {

class Point;

/*
 * An L1 norm for a Noir Space.
 */
struct Norm {
    /*
     * Calculate the L1 norm for the vector pointing to the specified point
     */
    double operator()(const Point* x) const;

    /*
     * Calculate distance between the specified points using the L1 norm.
     */
    double operator()(const Point* x, const Point* y) const;
};

}   // end namespace noir

#endif   // NOIR_NORM_H
