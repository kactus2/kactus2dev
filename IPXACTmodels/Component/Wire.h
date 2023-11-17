//-----------------------------------------------------------------------------
// File: Wire.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.09.2015
//
// Description:
// Describes the ipxact:wire element.
//-----------------------------------------------------------------------------

#ifndef WIRE_H
#define WIRE_H

#include "Driver.h"
#include "WireTypeDef.h"

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/Qualifier.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QStringList>
#include <QMap>

class Vector;

//-----------------------------------------------------------------------------
//! Describes the ipxact:wire element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Wire
{

public:

    //! The default constructor.
    Wire() noexcept = default;

    /*!
	 * Copy constructor.
	 */
	Wire(Wire const& other);

	/*!
	 *  Assignment operator.
	 */
	Wire &operator=(const Wire &other);

	/*!
	 *  Create a copy of the Wire.
	 *
	 *      @return A Wire identical to this.
	 */
	Wire* clone() const;

    /* The destructor.
     */
    ~Wire() = default;

	/*!
	 *  Get the value of all logical directions allowed element.
	 *
     *      @return True if all the logical directions are allowed, false otherwise.
	 */
	bool getAllLogicalDirectionsAllowed() const;

	/*!
	 *  Get the direction of the port.
	 *
     *      @return Enum direction specifying the port direction.
	 */
	DirectionTypes::Direction getDirection() const;

	QSharedPointer<Qualifier> getQualifier() const;

    /*!
     *  Get the vector element of this port.
     *
     *      @return Pointer to a vector element.
     */
    QSharedPointer<Vector> getVector() const;

    /*!
     *  Get the list of this port's wire type definitions.
     *
     *      @return Pointer to a list containing wire type definitions.
     */
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > getWireTypeDefs() const;

	/*!
	 *  Allow / disallow all logical directions allowed value for this port.
	 *
	 *      @param [in] allow     The allowed status.
	 */
	void setAllLogicalDirectionsAllowed(bool allow);

	/*!
	 *  Set the direction of this port.
	 *
	 *      @param [in] direction   The new direction.
	 */
	void setDirection(DirectionTypes::Direction direction);

    /*!
     *  Set the vector element for this port.
     *
     *      @param [in] newVector   The new vector element.
     */
    void setVector(QSharedPointer<Vector> newVector);

    /*!
     *  Set the wire type definitions.
     *
     *      @param [in] newWireTypeDefs     The new wire type definitions.
     */
    void setWireTypeDefs(QSharedPointer<QList<QSharedPointer<WireTypeDef> > > newWireTypeDefs);

	/*!
	 *  Get the default driver value for the wire.
	 *
     *      @return The default value.
	 */
	QString getDefaultDriverValue() const;

	/*!
	 *  Set the default driver value.
	 *
	 *      @param [in] defaultDriverValue  The new default value.
	 */
	void setDefaultDriverValue(const QString& defaultDriverValue);

    /*!
     *  Set the left bound expression.
     *
     *      @param [in] expression   The expression.
     */
    void setVectorLeftBound(QString const& expression);

    /*!
     *  Set the right bound expression.
     *
     *      @param [in] expression   The expression.
     */
    void setVectorRightBound(QString const& expression);

    /*!
     *  Get the left bound expression.
     *
     *      @return     The expression.
     */
    QString getVectorLeftBound() const;

    /*!
     *  Get the right bound expression.
     *
     *      @return     The expression.
     */
    QString getVectorRightBound() const;

	void setDriver(QSharedPointer<Driver> driver);

	QSharedPointer<Driver> getDriver() const;

	/*!
	 *  Get the type name of the port for a given view.
	 *
	 *      @param [in] viewName    The name of the used view.
     *
     *      @return The required type name.
	 */
	QString getTypeName(const QString& viewName = QString()) const;

	/*!
	 *  Check if the wire has a type name in given view.
	 *
	 *      @param [in] viewName    The name of the view that's type is searched for.
     *
     *      @return True, if a type is found, false otherwise.
	 */
	bool hasType(const QString& viewName = QString()) const;

	/*!
	 *  Check if the port has a type or type definition.
     *
	 *      @return True, if either the port type name or type definition are defined.
	 */
	bool hasTypeDefinition() const;

	/*!
	 *  Set the type name for the port in a given view.
	 *
	 *      @param [in] typeName    The name of the type.
	 *      @param [in] viewName    The name of the view used for this type.
	 */
	void setTypeName(const QString& typeName, const QString& viewName = QString());

	/*!
	 *  Get the type definition of the given type name.
	 *
	 *      @param [in] typeName    The name of the type that's type definition is searched for.
     *
     *      @return The required type definition.
	 */
	QString getTypeDefinition(const QString& typeName) const;

	/*!
	 *  Get the type definition.
	 *
     *      @return A list containing the type definition.
	 */
	QStringList getTypeDefinitions() const;

	/*!
	 *  Set the type definition of the given type name.
	 *
	 *      @param [in] typeName        Identifies the type definition.
	 *      @param [in] typeDefinition  The new type definition.
	 */
	void setTypeDefinition(const QString& typeName, const QString& typeDefinition);

private:

    //! The direction of the port.
	DirectionTypes::Direction direction_{ DirectionTypes::DIRECTION_INVALID };
    
	//! Defines whether the port may be mapped to a port in an abstraction definition with a different direction.
    bool allLogicalDirectionsAllowed_{ false };

	//! The wire qualifier.
	QSharedPointer<Qualifier> qualifier_{ new Qualifier };

    //! Determines the vector qualities of the port.
    //! Multiple vectors not supported.
	QSharedPointer<Vector> vector_{ nullptr };

    //! Specifies a driver for this port.
	//! Multiple drivers not supported.
	QSharedPointer<Driver> driver_{ nullptr };

    //! Describes the ports type as defined bu the implementation.
    WireTypeDef::List wireTypeDefs_{ new QList<QSharedPointer<WireTypeDef> > };

    //! DomainTypeDefs not supported.

    //! SignalTypeDefs not supported.
	 
    //! ConstraintSets not supported.

    //! PowerConstraints not supported.
};

#endif // WIRE_H
