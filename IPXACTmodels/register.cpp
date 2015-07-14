/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#include "register.h"
#include "generaldeclarations.h"
#include "alternateregister.h"
#include "parameter.h"
#include "registermodel.h"
#include "GenericVendorExtension.h"

#include <common/utils.h>
#include <IPXACTmodels/validators/ParameterValidator.h>

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QString>
#include <QList>
#include <QSharedPointer>
#include "XmlUtils.h"

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
}

//-----------------------------------------------------------------------------
// Function: Register::Register()
//-----------------------------------------------------------------------------
Register::Register():
RegisterModel(),
dim_(-1),
addressOffset_("'h0"),
alternateRegisters_(), 
registerDefinition_(),
vendorExtensions_(),
dimensionExpression_(),
isPresentExpression_()
{
}

//-----------------------------------------------------------------------------
// Function: Register::Register()
//-----------------------------------------------------------------------------
Register::Register( General::BooleanValue volatileValue, General::Access access ):
RegisterModel(),
dim_(-1),
addressOffset_(),
alternateRegisters_(), 
registerDefinition_(),
vendorExtensions_(),
dimensionExpression_(),
isPresentExpression_()
{
	registerDefinition_.setVolatile(volatileValue);
	registerDefinition_.setAccess(access);
}

//-----------------------------------------------------------------------------
// Function: Register::Register()
//-----------------------------------------------------------------------------
Register::Register( const Register& other ):
RegisterModel(other),
dim_(other.dim_),
addressOffset_(other.addressOffset_),
alternateRegisters_(),
registerDefinition_(other.registerDefinition_),
vendorExtensions_(),
dimensionExpression_(),
isPresentExpression_()
{
	foreach (QSharedPointer<AlternateRegister> altReg, other.alternateRegisters_)
    {
		if (altReg)
        {
			QSharedPointer<AlternateRegister> copy = QSharedPointer<AlternateRegister>(
				new AlternateRegister(*altReg.data()));
			alternateRegisters_.append(copy);
		}
	}

    copyVendorExtensions(other);
}

