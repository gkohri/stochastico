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

#ifndef SDM_DATA_POINT_H
#define SDM_DATA_POINT_H

#include "noir/noir_space.h"
#include "noir/point.h"

namespace sdm {

/*
 * A point in a Noir space can have Nominal, Ordinal, periodic Interval and
 * real coordinates.
 */
class DataPoint : public noir::Point{
 public:
    DataPoint( const int& id, const int& color, 
               const noir::NoirSpace *space ) : 
               noir::Point(space), id(id), color(color) {}

    virtual ~DataPoint() {}


    /*
     * Retrieve this data point's id.
     */
    int get_id() const {
        return id;
    }

    /*
     * Retrieve this data point's color (class).
     */
    int get_color() const {
        return color;
    }

 private:
    int id;
    int color;

    DataPoint(const DataPoint&) = delete;
    DataPoint& operator=(const DataPoint&) = delete;

};

}   // end namespace sdm

#endif   // SDM_DATA_POINT_H
