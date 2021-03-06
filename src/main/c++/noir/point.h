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

#ifndef NOIR_POINT_H
#define NOIR_POINT_H

#include "noir/noir_space.h"

namespace noir {

/*
 * A point in a Noir space can have Nominal, Ordinal, periodic Interval and
 * real coordinates.
 */
class Point {
 public:
    noir::NoirSpace const * const noirSpace;

    Point(const NoirSpace *noirSpace); 

    virtual ~Point();


    /*
     * Retrieve the value of the specified nominal coordinate.
     */
    int & get_nominal_coordinate( const int &coordinate ) const {
        return nominals[coordinate];
    }

    /*
     * Set the specified nominal coordinate to the specified value.
     */
    void set_nominal_coordinate( const int& coordinate, 
                                 const int& value ){
        nominals[coordinate] = value;
    }

    /*
     * Get a pointer to this point's array of nominal coordinates.
     */
    int const * get_nominal_coordinates() const {
        return nominals;
    }

    /*
     * Retrieve the value of the specified ordinal coordinate.
     */
    double const & get_ordinal_coordinate( const int &coordinate ) const {
        return ordinals[coordinate];
    }

    /*
     * Set the specified ordinal coordinate to the specified value.
     */
    void set_ordinal_coordinate( const int& coordinate, const double& value ){
        ordinals[coordinate] = value;
    }

    /*
     * Get a pointer to this point's array of ordinal coordinates.
     */
    double const * get_ordinal_coordinates() const {
        return ordinals;
    }

    /*
     * Retrieve the value of the specified interval coordinate.
     */
    double const & get_interval_coordinate( const int &coordinate ) const {
        return intervals[coordinate];
    }

    /*
     * Set the specified interval coordinate to the specified value.
     */
    void set_interval_coordinate( const int& coordinate, const double& value ){
        intervals[coordinate] = value;
    }

    /*
     * Get a pointer to this point's array of interval coordinates.
     */
    double const * get_interval_coordinates() const {
        return intervals;
    }

    /*
     * Retrieve the value of the specified real coordinate.
     */
    double const & get_real_coordinate( const int &coordinate ) const {
        return reals[coordinate];
    }

    /*
     * Set the specified real coordinate to the specified value.
     */
    void set_real_coordinate( const int& coordinate, const double& value ){
        reals[coordinate] = value;
    }

    /*
     * Get a pointer to this point's array of real coordinates.
     */
    double const * get_real_coordinates() const {
        return reals;
    }


 private:
    int         *nominals;
    double      *ordinals;
    double      *intervals;
    double      *reals;

    Point(const Point&) = delete;
    Point& operator=(const Point&) = delete;

};

}   // end namespace noir

#endif   // NOIR_POINT_H
