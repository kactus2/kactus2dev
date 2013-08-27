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
#include <common/widgets/LibrarySelectorWidget/LibrarySelectorWidget.h>

#include <LibraryManager/vlnv.h>
#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>
#include <QFont>
#include <QDir>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QCoreApplication>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: NewSWComponentPage()
//-----------------------------------------------------------------------------
NewSWComponentPage::NewSWComponentPage(LibraryInterface* libInterface, QWidget* parentDlg):
NewPage(libInterface, VLNV::COMPONENT, tr("New SW Component"), tr("Creates a SW component"), parentDlg)
{
    
    // Create the VLNV editor.
    vlnvEditor_->setImplementationFilter(true, KactusAttribute::KTS_SW);

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
    Q_ASSERT(prevalidate());

    // Check if the VLNV already exists.
    if (libInterface_->contains(vlnvEditor_->getVLNV()))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
            tr("The SW component cannot be created because the VLNV already exists in the library."),
            QMessageBox::Ok, this);
        msgBox.exec();
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void NewSWComponentPage::apply()
{
    emit createSWComponent(vlnvEditor_->getVLNV(), librarySelector_->getPath());
}
