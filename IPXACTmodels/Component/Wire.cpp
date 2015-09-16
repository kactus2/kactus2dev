//-----------------------------------------------------------------------------
// File: Wire.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.09.2015
//
// Description:
// Describes the ipxact:wire element.
//-----------------------------------------------------------------------------

#include "Wire.h"

#include <IPXACTmodels/common/Vector.h>
#include <IPXACTmodels/XmlUtils.h>

#include <QString>
#include <QList>
#include <QObject>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Function: Wire::Wire()
//-----------------------------------------------------------------------------
Wire::Wire():
direction_(DirectionTypes::DIRECTION_INVALID),
allLogicalDirectionsAllowed_(false),
vector_(),
wireTypeDefs_(new QList<QSharedPointer<WireTypeDef> > ()),
defaultDriverValue_(),
defaultValueAttributes_()
{

}

//-----------------------------------------------------------------------------
// Function: Wire::Wire()
//-----------------------------------------------------------------------------
Wire::Wire( DirectionTypes::Direction direction, int leftBound, int rightBound, const QString& defaultValue,
    bool allLogicalDirections ):
direction_(direction),
allLogicalDirectionsAllowed_(allLogicalDirections),
vector_(),
wireTypeDefs_(new QList<QSharedPointer<WireTypeDef> > ()),
defaultDriverValue_(defaultValue),
defaultValueAttributes_()
{
    vector_ = QSharedPointer<Vector>(new Vector(QString::number(leftBound), QString::number(rightBound)));
}

