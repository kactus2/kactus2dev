/* 
 *  Created on: 23.6.2010
 *  Author: Antti Kamppi
 */

#include "vlnv.h"

#include <QString>
#include <QStringList>
#include <QObject>

#include <QDomNode>
#include <QDomNamedNodeMap>

// constructor
VLNV::VLNV(const QString &type, 
		   const QString &vendor,
		   const QString &library,
		const QString &name, 
		const QString &version):
vendor_(vendor.simplified()), 
library_(library.simplified()), 
name_(name.simplified()),
version_(version.simplified()) {

	type_ = string2Type(type);
}

VLNV::VLNV(const IPXactType &type,
		   const QString &vendor,
		   const QString &library,
		const QString &name,
		const QString &version): 
vendor_(vendor.simplified()),
library_(library.simplified()), 
name_(name.simplified()), 
version_(version.simplified()), 
type_(type) {
}

// the destructor
VLNV::~VLNV() {
}

// the copy constructor
VLNV::VLNV(const VLNV &other): 
vendor_(other.vendor_.simplified()), 
library_(other.library_.simplified()),
name_(other.name_.simplified()), 
version_(other.version_.simplified()),
type_(other.type_) {
}

VLNV::VLNV(): 
vendor_(), 
library_(), 
name_(), 
version_(), 
type_(VLNV::INVALID) {
}

VLNV::VLNV( const IPXactType type, const QString& parseStr, const QString& separator /*= QString(":")*/ ):
vendor_(), 
	library_(), 
	name_(), 
	version_(), 
	type_(type) {

	QStringList fields = parseStr.split(separator, QString::SkipEmptyParts);
	
	if (fields.size() > 0) {
		vendor_ = fields.at(0);
	}
	if (fields.size() > 1) {
		library_ = fields.at(1);
	}
	if (fields.size() > 2) {
		name_ = fields.at(2);
	}
	if (fields.size() > 3) {
		version_ = fields.at(3);
	}
}

QString VLNV::getTypestr() const {
	return IPXactType2String(type_);
}

VLNV::IPXactType VLNV::getType() const {
	return type_;
}

QString VLNV::getVendor() const {
	return vendor_;
}

QString VLNV::getLibrary() const {
	return library_;
}

QString VLNV::getName() const {
	return name_;
}

QString VLNV::getVersion() const {
	return version_;
}

// the = operator
VLNV &VLNV::operator=(const VLNV &other) {
	// if we are not assigning to self
	if (this != &other) {
		vendor_ = other.vendor_.simplified();
		library_ = other.library_.simplified();
		name_ = other.name_.simplified();
		version_ = other.version_.simplified();
		type_ = other.type_;
	}
	return *this;
}

