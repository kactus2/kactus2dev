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
#include <QStringList>
#include <QObject>

#include <QDomNode>
#include <QDomNamedNodeMap>

//-----------------------------------------------------------------------------
// Function: VLNV::VLNV()
//-----------------------------------------------------------------------------
VLNV::VLNV(): vendor_(), library_(), name_(), version_(), type_(VLNV::INVALID)
{
}

//-----------------------------------------------------------------------------
// Function: VLNV::VLNV()
//-----------------------------------------------------------------------------
VLNV::VLNV(QString const& type, 
    QString const& vendor, QString const& library, QString const& name, QString const& version):
vendor_(vendor.simplified()), 
library_(library.simplified()), 
name_(name.simplified()),
version_(version.simplified()),
type_(INVALID)
{
	type_ = string2Type(type);
}

//-----------------------------------------------------------------------------
// Function: VLNV::VLNV()
//-----------------------------------------------------------------------------
VLNV::VLNV(IPXactType const& type,
    QString const& vendor, QString const& library, QString const& name, QString const& version): 
vendor_(vendor.simplified()),
library_(library.simplified()), 
name_(name.simplified()), 
version_(version.simplified()), 
type_(type)
{
}

//-----------------------------------------------------------------------------
// Function: VLNV::VLNV()
//-----------------------------------------------------------------------------
VLNV::VLNV(IPXactType const& type, QString const& parseStr, QString const& separator): 
vendor_(), 
    library_(),
    name_(), 
    version_(),
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
// Function: VLNV::VLNV()
//-----------------------------------------------------------------------------
VLNV::VLNV(const VLNV &other): 
vendor_(other.vendor_.simplified()), 
    library_(other.library_.simplified()),
    name_(other.name_.simplified()), 
    version_(other.version_.simplified()),
    type_(other.type_)
{
}

//-----------------------------------------------------------------------------
// Function: VLNV::~VLNV()
//-----------------------------------------------------------------------------
VLNV::~VLNV()
{

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
    // if type is invalid then automatically return false
    if (type_ == VLNV::INVALID)
    {
        return false;
    }

    // If one of the identification fields is empty then this is invalid.
    return !vendor_.isEmpty() && !library_.isEmpty() && !name_.isEmpty() && !version_.isEmpty();
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
    return vendor_ + separator + library_ + separator + name_ + separator + version_;
}

