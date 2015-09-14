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

#include "WireTypeDef.h"

#include <IPXACTmodels/common/DirectionTypes.h>

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

	/*!
	 *  The default constructor.
	 */
	Wire();

    /*!
     *  The constructor.
     *
     *      @param [in] direction               Direction of the port.
     *      @param [in] leftBound               Left bound of the port.
     *      @param [in] rightBound              Right bound of the port.
     *      @param [in] defaultValue            Default value of the port.
     *      @param [in] allLogicalDirections    Specifies allowed logical directions.
     */
    Wire(DirectionTypes::Direction direction, int leftBound, int rightBound, const QString& defaultValue,
        bool allLogicalDirections);

    /*!
	 * Copy constructor.
	 */
	Wire(Wire const& other);

	/*!
	 *  Assignment operator.
	 */
	Wire &operator=(const Wire &other);

	/*!
	 *  The destructor.
	 */
	~Wire();

	/*! \brief Check if the wire element is in valid state or not.
	* 
	* \param hasViews Must be true if component has at least one view. This is 
	* needed because if port has a type specified then at least one view is needed.
	* 
	* \return True if wire is in valid state.
	*/
	//bool isValid(bool hasViews) const;

	/*! \brief Check if the wire is in a valid state.
	 * 
	 * \param hasViews Must be true if component has at least one view. This is
	 * needed because if port has a type specified then at least one view is needed.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	//bool isValid(bool hasViews, QStringList& errorList, const QString& parentIdentifier) const;

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
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > getWireTypeDefs();

	/*!
	 *  Allow / disallow all logical directions allowed value for this port.
	 *
	 *      @param [in] allLogicalDirectionsAllowed     The allowed status.
	 */
	void setAllLogicalDirectionsAllowed(bool allLogicalDirectionsAllowed);

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
    QString getVectorLeftBound();

    /*!
     *  Get the right bound expression.
     *
     *      @return     The expression.
     */
    QString getVectorRightBound();

	/*!
	 *  Get the type name of the port for a given view.
	 *
	 *      @param [in] viewName    The name of the used view.
     *
     *      @return The required type name.
	 */
	QString getTypeName(const QString& viewName = QString("")) const;

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
	void setTypeName(const QString& typeName, const QString& viewName = QString(""));

	/*!
	 *  Get the type definition of the given type name.
	 *
	 *      @param [in] typeName    The name of the type that's type definition is searched for.
     *
     *      @return The required type definition.
	 */
	QString getTypeDefinition(const QString& typeName);

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
	DirectionTypes::Direction direction_;

    //! Defines whether the port may be mapped to a port in an abstraction definition with a different direction.
	bool allLogicalDirectionsAllowed_;

    //! Determines the vector qualities of the port.
	QSharedPointer<Vector> vector_;

    //! Describes the ports type as defined bu the implementation.
	QSharedPointer<QList<QSharedPointer<WireTypeDef> > > wireTypeDefs_;

    //! Specifies a static logic value for this port.
	QString defaultDriverValue_;

	//! OPTIONAL Attributes for the defaultDriverValue element.
	QMap<QString, QString> defaultValueAttributes_;
};

#endif // WIRE_H
