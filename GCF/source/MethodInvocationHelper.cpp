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

#include <GCF/MethodInvocationHelper.h>

#include <QObject>
#include <QVariant>
#include <QMetaObject>
#include <QMetaMethod>

#include <QColor>
#include <QPixmap>
#include <QTime>
#include <QDate>
#include <QDateTime>
#include <QString>
#include <QByteArray>
#include <QBitArray>
#include <QImage>
#include <QBitmap>
#include <QPointF>
#include <QPoint>
#include <QLineF>
#include <QLine>
#include <QSizeF>
#include <QSize>
#include <QRectF>
#include <QRect>
#include <QMatrix>
#include <QTransform>
#include <QUrl>
#include <QSizePolicy>
#include <QBrush>

// Temporary objects created during method invokation
// These are required for Q_ARG(), Q_RETURN_ARG() to work
// across system boundaries
class GCF::MethodInvocationData
{
public:
    QList<QString*> strings;
    QList<char*> chars;
    QList<bool*> bools;
    QList<QColor*> colors;
    QList<QDate*> dates;
    QList<QTime*> times;
    QList<QDateTime*> dateTimes;
    QList<double*> doubles;
    QList<int*> ints;
    QList<uint*> uints;
    QList<QPixmap*> pixmaps;
    QList<QByteArray*> byteArrays;
    QList<QBitArray*> bitArrays;
    QList<QImage*> images;
    QList<QUrl*> urls;
    QList<QStringList*> stringLists;
    QList<QSizePolicy*> sizePolicies;
    QList<QSizeF*> sizefs;
    QList<QSize*> sizes;
    QList<QPointF*> pointfs;
    QList<QPoint*> points;
    QList<QRectF*> rectfs;
    QList<QRect*> rects;
    QList<QLineF*> linefs;
    QList<QLine*> lines;
    QList<QRegExp*> regExps;
    QList<QPolygon*> polygons;
    QList<QMatrix*> matricies;
    QList<QBrush*> brushes;
    QList<QBitmap*> bitmaps;
    QList<QTransform*> transforms;

    ~MethodInvocationData();
};

GCF::MethodInvocationData::~MethodInvocationData()
{
    qDeleteAll(strings);
    qDeleteAll(chars);
    qDeleteAll(bools);
    qDeleteAll(colors);
    qDeleteAll(dates);
    qDeleteAll(times);
    qDeleteAll(dateTimes);
    qDeleteAll(doubles);
    qDeleteAll(ints);
    qDeleteAll(uints);
    qDeleteAll(pixmaps);
    qDeleteAll(byteArrays);
    qDeleteAll(bitArrays);
    qDeleteAll(images);
    qDeleteAll(urls);
    qDeleteAll(stringLists);
    qDeleteAll(sizePolicies);
    qDeleteAll(sizefs);
    qDeleteAll(sizes);
    qDeleteAll(pointfs);
    qDeleteAll(points);
    qDeleteAll(rectfs);
    qDeleteAll(rects);
    qDeleteAll(linefs);
    qDeleteAll(lines);
    qDeleteAll(regExps);
    qDeleteAll(polygons);
    qDeleteAll(matricies);
    qDeleteAll(brushes);
    qDeleteAll(bitmaps);
    qDeleteAll(transforms);
}

