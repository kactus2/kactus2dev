//-----------------------------------------------------------------------------
// File: RegisterFile.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Describes the ipxact:registerFile element.
//-----------------------------------------------------------------------------

#include "Register.h"
#include "RegisterFile.h"

#include <QString>
#include <QMap>
#include <QSharedPointer>

/*
RegisterFile::RegisterFile(QDomNode& registerNode): RegisterModel(registerNode),
dim_(-1), addressOffset_(), typeIdentifier_(), range_(0), rangeAttributes_(),
registerData_(),
vendorExtensions_()
{

	for (int i = 0; i < registerNode.childNodes().count(); ++i) {
		QDomNode tempNode = registerNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:dim")) {
			dim_ = tempNode.childNodes().at(0).nodeValue().toInt();
		}
		else if (tempNode.nodeName() == QString("spirit:addressOffset")) {
			addressOffset_ = tempNode.childNodes().at(0).nodeValue();
			addressOffset_ = XmlUtils::removeWhiteSpace(addressOffset_);
		}
		else if (tempNode.nodeName() == QString("spirit:typeIdentifier")) {
			typeIdentifier_ = tempNode.childNodes().at(0).nodeValue();
			typeIdentifier_ = XmlUtils::removeWhiteSpace(typeIdentifier_);
		}
		else if (tempNode.nodeName() == QString("spirit:range")) {
			range_ = tempNode.childNodes().at(0).nodeValue().toInt();
			rangeAttributes_ = XmlUtils::parseAttributes(tempNode);
		}
		else if (tempNode.nodeName() == QString("spirit:register")) {
			
			Register* reg = new Register(tempNode);
			registerData_.insert(reg->name(), QSharedPointer<Register>(
				reg));
		}
		else if (tempNode.nodeName() == QString("spirit:registerFile")) {
			RegisterFile* reg = new RegisterFile(tempNode);

			registerData_.insert(reg->name(),
				QSharedPointer<RegisterFile>(reg));
		}
        else if (tempNode.nodeName() == QString("spirit:vendorExtensions")) 
        {
            int extensionCount = tempNode.childNodes().count();
            for (int j = 0; j < extensionCount; ++j) {
                QDomNode extensionNode = tempNode.childNodes().at(j);
                vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extensionNode)));
            }
        }
	}
	return;
}*/

//-----------------------------------------------------------------------------
// Function: RegisterFile::RegisterFile()
//-----------------------------------------------------------------------------
RegisterFile::RegisterFile(QString const& name, QString const& addressOffset, QString const& range):
RegisterBase(name),
dimension_(),
addressOffset_(addressOffset),
range_(range),
registerData_(new QList<QSharedPointer<RegisterBase> > ())
{

}

//-----------------------------------------------------------------------------
// Function: RegisterFile::RegisterFile()
//-----------------------------------------------------------------------------
RegisterFile::RegisterFile(const RegisterFile& other):
RegisterBase(other),
dimension_(other.dimension_),
addressOffset_(other.addressOffset_),
range_(other.range_),
registerData_(new QList<QSharedPointer<RegisterBase> > ())
{
    copyRegisterData(other);
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::operator=()
//-----------------------------------------------------------------------------
RegisterFile& RegisterFile::operator=(const RegisterFile& other)
{
    if (this != &other)
    {
        RegisterBase::operator=(other);
        dimension_ = other.dimension_;
        addressOffset_ = other.addressOffset_;
        range_ = other.range_;
        
        registerData_->clear();
        copyRegisterData(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::~RegisterFile()
//-----------------------------------------------------------------------------
RegisterFile::~RegisterFile()
{

}

//-----------------------------------------------------------------------------
// Function: RegisterFile::clone()
//-----------------------------------------------------------------------------
QSharedPointer<RegisterBase> RegisterFile::clone()
{
    return QSharedPointer<RegisterBase>(new RegisterFile(*this));
}

/*
bool RegisterFile::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList& errorList,
						   const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("register file %1").arg(name()));

	if (name().isEmpty()) {
		errorList.append(QObject::tr("No name specified for register file"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	if (addressOffset_.isEmpty()) {
		errorList.append(QObject::tr("No address offset set for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	if (range_ == 0) {
		errorList.append(QObject::tr("No range set for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	foreach (QSharedPointer<RegisterModel> regModel, registerData_) {
		if (!regModel->isValid(componentChoices, errorList, thisIdentifier)) {
			valid = false;
		}
	}

//     ParameterValidator validator;
//     foreach (QSharedPointer<Parameter> param, parameters_)
//     {
//         errorList.append(validator.findErrorsIn(param.data(), parentIdentifier, componentChoices));
//         if (!validator.validate(param.data(), componentChoices)) 
//         {
//             valid = false;
//         }
// 	}

	return valid;
}*/
/*
bool RegisterFile::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const
{
	if (name().isEmpty())
    {
		return false;
	}

	if (addressOffset_.isEmpty())
    {
		return false;
	}

	if (range_ == 0)
    {
		return false;
	}

	foreach (QSharedPointer<RegisterModel> regModel, registerData_)
    {
		if (!regModel->isValid(componentChoices)) {
			return false;
		}
	}

//     ParameterValidator validator;
//     foreach (QSharedPointer<Parameter> param, parameters_)
//     {
//         if (!validator.validate(param.data(), componentChoices)) 
//         {
//             return false;
//         }
//     }
	return true;
}*/

//-----------------------------------------------------------------------------
// Function: RegisterFile::()
//-----------------------------------------------------------------------------
QString RegisterFile::getDimension() const
{
    return dimension_;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::setDimension()
//-----------------------------------------------------------------------------
void RegisterFile::setDimension(QString const& newDimension)
{
    dimension_ = newDimension;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::getAddressOffset()
//-----------------------------------------------------------------------------
QString RegisterFile::getAddressOffset() const
{
    return addressOffset_;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::setAddressOffset()
//-----------------------------------------------------------------------------
void RegisterFile::setAddressOffset(QString const& newAddressOffset)
{
    addressOffset_ = newAddressOffset;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::getRange()
//-----------------------------------------------------------------------------
QString RegisterFile::getRange() const
{
    return range_;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::setRange()
//-----------------------------------------------------------------------------
void RegisterFile::setRange(QString const& newRange)
{
    range_ = newRange;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::getRegisterData()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<RegisterBase> > > RegisterFile::getRegisterData() const
{
    return registerData_;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::setRegisterData()
//-----------------------------------------------------------------------------
void RegisterFile::setRegisterData(QSharedPointer<QList<QSharedPointer<RegisterBase> > > newRegisterData)
{
    registerData_ = newRegisterData;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::copyRegisterData()
//-----------------------------------------------------------------------------
void RegisterFile::copyRegisterData(const RegisterFile& other)
{
    foreach (QSharedPointer<RegisterBase> registerBase, *other.registerData_)
    {
        if (registerBase)
        {
            QSharedPointer<RegisterBase> copy = registerBase->clone();

            registerData_->append(copy);
        }
    }
}
