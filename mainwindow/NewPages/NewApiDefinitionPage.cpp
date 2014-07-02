//-----------------------------------------------------------------------------
// File: NewApiDefinitionPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.4.2012
//
// Description:
// New API definition page.
//-----------------------------------------------------------------------------

#include "NewApiDefinitionPage.h"

#include <library/LibraryManager/libraryinterface.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>

//-----------------------------------------------------------------------------
// Function: NewApiDefinitionPage()
//-----------------------------------------------------------------------------
NewApiDefinitionPage::NewApiDefinitionPage(LibraryInterface* libInterface, QWidget* parentDlg): 
    NewPage(libInterface, VLNV::APIDEFINITION, tr("New API definition"), tr("Creates an API definition"), 
        parentDlg)
{
}

//-----------------------------------------------------------------------------
// Function: ~NewApiDefinitionPage()
//-----------------------------------------------------------------------------
NewApiDefinitionPage::~NewApiDefinitionPage()
{

}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NewApiDefinitionPage::validate()
{
    // Check if the VLNV already exists.    
    bool valid = NewPage::validate();    

    if (!valid)
    {
        showErrorForReservedVLVN(vlnvEditor_->getVLNV());
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void NewApiDefinitionPage::apply()
{
    emit createApiDefinition(vlnvEditor_->getVLNV(), selectedPath());
}