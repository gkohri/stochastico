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

#ifndef SDM_ORTHOTOPE_MODEL_H
#define SDM_ORTHOTOPE_MODEL_H

#include <limits>
#include <vector>

#include "sdm/covered_point.h"
#include "sdm/model.h"
#include "noir/noir_space.h"
#include "noir/orthotope.h"
#include "rng/random.h"

namespace sdm {

/*
 * This Model implementation consists of a union of Orthotopes.
 */
class OrthotopeModel : public Model {
 public:
    OrthotopeModel(const int &principal_color, 
                   const double &total_principal_colors,
                   const double &total_other_colors):
                   Model( principal_color, total_principal_colors, 
                          total_other_colors ) {}

    virtual ~OrthotopeModel() {}

    /*
     * Expands this model by creating a hyper-rectangle using the specified
     * nexus and nearest neighbor points.
     */
    void expand( const noir::Orthotope &region, CoveredPoint *nexus,
                         CoveredPoint *nn, rng::Random *rand, 
                         const double &lp, const double &up );

    /*
     * Thickens the last hyper-rectangle in the model's list.
     */
    void thicken( const noir::Orthotope &region, rng::Random *rand,
                 const double &frac );
};

}   // end namespace sdm

#endif   // SDM_ORTHOTOPE_MODEL_H
