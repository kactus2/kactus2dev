//-----------------------------------------------------------------------------
// File: NewPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.8.2013
//
// Description:
// Base class for all New pages.
//-----------------------------------------------------------------------------

#include "NewPage.h"

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/kactusAttributeEditor/KactusAttributeEditor.h>
#include <common/widgets/LibrarySelectorWidget/LibrarySelectorWidget.h>
#include <common/dialogs/LibrarySettingsDialog/LibrarySettingsDialog.h>

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
// Function: NewPage()
//-----------------------------------------------------------------------------
NewPage::NewPage(LibraryInterface* libInterface, VLNV::IPXactType vlnvType, 
    QString title, QString description, QWidget* parentDlg)
    : PropertyPageView(),
      libInterface_(libInterface),
      vlnvEditor_(new VLNVEditor(vlnvType, libInterface, parentDlg, this, true)),
      librarySelector_(new LibrarySelectorWidget(this)),
      titleLabel_(new QLabel(title,this)),
      descLabel_(new QLabel(description,this))
{   
    QFont font = titleLabel_->font();
    font.setPointSize(12);
    font.setBold(true);
    titleLabel_->setFont(font);

    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(updateDirectory()));

	connect(librarySelector_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ~NewPage()
//-----------------------------------------------------------------------------
NewPage::~NewPage()
{
}

//-----------------------------------------------------------------------------
// Function: prevalidate()
//-----------------------------------------------------------------------------
bool NewPage::prevalidate() const
{
    return (vlnvEditor_->isValid() && librarySelector_->isValid());
}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NewPage::validate()
{
    Q_ASSERT(prevalidate());

    // Check if the VLNV already exists.
    if (libInterface_->contains(vlnvEditor_->getVLNV()))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
                           tr("The component cannot be created because the VLNV already exists in the library."),
                           QMessageBox::Ok, this);
        msgBox.exec();
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool NewPage::onPageChange()
{
    // Discard the VLNV and reset the attributes.
    librarySelector_->reset();
    vlnvEditor_->setVLNV(VLNV());
    return true;
}

//-----------------------------------------------------------------------------
// Function: updateDirectory()
//-----------------------------------------------------------------------------
void NewPage::updateDirectory()
{
    QString vlnvDir;

    VLNV vlnv = vlnvEditor_->getVLNV();

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

    librarySelector_->updatePath(vlnvDir);
}

//-----------------------------------------------------------------------------
// Function: setupLayout()
//-----------------------------------------------------------------------------
void NewPage::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(titleLabel_);
    layout->addWidget(descLabel_);
    layout->addSpacing(12);
    layout->addWidget(vlnvEditor_);
    layout->addSpacing(12);
    layout->addWidget(librarySelector_);
    layout->addStretch(1);
}
