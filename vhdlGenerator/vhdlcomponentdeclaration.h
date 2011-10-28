/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlcomponentdeclaration.h
 *		Project: Kactus 2
 */

#ifndef VHDLCOMPONENTDECLARATION_H
#define VHDLCOMPONENTDECLARATION_H

#include <models/component.h>
#include <models/generaldeclarations.h>

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

class VhdlGenerator2;

class VhdlComponentDeclaration : public QObject {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component model for this vhdl component.
	 * \param parent Pointer to the owner of this declaration.
	 *
	*/
	VhdlComponentDeclaration(QSharedPointer<Component> component, 
		VhdlGenerator2* parent);
	
	//! \brief The destructor
	virtual ~VhdlComponentDeclaration();

	/*! \brief Write the component declaration to the text stream.
	 *
	 * \param stream The text stream to write into.
	 *
	*/
	void write(QTextStream& stream);

	/*! \brief Get the type name of the vhdl component declaration.
	 *
	 * \return QString contains the type name.
	*/
	QString typeName() const;

	/*! \brief Get the description of the vhdl component declaration.
	 *
	 * \return QString contains the description for the component declaration.
	*/
	QString description() const;

	/*! \brief Get pointer to the IP-Xact model of the component declaration.
	 *
	 *
	 * \return QSharedPointer<Component> Pointer to the IP-Xact model of component.
	*/
	QSharedPointer<Component> componentModel() const;

	/*! \brief Add an instantiation of this component.
	 *
	 * \param instance Pointer to the instance.
	 *
	*/
	void addInstantiation(QSharedPointer<VhdlComponentInstance> instance);

	/*! \brief Check if the component has generic with given name.
	 *
	 * \param genericName The name of the generic to search for.
	 *
	 * \return bool True if the generic is found.
	*/
	bool hasGeneric(const QString& genericName) const;

	/*! \brief Check for port connections of the instances.
	 * 
	 * The ports that are not connected or don't have default value in any 
	 * of the instances are commented out and removed from the instance's 
	 * port maps.
	 *
	*/
	void checkPortConnections();

signals:

	//! \brief Send a notification to user.
	void noticeMessage(const QString& noticeMessage);

	//! \brief Send an error message to user.
	void errorMessage(const QString& errorMessage);

private:
	//! \brief No copying
	VhdlComponentDeclaration(const VhdlComponentDeclaration& other);

	//! \brief No assignment
	VhdlComponentDeclaration& operator=(const VhdlComponentDeclaration& other);

	//! \brief Pointer to the parent of this declaration.
	VhdlGenerator2* parent_;

	//! \brief Pointer to the component that this declaration represents.
	QSharedPointer<Component> component_;

	//! \brief The type of the vhdl component.
	QString typeName_;

	/*! \brief Contains the generic declarations for this component.
	 *
	 * Key: The name of the generic.
	 * Value: Pointer to the generic.
	 */ 
	QMap<QString, QSharedPointer<VhdlGeneric> > generics_;

	/*! \brief Contains the ports for this component.
	* 
	* Key: Class used to sort the ports in correct order.
	* Value: Pointer to the port.
	*/
	QMap<VhdlPortSorter, QSharedPointer<VhdlPort> > ports_;

	//! \brief Contains pointer to the instances of this port.
	QList<QSharedPointer<VhdlComponentInstance> > instantations_;
};

#endif // VHDLCOMPONENTDECLARATION_H
