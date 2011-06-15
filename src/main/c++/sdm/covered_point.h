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

#ifndef SDM_COVERED_POINT_H
#define SDM_COVERED_POINT_H

#include <functional>

#include "noir/noir_space.h"
#include "sdm/data_point.h"

namespace sdm {

/*
 * A wrapper around a Point which tracks the Point's coverage during the
 * learning stage.
 */
class CoveredPoint {
 public:
    CoveredPoint(const DataPoint *p): point(p), coverage(0.0) {}

    virtual ~CoveredPoint(){}

    const DataPoint* get_data_point() const {
        return point;
    }

    int get_id() const {
        return point->get_id();
    }

    int get_color() const {
        return point->get_color();
    }

    double get_coverage() const {
        return coverage;
    }

    void clear_coverage() {
        coverage = 0.0;
    }

    void increment_coverage( const double inc ) {
        coverage += inc;
    }

    void increment_coverage() {
        ++coverage;
    }

    const noir::NoirSpace* get_noir_space() const {
        return point->noirSpace;
    }

    int const & get_nominal_coordinate( const int &coordinate ) const {
        return point->get_nominal_coordinate(coordinate);
    }

    int const * get_nominal_coordinates() const {
        return point->get_nominal_coordinates();
    }

    double const & get_ordinal_coordinate( const int &coordinate ) const {
        return point->get_ordinal_coordinate(coordinate);
    }

    double const * get_ordinal_coordinates() const {
        return point->get_ordinal_coordinates();
    }

    double const & get_interval_coordinate( const int &coordinate ) const {
        return point->get_interval_coordinate(coordinate);
    }

    double const * get_interval_coordinates() const {
        return point->get_interval_coordinates();
    }

    double const & get_real_coordinate( const int &coordinate ) const {
        return point->get_real_coordinate(coordinate);
    }

    double const * get_real_coordinates() const {
        return point->get_real_coordinates();
    }

    struct CompareCoverage :
             public std::binary_function< CoveredPoint*, CoveredPoint*, bool> {
        bool operator()(const CoveredPoint* x, const CoveredPoint* y) const {
            return ( x->get_coverage() < y->get_coverage() ? true : false );
        }
    };

 private:
    const DataPoint *point;
    double coverage;

    CoveredPoint(const CoveredPoint&) = delete;
    CoveredPoint& operator=(const CoveredPoint&) = delete;
};

}   // end namespace sdm

#endif   // SDM_COVERED_POINT_H
