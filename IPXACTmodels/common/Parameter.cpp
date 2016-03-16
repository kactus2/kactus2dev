//-----------------------------------------------------------------------------
// File: Parameter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2015
//
// Description:
// Implementation for ipxact:parameter element.
//-----------------------------------------------------------------------------

#include "Parameter.h"

#include <QString>
#include <QList>
#include <QMap>

#include <QUuid>

//-----------------------------------------------------------------------------
// Function: Parameter::Parameter()
//-----------------------------------------------------------------------------
Parameter::Parameter(): NameGroup(),
    Extendable(),
    value_(), 
    attributes_(), 
    valueAttributes_(), 
    bitWidthVector_(), 
    vectors_(new QList<QSharedPointer<Vector> >()),
    arrays_(new QList<QSharedPointer<Array> >())
{
    createUuid();
}

//-----------------------------------------------------------------------------
// Function: Parameter::Parameter()
//-----------------------------------------------------------------------------
Parameter::Parameter( const Parameter &other ): NameGroup(other),
    Extendable(),
    value_(other.value_),
    attributes_(other.attributes_),
    valueAttributes_(other.valueAttributes_), 
    bitWidthVector_(other.bitWidthVector_), 
    vectors_(other.vectors_), 
    arrays_(other.arrays_)
{

}

//-----------------------------------------------------------------------------
// Function: Parameter::~Parameter()
//-----------------------------------------------------------------------------
Parameter::~Parameter()
{
    attributes_.clear();
    valueAttributes_.clear();
}