//-----------------------------------------------------------------------------
// Function: VLNV::operator=()
//-----------------------------------------------------------------------------
VLNV& VLNV::operator=(VLNV const& other)
{
	// if we are not assigning to self
	if (this != &other)
    {
		vendor_ = other.vendor_.simplified();
		library_ = other.library_.simplified();
		name_ = other.name_.simplified();
		version_ = other.version_.simplified();
		type_ = other.type_;
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: VLNV::operator<()
//-----------------------------------------------------------------------------
bool VLNV::operator<(const VLNV &other) const
{
    int vendorResult = vendor_.compare(other.vendor_, Qt::CaseInsensitive);

	if (vendorResult == 0)
    {
        int libraryResult = library_.compare(other.library_, Qt::CaseInsensitive);
		
        if (libraryResult == 0)
        {
            int nameResult = name_.compare(other.name_, Qt::CaseInsensitive);

			if (nameResult == 0)
            {
				return version_.compare(other.version_, Qt::CaseInsensitive) < 0;
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
	if (vendor_.compare(other.vendor_, Qt::CaseInsensitive) == 0)
    {
		if (library_.compare(other.library_, Qt::CaseInsensitive) == 0)
        {
			if (name_.compare(other.name_, Qt::CaseInsensitive) == 0)
            {
				return version_.compare(other.version_, Qt::CaseInsensitive) > 0;
			}
			else
            {
				return name_.compare(other.name_, Qt::CaseInsensitive) > 0;
			}
		}
		else 
        {
			return library_.compare(other.library_, Qt::CaseInsensitive) > 0;
		}
	}
	else
    {
		return vendor_.compare(other.vendor_, Qt::CaseInsensitive) > 0;
	}
}

//-----------------------------------------------------------------------------
// Function: VLNV::operator==()
//-----------------------------------------------------------------------------
bool VLNV::operator==(VLNV const& other) const
{
	return (vendor_.compare(other.vendor_, Qt::CaseInsensitive) == 0) &&
		(library_.compare(other.library_, Qt::CaseInsensitive) == 0) &&
		(name_.compare(other.name_, Qt::CaseInsensitive) == 0) &&
		(version_.compare(other.version_, Qt::CaseInsensitive) == 0);
}

//-----------------------------------------------------------------------------
// Function: VLNV::operator!=()
//-----------------------------------------------------------------------------
bool VLNV::operator!=(VLNV const& other) const
{
	return (vendor_.compare(other.vendor_, Qt::CaseInsensitive) != 0) ||
        (library_.compare(other.library_, Qt::CaseInsensitive) != 0) ||
        (name_.compare(other.name_, Qt::CaseInsensitive) != 0) ||
        (version_.compare(other.version_, Qt::CaseInsensitive) != 0);
}

//-----------------------------------------------------------------------------
// Function: VLNV::string2Type()
//-----------------------------------------------------------------------------
VLNV::IPXactType VLNV::string2Type(QString const& type)
{
	if (type.compare(QLatin1String("ipxact:abstractionDefinition"), Qt::CaseInsensitive) == 0)
    {
		return ABSTRACTIONDEFINITION;
	}
	else if (type.compare(QLatin1String("ipxact:busDefinition"), Qt::CaseInsensitive) == 0)
    {
		return BUSDEFINITION;
	}
	else if (type.compare(QLatin1String("ipxact:abstractor"), Qt::CaseInsensitive) == 0)
    {
		return ABSTRACTOR;
	}
	else if (type.compare(QLatin1String("ipxact:component"), Qt::CaseInsensitive) == 0) 
    {
		return COMPONENT;
	}
	else if (type.compare(QLatin1String("ipxact:design"), Qt::CaseInsensitive) == 0)
    {
		return DESIGN;
	}
	else if (type.compare(QLatin1String("ipxact:designConfiguration"), Qt::CaseInsensitive) == 0)
    {
		return DESIGNCONFIGURATION;
	}
	else if (type.compare(QLatin1String("ipxact:generatorChain"), Qt::CaseInsensitive) == 0)
    {
		return GENERATORCHAIN;
	}
    else if (type.compare(QLatin1String("kactus2:comDefinition"), Qt::CaseInsensitive) == 0)
    {
        return COMDEFINITION;
    }
    else if (type.compare(QLatin1String("kactus2:apiDefinition"), Qt::CaseInsensitive) == 0)
    {
        return APIDEFINITION;
    }
	// if the string was unrecognizable
	else
    {
		return INVALID;
	}
}

//-----------------------------------------------------------------------------
// Function: VLNV::IPXactType2String()
//-----------------------------------------------------------------------------
QString VLNV::IPXactType2String(IPXactType const& type)
{
	if (type == BUSDEFINITION)
    {
        return QStringLiteral("ipxact:busDefinition");
    }
    else if (type == COMPONENT)
    {
        return QStringLiteral("ipxact:component");
    }
    else if (type == DESIGN)
    {
        return QStringLiteral("ipxact:design");
    }
    else if (type == GENERATORCHAIN)
    {
        return QStringLiteral("ipxact:generatorChain");
    }
    else if (type == ABSTRACTOR)
    {
        return QStringLiteral("ipxact:abstractor");
    }
    else if (type == DESIGNCONFIGURATION)
    {
        return QStringLiteral("ipxact:designConfiguration");
    }
    else if (type == ABSTRACTIONDEFINITION)
    {
        return QStringLiteral("ipxact:abstractionDefinition");
    }
    else if (type == COMDEFINITION)
    {
        return QStringLiteral("kactus2:comDefinition");
    }
    else if (type == APIDEFINITION)
    {
        return QStringLiteral("kactus2:apiDefinition");
    }
    else
    {
        return QStringLiteral("invalid");
	}
}

//-----------------------------------------------------------------------------
// Function: vlnv::createVLNV()
//-----------------------------------------------------------------------------
VLNV VLNV::createVLNV(const QDomNode& node, IPXactType type)
{
    // the vlnv info is found as attributes in the node
    QDomNamedNodeMap attributeMap = node.attributes();
    QString vendor = attributeMap.namedItem("vendor").nodeValue();
    QString library = attributeMap.namedItem("library").nodeValue();
    QString name = attributeMap.namedItem("name").nodeValue();
    QString version = attributeMap.namedItem("version").nodeValue();

    // if invalid vlnv tag
    if (vendor.isEmpty() || library.isEmpty() || name.isEmpty() || version.isEmpty()) 
    {
        return VLNV();
    }

    return VLNV(type, vendor, library, name, version);
}
