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

//-----------------------------------------------------------------------------
// Function: Port::Port()
//-----------------------------------------------------------------------------
Port::Port(QString const& portName, DirectionTypes::Direction direction) :
NameGroup(portName),
    Extendable(),
    isPresent_(),
    wire_(QSharedPointer<Wire>(new Wire())),
    transactional_(),
    configurableArrays_(new QList<QSharedPointer<Array> > ())
{
    setDirection(direction);
}

//-----------------------------------------------------------------------------
// Function: Port::Port()
//-----------------------------------------------------------------------------
Port::Port(const Port &other): 
NameGroup(other),
    Extendable(other),
    isPresent_(other.isPresent_),
    wire_(),
    transactional_(),
    configurableArrays_(new QList<QSharedPointer<Array> > ())
{
    if (other.wire_)
    {
        wire_ = QSharedPointer<Wire>(new Wire(*other.wire_));
	}

	if (other.transactional_)
    {
        transactional_ = QSharedPointer<Transactional>(new Transactional(*other.transactional_.data()));
	}

    copyArrays(other);
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

		if (other.wire_)
        {
            wire_ = QSharedPointer<Wire>(new Wire(*other.wire_.data()));
        }
        else
        {
            wire_ = QSharedPointer<Wire>();
        }

		if (other.transactional_) 
        {
			transactional_ = QSharedPointer<Transactional>(new Transactional(*other.transactional_.data()));
		}
		else
        {
			transactional_ = QSharedPointer<Transactional>();
        }

        configurableArrays_->clear();
        copyArrays(other);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Port::~Port()
//-----------------------------------------------------------------------------
Port::~Port()
{
    wire_.clear();
    transactional_.clear();
    configurableArrays_.clear();
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
    if (wire_)
    {
        wire_.clear();
    }

    if (transactional_)
    {
        transactional_.clear();
    }

    // change the port type
    wire_ = QSharedPointer<Wire>(newWire);
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
	if (wire_)
    {
        wire_.clear();
	}

	if (transactional_)
    {
		transactional_.clear();
	}

	transactional_ = QSharedPointer<Transactional>(newTransactional);
}

//-----------------------------------------------------------------------------
// Function: Port::getLeftBound()
//-----------------------------------------------------------------------------
QString Port::getLeftBound() const
{
    if (wire_)
    {
        if (wire_->getVector())
        {
            return wire_->getVectorLeftBound();
        }
    }

    return QString("0");
}

//-----------------------------------------------------------------------------
// Function: Port::setLeftBound()
//-----------------------------------------------------------------------------
void Port::setLeftBound(QString const& newLeftBound)
{
    if (wire_)
    {
        wire_->setVectorLeftBound(newLeftBound);
    }
    else
    {
        wire_ = QSharedPointer<Wire>(new Wire());
        wire_->setVectorLeftBound(newLeftBound);
    }
}

//-----------------------------------------------------------------------------
// Function: Port::getRightBound()
//-----------------------------------------------------------------------------
QString Port::getRightBound() const
{
    if (wire_)
    {
        if (wire_->getVector())
        {
            return wire_->getVectorRightBound();
        }
    }

    return QString("0");
}

//-----------------------------------------------------------------------------
// Function: Port::setRightBound()
//-----------------------------------------------------------------------------
void Port::setRightBound(QString const& newRightBound)
{
    if (wire_)
    {
        wire_->setVectorRightBound(newRightBound);
    }
    else
    {
        wire_ = QSharedPointer<Wire>(new Wire());
        wire_->setVectorRightBound(newRightBound);
    }
}

//-----------------------------------------------------------------------------
// Function: Port::setPortSize()
//-----------------------------------------------------------------------------
void Port::setPortSize( int size )
{
    setLeftBound(QString::number(qMax(0, size-1)));
    setRightBound(QString("0"));
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
QString Port::getTypeName( const QString& viewName /*= QString("")*/ ) const
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
        return QString("");
    }
}