//-----------------------------------------------------------------------------
// Function: Wire()
//-----------------------------------------------------------------------------
Wire::Wire(Wire const& other) : 
direction_(other.direction_),
allLogicalDirectionsAllowed_(other.allLogicalDirectionsAllowed_),
vector_(),
wireTypeDefs_(), 
defaultDriverValue_(other.defaultDriverValue_),
defaultValueAttributes_(other.defaultValueAttributes_)
{
	if (other.vector_)
    {
        vector_ = QSharedPointer<Vector>(new Vector(*other.vector_));
	}

	foreach (QSharedPointer<WireTypeDef> typeDef, *other.wireTypeDefs_)
    {
		if (typeDef)
        {
			QSharedPointer<WireTypeDef> copy = QSharedPointer<WireTypeDef>(new WireTypeDef(*typeDef.data()));
			wireTypeDefs_->append(copy);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: Wire::operator=()
//-----------------------------------------------------------------------------
Wire & Wire::operator=( const Wire &other )
{
	if (this != &other)
    {
		direction_ = other.direction_;
		allLogicalDirectionsAllowed_ = other.allLogicalDirectionsAllowed_;
		defaultDriverValue_ = other.defaultDriverValue_;
		defaultValueAttributes_ = other.defaultValueAttributes_;

		if (other.vector_)
        {
            vector_ = QSharedPointer<Vector>(new Vector(*other.vector_));
		}
		else
        {
            vector_ = QSharedPointer<Vector>();
        }

        wireTypeDefs_.clear();
        foreach (QSharedPointer<WireTypeDef> typeDef, *other.wireTypeDefs_)
        {
			if (typeDef)
            {
                QSharedPointer<WireTypeDef> copy = QSharedPointer<WireTypeDef>(new WireTypeDef(*typeDef.data()));
                wireTypeDefs_->append(copy);
			}
		}
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Wire::~Wire()
//-----------------------------------------------------------------------------
Wire::~Wire()
{
    if(vector_)
    {
        vector_.clear();
    }
    wireTypeDefs_.clear();
}

/*
bool Wire::isValid(bool hasViews) const {

	// if direction is not specified
	if (direction_ == General::DIRECTION_INVALID)
		return false;

	// if vector exists but is not valid
	/*if (vector_ && !vector_->isValid())
		return false;
        *//*
	// if there are types defined but no views exist
	if (!wireTypeDefs_.isEmpty() && !hasViews) {
		return false;
	}

	// check all wireTypeDefs
	for (int i = 0; i < wireTypeDefs_.size(); ++i) {

		// if typeName or viewNameRef is not specified for wireTypeDef
		if (wireTypeDefs_.value(i)->typeName_.isEmpty())
			return false;
	}

	// everything ok
	return true;
}*/
/*
bool Wire::isValid( bool hasViews,
				   QStringList& errorList, 
				   const QString& parentIdentifier ) const {

	bool valid = true;

	if (direction_ == General::DIRECTION_INVALID) {
		errorList.append(QObject::tr("No direction set in wire within %1").arg(parentIdentifier));
		valid = false;
	}

	/*if (vector_ && !vector_->isValid(errorList, parentIdentifier)) {
		valid = false;
	}*//*

	// if there are types defined but no views exist
	if (!wireTypeDefs_.isEmpty() && !hasViews) {
		errorList.append(QObject::tr("%1 has port type definitions but component"
			" doesn't contain any views").arg(parentIdentifier));
		valid = false;
	}

	foreach (QSharedPointer<WireTypeDef> typeDef, wireTypeDefs_) {
		if (!typeDef->isValid(errorList, parentIdentifier)) {
			valid = false;
			break;
		}
	}

	return valid;
}*/

//-----------------------------------------------------------------------------
// Function: Wire::getDirection()
//-----------------------------------------------------------------------------
DirectionTypes::Direction Wire::getDirection() const
{
    return direction_;
}

//-----------------------------------------------------------------------------
// Function: Wire::getWireTypeDefs()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<WireTypeDef> > > Wire::getWireTypeDefs()
{
    return wireTypeDefs_;
}

//-----------------------------------------------------------------------------
// Function: Wire::getVector()
//-----------------------------------------------------------------------------
QSharedPointer<Vector> Wire::getVector() const
{
    return vector_;
}

//-----------------------------------------------------------------------------
// Function: Wire::setVector()
//-----------------------------------------------------------------------------
void Wire::setVector(QSharedPointer<Vector> newVector)
{
    vector_ = newVector;
}

//-----------------------------------------------------------------------------
// Function: Wire::setDirection()
//-----------------------------------------------------------------------------
void Wire::setDirection(DirectionTypes::Direction direction)
{
	direction_ = direction;
}

//-----------------------------------------------------------------------------
// Function: Wire::getAllLogicalDirectionsAllowed()
//-----------------------------------------------------------------------------
bool Wire::getAllLogicalDirectionsAllowed() const
{
    return allLogicalDirectionsAllowed_;
}

//-----------------------------------------------------------------------------
// Function: Wire::setWireTypeDefs()
//-----------------------------------------------------------------------------
void Wire::setWireTypeDefs(QSharedPointer<QList<QSharedPointer<WireTypeDef> > > newWireTypeDefs)
{
	wireTypeDefs_.clear();
	wireTypeDefs_ = newWireTypeDefs;
}

//-----------------------------------------------------------------------------
// Function: Wire::setAllLogicalDirectionsAllowed()
//-----------------------------------------------------------------------------
void Wire::setAllLogicalDirectionsAllowed(bool allLogicalDirectionsAllowed)
{
    allLogicalDirectionsAllowed_ = allLogicalDirectionsAllowed;
}

//-----------------------------------------------------------------------------
// Function: Wire::setDefaultDriverValue()
//-----------------------------------------------------------------------------
void Wire::setDefaultDriverValue(const QString& defaultDriverValue)
{
    defaultDriverValue_ = defaultDriverValue;
}

//-----------------------------------------------------------------------------
// Function: Wire::getDefaultDriverValue()
//-----------------------------------------------------------------------------
QString Wire::getDefaultDriverValue() const
{
    return defaultDriverValue_;
}

//-----------------------------------------------------------------------------
// Function: wire::setLeftBoundExpression()
//-----------------------------------------------------------------------------
void Wire::setVectorLeftBound(QString const& expression)
{
    if (!vector_)
    {
        vector_ = QSharedPointer<Vector>(new Vector("0","0"));
    }

    vector_->setLeft(expression);
}

//-----------------------------------------------------------------------------
// Function: wire::setRightBoundExpression()
//-----------------------------------------------------------------------------
void Wire::setVectorRightBound(QString const& expression)
{
    if (!vector_)
    {
        vector_ = QSharedPointer<Vector>(new Vector("0","0"));
    }

    vector_->setRight(expression);
}

//-----------------------------------------------------------------------------
// Function: wire::getLeftBoundExpression()
//-----------------------------------------------------------------------------
QString Wire::getVectorLeftBound()
{
    if (!vector_)
    {
        vector_ = QSharedPointer<Vector>(new Vector("", ""));
    }

    return vector_->getLeft();
}

//-----------------------------------------------------------------------------
// Function: wire::getRightBoundExpression()
//-----------------------------------------------------------------------------
QString Wire::getVectorRightBound()
{
    if (!vector_)
    {
        vector_ = QSharedPointer<Vector>(new Vector("", ""));
    }

    return vector_->getRight();
}

//-----------------------------------------------------------------------------
// Function: Wire::getTypeName()
//-----------------------------------------------------------------------------
QString Wire::getTypeName( const QString& viewName /*= QString("")*/ ) const
{
	foreach (QSharedPointer<WireTypeDef> wireTypeDefinition, *wireTypeDefs_)
    {
		if (wireTypeDefinition->hasView(viewName))
        {
            return wireTypeDefinition->getTypeName();
		}
	}

    return QString("");
}

//-----------------------------------------------------------------------------
// Function: Wire::setTypeName()
//-----------------------------------------------------------------------------
void Wire::setTypeName( const QString& typeName, const QString& viewName /*= QString("")*/ )
{
    foreach (QSharedPointer<WireTypeDef> wireTypeDefinition, *wireTypeDefs_)
    {
        if (wireTypeDefinition->getViewRefs().contains(viewName) || viewName.isEmpty())
        {
            wireTypeDefinition->setTypeName(typeName);
            return;
        }
    }

    wireTypeDefs_->clear();

    if (!typeName.isEmpty())
    {
        QSharedPointer<WireTypeDef> newWireTypeDefinition(new WireTypeDef(typeName, viewName));
        wireTypeDefs_->append(newWireTypeDefinition);
    }
}

//-----------------------------------------------------------------------------
// Function: Wire::getTypeDefinition()
//-----------------------------------------------------------------------------
QString Wire::getTypeDefinition( const QString& typeName )
{
    foreach (QSharedPointer<WireTypeDef> wireTypeDefinition, *wireTypeDefs_)
    {
		if (wireTypeDefinition->getTypeName() == typeName)
        {
            return wireTypeDefinition->getTypeDefinitions().value(0, QString(""));
		}
	}

    return QString("");
}

//-----------------------------------------------------------------------------
// Function: Wire::getTypeDefinitions()
//-----------------------------------------------------------------------------
QStringList Wire::getTypeDefinitions() const
{
    QStringList typeDefs;
    foreach (QSharedPointer<WireTypeDef> wireTypeDefinition, *wireTypeDefs_)
    {
        typeDefs.append(wireTypeDefinition->getTypeDefinitions());
	}
	return typeDefs;
}

//-----------------------------------------------------------------------------
// Function: Wire::setTypeDefinition()
//-----------------------------------------------------------------------------
void Wire::setTypeDefinition( const QString& typeName, const QString& typeDefinition )
{
    foreach (QSharedPointer<WireTypeDef> wireTypeDefinition, *wireTypeDefs_)
    {
        if (wireTypeDefinition->getTypeName() == typeName)
        {
            QStringList typeDefinitions;
            typeDefinitions.append(typeDefinition);
            wireTypeDefinition->setTypeDefinitions(typeDefinitions);
            return;
        }
    }

    wireTypeDefs_->clear();

    QSharedPointer<WireTypeDef> newWireTypeDefinition(new WireTypeDef(typeName));
    QStringList typeDefinitions;
    typeDefinitions.append(typeDefinition);
    newWireTypeDefinition->setTypeDefinitions(typeDefinitions);
    wireTypeDefs_->append(newWireTypeDefinition);
}

//-----------------------------------------------------------------------------
// Function: Wire::hasType()
//-----------------------------------------------------------------------------
bool Wire::hasType( const QString& viewName /*= QString("")*/ ) const
{
	foreach (QSharedPointer<WireTypeDef> wireTypeDefinition, *wireTypeDefs_)
    {
		if (viewName.isEmpty() && !wireTypeDefinition->getTypeName().isEmpty() ||
            (wireTypeDefinition->hasView(viewName) && !wireTypeDefinition->getTypeName().isEmpty()))
        {
            return true;
		}
	}
    return false;
}

//-----------------------------------------------------------------------------
// Function: Wire::hasTypeDefinition()
//-----------------------------------------------------------------------------
bool Wire::hasTypeDefinition() const
{
	foreach(QSharedPointer<WireTypeDef> wireTypeDefinition, *wireTypeDefs_)
    {
 		if (!wireTypeDefinition->getTypeName().isEmpty() ||
            !wireTypeDefinition->getTypeDefinitions().isEmpty())
        {
 			return true;
 		}
	}

	return false;
}
