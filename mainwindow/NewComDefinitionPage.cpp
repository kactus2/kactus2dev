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
    Q_ASSERT(prevalidate());

    // Check if the VLNV already exists.
    VLNV vlnv = vlnvEditor_->getVLNV();

    if (libInterface_->contains(vlnv))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
                           tr("The COM definition cannot be created because the VLNV %1"
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
void NewComDefinitionPage::apply()
{
    emit createComDefinition(vlnvEditor_->getVLNV(), librarySelector_->getPath());
}
