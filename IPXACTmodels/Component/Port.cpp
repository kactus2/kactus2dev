//-----------------------------------------------------------------------------
// File: Port.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.09.2015
//
// Description:
// Describes the ipxact:port element.
//-----------------------------------------------------------------------------

#include "Port.h"
#include "Transactional.h"
#include "Wire.h"

#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <IPXACTmodels/utilities/Copy.h>

//-----------------------------------------------------------------------------
// Function: Port::Port()
//-----------------------------------------------------------------------------
Port::Port(QString const& portName) :
NameGroup(portName),
Extendable()
{

}

//-----------------------------------------------------------------------------
// Function: Port::Port()
//-----------------------------------------------------------------------------
Port::Port(const Port &other): 
NameGroup(other),
Extendable(other),
isPresent_(other.isPresent_)
{
    if (other.wire_)
    {
        wire_ = QSharedPointer<Wire>(new Wire(*other.wire_));
	}

	if (other.transactional_)
    {
        transactional_ = QSharedPointer<Transactional>(other.transactional_->clone());
	}

    Copy::copyList(other.configurableArrays_, configurableArrays_);
}

//-----------------------------------------------------------------------------
// Function: Port::operator=()
//-----------------------------------------------------------------------------
Port & Port::operator=( const Port &other )
{
    if (this != &other)
    {
		NameGroup::operator=(other);
        Extendable::operator=(other);
        isPresent_ = other.isPresent_;

        wire_.clear();
		if (other.wire_)
        {
            wire_ = QSharedPointer<Wire>(new Wire(*other.wire_));
        }

        transactional_.clear();
		if (other.transactional_) 
        {
			transactional_ = QSharedPointer<Transactional>(other.transactional_->clone());
		}

        configurableArrays_->clear();
        Copy::copyList(other.configurableArrays_, configurableArrays_);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Port::getWire()
//-----------------------------------------------------------------------------
QSharedPointer<Wire> Port::getWire() const
{
    return wire_;
}

//-----------------------------------------------------------------------------
// Function: Port::setWire()
//-----------------------------------------------------------------------------
void Port::setWire(QSharedPointer<Wire> newWire)
{
    transactional_.clear();

    wire_ = newWire;
}

//-----------------------------------------------------------------------------
// Function: Port::getTransactional()
//-----------------------------------------------------------------------------
QSharedPointer<Transactional> Port::getTransactional() const
{
    return transactional_;
}

//-----------------------------------------------------------------------------
// Function: Port::setTransactional()
//-----------------------------------------------------------------------------
void Port::setTransactional(QSharedPointer<Transactional> newTransactional)
{
    wire_.clear();

	transactional_ = newTransactional;
}

//-----------------------------------------------------------------------------
// Function: Port::getLeftBound()
//-----------------------------------------------------------------------------
QString Port::getLeftBound() const
{
    if (wire_ && wire_->getVector())
    {
        return wire_->getVectorLeftBound();
    }

    return QStringLiteral("0");
}

//-----------------------------------------------------------------------------
// Function: Port::setLeftBound()
//-----------------------------------------------------------------------------
void Port::setLeftBound(QString const& newLeftBound)
{
    if (wire_.isNull())
    {
        wire_ = QSharedPointer<Wire>(new Wire());
    }

    wire_->setVectorLeftBound(newLeftBound);
}

//-----------------------------------------------------------------------------
// Function: Port::getRightBound()
//-----------------------------------------------------------------------------
QString Port::getRightBound() const
{
    if (wire_ && wire_->getVector())
    {
        return wire_->getVectorRightBound();
    }

    return QStringLiteral("0");
}

//-----------------------------------------------------------------------------
// Function: Port::setRightBound()
//-----------------------------------------------------------------------------
void Port::setRightBound(QString const& newRightBound)
{
    if (wire_.isNull())
    {
        wire_ = QSharedPointer<Wire>(new Wire());
    }

    wire_->setVectorRightBound(newRightBound);
}

//-----------------------------------------------------------------------------
// Function: Port::setPortSize()
//-----------------------------------------------------------------------------
void Port::setPortSize( int size )
{
    setLeftBound(QString::number(qMax(0, size-1)));
    setRightBound(QStringLiteral("0"));
}

//-----------------------------------------------------------------------------
// Function: Port::getDefaultValue()
//-----------------------------------------------------------------------------
QString Port::getDefaultValue() const
{
    if (!wire_)
    {
        return QString();
    }
    return wire_->getDefaultDriverValue();
}

//-----------------------------------------------------------------------------
// Function: Port::setDefaultValue()
//-----------------------------------------------------------------------------
void Port::setDefaultValue( const QString& defaultValue )
{
    if (wire_)
    {
        wire_->setDefaultDriverValue(defaultValue);
    }
    else if (!transactional_)
    {
        wire_ = QSharedPointer<Wire>(new Wire());
        wire_->setDefaultDriverValue(defaultValue);
    }
}

//-----------------------------------------------------------------------------
// Function: Port::getDirection()
//-----------------------------------------------------------------------------
DirectionTypes::Direction Port::getDirection() const
{
    if (wire_)
    {
        return wire_->getDirection();
    }
    return DirectionTypes::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: Port::setDirection()
//-----------------------------------------------------------------------------
void Port::setDirection(DirectionTypes::Direction direction)
{
    if (wire_)
    {
        wire_->setDirection(direction);
    }
    else if (!transactional_)
    {
        wire_ = QSharedPointer<Wire>(new Wire());
        wire_->setDirection(direction);
    }
}

//-----------------------------------------------------------------------------
// Function: Port::allLogicalDirectionsAllowed()
//-----------------------------------------------------------------------------
bool Port::allLogicalDirectionsAllowed() const
{
    if (wire_)
    {
        return wire_->getAllLogicalDirectionsAllowed();
    }
    else if (transactional_)
    {
        return transactional_->getAllLogicalInitiativesAllowed();
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: Port::setAllLogicalDirectionsAllowed()
//-----------------------------------------------------------------------------
void Port::setAllLogicalDirectionsAllowed( bool allowed )
{
    if (wire_)
    {
        wire_->setAllLogicalDirectionsAllowed(allowed);
    }
    else if (transactional_)
    {
        transactional_->setAllLogicalInitiativesAllowed(allowed);
    }
    else 
    {
        wire_ = QSharedPointer<Wire>(new Wire());
        wire_->setAllLogicalDirectionsAllowed(allowed);
    }
}

//-----------------------------------------------------------------------------
// Function: Port::getTypeName()
//-----------------------------------------------------------------------------
QString Port::getTypeName( const QString& viewName ) const
{
    if (wire_)
    {
        return wire_->getTypeName(viewName);
    }
    else if (transactional_)
    {
        return transactional_->getTypeName(viewName);
    }
    else 
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: Port::setTypeName()
//-----------------------------------------------------------------------------
void Port::setTypeName( const QString& typeName, const QString& viewName )
{
    if (wire_)
    {
        wire_->setTypeName(typeName, viewName);
    }
    else if (transactional_)
    {
        transactional_->setTypeName(typeName, viewName);
    }
    else
    {
        wire_ = QSharedPointer<Wire>(new Wire());
        wire_->setTypeName(typeName, viewName);
    }
}

//-----------------------------------------------------------------------------
// Function: Port::hasType()
//-----------------------------------------------------------------------------
bool Port::hasType( const QString& viewName ) const
{
    if (wire_)
    {
        return wire_->hasType(viewName);
    }
    else if (transactional_)
    {
        return transactional_->hasType(viewName);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: Port::getTypeDefinition()
//-----------------------------------------------------------------------------
QString Port::getTypeDefinition( const QString& typeName ) const
{
    if (wire_)
    {
        return wire_->getTypeDefinition(typeName);
    }
    else if (transactional_)
    {
        return transactional_->getTypeDefinition(typeName);
    }
    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: Port::getTypeDefinitions()
//-----------------------------------------------------------------------------
QStringList Port::getTypeDefinitions() const
{
    if (wire_)
    {
        return wire_->getTypeDefinitions();
    }
    else if (transactional_)
    {
        return transactional_->getTypeDefinitions();
    }
    else
    {
        return QStringList();
    }
}

//-----------------------------------------------------------------------------
// Function: Port::setTypeDefinition()
//-----------------------------------------------------------------------------
void Port::setTypeDefinition( const QString& typeName, const QString& typeDefinition )
{
    if (wire_)
    {
        wire_->setTypeDefinition(typeName, typeDefinition);
    }
    else if (transactional_)
    {
        transactional_->setTypeDefinition(typeName, typeDefinition);
    }
    else
    {
        wire_ = QSharedPointer<Wire>(new Wire());
        wire_->setTypeDefinition(typeName, typeDefinition);
    }
}

//-----------------------------------------------------------------------------
// Function: Port::hasTypeDefinitions()
//-----------------------------------------------------------------------------
bool Port::hasTypeDefinitions() const
{
    if (wire_)
    {
        return wire_->hasTypeDefinition();
    }
    else if (transactional_)
    {
        return transactional_->hasTypeDefinitions();
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: Port::isAdHocVisible()
//-----------------------------------------------------------------------------
bool Port::isAdHocVisible() const
{
    return std::any_of(getVendorExtensions()->cbegin(), getVendorExtensions()->cend(),
        [](auto const& extension) { return extension->type() == QStringLiteral("kactus2:adHocVisible"); });
}

//-----------------------------------------------------------------------------
// Function: Port::setAdHocVisible()
//-----------------------------------------------------------------------------
void Port::setAdHocVisible(bool visible)
{
    auto adHocExtension = findVendorExtension(QStringLiteral("kactus2:adHocVisible"));
   
    if (visible && adHocExtension.isNull())
    {
        QSharedPointer<Kactus2Placeholder> adHocVisibility(new Kactus2Placeholder(
            QStringLiteral("kactus2:adHocVisible")));
        getVendorExtensions()->append(adHocVisibility);
    }
    else if (visible == false)
    {
        getVendorExtensions()->removeAll(adHocExtension);
    }
}

//-----------------------------------------------------------------------------
// Function: Port::getDefaultPos()
//-----------------------------------------------------------------------------
QPointF Port::getDefaultPos() const
{
    if (auto positionExtension = findVendorExtension(QStringLiteral("kactus2:position")); 
        positionExtension.isNull() == false)
    {
        return positionExtension.dynamicCast<Kactus2Position>()->position();
    }

    return QPointF();
}

//-----------------------------------------------------------------------------
// Function: Port::setDefaultPos()
//-----------------------------------------------------------------------------
void Port::setDefaultPos(QPointF const& pos)
{
    auto positionExtension = findVendorExtension(QStringLiteral("kactus2:position")).dynamicCast<Kactus2Position>();
    if (positionExtension.isNull())
    {
        positionExtension = QSharedPointer<Kactus2Position>(new Kactus2Position(pos));
        getVendorExtensions()->append(positionExtension);
    }

    positionExtension->setPosition(pos);
}

//-----------------------------------------------------------------------------
// Function: port::getArrayLeft()
//-----------------------------------------------------------------------------
QString Port::getArrayLeft() const
{
    if (configurableArrays_->isEmpty())
    {
        return QString();
    }

    return configurableArrays_->first()->getLeft();
}

//-----------------------------------------------------------------------------
// Function: port::setArrayLeft()
//-----------------------------------------------------------------------------
void Port::setArrayLeft(QString const& newArrayLeft)
{
    if (configurableArrays_->isEmpty())
    {
        QSharedPointer<Array> newArray (new Array(newArrayLeft, QStringLiteral("0")));
        configurableArrays_->append(newArray);
    }
    else
    {
        QSharedPointer<Array> portArray = configurableArrays_->first();
        if (newArrayLeft.isEmpty() && portArray->getRight().isEmpty())
        {
            configurableArrays_->removeOne(portArray);
        }
        else
        {
            portArray->setLeft(newArrayLeft);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: port::getArrayRight()
//-----------------------------------------------------------------------------
QString Port::getArrayRight() const
{
    if (configurableArrays_->isEmpty())
    {
        return QString();
    }
    
    return configurableArrays_->first()->getRight();
}

//-----------------------------------------------------------------------------
// Function: port::setArrayRight()
//-----------------------------------------------------------------------------
void Port::setArrayRight(QString const& newArrayRight)
{
    if (configurableArrays_->isEmpty())
    {
        QSharedPointer<Array> newArray (new Array(QStringLiteral("0"), newArrayRight));
        configurableArrays_->append(newArray);
    }
    else
    {
        QSharedPointer<Array> portArray = configurableArrays_->first();
        if (portArray->getLeft().isEmpty() && newArrayRight.isEmpty())
        {
            configurableArrays_->removeOne(portArray);
        }
        else
        {
            portArray->setRight(newArrayRight);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: port::getPortTags()
//-----------------------------------------------------------------------------
QString Port::getPortTags() const
{
    auto tagExtension = findVendorExtension(QStringLiteral("kactus2:portTags")).dynamicCast<Kactus2Value>();
    if (tagExtension.isNull())
    {
        return QString();
    }

    return tagExtension->value();
}

//-----------------------------------------------------------------------------
// Function: port::setPortTags()
//-----------------------------------------------------------------------------
void Port::setPortTags(const QString& newPortTags)
{
    auto tagExtension = findVendorExtension(QStringLiteral("kactus2:portTags")).dynamicCast<Kactus2Value>();

    if (newPortTags.isEmpty())
    {
        getVendorExtensions()->removeAll(tagExtension);
    }
    else if (tagExtension.isNull())
    {
        tagExtension = QSharedPointer<Kactus2Value>(new Kactus2Value(QStringLiteral("kactus2:portTags"), newPortTags));
        getVendorExtensions()->append(tagExtension);
    }
    else
    {
        tagExtension->setValue(newPortTags);
    }    
}

//-----------------------------------------------------------------------------
// Function: Port::getIsPresent()
//-----------------------------------------------------------------------------
QString Port::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: Port::setIsPresent()
//-----------------------------------------------------------------------------
void Port::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: Port::getArrays()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Array> > > Port::getArrays() const
{
    return configurableArrays_;
}

//-----------------------------------------------------------------------------
// Function: Port::getTypes()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<WireTypeDef> > > Port::getWireTypes() const
{
    if (wire_)
    {
        return wire_->getWireTypeDefs();
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: Port::getTransactionalTypes()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<WireTypeDef> > > Port::getTransactionalTypes() const
{
    if (transactional_)
    {
        return transactional_->getTransTypeDef();
    }

    return nullptr;
}
