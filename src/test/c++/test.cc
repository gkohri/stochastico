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
#include <rng/mt_19937.h>
#include <rng/zran.h>
#include <rng/well_1024.h>
#include <stat/accumulator.h>
#include <util/timer.h>
#include <util/functions.h>

using rng::Random;
using rng::Ranmar;
using rng::MT_19937;
using rng::Well_1024;
using rng::Zran;
using stat::Accumulator;
using util::Timer;
using util::to_numeric;

const int N = 10000000;
double genrand();
void sgenrand(unsigned);

void test_ranmar()
{
	Random *random = new Ranmar(3127,4591);
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
        fprintf(stdout,"Test Ranmar: \t\t\t [failed]\n");
    } else {
        fprintf(stdout,"Test Ranmar: \t\t\t [passed]\n");
    }
}

void test_MT_19937()
{
	MT_19937 *mt_19937 = new MT_19937();
	double avg = 0.0;
	double avgs = 0.0;
	double dev = 0.0;

	for (int i=0;i<N;i++)
	{
        double rand = mt_19937->next();
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
        fprintf(stdout,"Test MT_19937: \t\t\t [failed]\n");
    } else {
        fprintf(stdout,"Test MT_19937: \t\t\t [passed]\n");
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
        fprintf(stdout,"Test Zran: \t\t\t [failed]\n");
    } else {
        fprintf(stdout,"Test Zran: \t\t\t [passed]\n");
    }
}

void test_Well_1024()
{
	Well_1024 *well_1024 = new Well_1024(37813);
	double avg = 0.0;
	double avgs = 0.0;
	double dev = 0.0;

	for (int i=0;i<N;i++)
	{
        double rand = well_1024->next();
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
        fprintf(stdout,"Test Well_1024: \t\t [failed]\n");
    } else {
        fprintf(stdout,"Test Well_1024: \t\t [passed]\n");
    }
}

void test_to_numeric() {

    double doubleC; 
    to_numeric("3.141", doubleC);
    if ( doubleC == 3.141 ) {
        fprintf(stdout,"Test to_numeric double: \t [passed]\n");
    } else {
        fprintf(stdout,"Test to_numeric double: \t [failed]  %16.7f\n", 
                doubleC);
    }

    double doubleA1;
    try {
        to_numeric("A", doubleA1);
        fprintf(stdout,"Test to_numeric string: \t [failed]  %16.7f\n",
                doubleA1);
    } catch (util::NumberFormatError nfe ) {
        fprintf(stdout,"Test to_numeric string: \t [passed]\n");
    }

    double doubleA;
    try {
        to_numeric("", doubleA);
        fprintf(stdout,"Test to_numeric empty string: \t [failed]  %16.7f\n", 
                                doubleA);
    } catch (util::NumberFormatError nfe ) {
        fprintf(stdout,"Test to_numeric empty string: \t [passed]\n");
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

void test_accumulator()
{
	Accumulator acc;
    int four = 4;
    acc.gather( four );
    int seven = 7;
    acc.gather( seven );
    int thirteen = 13;
    acc.gather( thirteen );
    int sixteen = 16;
    acc.gather( sixteen );

    if ( acc.mean() == 10.0 ) {
        fprintf(stdout,"Test accumulator mean: \t\t [passed]\n");
    } else {
        fprintf(stdout,"Test accumulator mean: \t [failed]  %16.7f\n", 
                        acc.mean());
    }

    if ( acc.variance_sample() == 30.0 ) {
        fprintf(stdout,"Test accumulator variance: \t [passed]\n");
    } else {
        fprintf(stdout,"Test accumulator variance: \t [failed]  %16.7f\n", 
                            acc.variance_sample());
    }

	Accumulator acc1;
    acc1.gather( 1.0e9+4.0 );
    acc1.gather( 1.0e9+7.0 );
    acc1.gather( 1.0e9+13.0 );
    acc1.gather( 1.0e9+16.0 );

    if ( acc1.mean() == 1.0e9+10.0 ) {
        fprintf(stdout,"Test-1 accumulator mean: \t [passed]\n");
    } else {
        fprintf(stdout,"Test-1 accumulator mean: \t [failed]  %16.7f\n", 
                            acc1.mean());
    }

    if ( acc1.variance_sample() == 30.0 ) {
        fprintf(stdout,"Test-1 accumulator variance: \t [passed]\n");
    } else {
        fprintf(stdout,"Test-1 accumulator variance: \t [failed]  %16.7f\n", 
                            acc1.variance_sample());
    }

    double data[] = { 1.0e9+4.0, 1.0e9+7.0, 1.0e9+13.0, 1.0e9+16.0 };

	Accumulator acc2;
    acc2.gather( data, data+4 );

    if ( acc2.mean() == 1.0e9+10.0 ) {
        fprintf(stdout,"Test-2 accumulator mean: \t [passed]\n");
    } else {
        fprintf(stdout,"Test-2 accumulator mean: \t [failed]  %16.7f\n", 
                            acc2.mean());
    }

    if ( acc2.variance_sample() == 30.0 ) {
        fprintf(stdout,"Test-2 accumulator variance: \t [passed]\n");
    } else {
        fprintf(stdout,"Test-2 accumulator variance:  \t [failed]  %16.7f\n", 
                            acc2.variance_sample());
    }


	Accumulator acc3;
    acc3.gather( 1.0e9+4.0 );
    acc3.gather( 1.0e9+16.0 );
    acc3.gather( data+1, data+3 );

    if ( acc3.mean() == 1.0e9+10.0 ) {
        fprintf(stdout,"Test-3 accumulator mean: \t [passed]\n");
    } else {
        fprintf(stdout,"Test-3 accumulator mean: \t [failed]  %16.7f\n", 
                            acc3.mean());
    }

    if ( acc3.variance_sample() == 30.0 ) {
        fprintf(stdout,"Test-3 accumulator variance: \t [passed]\n");
    } else {
        fprintf(stdout,"Test-3 accumulator variance:  \t [failed]  %16.7f\n", 
                            acc3.variance_sample());
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

    fprintf(stdout,"Time for Ranmar: \t\t\t %10.3f  %10.3f \n", real,cpu);

    fprintf(stdout,"Testing MT_19937...\n");

    timer.elapsed(real,cpu);
    test_MT_19937();
    timer.elapsed(real,cpu);

    fprintf(stdout,"Time for MT_19937: \t\t\t %10.3f  %10.3f \n", real,cpu);

    fprintf(stdout,"Testing Well_1024...\n");

    timer.elapsed(real,cpu);
    test_Well_1024();
    timer.elapsed(real,cpu);

    fprintf(stdout,"Time for Well_1024: \t\t\t %10.3f  %10.3f \n", real,cpu);

    fprintf(stdout,"Testing Zran\n");

    timer.elapsed(real,cpu);
    test_zran();
    timer.elapsed(real,cpu);

    fprintf(stdout,"Time for Zran: \t\t\t\t %10.3f  %10.3f \n", real,cpu);

    fprintf(stdout,"Testing to_numeric...\n");

    timer.elapsed(real,cpu);
    test_to_numeric();
    timer.elapsed(real,cpu);

    fprintf(stdout,"Time for to_numeric: \t\t\t %10.3f  %10.3f \n", real,cpu);

    fprintf(stdout,"Testing Accumulator...\n");
    test_accumulator();

}
