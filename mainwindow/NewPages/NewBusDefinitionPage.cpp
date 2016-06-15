//-----------------------------------------------------------------------------
// File: newbuspage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.06.2011
//
// Description:
// New bus page.
//-----------------------------------------------------------------------------

#include "NewBusDefinitionPage.h"

#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: NewBusPage()
//-----------------------------------------------------------------------------
NewBusDefinitionPage::NewBusDefinitionPage(LibraryInterface* libHandler, QWidget *parentDlg):
NewPage(libHandler, VLNV::BUSDEFINITION, tr("New Bus Definition"), tr("Creates a new bus definition"), parentDlg)
{
	Q_ASSERT_X(libHandler, "NewBusPage constructor", "Null LibraryInterface pointer given as parameter");	
}

//-----------------------------------------------------------------------------
// Function: ~NewBusPage()
//-----------------------------------------------------------------------------
NewBusDefinitionPage::~NewBusDefinitionPage()
{

}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NewBusDefinitionPage::validate()
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
void NewBusDefinitionPage::apply()
{
	 emit createBus(vlnvEditor_->getVLNV(), selectedPath());
}
