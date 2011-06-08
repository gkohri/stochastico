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

#ifndef SDM_DATA_STORE_H
#define SDM_DATA_STORE_H

/*
 * A convenience class used to store data for training or testing purposes.
 * 
 */

#include <vector>

#include "sdm/point.h"

namespace sdm {

class DataStore: public std::vector<Point*> {
 public:
    DataStore() : std::vector<Point*>() {}
    virtual ~DataStore() {}

    /*
     * Adds a data point to the data store
     */
    inline void add( Point *p ) {
        push_back( p );
    }

    /*
     * Deletes a data point from the data store
     */
    inline void remove( const Point *p ) {
        std::vector<Point*>::iterator vpit;
        for ( vpit = begin(); vpit != end(); ++vpit){
            if ( *vpit == p ){
                erase( vpit );
                break;
            }
        }
    }

    /*
     * Checks whether or not the specified point is contained in the 
     * DataStore
     */
    inline bool contains( const Point *p ) {
        bool found = false;
        std::vector<Point*>::iterator vpit;
        for ( vpit = begin(); vpit != end(); ++vpit){
            if ( *vpit == p ){
                found = true;
                break;
            }
        }
        return found;
    }

    /*
     * Checks whether or not this DataStore contains any data points.
     */
    inline bool is_empty() const {
        return empty();
    }

 private:

    DataStore(const DataStore&) = delete;
    DataStore& operator=(const DataStore&) = delete;

};

}   // end namespace sdm

#endif   // SDM_DATA_STORE_H