bool GCF::callMethod(QObject* object, const QString& methodName, const QVariantList& args, QVariant& returnValue)
{
    if( !object )
        return false;

    // Extract the method name (from signal/slot names)
    QByteArray methodName2 = methodName.toAscii();
    const QMetaObject* mo = object->metaObject();
    const char* method = methodName2.data();
    const char* actualMethod = (*method >= 0 && *method <= 2) ? method+1 : method;

    int methodIdx = GCF::findMethod(object, QString(actualMethod), args);
    if( methodIdx < 0 )
        return false;

    // Get hold of the meta method now.
    QMetaMethod metaMethod = mo->method(methodIdx);

    // Ensure that only public methods can be invoked from remote apps
    if( metaMethod.access() != QMetaMethod::Public )
        return false;

    // Now check to see whether the args and required params match.
    QList<QByteArray> paramTypes = metaMethod.parameterTypes();
    if( paramTypes.count() != args.count() )
    {
        // at this point we cant deal with default parameters.
        return false;
    }

    // Create a temporary structure where all generic argument data
    // will be stored.
    GCF::MethodInvocationData miData;

    // Prepare a list of generic arguments.
    QVector<QGenericArgument> gArgs(10);
    for(int i=0; i<args.count(); i++)
        GCF::toGenericArgument(gArgs[i], args[i], paramTypes[i].constData(), miData);

    // We need to send the reply of the method back to the sender if the method's
    // return type is not void
    QString returnType( metaMethod.typeName() );
    bool success = false;
    if( !returnType.isEmpty() )
    {
        QVariant::Type type = QVariant::nameToType( metaMethod.typeName() );

        // The following switch case seems laborious, but it is required.
        // Correct me if I am wrong.
        switch( type )
        {
        case QVariant::Bool: {

            bool retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(bool, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;
        case QVariant::Char: {

            char retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(char, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;
        case QVariant::Color: {

            QColor retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QColor, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;
        case QVariant::Date:{

            QDate retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QDate, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;
        case QVariant::DateTime: {

            QDateTime retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QDateTime, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;
        case QVariant::Double: {

            double retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(double, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;
        case QVariant::Int: {

            int retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(int, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;
        case QVariant::Time: {

            QTime retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QTime, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;
        case QVariant::UInt: {

            uint retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(uint, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;
        case QVariant::String: {

            QString retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QString, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;
        case QVariant::Pixmap: {

            QPixmap retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QPixmap, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;
        case QVariant::ByteArray: {

            QByteArray retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QByteArray, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;
        case QVariant::BitArray: {

            QBitArray retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QBitArray, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::Image: {

            QImage retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QImage, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::Url: {

            QUrl retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QUrl, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::StringList: {

            QStringList retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QStringList, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::SizePolicy: {

            QSizePolicy retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QSizePolicy, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::SizeF: {

            QSizeF retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QSizeF, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::Size: {

            QSize retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QSize, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::RegExp: {

            QRegExp retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QRegExp, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::RectF: {

            QRectF retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QRectF, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::Rect: {

            QRect retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QRect, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::Polygon: {

            QPolygon retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QPolygon, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::PointF: {

            QPointF retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QPointF, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::Point: {

            QPoint retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QPoint, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::Matrix: {

            QMatrix retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QMatrix, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::LineF: {

            QLineF retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QLineF, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::Line: {

            QLine retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QLine, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::Brush: {

            QBrush retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QBrush, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::Bitmap: {

            QBitmap retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QBitmap, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        case QVariant::Transform: {

            QTransform retVal;
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        Q_RETURN_ARG(QTransform, retVal),
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);
            if( success )
                returnValue = retVal;

            } break;

        default:
            // we dont know the return type, so we just place the function call and forget
            // about the return value.
            success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                        gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                        gArgs[7], gArgs[8], gArgs[9]);

            break;
        }

        return success;
    }
    else
    {
        // We need not worry about the return value now. Because the method
        // does not return anything. It is a void method.
        success = QMetaObject::invokeMethod(object, actualMethod, Qt::DirectConnection,
                    gArgs[0], gArgs[1], gArgs[2], gArgs[3], gArgs[4], gArgs[5], gArgs[6],
                    gArgs[7], gArgs[8], gArgs[9]);
        return success;
    }

    return false;
}


bool GCF::toGenericArgument(QGenericArgument& arg, const QVariant& value, const char* typeName, GCF::MethodInvocationData& miData)
{
    QVariant::Type type = QVariant::nameToType(typeName);

    switch( type )
    {
    case QVariant::Bool: {
        bool* val = new bool;
        *val = value.toBool();
        arg = Q_ARG(bool, *val);
        miData.bools.append(val);
        return true;
        } break;

    case QVariant::Char: {
        char* val = new char;
        *val = value.toChar().toAscii();
        arg = Q_ARG(char, *val);
        miData.chars.append(val);
        return true;
        } break;

    case QVariant::Color: {
        QColor* val = new QColor;
        *val = value.value<QColor>();
        arg = Q_ARG(QColor, *val);
        miData.colors.append(val);
        return true;
        } break;

    case QVariant::Date: {
        QDate* val = new QDate;
        *val = value.toDate();
        arg = Q_ARG(QDate, *val);
        miData.dates.append(val);
        return true;
        } break;

    case QVariant::DateTime: {
        QDateTime* val = new QDateTime;
        *val = value.toDateTime();
        arg = Q_ARG(QDateTime, *val);
        miData.dateTimes.append(val);
        return true;
        } break;

    case QVariant::Double: {
        double* val = new double;
        *val = value.toDouble();
        arg = Q_ARG(double, *val);
        miData.doubles.append(val);
        return true;
        } break;

    case QVariant::Int: {
        int* val = new int;
        *val = value.toInt();
        arg = Q_ARG(int, *val);
        miData.ints.append(val);
        return true;
        } break;

    case QVariant::Time: {
        QTime* val = new QTime;
        *val = value.toTime();
        arg = Q_ARG(QTime, *val);
        miData.times.append(val);
        return true;
        } break;

    case QVariant::UInt: {
        uint* val = new uint;
        *val = value.toUInt();
        arg = Q_ARG(uint, *val);
        miData.uints.append(val);
        return true;
        } break;

    case QVariant::String: {
        QString* val = new QString;
        *val = value.toString();
        arg = Q_ARG(QString, *val);
        miData.strings.append(val);
        return true;
        } break;

    case QVariant::Pixmap: {
        QPixmap* val = new QPixmap;
        *val = value.value<QPixmap>();
        arg = Q_ARG(QPixmap, *val);
        miData.pixmaps.append(val);
        return true;
        } break;

    case QVariant::ByteArray: {
        QByteArray* val = new QByteArray;
        *val = value.toByteArray();
        arg = Q_ARG(QByteArray, *val);
        miData.byteArrays.append(val);
        return true;
        } break;

    case QVariant::BitArray: {
        QBitArray* val = new QBitArray;
        *val = value.toBitArray();
        arg = Q_ARG(QBitArray, *val);
        miData.bitArrays.append(val);
        return true;
        } break;

    case QVariant::Image: {
        QImage* val = new QImage;
        *val = value.value<QImage>();
        arg = Q_ARG(QImage, *val);
        miData.images.append(val);
        return true;
        } break;

    case QVariant::Url: {
        QUrl* val = new QUrl;
        *val = value.toUrl();
        arg = Q_ARG(QUrl, *val);
        miData.urls.append(val);
        return true;
        } break;

    case QVariant::StringList: {
        QStringList* val = new QStringList;
        *val = value.toStringList();
        arg = Q_ARG(QStringList, *val);
        miData.stringLists.append(val);
        return true;
        } break;

    case QVariant::SizePolicy: {
        QSizePolicy* val = new QSizePolicy;
        *val = value.value<QSizePolicy>();
        arg = Q_ARG(QSizePolicy, *val);
        miData.sizePolicies.append(val);
        return true;
        } break;

    case QVariant::SizeF: {
        QSizeF* val = new QSizeF;
        *val = value.value<QSizeF>();
        arg = Q_ARG(QSizeF, *val);
        miData.sizefs.append(val);
        return true;
        } break;

    case QVariant::Size: {
        QSize* val = new QSize;
        *val = value.value<QSize>();
        arg = Q_ARG(QSize, *val);
        miData.sizes.append(val);
        return true;
        } break;

    case QVariant::RegExp: {
        QRegExp* val = new QRegExp;
        *val = value.value<QRegExp>();
        arg = Q_ARG(QRegExp, *val);
        miData.regExps.append(val);
        return true;
        } break;

    case QVariant::RectF: {
        QRectF* val = new QRectF;
        *val = value.value<QRectF>();
        arg = Q_ARG(QRectF, *val);
        miData.rectfs.append(val);
        return true;
        } break;

    case QVariant::Rect: {
        QRect* val = new QRect;
        *val = value.value<QRect>();
        arg = Q_ARG(QRect, *val);
        miData.rects.append(val);
        return true;
        } break;

    case QVariant::Polygon: {
        QPolygon* val = new QPolygon;
        *val = value.value<QPolygon>();
        arg = Q_ARG(QPolygon, *val);
        miData.polygons.append(val);
        return true;
        } break;

    case QVariant::PointF: {
        QPointF* val = new QPointF;
        *val = value.value<QPointF>();
        arg = Q_ARG(QPointF, *val);
        miData.pointfs.append(val);
        return true;
        } break;

    case QVariant::Point: {
        QPoint* val = new QPoint;
        *val = value.value<QPoint>();
        arg = Q_ARG(QPoint, *val);
        miData.points.append(val);
        return true;
        } break;

    case QVariant::Matrix: {
        QMatrix* val = new QMatrix;
        *val = value.value<QMatrix>();
        arg = Q_ARG(QMatrix, *val);
        miData.matricies.append(val);
        return true;
        } break;

    case QVariant::LineF: {
        QLineF* val = new QLineF;
        *val = value.value<QLineF>();
        arg = Q_ARG(QLineF, *val);
        miData.linefs.append(val);
        return true;
        } break;

    case QVariant::Line: {
        QLine* val = new QLine;
        *val = value.value<QLine>();
        arg = Q_ARG(QLine, *val);
        miData.lines.append(val);
        return true;
        } break;

    case QVariant::Brush: {
        QBrush* val = new QBrush;
        *val = value.value<QBrush>();
        arg = Q_ARG(QBrush, *val);
        miData.brushes.append(val);
        return true;
        } break;

    case QVariant::Bitmap: {
        QBitmap* val = new QBitmap;
        *val = value.value<QBitmap>();
        arg = Q_ARG(QBitmap, *val);
        miData.bitmaps.append(val);
        return true;
        } break;

    case QVariant::Transform: {
        QTransform* val = new QTransform;
        *val = value.value<QTransform>();
        arg = Q_ARG(QTransform, *val);
        miData.transforms.append(val);
        return true;
        } break;

    default:
        // Other types will be supported shortly.
        // TODO: support user defined types.
        break;
    }

    return false;
}

/**
Finds a method that can accept the arguments as passed by args.
It is quite possible for us to land up in situations where a local method
invoked remotely, and the local method exists in one of the several
overloaded forms. For example

\code
class MyClass : public QObject
{
public:
    Q_INVOKABLE void function(QString, char);
    Q_INVOKABLE void function(int, int);
    Q_INVOKABLE void function(int, char);
};
\endcode

In the above code the function names are same, but the parameters accepted are different.
This method helps you to figure out which function to call based on the arguments.

The function returns a method index on success, -1 on failure.
*/
int GCF::findMethod(QObject* object, const QString& methodName, const QVariantList& args)
{
    // Lets construct a method parameter type list
    QList<QByteArray> argTypes;
    for(int i=0; i<args.count(); i++)
        argTypes << QByteArray(args[i].typeName());

    // Now lets go find that method whose name is methodName
    if( methodName.isEmpty() )
        return false;

    // fetch the QMetaMethod object for the given method name
    const QMetaObject* mo = object->metaObject();
    int methodIdx = -1;
    QMetaMethod metaMethod;

    // Loop through all methods and figure out the method index of actualMethod
    for(int i=mo->methodCount()-1; i>=0; i--)
    {
        metaMethod = mo->method(i);
        QString sig( metaMethod.signature() );

        // If the method names dont match, the move on.
        if( sig.section('(', 0, 0) != methodName )
            continue;

        // Method names match, lets see if the parameter types match
        QList<QByteArray> paramTypes = metaMethod.parameterTypes();
        bool match = true; // optimistic search
        for(int j=0; j<argTypes.count(); j++)
        {
            if( argTypes[j] != paramTypes[j] )
            {
                match = false;
                break;
            }
        }

        // Stop searching if we have found a match.
        if( match )
        {
            methodIdx = i;
            break;
        }
    }

    // Return this method index.
    return methodIdx;
}

