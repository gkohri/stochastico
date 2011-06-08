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

#ifndef UTIL_PROPERTIES_H
#define UTIL_PROPERTIES_H

#include <string>
#include <stdexcept>
#include <map>

namespace util {

/*
 * The properties class holds pairs of the type <name,value>, where both 
 * "name" and "value" are strings.
 */
class Properties {
 public:
    Properties() {}

    virtual ~Properties() {}

    /*
     * Returns the value associated with the specified name or an empty string
     * if the name is not known.
     * 
     * NOTE: A name could have as its value an empty string. To check if
     *       the name is known to this Properties instance use the 
     *       "contains_property" method.
     */
    std::string get_property(const std::string& name) const {

        std::map<std::string, std::string>::const_iterator it = props.find(name);
        if (it != props.end())
            return it->second;
        else
            return std::string();
    }

    /*
    * Set a name - value pair. If the name already exists, the existing value
    * is overwritten by the specified value.
    */
    void set_property(const std::string& name, const std::string& value) {

        std::map<std::string, std::string>::iterator it = props.find(name);
        if (it != props.end()) {
            props.erase(it);
        }

        props[name] = value;
    }

    /*
    * Checks whether or not the specified name is known to this Properties
    * instance.
    */
    bool contains_property(const std::string& name) const {
        std::map<std::string, std::string>::const_iterator it = props.find(name);

        if (it != props.end())
            return true;
        else
            return false;
    }

    /*
    * Loads properties from the specified file.
    * 
    * The file format consits of lines of the form
    *            name = value
    * This cannot extend accross multiple lines.
    * Lines starting with "#" or "!" are ignored as are blank lines. Use quotes
    * "" to enclose blanks in either the value. 
    */
    void load(const std::string& props_filename);

 private:
    std::map<std::string, std::string> props;

    Properties(const Properties&) = delete;
    Properties& operator=(const Properties&) = delete;

};

}  // namespace util

#endif  // UTIL_OPTION_H