// the < comparison operator
bool VLNV::operator<(const VLNV &other) const {
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

// the > comparison operator
bool VLNV::operator>(const VLNV &other) const {
	if (0 == vendor_.compare(other.vendor_, Qt::CaseInsensitive)) {
		if (0 == library_.compare(other.library_, Qt::CaseInsensitive)) {
			if (0 == name_.compare(other.name_, Qt::CaseInsensitive)) {
				return version_.compare(other.version_, Qt::CaseInsensitive) > 0;
			}
			else {
				return name_.compare(other.name_, Qt::CaseInsensitive) > 0;
			}
		}
		else {
			return library_.compare(other.library_, Qt::CaseInsensitive) > 0;
		}
	}
	else {
		return vendor_.compare(other.vendor_, Qt::CaseInsensitive) > 0;
	}
}

// the == operator
bool VLNV::operator==(const VLNV &other) const {
	if ((0 == vendor_.compare(other.vendor_, Qt::CaseInsensitive)) &&
		(0 == library_.compare(other.library_, Qt::CaseInsensitive)) &&
		(0 == name_.compare(other.name_, Qt::CaseInsensitive)) &&
		(0 == version_.compare(other.version_, Qt::CaseInsensitive))) {
		return true;
	}
	else {
		return false;
	}
}

// the != operator
bool VLNV::operator!=(const VLNV &other) const {
	if (0 != vendor_.compare(other.vendor_, Qt::CaseInsensitive))
		return true;
	else if (0 != library_.compare(other.library_, Qt::CaseInsensitive))
		return true;
	else if (0 != name_.compare(other.name_, Qt::CaseInsensitive))
		return true;
	else if (0 != version_.compare(other.version_, Qt::CaseInsensitive))
		return true;
	else
		return false;
}

//-----------------------------------------------------------------------------
// Function: VLNV::writeAsElements()
//-----------------------------------------------------------------------------
void VLNV::writeAsElements(QXmlStreamWriter& writer) const
{
    writer.writeTextElement(SPIRIT_VENDOR, getVendor());
    writer.writeTextElement(SPIRIT_LIBRARY, getLibrary());
    writer.writeTextElement(SPIRIT_NAME, getName());
    writer.writeTextElement(SPIRIT_VERSION, getVersion());
}

//-----------------------------------------------------------------------------
// Function: VLNV::writeAsAttributes()
//-----------------------------------------------------------------------------
void VLNV::writeAsAttributes(QXmlStreamWriter& writer) const
{
    writer.writeAttribute(SPIRIT_VENDOR, getVendor());
    writer.writeAttribute(SPIRIT_LIBRARY, getLibrary());
    writer.writeAttribute(SPIRIT_NAME, getName());
    writer.writeAttribute(SPIRIT_VERSION, getVersion());
}

// static function
VLNV::IPXactType VLNV::string2Type(const QString &type) {

	// find the correct enum type

	// if abstractionDefinition
	if (QString::compare(type, QObject::tr("spirit:abstractionDefinition"),
			Qt::CaseInsensitive) == 0) {
		return ABSTRACTIONDEFINITION;
	}

	// if busdefinition
	else if (QString::compare(type, QObject::tr("spirit:busDefinition"),
			Qt::CaseInsensitive) == 0)  {
		return BUSDEFINITION;
	}

	// if abstractor
	else if (QString::compare(type, QObject::tr("spirit:abstractor"),
			Qt::CaseInsensitive) == 0)  {
		return ABSTRACTOR;
	}

	// if component
	else if (QString::compare(type, QObject::tr("spirit:component"),
			Qt::CaseInsensitive) == 0)  {
		return COMPONENT;
	}

	// if design
	else if (QString::compare(type, QObject::tr("spirit:design"),
			Qt::CaseInsensitive) == 0) {
		return DESIGN;
	}

	// if designConfiguration
	else if (QString::compare(type, QObject::tr("spirit:designConfiguration"),
			Qt::CaseInsensitive) == 0)  {
		return DESIGNCONFIGURATION;
	}

	// if generatorChain
	else if (QString::compare(type, QObject::tr("spirit:generatorChain"),
			Qt::CaseInsensitive) == 0) {
		return GENERATORCHAIN;
	}

    // if comDefinition
    else if (QString::compare(type, QObject::tr("kactus2:comDefinition"),
                              Qt::CaseInsensitive) == 0) {
        return COMDEFINITION;
    }

    // if apiDefinition
    else if (QString::compare(type, QObject::tr("kactus2:apiDefinition"),
                              Qt::CaseInsensitive) == 0) {
        return APIDEFINITION;
    }

	// if the string was unrecognizable
	else {
		return INVALID;
	}
}

// static function
QString VLNV::IPXactType2String(const IPXactType &type) {
	switch (type) {
	case BUSDEFINITION:
		return QObject::tr("spirit:busDefinition");
	case COMPONENT:
		return QObject::tr("spirit:component");
	case DESIGN:
		return QObject::tr("spirit:design");
	case GENERATORCHAIN:
		return QObject::tr("spirit:generatorChain");
	case ABSTRACTOR:
		return QObject::tr("spirit:abstractor");
	case DESIGNCONFIGURATION:
		return QObject::tr("spirit:designConfiguration");
	case ABSTRACTIONDEFINITION:
		return QObject::tr("spirit:abstractionDefinition");
    case COMDEFINITION:
        return QObject::tr("kactus2:comDefinition");
    case APIDEFINITION:
        return QObject::tr("kactus2:apiDefinition");
	default:
		return QObject::tr("invalid");
	}
}

QString VLNV::type2Print(const IPXactType &type) {
	switch (type) {
	case BUSDEFINITION:
		return QObject::tr("busDefinition");
	case COMPONENT:
		return QObject::tr("component");
	case DESIGN:
		return QObject::tr("design");
	case GENERATORCHAIN:
		return QObject::tr("generatorChain");
	case ABSTRACTOR:
		return QObject::tr("abstractor");
	case DESIGNCONFIGURATION:
		return QObject::tr("designConfiguration");
	case ABSTRACTIONDEFINITION:
		return QObject::tr("abstractionDefinition");
    case COMDEFINITION:
        return QObject::tr("comDefinition");
    case APIDEFINITION:
        return QObject::tr("apiDefinition");
	default:
		return QObject::tr("invalid");
	}
}

QString VLNV::type2Show(const IPXactType &type) {
    switch (type) {
    case BUSDEFINITION:
        return QObject::tr("Bus Definition");
    case COMPONENT:
        return QObject::tr("Component");
    case DESIGN:
        return QObject::tr("Design");
    case GENERATORCHAIN:
        return QObject::tr("Generator Chain");
    case ABSTRACTOR:
        return QObject::tr("Abstractor");
    case DESIGNCONFIGURATION:
        return QObject::tr("Design Configuration");
    case ABSTRACTIONDEFINITION:
        return QObject::tr("Abstraction Definition");
    case COMDEFINITION:
        return QObject::tr("COM Definition");
    case APIDEFINITION:
        return QObject::tr("API Definition");
    default:
        return QObject::tr("Invalid");
    }
}

//-----------------------------------------------------------------------------
// Function: vlnv::createVLNV()
//-----------------------------------------------------------------------------
VLNV VLNV::createVLNV(const QDomNode& node, IPXactType type)
{
    // the vlnv info is found as attributes in the node
    QDomNamedNodeMap attributeMap = node.attributes();
    QString vendor = attributeMap.namedItem(SPIRIT_VENDOR).nodeValue();
    QString library = attributeMap.namedItem(SPIRIT_LIBRARY).nodeValue();
    QString name = attributeMap.namedItem(SPIRIT_NAME).nodeValue();
    QString version = attributeMap.namedItem(SPIRIT_VERSION).nodeValue();

    // if invalid vlnv tag
    if (vendor.isNull() || library.isNull() || name.isNull() || version.isNull() ) 
    {
        return VLNV();
    }

    return VLNV(type, vendor, library, name, version);
}

QString VLNV::createDirPath() const {
	QString path;
	path += vendor_;
	path += QString("/") += library_;
	path += QString("/") += name_;
	path += QString("/") += version_;
	return path;
}

QString VLNV::createString(QString& separator) const {
	QString str(QObject::tr("Vendor: "));
	str += vendor_;
	str += separator;

	str += QObject::tr("Library: ");
	str += library_;
	str += separator;

	str += QObject::tr("Name: ");
	str += name_;
	str += separator;

	str += QObject::tr("Version: ");
	str += version_;
	return str;
}

QString VLNV::toString( const QString& separator /*= QString(":")*/ ) const {
	QString str(vendor_);
	str += separator;
	str += library_;
	str += separator;
	str += name_;
	str += separator;
	str += version_;
	return str;
}

void VLNV::setName( const QString name ) {
	name_ = name.simplified();
}

bool VLNV::isEmpty() const {

    // if all of the identification fields are empty then this is empty.
    return vendor_.isEmpty() && library_.isEmpty() && name_.isEmpty() &&
           version_.isEmpty();
}

bool VLNV::isValid() const {

	// if type is invalid then automatically return false
	if (type_ == VLNV::INVALID)
		return false;

	// if one of the identification fields is empty then this is invalid
	return !vendor_.isEmpty() && !library_.isEmpty() && !name_.isEmpty() &&
		!version_.isEmpty();
}

bool VLNV::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;

	if (type_ == VLNV::INVALID) {
		errorList.append(QObject::tr("The type of the vlnv is invalid within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (vendor_.isEmpty()) {
		errorList.append(QObject::tr("No vendor specified for vlnv within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (library_.isEmpty()) {
		errorList.append(QObject::tr("No library specified for vlnv within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for vlnv within %1").arg(
			parentIdentifier));
		valid = false;
	}
	
	if (version_.isEmpty()) {
		errorList.append(QObject::tr("No version specified for vlnv within %1").arg(
			parentIdentifier));
		valid = false;
	}

	return valid;
}

QString VLNV::getElement( int column ) const {
	
	switch (column) {
		case 0: 
			return VLNV::type2Print(type_);
		case 1:
			return vendor_;
		case 2:
			return library_;
		case 3:
			return name_;
		case 4:
			return version_;
		default:
			return QString();
	}
}

void VLNV::setVersion( const QString version ) {
	version_ = version.simplified();
}

void VLNV::clear() {
	vendor_.clear();
	library_.clear();
	name_.clear();
	version_.clear();
	type_ = VLNV::INVALID;
}

void VLNV::setType( IPXactType type ) {
	type_ = type;
}

void VLNV::setType( const QString& type ) {

	if (type == QString("busDefinition"))
		type_ = VLNV::BUSDEFINITION;
	else if (type == QString("component"))
		type_ = VLNV::COMPONENT;
	else if (type == QString("design"))
		type_ = VLNV::DESIGN;
	else if (type == QString("generatorChain"))
		type_ = VLNV::GENERATORCHAIN;
	else if (type == QString("abstractor"))
		type_ = VLNV::ABSTRACTOR;
	else if (type == QString("designConfiguration"))
		type_ = VLNV::DESIGNCONFIGURATION;
	else if (type == QString("abstractionDefinition"))
		type_ = VLNV::ABSTRACTIONDEFINITION;
    else if (type == QString("comDefinition"))
        type_ = VLNV::COMDEFINITION;
    else if (type == QString("apiDefinition"))
        type_ = VLNV::APIDEFINITION;
	else
		type_ = VLNV::INVALID;
}

void VLNV::setVendor( const QString& vendor ) {
	vendor_ = vendor.simplified();
}

void VLNV::setLibrary( const QString& library ) {
	library_ = library.simplified();
}
