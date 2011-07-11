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


#include "util/options.h"

//==========================================================================

// The Options class implementation.

//==========================================================================

#include <string>
#include <cstdlib>
#include <vector>

#include <stdio.h>

#include "util/functions.h"

namespace util {

using std::exit;
using std::string;
using std::vector;

/**
 * A function to read in all the command line arguements
 */
void Option::process_all_options(int argc, char **argv, 
                                 vector<Option*> &table) {
    unsigned op = 0;
    unsigned this_op = 0;
    int found= 0;
    int c = 0;

    for (int arg = 1; arg < argc; arg++ ){

        found = 0;
        for ( op = 0 ; op < table.size(); op++)
        {
            c = 0;
            while ( table[op]->get_name()[c] == argv[arg][c] )
            {
                if (argv[arg][++c] == '\0')
                {
                    found++;
                    this_op = op;
                    break;
                }
            }
        }

        if( found > 1 ) 
        {
            fprintf(stderr,"\n\t%s%s\n","Ambiguous argument:  ", argv[arg]);
            usage( argv[0], table );
        } else if (found == 1) {
            if ( table[this_op]->is_value_required() ){
                if ( arg+1 == argc ) {
                    fprintf(stderr,"\n\t%s%s\n","Argument required! ", 
                            argv[arg]);
                    usage( argv[0], table );
                }
                table[this_op]->set_value(argv[arg+1]);
                arg++ ;
            } else {
                table[this_op]->value = table[this_op]->default_value;
            }
        } else {
            fprintf(stderr,"\n\t%s%s\n","Invalid argument:  ", argv[arg]);
            usage( argv[0], table );
        }

    }

};


/**
 * A function to output a usage message.
 *
 */
void Option::usage( char *program, vector<Option*> &options ) {
    fprintf(stderr,"\n\t%s%s%s\n","Usage:  ", program," [option [value] ]"); 
    fprintf(stderr,"\n\n\t%s\n\n","where 'option' is one of:");

    for ( unsigned op = 0 ; op < options.size(); op++ )
    {
        if ( options[op]->value_required ){
            fprintf(stderr,"\t\t%s\t\t%s\n", options[op]->get_name().c_str(), 
                    "  Argument"); 
        } else {
            fprintf(stderr,"\t\t%s\t\t%s\n", options[op]->get_name().c_str(), 
                    "  No Argument"); 
        }
    }
    fprintf(stderr,"\n\t%s\n", "Option names may be abreviated provided the abreviation is unique ");

    exit( EXIT_FAILURE );
};

}  // namespace util
