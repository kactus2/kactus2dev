/* 
 *
 *  Created on: 20.6.2011
 *      Author: Antti Kamppi
 * 		filename: newbuspage.h
 */

#ifndef NEWBUSPAGE_H
#define NEWBUSPAGE_H

#include "NewPage.h"

#include <library/LibraryManager/libraryinterface.h>


#include <QPushButton>

class NewBusPage : public NewPage 
{
	Q_OBJECT

public:

	NewBusPage(LibraryInterface* libHandler, QWidget *parent);
	
	//! \brief The destructor
	virtual ~NewBusPage();

	
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

	//! \brief Signaled when new bus should be created.
	void createBus(VLNV const& vlnv, QString const& directory);

private:
	//! \brief No copying
	NewBusPage(const NewBusPage& other);

	//! \brief No assignment
	NewBusPage& operator=(const NewBusPage& other);	    

};

#endif // NEWBUSPAGE_H
