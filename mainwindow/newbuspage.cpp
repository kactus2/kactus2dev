/* 
 *
 *  Created on: 20.6.2011
 *      Author: Antti Kamppi
 * 		filename: newbuspage.cpp
 */

#include "newbuspage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QLabel>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QCoreApplication>

NewBusPage::NewBusPage(LibraryInterface* libHandler, QWidget *parentDlg):
PropertyPageView(),
libInterface_(libHandler), 
vlnvEditor_(VLNV::BUSDEFINITION, libHandler, parentDlg, this, true),
directoryEdit_(this) {

	Q_ASSERT_X(libHandler, "NewBusPage constructor",
		"Null LibraryInterface pointer given as parameter");

	connect(&vlnvEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(updateDirectory()));

	// Create the title and description labels labels.
	QLabel* titleLabel = new QLabel(tr("New Bus"), this);

	QFont font = titleLabel->font();
	font.setPointSize(12);
	font.setBold(true);
	titleLabel->setFont(font);

	QLabel* descLabel = new QLabel(tr("Creates a new bus"), this);

	// Create the directory line edit and label.
	QLabel *directoryLabel = new QLabel(tr("Directory:"), this);

	connect(&directoryEdit_, SIGNAL(editTextChanged(QString const&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	QHBoxLayout *pathLayout = new QHBoxLayout;
	pathLayout->addWidget(directoryLabel);
	pathLayout->addWidget(&directoryEdit_, 1);

	// Setup the layout.
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(titleLabel);
	layout->addWidget(descLabel);
	layout->addSpacing(12);
	layout->addWidget(&vlnvEditor_);
	layout->addSpacing(12);
	layout->addLayout(pathLayout);
	layout->addStretch(1);
}

NewBusPage::~NewBusPage() {
}

bool NewBusPage::prevalidate() const {
	return (vlnvEditor_.isValid() && !directoryEdit_.currentText().isEmpty());
}

bool NewBusPage::validate() {
	Q_ASSERT(prevalidate());

    VLNV vlnv = vlnvEditor_.getVLNV();

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

void NewBusPage::apply() {
	 emit createBus(vlnvEditor_.getVLNV(), directoryEdit_.currentText());
}

bool NewBusPage::onPageChange() {
	// Discard the VLNV.
	vlnvEditor_.setVLNV(VLNV());
	return true;
}

//-----------------------------------------------------------------------------
// Function: updateDirectory()
//-----------------------------------------------------------------------------
void NewBusPage::updateDirectory()
{
    QString dir = directoryEdit_.currentLocation();

    VLNV vlnv = vlnvEditor_.getVLNV();

    if (!vlnv.getVendor().isEmpty())
    {
        dir += "/" + vlnv.getVendor();

        if (!vlnv.getLibrary().isEmpty())
        {
            dir += "/" + vlnv.getLibrary();

            if (!vlnv.getName().isEmpty())
            {
                dir += "/" + vlnv.getName();

                if (!vlnv.getVersion().isEmpty())
                {
                    dir += "/" + vlnv.getVersion();
                }
            }
        }
    }

    directoryEdit_.setEditText(dir);
}
