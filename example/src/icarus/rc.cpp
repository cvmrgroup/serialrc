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

#include "rc.h"

RC::RC()
        :
        expoNent(1.),
        dualRate(1.)
{
}

RC::RC(double expo, double dualRate)
        : expoNent(expo),
          dualRate(dualRate)
{
}

void RC::setExpo(double expo)
{
    this->expoNent = expo;
}

void RC::setDualRate(double dualRate)
{
    this->dualRate = dualRate;
}

double RC::expo(double base)
{
    double v = pow(fabs(base), this->expoNent) * this->dualRate;

    if (base < 0.)
    {
        v = -v;
    }

    return v;
}

double RC::getControl(double normalizedValue)
{
    return this->expo(normalizedValue);
}
