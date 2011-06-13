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

#ifndef NOIR_ORTHOPLEX_H
#define NOIR_ORTHOPLEX_H

#include "noir/noir_dimensions.h"
#include "noir/point.h"

namespace noir {

/*
 * An orthoplex is defined by a point and a radius
 */
class Orthoplex : public Point {
 public:

    Orthoplex(const NoirDimensions *dimensions, const double &radius); 

    virtual ~Orthoplex();

    /*
     * Retrieve the radius
     */
    double get_radius(){
        return radius;
    }

    /*
     * Determines whether or the not the specified point is contained within
     * the closure of this Orthoplex.
     */
    bool in_closure( const Point *point ) const;


 private:
    double radius;

    Orthoplex(const Point&) = delete;
    Orthoplex& operator=(const Orthoplex&) = delete;

};

}   // end namespace noir

#endif   // NOIR_ORTHOPLEX_H
