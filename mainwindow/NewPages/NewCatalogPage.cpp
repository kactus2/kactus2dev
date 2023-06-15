//-----------------------------------------------------------------------------
// File: NewCatalogPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.02.2017
//
// Description:
// New Catalog page.
//-----------------------------------------------------------------------------

#include "NewCatalogPage.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: NewCatalogPage()
//-----------------------------------------------------------------------------
NewCatalogPage::NewCatalogPage(LibraryInterface* libInterface, QWidget* parentDlg):
NewPage(libInterface, VLNV::CATALOG, tr("New Catalog"), tr("Creates a catalog for organizing IP"), parentDlg)
{
    QVBoxLayout* topLayout = dynamic_cast<QVBoxLayout*>(layout());
    topLayout->addStretch(1);
}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NewCatalogPage::validate()
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
void NewCatalogPage::apply()
{
    emit createCatalog(vlnvEditor_->getVLNV(), selectedRevision(), selectedPath());
}
