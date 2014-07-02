/* 
 *
 *  Created on: 20.6.2011
 *      Author: Antti Kamppi
 * 		filename: newbuspage.cpp
 */

#include "newbuspage.h"

#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: NewBusPage()
//-----------------------------------------------------------------------------
NewBusPage::NewBusPage(LibraryInterface* libHandler, QWidget *parentDlg):
NewPage(libHandler, VLNV::BUSDEFINITION, tr("New Bus"), tr("Creates a new bus"), parentDlg)
{

	Q_ASSERT_X(libHandler, "NewBusPage constructor",
		"Null LibraryInterface pointer given as parameter");	
}

//-----------------------------------------------------------------------------
// Function: ~NewBusPage()
//-----------------------------------------------------------------------------
NewBusPage::~NewBusPage()
{

}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NewBusPage::validate()
{
    // Check if any of the VLNVs already exists.
    bool validVLNV = NewPage::validate();

    if (!validVLNV)
    {        
        showErrorForReservedVLVN(vlnvEditor_->getVLNV());
    }

	return validVLNV;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void NewBusPage::apply()
{
	 emit createBus(vlnvEditor_->getVLNV(), selectedPath());
}