//-----------------------------------------------------------------------------
// Function: Port::setTypeName()
//-----------------------------------------------------------------------------
void Port::setTypeName( const QString& typeName, const QString& viewName /*= QString("")*/ )
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
bool Port::hasType( const QString& viewName /*= QString("")*/ ) const
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
QString Port::getTypeDefinition( const QString& typeName )
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
        return QString("");
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
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:adHocVisible")
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: Port::setAdHocVisible()
//-----------------------------------------------------------------------------
void Port::setAdHocVisible(bool visible)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:adHocVisible")
        {
            if (!visible)
            {
                getVendorExtensions()->removeAll(extension);
            }
            return;
        }
    }

    QSharedPointer<Kactus2Placeholder> adHocVisibility (new Kactus2Placeholder("kactus2:adHocVisible"));
    getVendorExtensions()->append(adHocVisibility);
}

//-----------------------------------------------------------------------------
// Function: Port::getDefaultPos()
//-----------------------------------------------------------------------------
QPointF Port::getDefaultPos() const
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:position")
        {
            QSharedPointer<Kactus2Position> positionExtension = extension.dynamicCast<Kactus2Position>();
            return positionExtension->position();
        }
    }

    return QPointF();
}

//-----------------------------------------------------------------------------
// Function: Port::setDefaultPos()
//-----------------------------------------------------------------------------
void Port::setDefaultPos(QPointF const& pos)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:position")
        {
            QSharedPointer<Kactus2Position> positionExtension = extension.dynamicCast<Kactus2Position>();
            positionExtension->setPosition(pos);
            return;
        }
    }

    QSharedPointer<Kactus2Position> newPosition (new Kactus2Position(pos));
    getVendorExtensions()->append(newPosition);
}

//-----------------------------------------------------------------------------
// Function: port::getArrayLeft()
//-----------------------------------------------------------------------------
QString Port::getArrayLeft() const
{
    if (configurableArrays_->isEmpty())
    {
        return QString("");
    }
    else
    {
        return configurableArrays_->first()->getLeft();
    }
}

//-----------------------------------------------------------------------------
// Function: port::setArrayLeft()
//-----------------------------------------------------------------------------
void Port::setArrayLeft(QString const& newArrayLeft)
{
    if (configurableArrays_->isEmpty())
    {
        QSharedPointer<Array> newArray (new Array(newArrayLeft, "0"));
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
        return QString("");
    }
    else
    {
        return configurableArrays_->first()->getRight();
    }
}

//-----------------------------------------------------------------------------
// Function: port::setArrayRight()
//-----------------------------------------------------------------------------
void Port::setArrayRight(QString const& newArrayRight)
{
    if (configurableArrays_->isEmpty())
    {
        QSharedPointer<Array> newArray (new Array("0", newArrayRight));
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
const QString Port::getPortTags() const
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:portTags")
        {
            QSharedPointer<Kactus2Value> portTags = extension.dynamicCast<Kactus2Value>();
            return portTags->value();
        }
    }

    return QString("");
}

//-----------------------------------------------------------------------------
// Function: port::setPortTags()
//-----------------------------------------------------------------------------
void Port::setPortTags(const QString& newPortTags)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:portTags")
        {
            if (newPortTags.isEmpty())
            {
                getVendorExtensions()->removeAll(extension);
            }
            else
            {
                QSharedPointer<Kactus2Value> tagExtension = extension.dynamicCast<Kactus2Value>();
                tagExtension->setValue(newPortTags);
            }
            return;
        }
    }

    QSharedPointer<Kactus2Value> tagExtension (new Kactus2Value("kactus2:portTags", newPortTags));
    getVendorExtensions()->append(tagExtension);
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
// Function: Port::copyArrays()
//-----------------------------------------------------------------------------
void Port::copyArrays(const Port& other)
{
    foreach (QSharedPointer<Array> configurableArray, *other.configurableArrays_)
    {
        if (configurableArray)
        {
            QString arrayLeft = configurableArray->getLeft();
            QString arrayRight = configurableArray->getRight();

            QSharedPointer<Array> copy (new Array(arrayLeft, arrayRight));
            configurableArrays_->append(copy);
        }
    }
}