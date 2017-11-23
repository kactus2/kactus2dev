//-----------------------------------------------------------------------------
// File: vhdlcomponentdeclaration.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 26.10.2011
//
// Description:
// VHDL component declaration.
//-----------------------------------------------------------------------------

#ifndef VHDLCOMPONENTDECLARATION_H
#define VHDLCOMPONENTDECLARATION_H

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/DirectionTypes.h>

#include "vhdlobject.h"
#include "vhdlgeneric.h"
#include "vhdlport.h"
#include "vhdlportsorter.h"
#include "vhdlcomponentinstance.h"

#include <QObject>
#include <QSharedPointer>
#include <QMap>
#include <QString>
#include <QList>
#include <QTextStream>

//-----------------------------------------------------------------------------
//! VHDL component declaration.
//-----------------------------------------------------------------------------
class VhdlComponentDeclaration : public VhdlObject
{

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] component   Pointer to the component model for this vhdl component.
	 *      @param [in] parent      Pointer to the owner of this declaration.
	 */
	VhdlComponentDeclaration(QSharedPointer<Component> component, QString const& viewName);
	
	//! The destructor.
	virtual ~VhdlComponentDeclaration();

	/*!
     *  Write the component declaration to the text stream.
	 *
	 *      @param [in] stream  The text stream to write into.
	 */
	void write(QTextStream& stream) const;

	/*!
     *  Get the type name of the vhdl component declaration.
	 *
	 *      @return QString contains the type name.
	 */
	QString typeName() const;

	/*!
     *  Get the description of the vhdl component declaration.
	 *
	 *      @return QString contains the description for the component declaration.
	 */
	QString description() const;

	/*!
     *  Get pointer to the IP-Xact model of the component declaration.
	 *
	 *      @return QSharedPointer<Component> Pointer to the IP-Xact model of component.
	 */
	QSharedPointer<Component> componentModel() const;

	/*!
     *  Add an instantiation of this component.
	 *
	 *      @param instance Pointer to the instance.
	 */
	void addInstantiation(QSharedPointer<VhdlComponentInstance> instance);

	/*!
     *  Check if the component has generic with given name.
	 *
	 *      @param [in] genericName The name of the generic to search for.
	 *
	 *      @return bool True if the generic is found.
	 */
	bool hasGeneric(const QString& genericName) const;

	/*!
     *  Check for port connections of the instances.
	 * 
	 *  The ports that are not connected or don't have default value in any of the instances are commented out 
     *  and removed from the instance's port maps.
	 */
	void checkPortConnections();

	/*!
     *  Get the type of specified port.
	 *
	 *      @param [in] portName    The name of the port.
	 *
	 *      @return QString The type of the specified port.
	 */
	QString portType(const QString& portName) const;

	/*!
     *  Check if the type of the port is scalar.
	 *
	 *      @param [in] portName    Specifies the port to check.
	 *
	 *      return bool True if port is scalar.
	 */
	bool isScalarPort(const QString& portName) const;

	/*!
     *  Set the name of the entity for the component.
	 *
	 *      @param [in] entityName  The name for the entity of this component.
	 */
	void setEntityName(const QString& entityName);

	/*!
     *  Get the direction of the specified port on this component.
	 *
	 *      @param [in] portName    The name of the port that's direction is requested.
	 *
	 *      @return General::Direction Specifies the direction of the port.
	 */
	DirectionTypes::Direction portDirection(const QString& portName) const;

	/*!
     *  Get the physical left bound of the port.
	 *
	 *      @param [in] portName    The name of the port that's bound is wanted.
	 *
	 *      @return The left bound.
	 */
	QString getPortPhysLeftBound(const QString& portName) const;

	/*!
     *  Get the physical right bound of the port.
	 *
	 *      @param [in] portName    The name of the port that's bound is wanted.
	 *
	 *      @return The right bound.
	 */
	QString getPortPhysRightBound(const QString& portName) const;

private:
	
    //! No copying.
	VhdlComponentDeclaration(const VhdlComponentDeclaration& other);

	//! No assignment.
	VhdlComponentDeclaration& operator=(const VhdlComponentDeclaration& other);

    /*!
     *  Get the name of the bus interface containing the selected port.
     *
     *      @param [in] portName    The name of the selected port.
     *
     *      @return The name of the contianing bus interface, or "none" if no interface is found.
     */
    QString getContainingBusInterfaceName(QString const& portName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the component that this declaration represents.
	QSharedPointer<Component> component_;

	//! The type of the vhdl component.
	QString typeName_;

	/*!
     *  Contains the generic declarations for this component.
	 *
	 *  Key: The name of the generic. Value: Pointer to the generic.
	 */
	QMap<QString, QSharedPointer<VhdlGeneric> > generics_;

	/*!
     *  Contains the ports for this component.
	 * 
	 *  Key: Class used to sort the ports in correct order. Value: Pointer to the port.
	 */
	QMap<VhdlPortSorter, QSharedPointer<VhdlPort> > ports_;

	//! Contains pointer to the instances of this port.
	QList<QSharedPointer<VhdlComponentInstance> > instantations_;
};

#endif // VHDLCOMPONENTDECLARATION_H
