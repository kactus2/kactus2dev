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
    Q_ASSERT(prevalidate());

    // Check if the VLNV already exists.
    VLNV vlnv = vlnvEditor_->getVLNV();

    if (libInterface_->contains(vlnv))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
                           tr("The API definition cannot be created because the VLNV %1"
                              " already exists in the library.").arg(vlnv.toString()),
                           QMessageBox::Ok, this);
        msgBox.exec();
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void NewApiDefinitionPage::apply()
{
    emit createApiDefinition(vlnvEditor_->getVLNV(), librarySelector_->getPath());
}