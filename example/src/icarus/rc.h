/******************************************************
 *
 *   #, #,         CCCCCC  VV    VV MM      MM RRRRRRR
 *  %  %(  #%%#   CC    CC VV    VV MMM    MMM RR    RR
 *  %    %## #    CC        V    V  MM M  M MM RR    RR
 *   ,%      %    CC        VV  VV  MM  MM  MM RRRRRR
 *   (%      %,   CC    CC   VVVV   MM      MM RR   RR
 *     #%    %*    CCCCCC     VV    MM      MM RR    RR
 *    .%    %/
 *       (%.      Computer Vision & Mixed Reality Group
 *
 *****************************************************/
/** @copyright:   Hochschule RheinMain,
 *                University of Applied Sciences
 *     @author:   Marc Lieser
 *    @version:   1.0
 *       @date:   13.05.2014
 *****************************************************/

#ifndef RC_H
#define RC_H

#include <math.h>

/**
 * @brief Remote Control implementing expo and dual rate control principles.
 */
class RC
{

public:

    /**
     * Constructor.
     */
    RC();

    /**
     * Parameter constructor.
     * @param expo The exponent value.
     * @param dualRate The dual rate value.
     */
    RC(double expo, double dualRate);

    /**
     * @brief Get a single value expo/dualrate-modified.
     * @param normalizedValue A normalized control value.
     * @return The normalized control value modified by expo/dualrate.
     */
    double getControl(double normalizedValue);

    /**
     * Setter for the exponent.
     * @param exponent.
     */
    void setExpo(double expo);

    /**
     * Setter for the dual rate.
     */
    void setDualRate(double dualRate);

private:

    /// The expo.
    double expoNent;

    /// The dual rate.
    double dualRate;

    /// Expo and dual rate function.
    double expo(double base);
};

#endif /* RC_H */
