//-----------------------------------------------------------------------------
// File: NewSWComponentPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.8.2011
//
// Description:
// New SW component page.
//-----------------------------------------------------------------------------

#include "NewSWComponentPage.h"

#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <library/LibraryManager/libraryinterface.h>

//-----------------------------------------------------------------------------
// Function: NewSWComponentPage()
//-----------------------------------------------------------------------------
NewSWComponentPage::NewSWComponentPage(LibraryInterface* libInterface, QWidget* parentDlg):
NewPage(libInterface, VLNV::COMPONENT, tr("New SW Component"), tr("Creates a SW component"), parentDlg)
{
    vlnvEditor_->setImplementationFilter(true, KactusAttribute::SW);
}

//-----------------------------------------------------------------------------
// Function: ~NewSWComponentPage()
//-----------------------------------------------------------------------------
NewSWComponentPage::~NewSWComponentPage()
{

}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NewSWComponentPage::validate()
{
    bool valid = NewPage::validate();

    // Check if the VLNV already exists.
    if (!valid)
    {        
        showErrorForReservedVLVN(vlnvEditor_->getVLNV());
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void NewSWComponentPage::apply()
{
    emit createSWComponent(vlnvEditor_->getVLNV(), selectedPath());
}
