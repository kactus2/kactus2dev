/****************************************************************************
**
** Copyright (C) VCreate Logic Private Limited, Bangalore
**
** Use of this file is limited according to the terms specified by
** VCreate Logic Private Limited, Bangalore.  Details of those terms
** are listed in licence.txt included as part of the distribution package
** of this file. This file may not be distributed without including the
** licence.txt file.
**
** Contact info@vcreatelogic.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef I_MATH_UTILS_H
#define I_MATH_UTILS_H

#include <GCF/IContainer.h>

class IMathUtils : virtual public IContainer
{
public:
	virtual double toRadians(double angle) = 0;
	virtual double toDegrees(double radians) = 0;
	virtual double sinAngle(double angle) = 0;
	virtual double cosAngle(double angle) = 0;
	virtual double tanAngle(double angle) = 0;
	virtual double aSinAngle(double val) = 0;
	virtual double aCosAngle(double val) = 0;
	virtual double aTanAngle(double val) = 0;
	virtual double randomNumber(double min, double max) = 0;
};

Q_DECLARE_INTERFACE(IMathUtils, "com.vcreatelogic.IMathUtils/1.0");

#endif

