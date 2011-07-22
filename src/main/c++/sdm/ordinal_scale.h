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

#ifndef ORDINAL_SCALE_H
#define ORDINAL_SCALE_H

#include <string>
#include <map>

namespace sdm {

/**
 * An Ordering specifies how the values of an ordinal scale should be ordered.
 */
class Ordering {
 public:
    /**
     *  Orders the labels according to their correlation with the first
     *  class. This type of ordering makes sense in the case where there
     *  are only two classes.
     */
    static Ordering const ClassCorrelation;

    /// Indicates a Lexicographic ordering.
    static Ordering const Lexicographic;

    /// Indicates a user-defined ordering.
    static Ordering const UserDefined;

    /**
     * Retrieves a C-style string containing the name of this ordering.
     */
    char const* c_str() const;

    /**
     * Compare two Orderings for equality.
     */
    friend bool operator==(Ordering const& a, Ordering const& b) {
        return a.v_ == b.v_;
    }

    /**
     * Compare two Orderings for inequality.
     */
    friend bool operator!= (Ordering const& a, Ordering const& b) {
        return a.v_ != b.v_;
    }

 private:
    enum Value {ClassCorrelation_, Lexicographic_, Numeric_, UserDefined_} v_;

    explicit Ordering ( Value v ) : v_(v) {}

    static char const* labels[];
};

/**
 * An Ordinal scale is a collection of values ordered according to some
 * specified ordering.
 */
class OrdinalScale {
 public:
    OrdinalScale() : value_map(), index_map(), 
                     ordering(Ordering::UserDefined) {}

    virtual ~OrdinalScale() {}

    /**
     * Retrieve the current ordering for this scale.
     */
    Ordering get_ordering() const {
        return ordering;
    }

    /**
     * Change the ordering of this scale.
     */
    void set_ordering( Ordering new_ordering) {
        ordering = new_ordering;
    }

    /**
     * After all the values have been marked, this method will re-evaluate
     * the order of all values using the current ordering.
     * 
     * After calling this method, the user should call transcribe to get
     * the new ordering.
     */
    void apply_ordering();

    /**
     * Returns the number of values for this scale.
     */
    size_t size() const {
        return value_map.size();
    }

    /**
     * Checks whether the specified value is known.
     */
    bool is_known( const std::string &value ) {
        if ( value_map.find( value ) != value_map.end() )  {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Transcribes the specified scale value into a number. If the value is
     * unknown the value -1.0 is returned.
     */
    double transcribe( const std::string &value ) {
        if ( is_known( value ) ){
            int index = value_map[value];
            return ordering_map[index];
        } else {
            return -1.0;
        }
    }

    /**
     * Transcribes the specified index value into a number. If the index 
     * value is unknown the value -1.0 is returned.
     */
    double transcribe( const double &value ) {
        int index = static_cast<int>(value);
        if ( ordering_map.find( index ) != ordering_map.end() )  {
            return ordering_map[index];
        } else {
            return -1.0;
        }
    }


    /**
     * Marks an instance of the specified value as having the specified color.
     * If the value is already known then its transcription
     * is returned.
     */
    double mark( std::string value, int color ) {
        if ( is_known( value ) ){
            int index = value_map[value];
            if ( color == 0 ) ++color_map[index];
            ++total_map[index];
            return transcribe( value );
        } else {
            int index = value_map.size();
            value_map[value] = index;
            index_map[index] = value;
            if ( color == 0) ++color_map[index];
            ++total_map[index];
            double rank = static_cast<double>(index);
            ordering_map[index] = rank;
            return rank;
        }
    }

    /**
     * Enters the specified value into the collection of values for this
     * scale.
     */
    void set_value( std::string value ) {
        if ( !is_known( value ) ){
            int index = value_map.size();
            value_map[value] = index;
            index_map[index] = value;
            double rank = static_cast<double>(index);
            ordering_map[index] = rank;
        }
    }


    /**
     * Retrieves the ordinal value associated with the specified index.
     * If the index is not currently in use an empty string is returned.
     * Since an empty string is also a valid value, the user should 
     * use the "is_known" method to check whether or not the index
     * is actually in use.
     */
    std::string ascribe( double index ) {
        if ( index_map.find( static_cast<int>(index) ) != index_map.end() ) {
            return index_map[index];
        } else {
            return "";
        }
    }

 private:
    std::map<std::string, int> value_map;
    std::map<int, std::string> index_map;
    std::map<int, double> ordering_map;
    std::map<int, double> color_map;
    std::map<int, double> total_map;

    Ordering ordering;
};

}   // end namespace sdm

#endif   // ORDINAL_SCALE_H
