/* 
 *
 *  Created on: 20.6.2011
 *      Author: Antti Kamppi
 * 		filename: newbuspage.cpp
 */

#include "newbuspage.h"

#include <common/widgets/LibrarySelectorWidget/LibrarySelectorWidget.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QLabel>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: NewBusPage()
//-----------------------------------------------------------------------------
NewBusPage::NewBusPage(LibraryInterface* libHandler, QWidget *parentDlg):
NewPage(libHandler, VLNV::BUSDEFINITION, tr("New Bus"), tr("Creates a new bus"), parentDlg)
{

	Q_ASSERT_X(libHandler, "NewBusPage constructor",
		"Null LibraryInterface pointer given as parameter");	
}

//-----------------------------------------------------------------------------
// Function: ~NewBusPage()
//-----------------------------------------------------------------------------
NewBusPage::~NewBusPage() {
}


//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NewBusPage::validate() {
	Q_ASSERT(prevalidate());

    VLNV vlnv = vlnvEditor_->getVLNV();

    /*VLNV absVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
                 vlnv.getName().remove(".busDef", Qt::CaseInsensitive) + ".absDef", vlnv.getVersion());*/

    // Check if any of the VLNVs already exists.
    if (libInterface_->contains(vlnv))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
            tr("The bus cannot be created because the VLNV %1 already exists in the library.").arg(vlnv.toString()),
            QMessageBox::Ok, this);
        msgBox.exec();
        return false;
    }

    /*if (libInterface_->contains(absVLNV))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
            tr("The bus cannot be created because the VLNV %1 already exists in the library.").arg(absVLNV.toString()),
            QMessageBox::Ok, this);
        msgBox.exec();
        return false;
    }*/

	return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void NewBusPage::apply() {
	 emit createBus(vlnvEditor_->getVLNV(), librarySelector_->getPath());
}
