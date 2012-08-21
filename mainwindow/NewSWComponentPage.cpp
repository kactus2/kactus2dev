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
libInterface_(libInterface),
vlnvEditor_(0),
directoryEdit_(0)
{
    // Create the title and description labels labels.
    QLabel* titleLabel = new QLabel(tr("New SW Component"), this);

    QFont font = titleLabel->font();
    font.setPointSize(12);
    font.setBold(true);
    titleLabel->setFont(font);

    QLabel* descLabel = new QLabel(tr("Creates a SW component"), this);

    // Create the VLNV editor.
    vlnvEditor_ = new VLNVEditor(VLNV::COMPONENT, libInterface, parentDlg, this, true);
    vlnvEditor_->setImplementationFilter(true, KactusAttribute::KTS_SW);

    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(updateDirectory()));

    // Create the directory line edit and label.
    QLabel *directoryLabel = new QLabel(tr("Directory:"), this);

	directoryEdit_ = new LibraryPathSelector(this);
	connect(directoryEdit_, SIGNAL(editTextChanged(QString const&)), this, SIGNAL(contentChanged()));

    QHBoxLayout *pathLayout = new QHBoxLayout;
    pathLayout->addWidget(directoryLabel);
    pathLayout->addWidget(directoryEdit_, 1);

    // Setup the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(titleLabel);
    layout->addWidget(descLabel);
    layout->addSpacing(12);
    layout->addWidget(vlnvEditor_);
    layout->addSpacing(12);
    layout->addLayout(pathLayout);
    layout->addStretch(1);
}

//-----------------------------------------------------------------------------
// Function: ~NewSWComponentPage()
//-----------------------------------------------------------------------------
NewSWComponentPage::~NewSWComponentPage()
{
}

//-----------------------------------------------------------------------------
// Function: prevalidate()
//-----------------------------------------------------------------------------
bool NewSWComponentPage::prevalidate() const
{
    return (vlnvEditor_->isValid() && !directoryEdit_->currentText().isEmpty());
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
    emit createSWComponent(vlnvEditor_->getVLNV(), directoryEdit_->currentText());
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool NewSWComponentPage::onPageChange()
{
    // Reset all fields.
    vlnvEditor_->setVLNV(VLNV());
    return true;
}

//-----------------------------------------------------------------------------
// Function: updateDirectory()
//-----------------------------------------------------------------------------
void NewSWComponentPage::updateDirectory()
{
    QString dir = directoryEdit_->currentLocation();

    VLNV vlnv = vlnvEditor_->getVLNV();

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

    directoryEdit_->setEditText(dir);
}
