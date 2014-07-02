//-----------------------------------------------------------------------------
// File: NewComDefinitionPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.4.2012
//
// Description:
// New COM definition page.
//-----------------------------------------------------------------------------

#include "NewComDefinitionPage.h"

#include <library/LibraryManager/libraryinterface.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>

//-----------------------------------------------------------------------------
// Function: NewComDefinitionPage()
//-----------------------------------------------------------------------------
NewComDefinitionPage::NewComDefinitionPage(LibraryInterface* libInterface, QWidget* parentDlg)
    : NewPage(libInterface, VLNV::COMDEFINITION, tr("New COM definition"), 
        tr("Creates a communication definition"), parentDlg)
{
 
}

//-----------------------------------------------------------------------------
// Function: ~NewComDefinitionPage()
//-----------------------------------------------------------------------------
NewComDefinitionPage::~NewComDefinitionPage()
{

}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NewComDefinitionPage::validate()
{
    // Check if the VLNV already exists.    
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
void NewComDefinitionPage::apply()
{
    emit createComDefinition(vlnvEditor_->getVLNV(), selectedPath());
}
