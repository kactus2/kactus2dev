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

#include <LibraryManager/libraryinterface.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/LibrarySelectorWidget/LibrarySelectorWidget.h>

#include <QVBoxLayout>
#include <QFont>
#include <QDir>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QCoreApplication>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: NewSWDesignPage()
//-----------------------------------------------------------------------------
NewSWDesignPage::NewSWDesignPage(LibraryInterface* libInterface, QWidget* parentDlg):
NewPage(libInterface, VLNV::COMPONENT, tr("New SW Design"), tr("Creates a hierarchical SW design"), parentDlg)
{    
    // Create the VLNV editor.
    vlnvEditor_->setImplementationFilter(true, KactusAttribute::KTS_SW);
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
    Q_ASSERT(prevalidate());

    // Check if the VLNV already exists.
    VLNV vlnv = vlnvEditor_->getVLNV();

    VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".design", vlnv.getVersion());

    // Check if any of the VLNVs already exists.
    if (libInterface_->contains(vlnv))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
            tr("The SW design cannot be created because the VLNV %1 already exists in the library.").arg(vlnv.toString()),
            QMessageBox::Ok, this);
        msgBox.exec();
        return false;
    }

    if (libInterface_->contains(designVLNV))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
            tr("The SW design cannot be created because the VLNV %1 already exists in the library.").arg(designVLNV.toString()),
            QMessageBox::Ok, this);
        msgBox.exec();
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void NewSWDesignPage::apply()
{
    emit createSWDesign(vlnvEditor_->getVLNV(), librarySelector_->getDirectory());
}

