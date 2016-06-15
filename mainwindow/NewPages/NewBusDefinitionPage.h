//-----------------------------------------------------------------------------
// File: newbuspage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.06.2011
//
// Description:
// New bus page.
//-----------------------------------------------------------------------------

#ifndef NEWBUSPAGE_H
#define NEWBUSPAGE_H

#include "NewPage.h"

class LibraryInterface;

//-----------------------------------------------------------------------------
//! New bus page.
//-----------------------------------------------------------------------------
class NewBusDefinitionPage : public NewPage 
{
	Q_OBJECT

public:

	NewBusDefinitionPage(LibraryInterface* libHandler, QWidget *parent);
	
	//! The destructor.
	virtual ~NewBusDefinitionPage();
	
    /*!
     *  Validates the contents of the page thoroughly.
     *
     *      @return True, if the contents are valid. False, if they are invalid.
     *
     *      @remarks Showing message boxes for errors is allowed.
     */
    virtual bool validate();

    /*!
     *  Applies the changes that were done in the page.
     */
    virtual void apply();


signals:

	//! Signaled when new bus should be created.
	void createBus(VLNV const& vlnv, QString const& directory);

private:
	//! No copying.
	NewBusDefinitionPage(const NewBusDefinitionPage& other);

	//! No assignment.
	NewBusDefinitionPage& operator=(const NewBusDefinitionPage& other);	    

};

#endif // NEWBUSPAGE_H
