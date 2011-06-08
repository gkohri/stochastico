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

#ifndef NOMINAL_SCALE_H
#define NOMINAL_SCALE_H

#include <string>
#include <map>

namespace sdm {

/*
 * A Nominal scale is a collection of labels. In this class they are mapped
 * to integers for faster algorithmic processing.
 */
class NominalScale {
 public:
    NominalScale() : label_map(), index_map() {}

    virtual ~NominalScale() {}

    /*
     * Returns the number of labels for this scale.
     */
    size_t size() const {
        return label_map.size();
    }

    /*
     * Checks wether the specified label is known.
     */
    bool is_known( const std::string &label ) {
        if ( label_map.find( label ) != label_map.end() )  {
            return true;
        } else {
            return false;
        }
    }

    /*
     * Checks wether the specified index is in use.
     */
    bool is_known( const int &index ) {
        if ( index > -1 && index < static_cast<int>(size()) ) {
            return true;
        } else {
            return false;
        }
    }


    /*
     * Transcribes the specified label into an integer. If the label is
     * unknown the value -1 is returned.
     */
    int transcribe( const std::string &label ) {
        if ( is_known( label ) ){
            return label_map[label];
        } else {
            return -1;
        }
    }

    /*
     * Enters the specified label into the collection of labels for this
     * scale. If the label is already known then its integer transcription
     * is returned.
     */
    int mark( std::string label ) {
        if ( is_known( label ) ){
            return transcribe( label );
        } else {
            int index = label_map.size();
            label_map[label] = index;
            index_map[index] = label;
            return index;
        }
    }

    /*
     * Retrieves the label associated with the specified index.
     * If the index is not currently in use an empty string is returned.
     * Since an empty string is also a valid label, the user should 
     * use the "is_known" method to check whether or not the index
     * is actually in use.
     */
    std::string ascribe( int index ) {
        if ( index_map.find( index ) != index_map.end() ) {
            return index_map[index];
        } else {
            return "";
        }
    }

 private:
    std::map<std::string,int> label_map;
    std::map<int,std::string> index_map;
};

}   // end namespace sdm

#endif   // NOMINAL_SCALE_H
