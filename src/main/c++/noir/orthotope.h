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

#ifndef NOIR_ORTHOTOPE_H
#define NOIR_ORTHOTOPE_H

#include <cmath>
#include <limits>
#include <set>

#include "noir/point.h"
#include "noir/noir_space.h"

namespace noir {

/*
 * A Orthotope is a Cartesian product of two or more intervals. In Noir space
 * these intervals are very general, consiting of Nominal, Ordinal, periodic
 * and real values.
 * 
 */
class Orthotope {
 public:
    // The space in which this orthotope lives
    NoirSpace const * const noirSpace;
    
    explicit Orthotope(const NoirSpace *noirSpace);

    virtual ~Orthotope();

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

 private:
    double **ordinal_boundaries;
    double **interval_boundaries;
    double **real_boundaries;

    std::set<int> *allowed_nominals;

    Orthotope(const Orthotope&) = delete;
    Orthotope& operator=(const Orthotope&) = delete;
};

}   // end namespace noir

#endif   // NOIR_ORTHOTOPE_H
