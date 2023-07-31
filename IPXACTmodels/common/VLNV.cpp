//-----------------------------------------------------------------------------
// File: vlnv.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 23.6.2010
//
// Description:
// A class to hold VLNV information of a single IP-XACT document.
//-----------------------------------------------------------------------------

#include "VLNV.h"

#include <QString>
#include <QStringBuilder>
#include <QStringList>
#include <QObject>
#include <QMap>

namespace
{
    const QMap<QString, VLNV::IPXactType> types =
    {
        { QStringLiteral("ipxact:abstractionDefinition"), VLNV::ABSTRACTIONDEFINITION },
        { QStringLiteral("ipxact:abstractor"), VLNV::ABSTRACTOR },
        { QStringLiteral("ipxact:busDefinition"), VLNV::BUSDEFINITION },
        { QStringLiteral("ipxact:catalog"), VLNV::CATALOG },
        { QStringLiteral("ipxact:component"), VLNV::COMPONENT },
        { QStringLiteral("ipxact:design"), VLNV::DESIGN },
        { QStringLiteral("ipxact:designConfiguration"), VLNV::DESIGNCONFIGURATION },
        { QStringLiteral("ipxact:generatorChain"), VLNV::GENERATORCHAIN },
        { QStringLiteral("kactus2:comDefinition"), VLNV::COMDEFINITION },
        { QStringLiteral("kactus2:apiDefinition"), VLNV::APIDEFINITION },
        { QStringLiteral("invalid"), VLNV::INVALID }

    };
}

//-----------------------------------------------------------------------------
// Function: VLNV::VLNV()
//-----------------------------------------------------------------------------
VLNV::VLNV()
{

}

//-----------------------------------------------------------------------------
// Function: VLNV::VLNV()
//-----------------------------------------------------------------------------
VLNV::VLNV(QString const& type,
    QString const& vendor, QString const& library, QString const& name, QString const& version) :
    vendor_(vendor),
    library_(library),
    name_(name),
    version_(version)
{
	type_ = string2Type(type);
}

//-----------------------------------------------------------------------------
// Function: VLNV::VLNV()
//-----------------------------------------------------------------------------
VLNV::VLNV(IPXactType const& type,
    QString const& vendor, QString const& library, QString const& name, QString const& version) :
    vendor_(vendor),
    library_(library),
    name_(name),
    version_(version),
    type_(type)
{
}

//-----------------------------------------------------------------------------
// Function: VLNV::VLNV()
//-----------------------------------------------------------------------------
VLNV::VLNV(IPXactType const& type, QString const& parseStr, QString const& separator):
    type_(type)
{
    QStringList vlnvFields = parseStr.split(separator);
    if (vlnvFields.count() == 4)
    {
        vendor_ = vlnvFields.at(0);
        library_ = vlnvFields.at(1);
        name_ = vlnvFields.at(2);
        version_ = vlnvFields.at(3);
    }
}

//-----------------------------------------------------------------------------
// Function: VLNV::getVendor()
//-----------------------------------------------------------------------------
QString VLNV::getVendor() const
{
    return vendor_;
}

//-----------------------------------------------------------------------------
// Function: VLNV::setVendor()
//-----------------------------------------------------------------------------
void VLNV::setVendor(QString const& vendor)
{
    vendor_ = vendor.simplified();
}

//-----------------------------------------------------------------------------
// Function: VLNV::getLibrary()
//-----------------------------------------------------------------------------
QString VLNV::getLibrary() const
{
    return library_;
}

//-----------------------------------------------------------------------------
// Function: VLNV::setLibrary()
//-----------------------------------------------------------------------------
void VLNV::setLibrary(QString const& library)
{
    library_ = library.simplified();
}

//-----------------------------------------------------------------------------
// Function: VLNV::getName()
//-----------------------------------------------------------------------------
QString VLNV::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: VLNV::setName()
//-----------------------------------------------------------------------------
void VLNV::setName(QString const& name)
{
    name_ = name.simplified();
}

//-----------------------------------------------------------------------------
// Function: VLNV::getVersion()
//-----------------------------------------------------------------------------
QString VLNV::getVersion() const
{
    return version_;
}

//-----------------------------------------------------------------------------
// Function: VLNV::setVersion()
//-----------------------------------------------------------------------------
void VLNV::setVersion(QString const& version)
{
    version_ = version.simplified();
}

//-----------------------------------------------------------------------------
// Function: VLNV::getType()
//-----------------------------------------------------------------------------
VLNV::IPXactType VLNV::getType() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: VLNV::setType()
//-----------------------------------------------------------------------------
void VLNV::setType(IPXactType type)
{
    type_ = type;
}

