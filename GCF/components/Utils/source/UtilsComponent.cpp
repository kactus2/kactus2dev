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

#include "UtilsComponent.h"
#include <GCF/ComponentFactory.h>
#include <GCF/IComponentPlugin.h>

#include <QTextStream>
#include <QBuffer>
#include <QByteArray>
#include <QImageWriter>
#include <math.h>

static const char GHexChars[] = "0123456789abcdef";
static const char CharIndex(const char ch)
{
    for(int i=0; i<16; i++)
        if(GHexChars[i] == ch)
            return i;
    return 16;
}

struct GCF::Components::UtilsComponentData
{

};

GCF_DEFINE_STD_COMPONENT(UtilsComponent)

GCF::Components::UtilsComponent& GCF::Components::UtilsComponent::instance()
{
    static GCF::Components::UtilsComponent* theInstance = GCF_CREATE_COMPONENT(UtilsComponent);
    return *theInstance;
}

GCF::Components::UtilsComponent::UtilsComponent()
{
    d = new GCF::Components::UtilsComponentData;
}

GCF::Components::UtilsComponent::~UtilsComponent()
{
    delete d;
}

// IContainer implementation
QObject* GCF::Components::UtilsComponent::containerObject()
{
    return this;
}

// IXmlUtils
QList<QDomElement> GCF::Components::UtilsComponent::findElements(QDomElement parentE, QString tagName)
{
    QDomNodeList nodes = parentE.childNodes();
    QList<QDomElement> retList;
    for(int i=0; i<nodes.count(); i++)
    {
        QDomElement nodeE = nodes.at(i).toElement();
        if(nodeE.isNull() || nodeE.tagName() != tagName)
            continue;
        retList.append(nodeE);
    }

    return retList;
}

QDomElement GCF::Components::UtilsComponent::findElement(QDomElement parentE, QString tagName)
{
    QDomNodeList nodes = parentE.childNodes();
    for(int i=0; i<nodes.count(); i++)
    {
        QDomElement nodeE = nodes.at(i).toElement();
        if(nodeE.isNull() || nodeE.tagName() != tagName)
            continue;
        return nodeE;
    }

    return QDomElement();
}

bool GCF::Components::UtilsComponent::savePixmap(QDomDocument doc, QDomElement parentE, QPixmap pix)
{
    QImage img = pix.toImage();

    // Modified from void Resource::saveImageData()
    // function of Qt Designer Version 3.3.6.
    // resource.cpp Line 2343
    QByteArray ba;
    QBuffer buf( &ba );
    buf.open( QIODevice::WriteOnly | QIODevice::Text );
    QString format;
    bool compress = FALSE;
    if (img.hasAlphaChannel())
    {
        format = "PNG";
    }
    else
    {
        format = img.depth() > 1 ? "XPM" : "XBM";
        compress = TRUE;
    }
    QImageWriter iio( &buf, format.toAscii() );
    iio.write(img);
    QByteArray bazip = ba;
    int i = 0;
    if (compress)
    {
        bazip = qCompress( ba );
        format += ".GZ";
        // The first 4 bytes in qCompress() are the length of the unzipped
        // format. The XPM.GZ format does not use these.
        i = 4;
    }
    ulong len = bazip.size();
    QString imageText;
    QTextStream ts(&imageText, QIODevice::WriteOnly);
    static const char hexchars[] = "0123456789abcdef";
    for (; i < (int)len; ++i )
    {
        uchar s = (uchar) bazip[i];
        ts << hexchars[s >> 4];
        ts << hexchars[s & 0x0f];
    }

    QDomElement imageE = doc.createElement("image");
    parentE.appendChild(imageE);
    imageE.setAttribute("format", format);
    imageE.setAttribute("length", ba.size());
    QDomCDATASection imageCData = doc.createCDATASection(imageText);
    imageE.appendChild(imageCData);

    return true;
}