//-----------------------------------------------------------------------------
// Function: Parameter::operator=()
//-----------------------------------------------------------------------------
Parameter & Parameter::operator=(Parameter const& other) 
{
	if (this != &other) 
    {
		NameGroup::operator=(other);
        Extendable::operator=(other);
		value_ = other.value_;
        attributes_ = other.attributes_;
		valueAttributes_ = other.valueAttributes_;

        copyVendorExtensions(other);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Parameter::elementName()
//-----------------------------------------------------------------------------
QString Parameter::elementName() const
{
    return "parameter";
}

//-----------------------------------------------------------------------------
// Function: Parameter::getValue()
//-----------------------------------------------------------------------------
QString Parameter::getValue() const
{
    return value_;
}

//-----------------------------------------------------------------------------
// Function: Parameter::setValue()
//-----------------------------------------------------------------------------
void Parameter::setValue(QString const& value) 
{
	value_ = value;
}

//-----------------------------------------------------------------------------
// Function: Parameter::getChoiceRef()
//-----------------------------------------------------------------------------
QString Parameter::getChoiceRef() const
{
    return getAttribute("choiceRef");
}

//-----------------------------------------------------------------------------
// Function: Parameter::setChoiceRef()
//-----------------------------------------------------------------------------
void Parameter::setChoiceRef(QString const& choiceRef)
{ 
    setAttribute("choiceRef", choiceRef);
}

//-----------------------------------------------------------------------------
// Function: parameter::getType()
//-----------------------------------------------------------------------------
QString Parameter::getType() const
{
    return attributes_.value("type");
}

//-----------------------------------------------------------------------------
// Function: parameter::setType()
//-----------------------------------------------------------------------------
void Parameter::setType(QString const& type)
{
    setAttribute("type", type);
}

//-----------------------------------------------------------------------------
// Function: Parameter::getMinimumValue()
//-----------------------------------------------------------------------------
QString Parameter::getMinimumValue() const
{
    return getAttribute("minimum");
}

//-----------------------------------------------------------------------------
// Function: Parameter::setMinimumValue()
//-----------------------------------------------------------------------------
void Parameter::setMinimumValue(QString const& minimum)
{
    setAttribute("minimum", minimum);
}

//-----------------------------------------------------------------------------
// Function: Parameter::getMaximumValue()
//-----------------------------------------------------------------------------
QString Parameter::getMaximumValue() const
{
    return getAttribute("maximum");
}

//-----------------------------------------------------------------------------
// Function: Parameter::setMaximumValue()
//-----------------------------------------------------------------------------
void Parameter::setMaximumValue(QString const& maximum)
{
    setAttribute("maximum", maximum);
}

//-----------------------------------------------------------------------------
// Function: Parameter::getValueResolve()
//-----------------------------------------------------------------------------
QString Parameter::getValueResolve() const
{
    return getAttribute("resolve");
}

//-----------------------------------------------------------------------------
// Function: Parameter::setValueResolve()
//-----------------------------------------------------------------------------
void Parameter::setValueResolve(QString const& resolve)
{
    setAttribute("resolve", resolve);
}

//-----------------------------------------------------------------------------
// Function: Parameter::getValueId()
//-----------------------------------------------------------------------------
QString Parameter::getValueId() const
{
     return getAttribute("parameterId");
}

//-----------------------------------------------------------------------------
// Function: Parameter::setValueId()
//-----------------------------------------------------------------------------
void Parameter::setValueId(QString const& id)
{
     setAttribute("parameterId", id);
}

//-----------------------------------------------------------------------------
// Function: parameter::getUsageCount()
//-----------------------------------------------------------------------------
int Parameter::getUsageCount() const
{
    return getAttribute("usageCount").toInt();
}

//-----------------------------------------------------------------------------
// Function: parameter::increaseUsageCount()
//-----------------------------------------------------------------------------
void Parameter::increaseUsageCount()
{
    attributes_.insert("usageCount", QString::number(getUsageCount() + 1));
}

//-----------------------------------------------------------------------------
// Function: parameter::decreaseUsageCount()
//-----------------------------------------------------------------------------
void Parameter::decreaseUsageCount()
{
    if (getUsageCount() < 2)
    {
        attributes_.remove("usageCount");
    }

    else
    {
        attributes_.insert("usageCount", QString::number(getUsageCount() - 1));
    }
}

//-----------------------------------------------------------------------------
// Function: Parameter::hasAttribute()
//-----------------------------------------------------------------------------
bool Parameter::hasAttribute(QString const& attributeName) const
{
    return attributes_.contains(attributeName);
}

//-----------------------------------------------------------------------------
// Function: Parameter::getAttribute()
//-----------------------------------------------------------------------------
QString Parameter::getAttribute(QString const& attributeName) const
{
    return attributes_.value(attributeName);
}

//-----------------------------------------------------------------------------
// Function: Parameter::setAttribute()
//-----------------------------------------------------------------------------
void Parameter::setAttribute(QString const& attributeName, QString const& attributeValue)
{
    if (!attributeValue.isEmpty())
    {
        attributes_.insert(attributeName, attributeValue);
    }
    else
    {
        attributes_.remove(attributeName);
    }
}

//-----------------------------------------------------------------------------
// Function: Parameter::getValueAttribute()
//-----------------------------------------------------------------------------
QString Parameter::getValueAttribute(QString const& attributeName) const
{
    return valueAttributes_.value(attributeName);
}

//-----------------------------------------------------------------------------
// Function: Parameter::setValueAttribute()
//-----------------------------------------------------------------------------
void Parameter::setValueAttribute(QString const& attributeName, QString const& attributeValue)
{
    if (!attributeValue.isEmpty())
    {
        valueAttributes_.insert(attributeName, attributeValue);
    }
    else
    {
        valueAttributes_.remove(attributeName);
    }
}

//-----------------------------------------------------------------------------
// Function: Parameter::getAttributeNames()
//-----------------------------------------------------------------------------
QStringList Parameter::getAttributeNames() const
{
    return attributes_.keys();
}

//-----------------------------------------------------------------------------
// Function: Parameter::getValueAttributeNames()
//-----------------------------------------------------------------------------
QStringList Parameter::getValueAttributeNames() const
{
    return valueAttributes_.keys();
}

//-----------------------------------------------------------------------------
// Function: Parameter::getVectors()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Vector> > > Parameter::getVectors() const
{
    return vectors_;
}

//-----------------------------------------------------------------------------
// Function: Parameter::getVectorLeft()
//-----------------------------------------------------------------------------
QString Parameter::getVectorLeft() const
{
    if (vectors_->isEmpty())
    {
        return QString();
    }
    return vectors_->first()->getLeft();
}

//-----------------------------------------------------------------------------
// Function: Parameter::getVectorRight()
//-----------------------------------------------------------------------------
QString Parameter::getVectorRight() const
{
    if (vectors_->isEmpty())
    {
        return QString();
    }
    return vectors_->first()->getRight();
}

//-----------------------------------------------------------------------------
// Function: Parameter::setVectorLeft()
//-----------------------------------------------------------------------------
void Parameter::setVectorLeft(QString const& leftExpression)
{
    if (vectors_->isEmpty())
    {
        if (!leftExpression.isEmpty())
        {
            vectors_->append(QSharedPointer<Vector>(new Vector(leftExpression, "")));
        }
    }
    else
    {
        vectors_->first()->setLeft(leftExpression);
        if (vectors_->first()->getRight().isEmpty())
        {
            vectors_->removeFirst();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Parameter::setVectorRight()
//-----------------------------------------------------------------------------
void Parameter::setVectorRight(QString const& rightExpression)
{
    if (vectors_->isEmpty())
    {
        if (!rightExpression.isEmpty())
        {
            vectors_->append(QSharedPointer<Vector>(new Vector("", rightExpression)));
        }
    }
    else
    {
        vectors_->first()->setRight(rightExpression);
        if (vectors_->first()->getLeft().isEmpty())
        {
            vectors_->removeFirst();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Parameter::getArrays()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Array> > > Parameter::getArrays() const
{
    return arrays_;
}

//-----------------------------------------------------------------------------
// Function: Parameter::getArrayLeft()
//-----------------------------------------------------------------------------
QString Parameter::getArrayLeft() const
{
    if (arrays_->isEmpty())
    {
        return QString();
    }
    return arrays_->first()->getLeft();
}

//-----------------------------------------------------------------------------
// Function: Parameter::getArrayRight()
//-----------------------------------------------------------------------------
QString Parameter::getArrayRight() const
{
    if (arrays_->isEmpty())
    {
        return QString();
    }

    return arrays_->first()->getRight();
}
//-----------------------------------------------------------------------------
// Function: Parameter::setArrayLeft()
//-----------------------------------------------------------------------------
void Parameter::setArrayLeft(QString const& leftExpression)
{
    if (arrays_->isEmpty())
    {
        arrays_->append(QSharedPointer<Array>(new Array(leftExpression, "")));
    }
    else
    {
        arrays_->first()->setLeft(leftExpression);
    }
}


//-----------------------------------------------------------------------------
// Function: Parameter::setArrayRight()
//-----------------------------------------------------------------------------
void Parameter::setArrayRight(QString const& rightExpression)
{
    if (arrays_->isEmpty())
    {
        arrays_->append(QSharedPointer<Array>(new Array("", rightExpression)));
    }
    else
    {
        arrays_->first()->setRight(rightExpression);
    }
}

//-----------------------------------------------------------------------------
// Function: Parameter::createUuid()
//-----------------------------------------------------------------------------
void Parameter::createUuid()
{
    QString formattedUuid = "uuid_" + QUuid::createUuid().toString().remove('{').remove('}').replace('-', '_');
    setValueId(formattedUuid);
}
