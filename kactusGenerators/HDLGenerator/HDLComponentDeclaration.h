//-----------------------------------------------------------------------------
// File: HDLComponentDeclaration.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Base class for HDl component declarations.
//-----------------------------------------------------------------------------

#ifndef HDLCOMPONENTDECLARATION_H
#define HDLCOMPONENTDECLARATION_H

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/generaldeclarations.h>

#include "HDLObject.h"
#include "HDLComponentInstance.h"

#include <QObject>
#include <QSharedPointer>
#include <QMap>
#include <QString>
#include <QList>
#include <QTextStream>

//-----------------------------------------------------------------------------
//! Base class for HDl component declarations.
//-----------------------------------------------------------------------------
class HDLComponentDeclaration : public HDLObject {
	Q_OBJECT

public:

	/* 
     * The constructor
	 *
	 *      @param [in] component   The component model for this HDL declaration.
	 *      @param [in] parent      The parent object.	 
	 */
	HDLComponentDeclaration(QSharedPointer<Component> component, QObject* parent);
	
	//! The destructor
	virtual ~HDLComponentDeclaration();

	/*! Get the IP-Xact model of the component declaration.
     *
	 *      @return The IP-Xact model of component.
	 */
	QSharedPointer<Component> componentModel() const;

	/*! Add an instantiation of this component.
	 *
	 *      @param [in] instance Pointer to the instance.
	 */
	void addInstantiation(QSharedPointer<HDLComponentInstance> instance);

	/*! Check for port connections of the instances.
	 * 
	 * The ports that are not connected or don't have default value in any 
	 * of the instances are commented out and removed from the instance's 
	 * port maps.
	 *
	*/
	virtual void checkPortConnections() = 0;

protected:
    
    /*!
     *  Get the instances of this declaration.
     *
     *      @return The instances of this declaration.
     */
    QList<QSharedPointer< HDLComponentInstance> > instances() const;

private:
	//! No copying
	HDLComponentDeclaration(const HDLComponentDeclaration& other);

	//! No assignment
	HDLComponentDeclaration& operator=(const HDLComponentDeclaration& other);

	//! Pointer to the component that this declaration represents.
	QSharedPointer<Component> component_;

	//! Contains pointer to the instances of this port.
	QList<QSharedPointer<HDLComponentInstance> > instantations_;
};

#endif // HDLCOMPONENTDECLARATION_H