bool GCF::Components::UtilsComponent::loadPixmap(QDomElement parentE, QPixmap& pixmap)
{
    pixmap = QPixmap();

    QDomElement imageElement;
    QDomNodeList childNodes = parentE.childNodes();
    for(int i=0; childNodes.count(); i++)
    {
        imageElement = childNodes.item(i).toElement();
        if(imageElement.tagName() == "image")
            break;
    }

    if(imageElement.tagName() != "image")
        return false;

    // modified from static QImage loadImageData()
    // function of Qt Designer Version 3.3.6
    // resource.cpp Line 2398
    QImage img;
    QString data = imageElement.firstChild().toText().data();
    const int lengthOffset = 4;
    int baSize = data.length() / 2 + lengthOffset;
    uchar *ba = new uchar[ baSize ];
    for ( int i = lengthOffset; i < baSize; ++i )
    {
        char h = data[ 2 * (i-lengthOffset) ].toLatin1();
        char l = data[ 2 * (i-lengthOffset) + 1 ].toLatin1();
        uchar r = 0;
        if ( h <= '9' )
            r += h - '0';
        else
            r += h - 'a' + 10;
        r = r << 4;
        if ( l <= '9' )
            r += l - '0';
        else
            r += l - 'a' + 10;
        ba[ i ] = r;
    }
    QString format = imageElement.attribute("format", "PNG");
    if(format == "XPM.GZ" || format == "XBM.GZ")
    {
        long len = imageElement.attribute("length").toULong();
        if ( len < data.length() * 5 )
            len = data.length() * 5;
        // qUncompress() expects the first 4 bytes to be the expected length of
        // the uncompressed data
        ba[0] = ( len & 0xff000000 ) >> 24;
        ba[1] = ( len & 0x00ff0000 ) >> 16;
        ba[2] = ( len & 0x0000ff00 ) >> 8;
        ba[3] = ( len & 0x000000ff );
        QByteArray baunzip = qUncompress( ba, baSize );
        img.loadFromData( (const uchar*)baunzip.data(), baunzip.size(), qPrintable(format.left(format.indexOf('.'))) );
    }
    else
    {
        img.loadFromData( (const uchar*)ba+lengthOffset, baSize-lengthOffset, qPrintable(format) );
    }
    delete [] ba;

    pixmap = pixmap.fromImage(img);
    return true;
}

bool GCF::Components::UtilsComponent::parseDom(QDomDocument doc, QStringList tagNames, QMap<QString, QList<QDomElement> > & elementMap)
{
    // Not implemented as yet.
    Q_UNUSED(doc);
    Q_UNUSED(tagNames);
    Q_UNUSED(elementMap);
    return false;
}

bool GCF::Components::UtilsComponent::saveData(QDomDocument doc, QDomElement element, QByteArray array)
{
    // Compress the array
    QByteArray ba = qCompress(array);

    // Store the byte array as a hex string
    QString baText;
    QTextStream ts( &baText );
    for(int i=0; i<ba.size(); i++)
    {
        uchar s = (uchar) ba[i];
        ts << GHexChars[s >> 4];
        ts << GHexChars[s & 0x0f];
    }

    // Store the hex string into XML
    QDomElement dataE = doc.createElement("ByteArray");
    element.appendChild(dataE);
    element.setAttribute("Length", ba.size());
    element.setAttribute("OrigLength", array.size());
    QDomCDATASection cDataE = doc.createCDATASection(baText);
    dataE.appendChild(cDataE);

    return true;
}

bool GCF::Components::UtilsComponent::loadData(QDomElement element, QByteArray& array)
{
    QDomElement dataE = findElement(element, "ByteArray");
    if( dataE.isNull() )
        return false;

    int baLength = element.attribute("Length").toInt();
    int origLength = element.attribute("OrigLength").toInt();
    uchar* ba = new uchar[ baLength ];
    QString baText = dataE.firstChild().toText().data();
    int baIndex = 0;

    for(int i=0; i<baText.length(); i+=2)
    {
        unsigned char baChar = 0;

        // First character
        const char c1 = baText[i].toAscii();
        baChar = CharIndex(c1) << 4;

        // Second character
        const char c2 = baText[i+1].toAscii();
        baChar = baChar + CharIndex(c2);

        ba[baIndex++] = baChar;
    }

    array = qUncompress(ba, baLength);
    delete [] ba;
    if( array.size() != origLength )
    {
        qDebug("Invalid uncompression %d/%d", array.size(), origLength);
        return false;
    }

    return true;
}

