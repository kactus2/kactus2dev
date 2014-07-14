/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlcomponentdeclaration.h
 *		Project: Kactus 2
 */

#ifndef VHDLCOMPONENTDECLARATION_H
#define VHDLCOMPONENTDECLARATION_H

#include <kactusGenerators/HDLGenerator/HDLComponentDeclaration.h>

#include "vhdlgeneric.h"

#include <kactusGenerators/HDLGenerator/vhdlportsorter.h>
#include <kactusGenerators/HDLGenerator/HDLPort.h>

#include <IPXACTmodels/component.h>

#include <QObject>
#include <QSharedPointer>
#include <QMap>
#include <QString>
#include <QList>
#include <QTextStream>

class VhdlComponentDeclaration : public HDLComponentDeclaration {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] component Pointer to the component model for this vhdl component.
	 *      @param [in] parent Pointer to the owner of this declaration.
	 *
	*/
	VhdlComponentDeclaration(QSharedPointer<Component> component, QObject* parent);
	
	//! The destructor
	virtual ~VhdlComponentDeclaration();

	/*! Write the component declaration to the text stream.
	 *
	 *      @param [in] stream The text stream to write into.
	 *
	*/
	void write(QTextStream& stream) const;

	/*! Check for port connections of the instances.
	 * 
	 * The ports that are not connected or don't have default value in any 
	 * of the instances are commented out and removed from the instance's 
	 * port maps.
	 *
	*/
	virtual void checkPortConnections();

	/*! Get the type of specified port.
	 *
	 *      @param [in] portName The name of the port.
	 *
	 *      @return The type of the specified port.
	*/
	virtual QString portType(QString const& portName) const;

private:
	//! No copying
	VhdlComponentDeclaration(const VhdlComponentDeclaration& other);

	//! No assignment
	VhdlComponentDeclaration& operator=(const VhdlComponentDeclaration& other);

    /*!
     *  Writes the generic declarations.
     *
     *      @param [in] stream   The output stream to write to.
     */
    void writeGenerics(QTextStream& stream) const;

    /*!
     *  Writes the port declarations.
     *
     *      @param [in] stream   The output stream to write to.
     */
    void writePorts(QTextStream& stream) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	/*! Contains the generic declarations for this component.
	 *
	 * Key: The name of the generic.
	 * Value: Pointer to the generic.
	 */ 
	QMap<QString, QSharedPointer<VhdlGeneric> > generics_;

	/*! Contains the ports for this component.
	* 
	* Key: Class used to sort the ports in correct order.
	* Value: Pointer to the port.
	*/
	QMap<VhdlPortSorter, QSharedPointer<HDLPort> > ports_;

};

#endif // VHDLCOMPONENTDECLARATION_H
