/**********************************************************************************************************************
This file is part of the Control Toobox (https://adrlab.bitbucket.io/ct), copyright by ETH Zurich, Google Inc.
Authors:  Michael Neunert, Markus Giftthaler, Markus Stäuble, Diego Pardo, Farbod Farshidian
Licensed under Apache2 license (see LICENSE file in main directory)
**********************************************************************************************************************/

#pragma once

#include <kindr/Core>

const size_t DIM = 3;  //!< dimension of vector

//! the Jacobian codegen class
using derivativesCppadJIT = DerivativesCppadJIT<DIM, DIM>;
using derivativesCppadCG = DerivativesCppadCG<DIM, DIM>;


/*!
 * A 3x3 function which constructs a a kindr EulerAngles object in between.
 * The aim of this function is to verify that JIT gets along with kindr Euler angle types
 * (which it should do naturally)
 *
 * @param x the input
 * @return output y = f(x)
 */
template <typename SCALAR>
Eigen::Matrix<SCALAR, 3, 1> testFunction(const Eigen::Matrix<SCALAR, 3, 1>& x)
{
    kindr::EulerAnglesXyz<SCALAR> anglesIn(x);
    kindr::EulerAnglesXyz<SCALAR> anglesTemp;

    anglesTemp.setX(3 * anglesIn.x() + 2 * anglesIn.x() * anglesIn.x() - anglesIn.y() * anglesIn.z());
    anglesTemp.setY(anglesIn.z() + anglesIn.y() + 3);
    anglesTemp.setZ(anglesIn.z());

    kindr::EulerAnglesXyz<SCALAR> anglesOut(anglesTemp);

    return anglesOut.toImplementation();
}

/*!
 * The analytically, manually derived Jacobian of testFunction() used for verification
 *
 * @param x the input
 * @return output y = f(x)
 */
template <typename SCALAR>
Eigen::Matrix<SCALAR, DIM, DIM> jacobianCheck(const Eigen::Matrix<SCALAR, 3, 1>& x)
{
    Eigen::Matrix<SCALAR, DIM, DIM> jac;

    jac << 3 + 4 * x(0), -x(2), -x(1), 0, 1, 1, 0, 0, 1;

    return jac;
}


/*!
 * Test for just-in-time compilation of the Jacobian and subsequent evaluation of it, with kindr in the loop
 */
TEST(KindrJitTest, EulerAnglesTest)
{
    try
    {
        // create a function handle (also works for class methods, lambdas, function pointers, ...)
        typename derivativesCppadJIT::FUN_TYPE_CG f = testFunction<derivativesCppadJIT::CG_SCALAR>;

        // initialize the Auto-Diff Codegen Jacobian
        derivativesCppadJIT jacCG(f);

        DerivativesCppadSettings settings;
        settings.createJacobian_ = true;

        // compile the Jacobian
        jacCG.compileJIT(settings, "kindrTestLib");

        // create an input vector
        Eigen::Matrix<double, 3, 1> angles;

        for (size_t i = 0; i < 10; i++)
        {
            // create a random input
            angles.setRandom();

            // verify agains the analytical Jacobian

            ASSERT_LT((jacCG.jacobian(angles) - jacobianCheck(angles)).array().abs().maxCoeff(), 1e-10);
        }
    } catch (std::exception& e)
    {
        std::cout << "Kindr Euler angle test failed: " << e.what() << std::endl;
        ASSERT_TRUE(false);
    }
}


template <typename SCALAR>
Eigen::Matrix<SCALAR, 3, 1> rotateTestFunction(const Eigen::Matrix<SCALAR, 3, 1>& x)
{
    kindr::EulerAnglesXyz<SCALAR> angles(x);

    // create a position and set to a trivial hard-coded value
    kindr::Position<SCALAR, 3> pos;
    pos.toImplementation()(0) = 1;
    pos.toImplementation()(1) = 1;
    pos.toImplementation()(2) = 1;

    kindr::Position<SCALAR, 3> pos_rot = angles.rotate(pos);

    return angles.rotate(pos).toImplementation();
}


TEST(KindrJitTest, DISABLED_EulerRotateTest)
{
    try
    {
        // create a function handle (also works for class methods, lambdas, function pointers, ...)
        typename derivativesCppadJIT::FUN_TYPE_CG f = rotateTestFunction<derivativesCppadJIT::CG_SCALAR>;

        // initialize the Auto-Diff Codegen Jacobian
        derivativesCppadJIT jacCG(f);

        DerivativesCppadSettings settings;
        settings.createJacobian_ = true;

        // compile the Jacobian
        jacCG.compileJIT(settings, "kindrRotateTestLib");

    } catch (std::exception& e)
    {
        std::cout << "Kindr rotate test failed: " << e.what() << std::endl;
        ASSERT_TRUE(false);
    }
}


template <typename SCALAR>
Eigen::Matrix<SCALAR, 3, 1> kindrRotationMatrixTestFunction(const Eigen::Matrix<SCALAR, 3, 1>& x)
{
    kindr::EulerAnglesXyz<SCALAR> angles(x);

    // create a position and set to a trivial hard-coded value
    kindr::Position<SCALAR, 3> pos;
    pos.toImplementation()(0) = 1;
    pos.toImplementation()(1) = 1;
    pos.toImplementation()(2) = 1;

    return kindr::RotationMatrix<SCALAR>(angles).toImplementation() * pos.toImplementation();

}


TEST(KindrJitTest, DISABLED_RotationMatrixTest)
{
    try
    {
        // create a function handle (also works for class methods, lambdas, function pointers, ...)
        typename derivativesCppadJIT::FUN_TYPE_CG f = kindrRotationMatrixTestFunction<derivativesCppadJIT::CG_SCALAR>;

        // initialize the Auto-Diff Codegen Jacobian
        derivativesCppadJIT jacCG(f);

        DerivativesCppadSettings settings;
        settings.createJacobian_ = true;

        // compile the Jacobian
        jacCG.compileJIT(settings, "testLib");

    } catch (std::exception& e)
    {
        std::cout << "Kindr rotation matrix test failed: " << e.what() << std::endl;
        ASSERT_TRUE(false);
    }
}
