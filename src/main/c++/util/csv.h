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

#ifndef UTIL_CSV_H
#define UTIL_CSV_H

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "util/functions.h"
#include "util/io_error.h"
#include "util/misc.h"

namespace util {

/**
 * A simple class for reading CSV encoded files.
 *
 * Note: This class does not yet handle quoted fields
 */
class CSVReader {
 public:

    /*
     * Constructor.
     */
    explicit CSVReader(const std::string& filename):filename(filename),
                       delimiter(Delimiters::COMMA) {

        file = fopen( filename.c_str(), "r" );
        int errsv = errno;
        if ( file == NULL ) {
            std::string msg = "ERROR: " + std::string(strerror(errsv)) + " '" 
                + filename + "'";
            throw IOError(__FILE__, __LINE__, msg );
        }
    };

    /*
     * Destructor.
     */
    virtual ~CSVReader() {
        if ( fileno(file) != -1 ){
            fclose(file);
        }
    }

    /*
     * Set the field delimiter for use in reading this CSV file. The default
     * field delimiter is the comma.
     */
    void set_field_delimiter(const std::string& field_delimiter) {
        delimiter = field_delimiter;
    }

    /*
     * Get the field delimiter currently in use.
     */
    std::string get_field_delimiter() const {
        return delimiter;
    }

    /*
     * Reads the next line of the file and returns the fields in order in the
     * specified vector.
     */
    void next_line(std::vector<std::string>& fields) {
        if ( has_more_lines() ) {
            std::string rawData;

            char *line = NULL;
            size_t len = 0;
            ssize_t read;

            if ( (read = getline(&line,&len,file)) != -1 ){
                if ( line ) {
                    rawData = line;
                    tokenize<std::vector<std::string> >(rawData, fields, 
                                                         delimiter);
                    free(line);
                }
            }
        }
    }

    /*
     * Check if there are any more lines to read.
     */
    bool has_more_lines() {
        int next_char;
        next_char = fgetc(file);
        ungetc(next_char, file);

        return ( next_char == EOF ? false : true );
    }

 private:
    std::string filename;
    std::string delimiter;
    FILE *file;

    CSVReader(const CSVReader&) = delete;
    CSVReader& operator=(const CSVReader&) = delete;
};

}   // namespace util

#endif   // UTIL_CSV_H
