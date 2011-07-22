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
#include "noir/orthotope.h"
#include "rng/random.h"

namespace sdm {

/*
 * This Model implementation consists of a union of Orthotopes.
 */
class Model {
 public:
    Model(const int &principal_color, const double &total_principal_colors,
          const double &total_other_colors): spaces(),
          totalPrincipalColors(total_principal_colors),
          totalOtherColors(total_other_colors),
          numPrincipalColor(0.0), numOtherColor(0.0),
          principalColor( principal_color) {}

    virtual ~Model(){
        std::vector<noir::ClosedSpace*>::iterator hit;
        if ( spaces.size() > 0 ) {
            for ( hit = spaces.begin(); hit != spaces.end(); ++hit ){
                delete (*hit);
            }
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
        return static_cast<int>(numPrincipalColor);
    }

    /*
     * Retrieves the number of points belonging to the other colors (classes)
     * contained by this model.
     */
    int get_num_other_color() const {
        return static_cast<int>(numOtherColor);
    }

    int get_num_elements() const {
        return static_cast<int>(spaces.size());
    }

    /*
     * Expands this model by creating a new closed subspace of the the 
     * specified region. The nexus and nearest neighbor point characterize the
     * desired location of the new subspace.  The random number generator
     * may be used to create random variations of the subspace. 
     * lp and up are guidelines for the size of the subspace.
     */
    virtual void expand( const noir::Orthotope &region, CoveredPoint *nexus,
                         CoveredPoint *nn, rng::Random *rand, 
                         const double &lp, const double &up ) = 0;

    /*
     * Thickens the last hyper-rectangle in the model's list.
     */
    virtual void thicken( const noir::Orthotope &region, rng::Random *rand,
                 const double &frac ) = 0;

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
     * Evaluates the normalized characteristic function for the specified
     * point with respect to this model.
     * 
     * X = (C(p) - g_frac)/(r_frac - g_frac)
     * where: C(p) is 1 if point p is covered my this model and 0 otherwise
     *        g_frac is the green fraction for this model
     *        r_frac is the red fraction for this model
     */
    double characteristic( const DataPoint *p);

 protected:
    std::vector<noir::ClosedSpace*> spaces;

 private:
    double totalPrincipalColors;
    double totalOtherColors;
    double numPrincipalColor;
    double numOtherColor;
    int    principalColor;

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
};

struct ModelTypes{
    enum Types{ Ball, Orthotope};
};

/*
 * A factory for creating models of a particular class.
 */
class ModelFactory {
 public:
    ModelFactory() {}
    virtual ~ModelFactory() {}

    virtual Model* get_model(const int &principal_color, 
                             const double &total_principal_colors,
                             const double &total_other_colors) = 0;
 private:
    ModelFactory(const ModelFactory&) = delete;
    ModelFactory& operator=(const ModelFactory&) = delete;
};

}   // end namespace sdm

#endif   // SDM_MODEL_H
