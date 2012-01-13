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

#include <QVBoxLayout>
#include <QFont>
#include <QDir>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QCoreApplication>
#include <QMessageBox>
#include <QSettings>

//-----------------------------------------------------------------------------
// Function: NewSWDesignPage()
//-----------------------------------------------------------------------------
NewSWDesignPage::NewSWDesignPage(LibraryInterface* libInterface,
                                 QWidget* parentDlg) : libInterface_(libInterface),
                                 vlnvEditor_(0), directoryEdit_(0)
{
    // Create the title and description labels labels.
    QLabel* titleLabel = new QLabel(tr("New SW Design"), this);

    QFont font = titleLabel->font();
    font.setPointSize(12);
    font.setBold(true);
    titleLabel->setFont(font);

    QLabel* descLabel = new QLabel(tr("Creates a software mapping design"), this);

    // Create the VLNV editor.
    vlnvEditor_ = new VLNVEditor(VLNV::COMPONENT, libInterface, parentDlg, this, true);
    vlnvEditor_->setImplementationFilter(true, KactusAttribute::KTS_SW);
    vlnvEditor_->setSWTypeFilter(true, KactusAttribute::KTS_SW_MAPPING);
    vlnvEditor_->updateFiltering();

    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(updateDirectory()));

    // Create the directory line edit and label.
    QLabel *directoryLabel = new QLabel(tr("Directory:"), this);

    QSettings settings;
    QString defaultDir = settings.value("library/defaultLocation", QCoreApplication::applicationDirPath()).toString();
    directoryEdit_ = new QLineEdit(defaultDir, this);
    connect(directoryEdit_, SIGNAL(textChanged(QString const&)), this, SIGNAL(contentChanged()));

    QPushButton* pathButton = new QPushButton("Browse...", this);
    connect(pathButton, SIGNAL(clicked()), this, SLOT(selectDirectory()));
    QHBoxLayout *pathLayout = new QHBoxLayout;
    pathLayout->addWidget(directoryLabel);
    pathLayout->addWidget(directoryEdit_, 1);
    pathLayout->addWidget(pathButton);

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
// Function: ~NewSWDesignPage()
//-----------------------------------------------------------------------------
NewSWDesignPage::~NewSWDesignPage()
{
}

//-----------------------------------------------------------------------------
// Function: selectDirectory()
//-----------------------------------------------------------------------------
void NewSWDesignPage::selectDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        directoryEdit_->text());

    if (!dir.isEmpty())
    {
        directoryEdit_->setText(dir);
    }
}

//-----------------------------------------------------------------------------
// Function: prevalidate()
//-----------------------------------------------------------------------------
bool NewSWDesignPage::prevalidate() const
{
    return (vlnvEditor_->isValid() && !directoryEdit_->text().isEmpty());
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
    emit createSWDesign(vlnvEditor_->getVLNV(), directoryEdit_->text());
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool NewSWDesignPage::onPageChange()
{
    // Discard the VLNV.
    vlnvEditor_->setVLNV(VLNV());
    return true;
}

//-----------------------------------------------------------------------------
// Function: updateDirectory()
//-----------------------------------------------------------------------------
void NewSWDesignPage::updateDirectory()
{
    QSettings settings;
    QString dir = settings.value("library/defaultLocation", QCoreApplication::applicationDirPath()).toString();

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

    directoryEdit_->setText(dir);
}