//-----------------------------------------------------------------------------
// Function: Register::operator=()
//-----------------------------------------------------------------------------
Register& Register::operator=( const Register& other )
{
	if (this != &other)
    {
		RegisterModel::operator=(other);

		dim_ = other.dim_;
		addressOffset_ = other.addressOffset_;
		registerDefinition_ = other.registerDefinition_;

		alternateRegisters_.clear();
		foreach (QSharedPointer<AlternateRegister> altReg, other.alternateRegisters_)
        {
			if (altReg)
            {
				QSharedPointer<AlternateRegister> copy = QSharedPointer<AlternateRegister>(
					new AlternateRegister(*altReg.data()));
				alternateRegisters_.append(copy);
			}
		}

        copyVendorExtensions(other);
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
QSharedPointer<RegisterModel> Register::clone()
{
	return QSharedPointer<RegisterModel>(new Register(*this));
}

//-----------------------------------------------------------------------------
// Function: Register::write()
//-----------------------------------------------------------------------------
void Register::write(QXmlStreamWriter& writer)
{
	writer.writeStartElement("spirit:register");

	// call base class to write base class info
	RegisterModel::write(writer);

	// if optional spirit:dim is defined
	if (dim_ >= 0)
    {
		writer.writeTextElement("spirit:dim", QString::number(dim_));
	}

	writer.writeTextElement("spirit:addressOffset", addressOffset_);

	// call registerDefinition to write itself
	registerDefinition_.write(writer);

	// if any alternate registers exist
	if (alternateRegisters_.size() != 0)
    {
		writer.writeStartElement("spirit:alternateRegisters");

		// write each alternate register
		for (int i = 0; i < alternateRegisters_.size(); ++i) {
			alternateRegisters_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:alternateRegisters
	}

    if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
        writer.writeEndElement(); // spirit:vendorExtensions
    }

	writer.writeEndElement(); // spirit:register
}

//-----------------------------------------------------------------------------
// Function: Register::isValid()
//-----------------------------------------------------------------------------
bool Register::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList& errorList,QString const& parentIdentifier ) const
{
	bool valid = true;
	const QString thisIdentifier(QObject::tr("register %1").arg(nameGroup_.name()));

	if (nameGroup_.name().isEmpty())
    {
		errorList.append(QObject::tr("No name specified for register within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (addressOffset_.isEmpty())
    {
		errorList.append(QObject::tr("No address offset set for register %1"
			" within %2").arg(nameGroup_.name()).arg(parentIdentifier));
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

    ParameterValidator validator;
    foreach (QSharedPointer<Parameter> param, parameters_)
    {
        errorList.append(validator.findErrorsIn(param.data(), thisIdentifier, componentChoices));

        if (!validator.validate(param.data(), componentChoices)) 
        {
            valid = false;
        }
	}

	return valid;
}

//-----------------------------------------------------------------------------
// Function: Register::isValid()
//-----------------------------------------------------------------------------
bool Register::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const
{
	if (nameGroup_.name().isEmpty()) 
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

    ParameterValidator validator;
    foreach (QSharedPointer<Parameter> param, parameters_)
    {
        if (!validator.validate(param.data(), componentChoices)) 
        {
            return false;
        }
    }

	return true;
}

//-----------------------------------------------------------------------------
// Function: Register::getAddressOffset()
//-----------------------------------------------------------------------------
QString Register::getAddressOffset() const
{
    return addressOffset_;
}

//-----------------------------------------------------------------------------
// Function: Register::getOffset()
//-----------------------------------------------------------------------------
quint64 Register::getOffset() const
{
	return General::str2Uint(addressOffset_);
}

//-----------------------------------------------------------------------------
// Function: Register::getAlternateRegisters()
//-----------------------------------------------------------------------------
const QList<QSharedPointer<AlternateRegister> >& Register::getAlternateRegisters() const
{
    return alternateRegisters_;
}

//-----------------------------------------------------------------------------
// Function: Register::getDim()
//-----------------------------------------------------------------------------
int Register::getDim() const
{
    return dim_;
}

//-----------------------------------------------------------------------------
// Function: register::setDimExpression()
//-----------------------------------------------------------------------------
void Register::setDimensionExpression(QString const& expression)
{
    if (dimensionExpression_.isNull())
    {
        createDimensionExpressionExtension(expression);
    }

    dimensionExpression_->setValue(expression);
}

//-----------------------------------------------------------------------------
// Function: register::getDimExpression()
//-----------------------------------------------------------------------------
QString Register::getDimensionExpression()
{
    if (hasDimensionExpression())
    {
        return dimensionExpression_->value();
    }
    else
    {
        return QString::number(getDim());
    }
}

//-----------------------------------------------------------------------------
// Function: register::removeDimExpression()
//-----------------------------------------------------------------------------
void Register::removeDimensionExpression()
{
    vendorExtensions_.removeAll(dimensionExpression_);
    dimensionExpression_.clear();
}

//-----------------------------------------------------------------------------
// Function: register::hasDimExpression()
//-----------------------------------------------------------------------------
bool Register::hasDimensionExpression()
{
    if (dimensionExpression_.isNull())
    {
        return false;
    }
    QString dimensionValue = dimensionExpression_->value();

    if (dimensionValue.isEmpty())
    {
        return false;
    }
    else
    {
        return true;
    }
}

//-----------------------------------------------------------------------------
// Function: register::setSizeExpression()
//-----------------------------------------------------------------------------
void Register::setSizeExpression(QString const& expression)
{
    registerDefinition_.setSizeExpression(expression);
}

//-----------------------------------------------------------------------------
// Function: register::getSizeExpression()
//-----------------------------------------------------------------------------
QString Register::getSizeExpression()
{
    return registerDefinition_.getSizeExpression();
}

//-----------------------------------------------------------------------------
// Function: register::removeSizeExpression()
//-----------------------------------------------------------------------------
void Register::removeSizeExpression()
{
    registerDefinition_.removeSizeExpression();
}

//-----------------------------------------------------------------------------
// Function: Register::setIsPresentExpression()
//-----------------------------------------------------------------------------
void Register::setIsPresentExpression(QString const& expression)
{
    if (hasIsPresentExpression())
    {
        isPresentExpression_->setValue(expression);
    }
    else if (!expression.isEmpty())
    {
        createIsPresentExpressionExtension(expression);
    }
}

//-----------------------------------------------------------------------------
// Function: Register::getIsPresentExpression()
//-----------------------------------------------------------------------------
QString Register::getIsPresentExpression()
{
    if (hasIsPresentExpression())
    {
        return isPresentExpression_->value();
    }

    return QString("1");
}

//-----------------------------------------------------------------------------
// Function: Register::removeIsPresentExpression()
//-----------------------------------------------------------------------------
void Register::removeIsPresentExpression()
{
    vendorExtensions_.removeAll(isPresentExpression_);
    isPresentExpression_.clear();
}

//-----------------------------------------------------------------------------
// Function: Register::hasIsPresentExpression()
//-----------------------------------------------------------------------------
bool Register::hasIsPresentExpression()
{
    return !isPresentExpression_.isNull();
}

//-----------------------------------------------------------------------------
// Function: Register::setAddressOffset()
//-----------------------------------------------------------------------------
void Register::setAddressOffset(const QString& addressOffset)
{
    addressOffset_ = addressOffset;
}

//-----------------------------------------------------------------------------
// Function: Register::setAlternateRegisters()
//-----------------------------------------------------------------------------
void Register::setAlternateRegisters(const QList<QSharedPointer<AlternateRegister> >& alternateRegisters)
{
	alternateRegisters_.clear();
    alternateRegisters_ = alternateRegisters;
}

//-----------------------------------------------------------------------------
// Function: Register::setDim()
//-----------------------------------------------------------------------------
void Register::setDim(int dim)
{
    dim_ = dim;
}

//-----------------------------------------------------------------------------
// Function: Register::getFields()
//-----------------------------------------------------------------------------
const QList<QSharedPointer<Field> >& Register::getFields() const
{
	return registerDefinition_.getFields();
}

//-----------------------------------------------------------------------------
// Function: Register::getFields()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Field> >& Register::getFields()
{
	return registerDefinition_.getFields();
}

//-----------------------------------------------------------------------------
// Function: Register::getWidth()
//-----------------------------------------------------------------------------
unsigned int Register::getWidth() const
{
	return registerDefinition_.getSize();
}

//-----------------------------------------------------------------------------
// Function: Register::getTypeIdentifier()
//-----------------------------------------------------------------------------
QString Register::getTypeIdentifier() const
{
	return registerDefinition_.getTypeIdentifier();
}

//-----------------------------------------------------------------------------
// Function: Register::setTypeIdentifier()
//-----------------------------------------------------------------------------
void Register::setTypeIdentifier(QString const& typeIdentifier )
{
	registerDefinition_.setTypeIdentifier(typeIdentifier);
}

//-----------------------------------------------------------------------------
// Function: Register::getSize()
//-----------------------------------------------------------------------------
unsigned int Register::getSize() const
{
	return registerDefinition_.getSize();
}

//-----------------------------------------------------------------------------
// Function: Register::setSize()
//-----------------------------------------------------------------------------
void Register::setSize( unsigned int size )
{
	registerDefinition_.setSize(size);
}

//-----------------------------------------------------------------------------
// Function: Register::getVolatile()
//-----------------------------------------------------------------------------
General::BooleanValue Register::getVolatile() const
{
	return registerDefinition_.getVolatile();
}

//-----------------------------------------------------------------------------
// Function: Register::setVolatile()
//-----------------------------------------------------------------------------
void Register::setVolatile( General::BooleanValue volatileValue )
{
	registerDefinition_.setVolatile(volatileValue);
}

//-----------------------------------------------------------------------------
// Function: Register::getAccess()
//-----------------------------------------------------------------------------
General::Access Register::getAccess() const
{
	return registerDefinition_.getAccess();
}

//-----------------------------------------------------------------------------
// Function: Register::setAccess()
//-----------------------------------------------------------------------------
void Register::setAccess( General::Access access )
{
	registerDefinition_.setAccess(access);
}

//-----------------------------------------------------------------------------
// Function: Register::getRegisterValue()
//-----------------------------------------------------------------------------
QString Register::getRegisterValue() const
{
	return registerDefinition_.getRegisterValue();
}

//-----------------------------------------------------------------------------
// Function: Register::setRegisterValue()
//-----------------------------------------------------------------------------
void Register::setRegisterValue(QString const& registerValue)
{
	registerDefinition_.setRegisterValue(registerValue);
}

//-----------------------------------------------------------------------------
// Function: Register::clearReset()
//-----------------------------------------------------------------------------
void Register::clearReset()
{
	registerDefinition_.clearReset();
}

//-----------------------------------------------------------------------------
// Function: Register::getRegisterMask()
//-----------------------------------------------------------------------------
QString Register::getRegisterMask() const
{
	return registerDefinition_.getRegisterMask();
}

//-----------------------------------------------------------------------------
// Function: Register::setRegisterMask()
//-----------------------------------------------------------------------------
void Register::setRegisterMask(QString const& registerMask )
{
	registerDefinition_.setRegisterMask(registerMask);
}

//-----------------------------------------------------------------------------
// Function: Register::getMSB()
//-----------------------------------------------------------------------------
unsigned int Register::getMSB() const
{
	return registerDefinition_.getMSB();
}

//-----------------------------------------------------------------------------
// Function: register::parseVendorExtensions()
//-----------------------------------------------------------------------------
void Register::parseVendorExtensions(QDomNode const& registerNode)
{
    int extensionCount = registerNode.childNodes().count();
    for (int j = 0; j < extensionCount; j++)
    {
        QDomNode extensionNode = registerNode.childNodes().at(j);

        if (extensionNode.nodeName() == QString("kactus2:dimExpression"))
        {
            createDimensionExpressionExtension(extensionNode.childNodes().at(0).nodeValue());
        }
        else if (extensionNode.nodeName() == QString("kactus2:isPresent"))
        {
            createIsPresentExpressionExtension(extensionNode.childNodes().at(0).nodeValue());
        }
        else
        {
            vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extensionNode)));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: register::createExpressionExtension()
//-----------------------------------------------------------------------------
void Register::createDimensionExpressionExtension(QString const& expression)
{
    if (dimensionExpression_.isNull())
    {
        dimensionExpression_ = QSharedPointer<Kactus2Value>(new Kactus2Value("kactus2:dimExpression", expression));
        vendorExtensions_.append(dimensionExpression_);
    }
}

//-----------------------------------------------------------------------------
// Function: Register::createIsPresentExpressionExtension()
//-----------------------------------------------------------------------------
void Register::createIsPresentExpressionExtension(QString const& expression)
{
    if (isPresentExpression_.isNull())
    {
        isPresentExpression_ = QSharedPointer<Kactus2Value>(new Kactus2Value("kactus2:isPresent", expression));
        vendorExtensions_.append(isPresentExpression_);
    }
}

//-----------------------------------------------------------------------------
// Function: Register::copyVendorExtensions()
//-----------------------------------------------------------------------------
void Register::copyVendorExtensions(const Register & other)
{
    foreach (QSharedPointer<VendorExtension> extension, other.vendorExtensions_)
    {
        if (extension->type() == "kactus2:dimExpression")
        {
            dimensionExpression_ = QSharedPointer<Kactus2Value>(other.dimensionExpression_->clone());
            vendorExtensions_.append(dimensionExpression_);
        }
        else if (extension->type() == "kactus2:isPresent")
        {
            isPresentExpression_ = QSharedPointer<Kactus2Value>(other.isPresentExpression_->clone());
            vendorExtensions_.append(isPresentExpression_);
        }
        else
        {
            vendorExtensions_.append(QSharedPointer<VendorExtension>(extension->clone()));
        }
    }
}