//-----------------------------------------------------------------------------
// File: KactusAttribute.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.6.2011
//
// Description:
// Kactus attribute definitions and related utility functions.
//-----------------------------------------------------------------------------

#include "KactusAttribute.h"

namespace
{
    QString const PROD_HIER_NAMES[KactusAttribute::KTS_PRODHIER_COUNT] =
    {
        "Flat",
        "Product",
        "Board",
        "Chip",
        "SoC",
        "IP"
    };

    QString const FIRMNESS_NAMES[KactusAttribute::KTS_REUSE_LEVEL_COUNT] =
    {
        "Mutable",
        "Template",
        "Fixed"
    };

    QString const IMPLEMENTATION_NAMES[KactusAttribute::KTS_IMPLEMENTATION_COUNT] =
    {
        "HW",
        "SW",
        "SYS"
    };
}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::KactusAttribute()
//-----------------------------------------------------------------------------
KactusAttribute::KactusAttribute():
productHierarchy_(KactusAttribute::KTS_PRODHIER_COUNT),
    implementation_(KactusAttribute::KTS_IMPLEMENTATION_COUNT),
    firmness_(KactusAttribute::KTS_REUSE_LEVEL_COUNT)
{

}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::~KactusAttribute()
//-----------------------------------------------------------------------------
KactusAttribute::~KactusAttribute()
{

}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::clone()
//-----------------------------------------------------------------------------
VendorExtension* KactusAttribute::clone() const
{
    return new KactusAttribute(*this);
}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::type()
//-----------------------------------------------------------------------------
QString KactusAttribute::type() const
{
    return QStringLiteral("kactus2:extensions");
}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::write()
//-----------------------------------------------------------------------------
void KactusAttribute::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:kts_attributes");

    if (productHierarchy_ != KTS_PRODHIER_COUNT)
    {
        writer.writeTextElement("kactus2:kts_productHier", hierarchyToString(productHierarchy_));
    }
   
    if (implementation_ != KTS_IMPLEMENTATION_COUNT)
    {
        writer.writeTextElement("kactus2:kts_implementation", implementationToString(implementation_));
    }
   
    if (firmness_ != KTS_REUSE_LEVEL_COUNT)
    {
        writer.writeTextElement("kactus2:kts_firmness", firmnessToString(firmness_));
    }

    writer.writeEndElement(); // kactus2:kts_attributes
}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::setHierarchy()
//-----------------------------------------------------------------------------
void KactusAttribute::setHierarchy(KactusAttribute::ProductHierarchy hierarchy)
{
    productHierarchy_ = hierarchy;
}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::getHierarchy()
//-----------------------------------------------------------------------------
KactusAttribute::ProductHierarchy KactusAttribute::getHierarchy() const
{
    return productHierarchy_;
}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::setImplementation()
//-----------------------------------------------------------------------------
void KactusAttribute::setImplementation(KactusAttribute::Implementation implementation)
{
    implementation_ = implementation;
}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::setImplementation()
//-----------------------------------------------------------------------------
KactusAttribute::Implementation KactusAttribute::getImplementation() const
{
    return implementation_;
}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::setFirmness()
//-----------------------------------------------------------------------------
void KactusAttribute::setFirmness(Firmness firmness)
{
    firmness_ = firmness;
}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::getFirmness()
//-----------------------------------------------------------------------------
KactusAttribute::Firmness KactusAttribute::getFirmness() const
{
    return firmness_;
}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::hierarchyFrom()
//-----------------------------------------------------------------------------
KactusAttribute::ProductHierarchy KactusAttribute::hierarchyFrom(QString const& string)
{
    QString lowStr = string.toLower();
    for (unsigned int i = 0; i < KactusAttribute::KTS_PRODHIER_COUNT; ++i)
    {
        if (lowStr == PROD_HIER_NAMES[i].toLower())
        {
            return static_cast<ProductHierarchy>(i);
        }
    }

    return KactusAttribute::KTS_PRODHIER_COUNT;
}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::implementationFrom()
//-----------------------------------------------------------------------------
KactusAttribute::Implementation KactusAttribute::implementationFrom(QString const& string)
{
    QString lowStr = string.toLower();

    for (unsigned int i = 0; i < KactusAttribute::KTS_IMPLEMENTATION_COUNT; i++)
    {
        if (lowStr == IMPLEMENTATION_NAMES[i].toLower())
        {
            return static_cast<KactusAttribute::Implementation>(i);            
        }
    }

    return KactusAttribute::KTS_IMPLEMENTATION_COUNT;
}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::firmnessFrom()
//-----------------------------------------------------------------------------
KactusAttribute::Firmness KactusAttribute::firmnessFrom(QString const& string)
{
    QString lowStr = string.toLower();

    for (unsigned int i = 0; i < KactusAttribute::KTS_REUSE_LEVEL_COUNT; ++i)
    {
        if (lowStr == FIRMNESS_NAMES[i].toLower())
        {
            return static_cast<Firmness>(i);
            
        }
    }

    return KactusAttribute::KTS_REUSE_LEVEL_COUNT;
}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::hierarchyToString()
//-----------------------------------------------------------------------------
QString KactusAttribute::hierarchyToString(ProductHierarchy prodHier)
{
    if (prodHier == KTS_PRODHIER_COUNT)
    {
        return QString();
    }

    return PROD_HIER_NAMES[prodHier];
}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::firmnessToString()
//-----------------------------------------------------------------------------
QString KactusAttribute::firmnessToString(Firmness firmness)
{
    if (firmness == KTS_REUSE_LEVEL_COUNT)
    {
        return QString();
    }

    return FIRMNESS_NAMES[firmness];
}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::implementationToString()
//-----------------------------------------------------------------------------
QString KactusAttribute::implementationToString(Implementation implementation)
{
    if (implementation == KTS_IMPLEMENTATION_COUNT)
    {
        return QString();
    }

    return IMPLEMENTATION_NAMES[implementation];
}

//-----------------------------------------------------------------------------
// Function: KactusAttribute::KactusAttribute()
//-----------------------------------------------------------------------------
KactusAttribute::KactusAttribute(KactusAttribute const& rhs):
productHierarchy_(rhs.productHierarchy_),
    implementation_(rhs.implementation_),
    firmness_(rhs.firmness_)
{

}
