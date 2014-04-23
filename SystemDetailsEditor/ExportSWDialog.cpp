//-----------------------------------------------------------------------------
// File: ExportSWDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 31.7.2012
//
// Description:
// Dialog for exporting SW from system design.
//-----------------------------------------------------------------------------

#include "ExportSWDialog.h"

#include <models/component.h>

#include <library/LibraryManager/libraryinterface.h>

#include <common/widgets/LineEditEx/LineEditEx.h>

#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: ExportSWDialog::ExportSWDialog()
//-----------------------------------------------------------------------------
ExportSWDialog::ExportSWDialog(LibraryInterface* lh, QWidget* parent)
    : QDialog(parent),
      lh_(lh),
      infoLabel_(new QLabel(tr("Exporting SW requires a new system component to be created."), this)),
      vlnvEdit_(new VLNVEditor(VLNV::COMPONENT, lh, this, this)),
      directoryEditor_(new LibrarySelectorWidget(this)),
      buttonBox_(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this)),
      layout_(new QVBoxLayout(this))
{
    setWindowTitle(tr("Export SW"));

    // Set widget settings.
    vlnvEdit_->setTitle(tr("VLNV for the new system component"));
    vlnvEdit_->addNameExtension(".sysdesign");
    vlnvEdit_->addNameExtension(".sysdesigncfg");

    QGroupBox* separator = new QGroupBox(this);
    separator->setFlat(true);

    directoryEditor_->layout()->setContentsMargins(0,11,0,11);

    // Create layouts.
    layout_->addWidget(infoLabel_);
    layout_->addSpacing(12);
    layout_->addWidget(vlnvEdit_);
    layout_->addWidget(directoryEditor_);
    layout_->addWidget(separator);
    layout_->addWidget(buttonBox_);

    // Setup connections.
    connect(buttonBox_->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
        this, SLOT(accept()), Qt::UniqueConnection);
    connect(buttonBox_->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
        this, SLOT(reject()), Qt::UniqueConnection);

    connect(vlnvEdit_, SIGNAL(contentChanged()), this, SLOT(updateDirectory()), Qt::UniqueConnection);
    connect(vlnvEdit_, SIGNAL(contentChanged()), this, SLOT(validate()), Qt::UniqueConnection);

    setFixedHeight(sizeHint().height());
    resize(400, sizeHint().height());
    validate();
}

//-----------------------------------------------------------------------------
// Function: ExportSWDialog::~ExportSWDialog()
//-----------------------------------------------------------------------------
ExportSWDialog::~ExportSWDialog()
{
}

//-----------------------------------------------------------------------------
// Function: ExportSWDialog::accept()
//-----------------------------------------------------------------------------
void ExportSWDialog::accept()
{
    VLNV vlnv = vlnvEdit_->getVLNV();

    VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName() + ".sysdesign", vlnv.getVersion());
    VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName() + ".sysdesigncfg", vlnv.getVersion());

    if (lh_->contains(vlnv))
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                           tr("VLNV %1 already exists in the library.").arg(vlnv.toString()),
                           QMessageBox::Ok, (QWidget*)parent());
        msgBox.exec();
        return;
    }

    if (lh_->contains(designVLNV))
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            tr("VLNV %1 already exists in the library.").arg(designVLNV.toString()),
            QMessageBox::Ok, (QWidget*)parent());
        msgBox.exec();
        return;
    }

    if (lh_->contains(desConfVLNV))
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            tr("VLNV %1 already exists in the library.").arg(desConfVLNV.toString()),
            QMessageBox::Ok, (QWidget*)parent());
        msgBox.exec();
        return;
    }

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: ExportSWDialog::getVLNV()
//-----------------------------------------------------------------------------
VLNV ExportSWDialog::getVLNV() const
{
    return vlnvEdit_->getVLNV();
}

//-----------------------------------------------------------------------------
// Function: ExportSWDialog::getPath()
//-----------------------------------------------------------------------------
QString ExportSWDialog::getPath() const
{
    return directoryEditor_->getPath();
}

//-----------------------------------------------------------------------------
// Function: updateDirectory()
//-----------------------------------------------------------------------------
void ExportSWDialog::updateDirectory()
{
    QString vlnvDir;

    VLNV vlnv = vlnvEdit_->getVLNV();

    if (!vlnv.getVendor().isEmpty())
    {
        vlnvDir += "/" + vlnv.getVendor();

        if (!vlnv.getLibrary().isEmpty())
        {
            vlnvDir += "/" + vlnv.getLibrary();

            if (!vlnv.getName().isEmpty())
            {
                vlnvDir += "/" + vlnv.getName();

                if (!vlnv.getVersion().isEmpty())
                {
                    vlnvDir += "/" + vlnv.getVersion();
                }
            }
        }
    }

    directoryEditor_->updatePath(vlnvDir);
}

//-----------------------------------------------------------------------------
// Function: ExportSWDialog::validate()
//-----------------------------------------------------------------------------
void ExportSWDialog::validate()
{
    QAbstractButton* btnOK = buttonBox_->button(QDialogButtonBox::Ok);
    btnOK->setEnabled(vlnvEdit_->isValid() && directoryEditor_->isValid());
}
