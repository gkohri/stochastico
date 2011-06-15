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

#ifndef NOIR_BALL_H
#define NOIR_BALL_H

#include <set>

#include "noir/noir_space.h"
#include "noir/point.h"


namespace noir {

/*
 * An Ball in Noir Space is defined by a point, a radius and the metric used
 * to define the space.
 */
class Ball : public ClosedSpace {
 public:
    noir::NoirSpace const * const noirSpace;

    Ball(const NoirSpace *noirSpace, const Point *center, 
         const double &radius); 

    virtual ~Ball();

    /*
     * Retrieve the center
     */
    const Point* get_center() const {
        return center;
    }

    /*
     * Retrieve the radius
     */
    double get_radius(){
        return radius;
    }

    /*
     * Determines whether or the not the specified point is contained within
     * the closure of this ball.
     */
    bool in_closure( const Point *point ) const;

    /*
     * Adds a nominal value to the set of values for the specified coordinate.
     */
    void add_nominal(const int &coordinate, const int &nominal_value) {
        allowed_nominals[coordinate].insert(nominal_value);
    }

    /*
     * Retrieves the set of nominal values for the specified coordinate
     */
    const std::set<int>& get_nominals(const int &coordinate) const {
        return allowed_nominals[coordinate];
    }

 private:
    const Point *center;
    double radius;
    std::set<int> *allowed_nominals;

    Ball(const Point&) = delete;
    Ball& operator=(const Ball&) = delete;

};

}   // end namespace noir

#endif   // NOIR_BALL_H
