/**********************************************************************************************************************
This file is part of the Control Toobox (https://adrlab.bitbucket.io/ct), copyright by ETH Zurich, Google Inc.
Authors:  Michael Neunert, Markus Giftthaler, Markus Stäuble, Diego Pardo, Farbod Farshidian
Licensed under Apache2 license (see LICENSE file in main directory)
**********************************************************************************************************************/

#pragma once

#include "../activations/ActivationBase.hpp"
#include "../activations/PeriodicActivation.hpp"
#include "../activations/RBFGaussActivation.h"
#include "../activations/SingleActivation.hpp"
#include "../activations/utilities/ActivationLoadMacros.h"

#define CT_LOADABLE_ACTIVATIONS(SCALAR)                            \
    CT_LOADABLE_ACTIVATION(SCALAR, SingleActivation, "single")     \
    CT_LOADABLE_ACTIVATION(SCALAR, PeriodicActivation, "periodic") \
    CT_LOADABLE_ACTIVATION(SCALAR, RBFGaussActivation, "rbf")
