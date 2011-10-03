/****************************************************************************
**
** Copyright (C) VCreate Logic Private Limited, Bangalore
**
** Use of this file is limited according to the terms specified by
** VCreate Logic Private Limited, Bangalore. Details of those terms
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

#ifndef USER_DEFINED_DATA_TYPES_H
#define USER_DEFINED_DATA_TYPES_H

#include <QVariant>
#include <QList>

struct Point3D
{
    Point3D() : x(0), y(0), z(0) { }
    Point3D(double* val) : x(val[0]), y(val[1]), z(val[2]) { }
    Point3D(double* val, bool) : x(val[0]), y(val[1]), z(0) { }
    Point3D(double _x, double _y, double _z) : x(_x), y(_y), z(_z) { }

    double x;
    double y;
    double z;
};
Q_DECLARE_METATYPE(Point3D)

struct Vector3D
{
    Vector3D() : x(0), y(0), z(0) { }
    Vector3D(double* val) : x(val[0]), y(val[1]), z(val[2]) { }
    Vector3D(double* val, bool) : x(val[0]), y(val[1]), z(0) { }
    Vector3D(double _x, double _y, double _z) : x(_x), y(_y), z(_z) { }

    double x;
    double y;
    double z;
};
Q_DECLARE_METATYPE(Vector3D)

struct Triangle
{
    Triangle() : a(0), b(0), c(0) { }
    Triangle(int* val) : a(val[0]), b(val[1]), c(val[2]) { }
    Triangle(int _a, int _b, int _c) : a(_a), b(_b), c(_c) { }

    uint a;
    uint b;
    uint c;
};
Q_DECLARE_METATYPE(Triangle)

struct BoundingBox3D
{
    BoundingBox3D() : xMin(0), xMax(0), yMin(0), yMax(0), zMin(0), zMax(0) { }
    BoundingBox3D(double x1, double x2, double y1, double y2, double z1, double z2)
        : xMin(x1), xMax(x2), yMin(y1), yMax(y2), zMin(z1), zMax(z2) { }
    BoundingBox3D(double* val)
        : xMin(val[0]), xMax(val[3]), yMin(val[1]), yMax(val[4]), zMin(val[2]), zMax(val[5]) { }

    double xMin;
    double xMax;
    double yMin;
    double yMax;
    double zMin;
    double zMax;
};
Q_DECLARE_METATYPE(BoundingBox3D)

struct ValueRange
{
    ValueRange() : min(0), max(0) { }
    ValueRange(double v1, double v2) : min(v1), max(v2) { }
    ValueRange(double *val) : min(val[0]), max(val[1]) { }

    double min;
    double max;
};
Q_DECLARE_METATYPE(ValueRange)

struct LineSegment
{
    LineSegment() : a(0), b(0) { }
    LineSegment(int* val) : a(val[0]), b(val[1]) { }
    LineSegment(int _a, int _b) : a(_a), b(_b) { }

    int a;
    int b;
};
Q_DECLARE_METATYPE(LineSegment)

Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(QList<double>)

#endif
