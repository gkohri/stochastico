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

#include <sdm/ordinal_scale.h>


#include <map>


namespace sdm {

using std::map;
using std::string;

Ordering const Ordering::ClassCorrelation (Ordering::ClassCorrelation_);
Ordering const Ordering::Lexicographic (Ordering::Lexicographic_);
Ordering const Ordering::UserDefined (Ordering::UserDefined_);

char const* Ordering::labels[] = {"Class Correlation", "Lexicographic", 
                                  "Numeric", "User Defined"};

char const* Ordering::c_str() const {
    return labels[v_];
}

void OrdinalScale::apply_ordering() {
    if ( ordering == Ordering::Lexicographic ) {
        double norm = 1.0/static_cast<double>(value_map.size());
        map<string,int>::iterator mit;
        map<string,int>::iterator vme = value_map.end();
        double rank = 0.0;
        for ( mit = value_map.begin() ; mit != vme; ++mit ) {
            int index = (*mit).second;
            ordering_map[index] = (++rank)*norm;
        }
    } else if ( ordering == Ordering::ClassCorrelation ) {
        map<string,int>::iterator mit;
        map<string,int>::iterator vme = value_map.end();
        for ( mit = value_map.begin() ; mit != vme; ++mit ) {
            int index = (*mit).second;
            ordering_map[index] = color_map[index]/total_map[index];
        }
    } else if ( ordering == Ordering::UserDefined ) {
        double norm = 1.0/static_cast<double>(value_map.size());
        map<string,int>::iterator mit;
        map<string,int>::iterator vme = value_map.end();
        for ( mit = value_map.begin() ; mit != vme; ++mit ) {
            int index = (*mit).second;
            ordering_map[index] *= norm;
        }
    }
}

} // end namespace sdm
