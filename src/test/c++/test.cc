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
 *
 */

#include <new>
#include <string>
#include <limits>

#include <fenv.h>
#include <math.h>

#include <rng/random.h>
#include <rng/ranmar.h>
#include <rng/mt19937.h>
#include <rng/zran.h>
#include <util/timer.h>
#include <util/functions.h>

using rng::Random;
using rng::Ranmar;
using rng::MTwist;
using rng::Zran;
using util::Timer;
using util::to_numeric;

const int N = 10000000;
double genrand();
void sgenrand(unsigned);

void test_ranmar()
{
	Random *random = new Ranmar();
	double avg = 0.0;
	double avgs = 0.0;
	double dev = 0.0;

	for (int i=0;i<N;i++)
	{
        double rand = random->next();
		avg  += rand;
		avgs += (rand*rand);
	}
	avg  /= ((double) N);
	avgs /= ((double) N);
    dev = sqrt( avgs - avg*avg );

    double avg_diff = fabs(avg - 0.5);
    double dev_diff = fabs(dev - 1.0/sqrt(12.0));

    if ( ( avg_diff > dev/sqrt(static_cast<double>(N)) ) ||
         ( dev_diff > 1.0/sqrt(static_cast<double>(N)) )   ) {
        fprintf(stdout,"Test Ranmar:  [failed]\n");
    } else {
        fprintf(stdout,"Test Ranmar:  [passed]\n");
    }
}

void test_mtwist()
{
	MTwist *mtwist = new MTwist();
	double avg = 0.0;
	double avgs = 0.0;
	double dev = 0.0;

	for (int i=0;i<N;i++)
	{
        double rand = mtwist->next();
		avg  += rand;
		avgs += (rand*rand);
	}
	avg  /= ((double) N);
	avgs /= ((double) N);
    dev = sqrt( avgs - avg*avg );

    double avg_diff = fabs(avg - 0.5);
    double dev_diff = fabs(dev - 1.0/sqrt(12.0));

    if ( ( avg_diff > dev/sqrt(static_cast<double>(N)) ) ||
         ( dev_diff > 1.0/sqrt(static_cast<double>(N)) )   ) {
        fprintf(stdout,"Test MTwist:  [failed]\n");
    } else {
        fprintf(stdout,"Test MTwist:  [passed]\n");
    }
}

void test_zran()
{
	Random *random = new Zran();
	double avg = 0.0;
	double avgs = 0.0;
	double dev = 0.0;

	for (int i=0;i<N;i++)
	{
        double rand = random->next();
		avg  += rand;
		avgs += (rand*rand);
	}
	avg  /= ((double) N);
	avgs /= ((double) N);
    dev = sqrt( avgs - avg*avg );

    double avg_diff = fabs(avg - 0.5);
    double dev_diff = fabs(dev - 1.0/sqrt(12.0));

    if ( ( avg_diff > dev/sqrt(static_cast<double>(N)) ) ||
         ( dev_diff > 1.0/sqrt(static_cast<double>(N)) )   ) {
        fprintf(stdout,"Test Zran:  [failed]\n");
    } else {
        fprintf(stdout,"Test Zran:  [passed]\n");
    }
}

void test_to_numeric() {

    double doubleC; 
    to_numeric("3.141", doubleC);
    if ( doubleC == 3.141 ) {
        fprintf(stdout,"Test to_numeric double:  [passed]\n");
    } else {
        fprintf(stdout,"Test to_numeric double:  [failed]  %16.7f\n", doubleC);
    }

    double doubleA1;
    try {
        to_numeric("A", doubleA1);
        fprintf(stdout,"Test to_numeric string:  [failed]  %16.7f\n", doubleA1);
    } catch (util::NumberFormatError nfe ) {
        fprintf(stdout,"Test to_numeric string:  [passed]\n");
    }

    double doubleA;
    try {
        to_numeric("", doubleA);
        fprintf(stdout,"Test to_numeric empty string:  [failed]  %16.7f\n", 
                                doubleA);
    } catch (util::NumberFormatError nfe ) {
        fprintf(stdout,"Test to_numeric empty string:  [passed]\n");
    }

    double doubleB;
    try {
        to_numeric("3.1B1", doubleB);
        fprintf(stdout,"Test to_numeric partial string:  [failed]  %16.7f\n", 
                                doubleB);
    } catch (util::NumberFormatError nfe ) {
        fprintf(stdout,"Test to_numeric partial string:  [passed]\n");
    }
}

int main(int argc, char * argv[])
{
    Timer timer;
    double real = 0.0;
    double cpu = 0.0;

    fprintf(stdout,"Testing Ranmar\n");

    timer.elapsed(real,cpu);
    test_ranmar();
    timer.elapsed(real,cpu);

    fprintf(stdout,"Time for Ranmar: %10.3f  %10.3f \n", real,cpu);

    fprintf(stdout,"Testing MTwist...\n");

    timer.elapsed(real,cpu);
    test_mtwist();
    timer.elapsed(real,cpu);

    fprintf(stdout,"Time for MTwist: %10.3f  %10.3f \n", real,cpu);

    fprintf(stdout,"Testing Zran\n");

    timer.elapsed(real,cpu);
    test_zran();
    timer.elapsed(real,cpu);

    fprintf(stdout,"Time for Zran: %10.3f  %10.3f \n", real,cpu);

    fprintf(stdout,"Testing to_numeric...\n");

    timer.elapsed(real,cpu);
    test_to_numeric();
    timer.elapsed(real,cpu);

    fprintf(stdout,"Time for to_numeric: %10.3f  %10.3f \n", real,cpu);

}
