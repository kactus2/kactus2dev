//-----------------------------------------------------------------------------
// File: Register.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Describes the ipxact:register element.
//-----------------------------------------------------------------------------

#include "Register.h"
#include "AlternateRegister.h"

/*
//-----------------------------------------------------------------------------
// Function: Register::Register()
//-----------------------------------------------------------------------------
Register::Register(QDomNode& registerNode):
RegisterModel(registerNode),
dim_(-1),
addressOffset_(),
alternateRegisters_(), 
registerDefinition_(registerNode),
vendorExtensions_(),
dimensionExpression_(),
isPresentExpression_()
{

	for (int i = 0; i < registerNode.childNodes().count(); ++i) {
		QDomNode tempNode = registerNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:dim")) {
			dim_ = tempNode.childNodes().at(0).nodeValue().toInt();
		}
		else if (tempNode.nodeName() == QString("spirit:addressOffset")) {
			addressOffset_ = tempNode.childNodes().at(0).nodeValue();
			//addressOffset_ = XmlUtils::removeWhiteSpace(addressOffset_);
		}
		else if (tempNode.nodeName() == QString("spirit:alternateRegisters")) {

			// parse each alternate register
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode alternateRegNode = tempNode.childNodes().at(j);

				alternateRegisters_.append(QSharedPointer<AlternateRegister>(
						new AlternateRegister(alternateRegNode)));
			}
		}
        else if (tempNode.nodeName() == QString("spirit:vendorExtensions")) 
        {
            parseVendorExtensions(tempNode);
        }
	}
}*/

//-----------------------------------------------------------------------------
// Function: Register::Register()
//-----------------------------------------------------------------------------
Register::Register(QString const& name, QString const& addressOffset, QString const& size):
RegisterDefinition(name),
dimension_(),
addressOffset_(addressOffset),
size_(size),
alternateRegisters_(new QList<QSharedPointer<AlternateRegister> > ())
{

}

//-----------------------------------------------------------------------------
// Function: Register::Register()
//-----------------------------------------------------------------------------
Register::Register(const Register& other):
RegisterDefinition(other),
dimension_(other.dimension_),
addressOffset_(other.addressOffset_),
size_(other.size_),
alternateRegisters_(new QList<QSharedPointer<AlternateRegister> > ())
{
    copyAlternateRegisters(other);
}

//-----------------------------------------------------------------------------
// Function: Register::operator=()
//-----------------------------------------------------------------------------
Register& Register::operator=(const Register& other)
{
    if (this != &other)
    {
        RegisterDefinition::operator=(other);
        dimension_ = other.dimension_;
        addressOffset_ = other.addressOffset_;
        size_ = other.size_;
        
        alternateRegisters_->clear();
        copyAlternateRegisters(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: Register::~Register()
//-----------------------------------------------------------------------------
Register::~Register()
{
    alternateRegisters_.clear();
}

//-----------------------------------------------------------------------------
// Function: Register::clone()
//-----------------------------------------------------------------------------
QSharedPointer<RegisterBase> Register::clone()
{
    return QSharedPointer<RegisterBase>(new Register(*this));
}

/*
//-----------------------------------------------------------------------------
// Function: Register::isValid()
//-----------------------------------------------------------------------------
bool Register::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList& errorList,QString const& parentIdentifier ) const
{
	bool valid = true;
	const QString thisIdentifier(QObject::tr("register %1").arg(name()));

	if (name().isEmpty())
    {
		errorList.append(QObject::tr("No name specified for register within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (addressOffset_.isEmpty())
    {
		errorList.append(QObject::tr("No address offset set for register %1"
			" within %2").arg(name()).arg(parentIdentifier));
		valid = false;
	}

	if (!registerDefinition_.isValid(componentChoices, errorList, thisIdentifier))
    {
			valid = false;
	}

	foreach (QSharedPointer<AlternateRegister> alterRegister, alternateRegisters_)
    {
		if (!alterRegister->isValid(componentChoices, errorList, thisIdentifier))
        {
			valid = false;
		}
	}

//     ParameterValidator validator;
//     foreach (QSharedPointer<Parameter> param, parameters_)
//     {
//         errorList.append(validator.findErrorsIn(param.data(), thisIdentifier, componentChoices));
// 
//         if (!validator.validate(param.data(), componentChoices)) 
//         {
//             valid = false;
//         }
// 	}

	return valid;
}*/
/*
//-----------------------------------------------------------------------------
// Function: Register::isValid()
//-----------------------------------------------------------------------------
bool Register::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const
{
	if (name().isEmpty()) 
    {
		return false;
	}

	if (addressOffset_.isEmpty())
    {
		return false;
	}

	if (!registerDefinition_.isValid(componentChoices))
    {
		return false;
	}

	foreach (QSharedPointer<AlternateRegister> alterRegister, alternateRegisters_)
    {
		if (!alterRegister->isValid(componentChoices))
        {
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
// Function: Register::getDim()
//-----------------------------------------------------------------------------
QString Register::getDimension() const
{
    return dimension_;
}

//-----------------------------------------------------------------------------
// Function: Register::setDim()
//-----------------------------------------------------------------------------
void Register::setDimension(QString const& newDimension)
{
    dimension_ = newDimension;
}

//-----------------------------------------------------------------------------
// Function: Register::getAddressOffset()
//-----------------------------------------------------------------------------
QString Register::getAddressOffset() const
{
    return addressOffset_;
}

//-----------------------------------------------------------------------------
// Function: Register::setAddressOffset()
//-----------------------------------------------------------------------------
void Register::setAddressOffset(QString const& addressOffset)
{
    addressOffset_ = addressOffset;
}

//-----------------------------------------------------------------------------
// Function: Register::getAlternateRegisters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<AlternateRegister> > > Register::getAlternateRegisters() const
{
    return alternateRegisters_;
}

//-----------------------------------------------------------------------------
// Function: Register::setAlternateRegisters()
//-----------------------------------------------------------------------------
void Register::setAlternateRegisters
    (QSharedPointer<QList<QSharedPointer<AlternateRegister> > > newAlternateRegisters)
{
    alternateRegisters_->clear();
    alternateRegisters_ = newAlternateRegisters;
}

//-----------------------------------------------------------------------------
// Function: Register::getSize()
//-----------------------------------------------------------------------------
QString Register::getSize() const
{
    return size_;
}

//-----------------------------------------------------------------------------
// Function: Register::setSize()
//-----------------------------------------------------------------------------
void Register::setSize(QString const& newSize)
{
    size_ = newSize;
}

//-----------------------------------------------------------------------------
// Function: Register::copyAlternateRegisters()
//-----------------------------------------------------------------------------
void Register::copyAlternateRegisters(const Register& other)
{
    foreach (QSharedPointer<AlternateRegister> alternateRegister, *other.alternateRegisters_)
    {
        if (alternateRegister)
        {
            QSharedPointer<AlternateRegister> copy =
                QSharedPointer<AlternateRegister>(new AlternateRegister(*alternateRegister.data()));
            alternateRegisters_->append(copy);
        }
    }
}