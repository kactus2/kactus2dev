//-----------------------------------------------------------------------------
// File: Port.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.09.2015
//
// Description:
// Describes the ipxact:port element.
//-----------------------------------------------------------------------------

#ifndef PORT_H
#define PORT_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/Array.h>

#include <IPXACTmodels/Component/Transactional.h>
#include <IPXACTmodels/Component/Wire.h>

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QStringList>
#include <QPointF>

//-----------------------------------------------------------------------------
//! Describes the ipxact:port element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Port : public NameGroup, public Extendable
{

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] portName    Name of the port.
	 *      @param [in] direction   Direction of the port.
	 */
	Port(QString const& portName = QString(), DirectionTypes::Direction direction = DirectionTypes::IN);

    /*!
     *  Copy constructor.
	 */
	Port(const Port &other);

	/*!
	 *  Assignment operator.
	 */
	Port &operator=(const Port &other);

	/*!
	 *  The destructor.
	 */
	~Port();
    
    /*!
     *  Get a pointer to the wire element.
     *
     *      @return A pointer to the wire instance.
     */
    QSharedPointer<Wire> getWire() const;

    /*!
     *  Set the wire element for the port.
     *
     *      @param [in] newWire     The new wire element.
     */
    void setWire(QSharedPointer<Wire> newWire);

    /*!
     *  Get a pointer to the transactional element.
     *
     *      @return Pointer to the transactional element.
     */
    QSharedPointer<Transactional> getTransactional() const;

    /*!
     *  Set the transactional element for the port.
     *
     *      @param [in] newTransactional    The new transactional element.
     */
    void setTransactional(QSharedPointer<Transactional> newTransactional);
    
    /*!
     *  Get the left bound of a vectored port.
     *
     *      @return the left bound of the port.
     */
    QString getLeftBound() const;
        
    /*!
     *  Set the left bound of the port.
     *
     *      @param [in] newLeftBound    The new left bound of the port.
     */
    void setLeftBound(QString const& newLeftBound);
    
    /*!
     *  Get the right bound of a vectored port.
     *
     *      @return The right bound of the port.
     */
    QString getRightBound() const;

    /*!
     *  Set the right bound of the port.
     *
     *      @param [in] newRightBound   The new right bound of the port.
     */
    void setRightBound(QString const& newRightBound);

	/*!
	 *  Set the size of the port.
	 *
	 *      @param [in] size    The new size. Port bounds are set to size-1 and 0;
	 */
	void setPortSize(int size);
    
    /*!
     *  Get the default value of the port.
     *
     *      @return The contained default value.
     */
    QString getDefaultValue() const;
    
	/*!
	 *  Set the default value for the port.
	 *
	 *      @param [in] defaultValue    The new default value.
	 */
	void setDefaultValue(const QString& defaultValue);
    
    /*!
     *  Get the direction of the port.
     *
     *      @return Enum direction.
     */
    DirectionTypes::Direction getDirection() const;

	/*!
	 *  Set the direction.
	 *
	 *      @param [in] direction   The new direction.
	 */
	void setDirection(DirectionTypes::Direction direction);
    
	/*!
	 *  Get the value of the logical directions allowed for wires or logical initiatives allowed for transactionals.
	 *
     *      @return True, if the given value is allowed, false otherwise.
	 */
	bool allLogicalDirectionsAllowed() const;
    
	/*!
	 *  Set the port to allow/disallow logical directions / initiatives.
	 *
	 *      @param [in] allowed     The setting.
	 */
	void setAllLogicalDirectionsAllowed(bool allowed);

	/*!
	 *  Get the type name of the port for a given view.
	 *
	 *      @param [in] viewName    The name of the view.
     *
     *      @return The matching type name.
	 */
	QString getTypeName(const QString& viewName = QString("")) const;
    
	/*!
	 *  Set the type name for the port in given view.
	 *
	 *      @param [in] typeName    The name of the type.
	 *      @param [in] viewName    The name of the view used for this type.
	 */
	void setTypeName(const QString& typeName, const QString& viewName = QString(""));

	/*!
	 *  Check if the port has a type name in given view.
	 *
	 *      @param [in] viewName    The name of the view that's type name is searched for.
     *
     *      @return True if a type is found, false otherwise.
	 */
	bool hasType(const QString& viewName = QString()) const;
    
	/*!
	 *  Get the type definition of the given type name.
	 *
	 *      @param [in] typeName    The name of the type that's type definition is wanted.
     *      @return The found type definition.
	 */
	QString getTypeDefinition(const QString& typeName);

	/*!
	 *  Get the type definitions of the wired port.
	 *
     *      @return A list containing the type definitions.
	 */
	QStringList getTypeDefinitions() const;

	/*!
	 *  Set the type definition of the given type name.
	 *
	 *      @param [in] typeName        Identifies the type definition.
	 *      @param [in] typeDefinition  The type definition to be set.
	 */
	void setTypeDefinition(const QString& typeName, const QString& typeDefinition);

	/*!
	 *  Check if the port has a type definition.
	 *
     *      @return True if the type definition is defined.
	 */
	bool hasTypeDefinitions() const;
        
    /*!
     *  Returns true if the ad-hoc visibility is turned on.
     */
    bool isAdHocVisible() const;

    /*!
     *  Sets default the ad-hoc visibility of the port.
     *
     *      @param [in] visible If true, the port is set visible for ad-hoc connections.
     */
    void setAdHocVisible(bool visible);

    /*!
     *  Returns the default position in the parent component's graphical representation.
     */
    QPointF getDefaultPos() const;

    /*!
     *  Sets the default position in the parent component's graphical representation.
     *
     *      @param [in] pos The position to set.
     */
    void setDefaultPos(QPointF const& pos);

    /*!
     *  Gets the left value of the array.
     *
     *      @return The left value of the array.
     */
    QString getArrayLeft() const;

    /*!
     *  Sets the left value for the array.
     *
     *      @param [in] newArrayLeft    The value to be set as left.
     */
    void setArrayLeft(QString const& newArrayLeft);

    /*!
     *  Gets the right value of the array.
     *
     *      @return The right value of the array.
     */
    QString getArrayRight() const;

    /*!
     *  Sets the right value for the array.
     *
     *      @param [in] newArrayRight   The value to be set as right.
     */
    void setArrayRight(QString const& newArrayRight);

    /*!
     *  Gets the tags associated with the port.
     *
     *      @return Tags associated with the port.
     */
    const QString getPortTags() const;

    /*!
     *  Set the port tags.
     *
     *      @param [in] portTags    The tags separated by a comma.
     */
    void setPortTags(const QString& portTags);

    /*!
     *  Get the presence of the port.
     *
     *      @return The presence.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence for the port.
     *
     *      @param [in] newIsPresent    The new presence expression.
     */
    void setIsPresent(QString const& newIsPresent);
    
    /*!
     *  Get the arrays associated with the port.
     *
     *      @return Pointer to a list containing the arrays.
     */
    QSharedPointer<QList<QSharedPointer<Array> > > getArrays() const;

private:

    /*!
     *  Copy the list of arrays.
     *
     *      @param [in] other   The target of the copying.
     */
    void copyArrays(const Port& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The presence of the port.
    QString isPresent_;

    //! The wire port type.
	QSharedPointer<Wire> wire_;

    //! Transactional port type.
	QSharedPointer<Transactional> transactional_;

    //! The list of arrays.
    QSharedPointer<QList<QSharedPointer<Array> > > configurableArrays_;
};

#endif // PORT_H
