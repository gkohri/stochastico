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

#ifndef SDM_MODEL_H
#define SDM_MODEL_H

#include <limits>
#include <vector>

#include "sdm/covered_point.h"
#include "noir/noir_space.h"
#include "rng/random.h"

namespace sdm {

/*
 * This Model implementation consists of a uninion of hyper-rectangles
 */
class Model {
 public:

    Model(const int &principal_color): spaces(),
        fracPrincipal(0.0), fracOther(0.0), probNorm(1.0),
        principalColor( principal_color),
        numPrincipalColor(0), numOtherColor(0) {}

    virtual ~Model(){
        std::vector<noir::NoirSpace*>::iterator hit;
        for ( hit = spaces.begin(); hit != spaces.end(); ++hit ){
            delete (*hit);
        }
    }

    /*
     * Retrieves the principal color (class) for this model.
     */
    int get_principal_color() const {
        return principalColor;
    }

    /*
     * Retrieves the number of points belonging to the principal color (class)
     * contained by this model.
     */
    int get_num_principal_color() const {
        return numPrincipalColor;
    }

    /*
     * Retrieves the number of points belonging to the other colors (classes)
     * contained by this model.
     */
    int get_num_other_color() const {
        return numOtherColor;
    }

    /*
     * Expands this model by creating a hyper-rectangle using the specified
     * nexus and nearest neighbor points.
     */
    void expand( const noir::NoirSpace &region, CoveredPoint *nexus, 
                 CoveredPoint *nn, 
                 rng::Random *rand, const double &lp, const double &up );

    /*
     * Thickens the last hyper-rectangle in the model's list.
     */
    void thicken( const noir::NoirSpace &region, rng::Random *rand,
                 const double &frac );

    /*
     * Resets the registers used to track checked points.
     */
    void clear_checked_points();

    /*
     * Checks whether or not the specified point is covered by this model.
     * It returns true if the point is covered by this model and has the same
     * color as this model's principal color.
     *
     * This method increments the registers used to track checked points.
     */
    bool check_point( const CoveredPoint *p );

    /*
     * Checks whether or not the specified point is covered by this model.
     * It returns true if the point is covered by this model.
     */
    bool covers( const CoveredPoint *p ) const;
    
    /*
     * Informs this model that it has been accpeted by a discriminator
     * which contains numPC points of principal color and numOC points of
     * other colors.
     */
    void accepted( const double &numPC, const double &numOC );

    /*
     * Returns the probability that the specified point has the same color
     * as this model's principal color.
     */
    double predict( const DataPoint *p);

 private:
    std::vector<noir::NoirSpace*> spaces;
    double fracPrincipal;
    double fracOther;
    double probNorm;
    int principalColor;
    int numPrincipalColor;
    int numOtherColor;
};

}   // end namespace sdm

#endif   // SDM_MODEL_H