//-----------------------------------------------------------------------------
// Function: VLNV::getTypestr()
//-----------------------------------------------------------------------------
QString VLNV::getTypestr() const
{
	return IPXactType2String(type_);
}

//-----------------------------------------------------------------------------
// Function: VLNV::isEmpty()
//-----------------------------------------------------------------------------
bool VLNV::isEmpty() const
{
    return vendor_.isEmpty() && library_.isEmpty() && name_.isEmpty() && version_.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: VLNV::clear()
//-----------------------------------------------------------------------------
void VLNV::clear()
{
    vendor_.clear();
    library_.clear();
    name_.clear();
    version_.clear();
    type_ = VLNV::INVALID;
}

//-----------------------------------------------------------------------------
// Function: VLNV::isValid()
//-----------------------------------------------------------------------------
bool VLNV::isValid() const 
{
    // If one of the identification fields is empty then this is invalid.
    return type_ != VLNV::INVALID && 
        !vendor_.isEmpty() && !library_.isEmpty() && !name_.isEmpty() && !version_.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: VLNV::isValid()
//-----------------------------------------------------------------------------
bool VLNV::isValid(QVector<QString>& errors, QString const& parentIdentifier) const
{
    bool valid = true;

    if (type_ == VLNV::INVALID)
    {
        errors.append(QObject::tr("The type of the vlnv is invalid within %1").arg(parentIdentifier));
        valid = false;
    }

    if (vendor_.isEmpty())
    {
        errors.append(QObject::tr("No vendor specified for vlnv within %1").arg(parentIdentifier));
        valid = false;
    }

    if (library_.isEmpty())
    {
        errors.append(QObject::tr("No library specified for vlnv within %1").arg(parentIdentifier));
        valid = false;
    }

    if (name_.isEmpty())
    {
        errors.append(QObject::tr("No name specified for vlnv within %1").arg(parentIdentifier));
        valid = false;
    }

    if (version_.isEmpty())
    {
        errors.append(QObject::tr("No version specified for vlnv within %1").arg(parentIdentifier));
        valid = false;
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: VLNV::toString()
//-----------------------------------------------------------------------------
QString VLNV::toString(QString const& separator) const
{
    return vendor_ % separator % library_ % separator % name_ % separator % version_;
}

//-----------------------------------------------------------------------------
// Function: VLNV::operator<()
//-----------------------------------------------------------------------------
bool VLNV::operator<(const VLNV &other) const
{
    int vendorResult = vendor_.compare(other.vendor_);

	if (vendorResult == 0)
    {
        int libraryResult = library_.compare(other.library_);
		
        if (libraryResult == 0)
        {
            int nameResult = name_.compare(other.name_);

			if (nameResult == 0)
            {
				return version_.compare(other.version_) < 0;
			}
			else
            {
				return (nameResult < 0);
			}
		}
		else
        {
			return (libraryResult < 0);
		}
	}
	else
    {
		return (vendorResult < 0);
	}
}

//-----------------------------------------------------------------------------
// Function: VLNV::operator>()
//-----------------------------------------------------------------------------
bool VLNV::operator>(const VLNV &other) const
{
	if (vendor_.compare(other.vendor_) == 0)
    {
		if (library_.compare(other.library_) == 0)
        {
			if (name_.compare(other.name_) == 0)
            {
				return version_.compare(other.version_) > 0;
			}
			else
            {
				return name_.compare(other.name_) > 0;
			}
		}
		else 
        {
			return library_.compare(other.library_) > 0;
		}
	}
	else
    {
		return vendor_.compare(other.vendor_) > 0;
	}
}

//-----------------------------------------------------------------------------
// Function: VLNV::operator==()
//-----------------------------------------------------------------------------
bool VLNV::operator==(VLNV const& other) const
{
	return vendor_.compare(other.vendor_, Qt::CaseInsensitive) == 0 &&
		library_.compare(other.library_, Qt::CaseInsensitive) == 0 &&
		name_.compare(other.name_, Qt::CaseInsensitive) == 0 &&
		version_.compare(other.version_, Qt::CaseInsensitive) == 0;
}

//-----------------------------------------------------------------------------
// Function: VLNV::operator!=()
//-----------------------------------------------------------------------------
bool VLNV::operator!=(VLNV const& other) const
{
	return !operator==(other);
}

//-----------------------------------------------------------------------------
// Function: VLNV::string2Type()
//-----------------------------------------------------------------------------
VLNV::IPXactType VLNV::string2Type(QString const& type)
{
    return types.value(type, INVALID);	
}

//-----------------------------------------------------------------------------
// Function: VLNV::IPXactType2String()
//-----------------------------------------------------------------------------
QString VLNV::IPXactType2String(IPXactType const& type)
{
    return types.key(type, QStringLiteral("invalid"));	
}
