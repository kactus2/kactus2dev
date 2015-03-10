//-----------------------------------------------------------------------------
// File: NewSWDesignPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 14.11.2011
//
// Description:
// New software design page.
//-----------------------------------------------------------------------------

#include "NewSWDesignPage.h"

#include <library/LibraryManager/libraryinterface.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>

//-----------------------------------------------------------------------------
// Function: NewSWDesignPage()
//-----------------------------------------------------------------------------
NewSWDesignPage::NewSWDesignPage(LibraryInterface* libInterface, QWidget* parentDlg):
NewPage(libInterface, VLNV::COMPONENT, tr("New SW Design"), tr("Creates a hierarchical SW design"), parentDlg)
{    
    // Create the VLNV editor.
    vlnvEditor_->setImplementationFilter(true, KactusAttribute::SW);
}

//-----------------------------------------------------------------------------
// Function: ~NewSWDesignPage()
//-----------------------------------------------------------------------------
NewSWDesignPage::~NewSWDesignPage()
{
}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NewSWDesignPage::validate()
{
    // Check if the VLNV already exists.
    VLNV vlnv = vlnvEditor_->getVLNV();
    bool validVLNV = NewPage::validate();

    VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".design", vlnv.getVersion());

    // Check if any of the VLNVs already exists.
    if (!validVLNV)
    {
        showErrorForReservedVLVN(vlnv);
        return false;
    }

    if (!isUnusedVLNV(designVLNV))
    {
        showErrorForReservedVLVN(designVLNV);       
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void NewSWDesignPage::apply()
{
    emit createSWDesign(vlnvEditor_->getVLNV(), selectedPath());
}

