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

#include <cmath>
#include <limits>
#include <set>

#include "noir/point.h"
#include "noir/noir_dimensions.h"

namespace noir {

/*
 * A Noir space is a Banach constructed from a Cartesian product of discrete
 * and continuous dimensional spaces. A Noir space can contain Nominal values,
 * Ordinal values, periodic Interval values and Real values.
 */
class NoirSpace {
 public:
    // The dimensions of this space
    NoirDimensions const * const dimensions;
    
    explicit NoirSpace(const NoirDimensions *dimensions);

    virtual ~NoirSpace();

    /*
     * Adds a nominal value to the set of values for the specified coordinate.
     */
    void add_nominal(const int &coordinate, int &nominal_value) {
        allowed_nominals[coordinate].insert(nominal_value);
    }

    /*
     * Retrieves the set of nominal values for the specified coordinate
     */
    const std::set<int>& get_nominals(const int &coordinate) const {
        return allowed_nominals[coordinate];
    }

    /*
     * Set lower and upper boundaries for the specified ordinal coordinate.
     */
    void set_ordinal_boundaries(const int &coordinate, double lower, 
                                                                double upper) {
        ordinal_boundaries[coordinate][0] = lower;
        ordinal_boundaries[coordinate][1] = upper;
    }

    /*
     * Retrieves the boundaries for the specified ordinal coordinate
     */
    void get_ordinal_boundaries(const int &coordinate, 
                                 double &lower, double &upper) const {
        lower = ordinal_boundaries[coordinate][0];
        upper = ordinal_boundaries[coordinate][1];
    }

    /*
     * Set lower and upper boundaries for the specified interval coordinate.
     */
    void set_interval_boundaries(const int &coordinate, 
                                 double lower, double upper) {
        interval_boundaries[coordinate][0] = lower;
        interval_boundaries[coordinate][1] = upper;
    }

    /*
     * Retrieves the boundaries for the specified interval coordinate
     */
    void get_interval_boundaries(const int &coordinate, 
                                 double &lower, double &upper) const {
        lower = interval_boundaries[coordinate][0];
        upper = interval_boundaries[coordinate][1];
    }

    /*
     * Set period for the specified interval coordinate.
     */
    void set_interval_period(const int &coordinate, const double period) {
        interval_periods[coordinate] = period;
    }

    /*
     * Retrieves the period for the specified interval coordinate
     */
    double get_interval_period(const int &coordinate) const {
        return interval_periods[coordinate];
    }

    /*
     * Set lower and upper boundaries for the specified real coordinate.
     */
    void set_real_boundaries(const int &coordinate, double lower, double upper){
        real_boundaries[coordinate][0] = lower;
        real_boundaries[coordinate][1] = upper;
    }

    /*
     * Retrieves the boundaries for the specified real coordinate
     */
    void get_real_boundaries(const int &coordinate, 
                             double &lower, double &upper) const {
        lower = real_boundaries[coordinate][0];
        upper = real_boundaries[coordinate][1];
    }

    /*
     * Determines whether or the not the specified point is contained within
     * the closure of this Noir space.
     */
    bool in_closure( const Point *point ) const;

    /*
     * returns the diameter of this space.
     */
    double get_diameter();

 private:
    double **ordinal_boundaries;
    double **interval_boundaries;
    double *interval_periods;
    double **real_boundaries;
    double diameter;

    std::set<int> *allowed_nominals;

    NoirSpace(const NoirSpace&) = delete;
    NoirSpace& operator=(const NoirSpace&) = delete;
};

}   // end namespace noir

#endif   // NOIR_NOIR_SPACE_H