// IMathUtils
double GCF::Components::UtilsComponent::toRadians(double angle)
{
    return angle * 3.1415f / 180.0f;
}

double GCF::Components::UtilsComponent::toDegrees(double radians)
{
    return radians * 180.0f / 3.1415f;
}

double GCF::Components::UtilsComponent::sinAngle(double angle)
{
    return sin( toRadians(angle) );
}

double GCF::Components::UtilsComponent::cosAngle(double angle)
{
    return cos( toRadians(angle) );
}

double GCF::Components::UtilsComponent::tanAngle(double angle)
{
    return tan( toRadians(angle) );
}

double GCF::Components::UtilsComponent::aSinAngle(double val)
{
    return toDegrees( asin(val) );
}

double GCF::Components::UtilsComponent::aCosAngle(double val)
{
    return toDegrees( acos(val) );
}

double GCF::Components::UtilsComponent::aTanAngle(double val)
{
    return toDegrees( atan(val) );
}

double GCF::Components::UtilsComponent::randomNumber(double min, double max)
{
    double rand_max = double(RAND_MAX);
    double r = double( rand() );
    double value = (max - min) / (rand_max) * r + min;
    return value;
}

QString GCF::Components::UtilsComponent::productName() const
{
    return "Utils Component";
}

QString GCF::Components::UtilsComponent::organization() const
{
    return "VCreate Logic (P) Ltd";
}

QImage GCF::Components::UtilsComponent::programLogo() const
{
    return QImage();
}

QString GCF::Components::UtilsComponent::version() const
{
    return "1.0";
}

QString GCF::Components::UtilsComponent::shortDescription() const
{
    return "This component provides common math and XML utility functions for use by "
           "the rest of the system.";
}

QString GCF::Components::UtilsComponent::homepage() const
{
    return "http://www.vcreatelogic.com/oss/gcf";
}

QString GCF::Components::UtilsComponent::bugAddress() const
{
    return "http://www.vcreatelogic.com/oss/gcf/feedback/bugs";
}

const QList<GCF::AboutPerson> GCF::Components::UtilsComponent::authors() const
{
    static QList<GCF::AboutPerson> retList;
    if(retList.count() == 0)
    {
        retList << GCF::AboutPerson("Prashanth N Udupa", "prashanth@vcreatelogic.com", "http://www.vcreatelogic.com", "Founder, Vision, Design, Development");
        retList << GCF::AboutPerson("Lohith D. S", "NA", "http://www.vcreatelogic.com", "Design, Development");
    }
    return retList;
}

const QList<GCF::AboutPerson> GCF::Components::UtilsComponent::credits() const
{
    static QList<GCF::AboutPerson> retList;
    return retList;
}

QString GCF::Components::UtilsComponent::license() const
{
    return "Dual License: GPL Version 2 for FOSS development, Commercial for others.";
}

QString GCF::Components::UtilsComponent::copyrightStatement() const
{
    return QString();
}

QObject* GCF::Components::UtilsComponent::fetchObject(const QString& completeName) const
{
    QStringList comps = completeName.split('.');

	GCF::Components::UtilsComponent* that = const_cast<GCF::Components::UtilsComponent*>(this);

    if(comps.last() == "controller")
        return that;

    if(comps.last() == "xmlutils")
        return that;

    if(comps.last() == "mathutils")
        return that;

    return 0;
}

void GCF::Components::UtilsComponent::initializeComponent()
{
    // Do nothing
}

void GCF::Components::UtilsComponent::finalizeComponent()
{
    // Do nothing.
}

GCF_EXPORT_STD_COMPONENT_PLUGIN(UtilsComponent)

