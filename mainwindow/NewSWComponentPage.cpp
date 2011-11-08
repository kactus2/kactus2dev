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
#include <QSettings>

namespace
{
    QString const SW_CREATE_NAMES[SW_CREATE_COUNT] =
    {
        "SW Application\nCreates a software component for packetizing application code.",
        "MCAPI Endpoints\nCreates a software component for packetizing MCAPI endpoints.",
        "SW Platform\nCreates a flat (non-hierarchical) software platform component.",
        "SW Platform Stack\nCreates a hierarchical software platform for combining software platform components."
    };
}
//-----------------------------------------------------------------------------
// Function: NewSWComponentPage()
//-----------------------------------------------------------------------------
NewSWComponentPage::NewSWComponentPage(LibraryInterface* libInterface,
                                       QWidget* parentDlg) : libInterface_(libInterface), vlnvEditor_(0),
                                                             directoryEdit_(0)
{
    // Create the title and description labels labels.
    QLabel* titleLabel = new QLabel(tr("New SW Component"), this);

    QFont font = titleLabel->font();
    font.setPointSize(12);
    font.setBold(true);
    titleLabel->setFont(font);

    QLabel* descLabel = new QLabel(tr("Creates a SW component"), this);

    // Create the SW create type radio button group and a layout for it.
    typeGroup_ = new QGroupBox(tr("Type"), this);
    QGridLayout* itemLayout = new QGridLayout(typeGroup_);

    for (unsigned int i = 0; i < SW_CREATE_COUNT; ++i)
    {
        typeButtons_[i] = new QRadioButton(SW_CREATE_NAMES[i], typeGroup_);
        itemLayout->addWidget(typeButtons_[i], i, 0, 1, 1);
    }

    typeButtons_[0]->setChecked(true);

    // Create the VLNV editor.
    vlnvEditor_ = new VLNVEditor(VLNV::COMPONENT, libInterface, parentDlg, this, true);
    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(updateDirectory()));

    // Create the directory line edit and label.
    QLabel *directoryLabel = new QLabel(tr("Directory:"), this);

    QSettings settings;
    QString defaultDir = settings.value("general/defaultDir", QCoreApplication::applicationDirPath()).toString();
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
    layout->addWidget(typeGroup_);
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
// Function: selectDirectory()
//-----------------------------------------------------------------------------
void NewSWComponentPage::selectDirectory()
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
bool NewSWComponentPage::prevalidate() const
{
    return (vlnvEditor_->isValid() && !directoryEdit_->text().isEmpty());
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
    // Determine the create type.
    SWCreateType createType = SW_CREATE_APPLICATION;

    for (unsigned int i = 0; i < SW_CREATE_COUNT; ++i)
    {
        if (typeButtons_[i]->isChecked())
        {
            createType = static_cast<SWCreateType>(i);
            break;
        }
    }

    emit createSWComponent(createType, vlnvEditor_->getVLNV(), directoryEdit_->text());
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool NewSWComponentPage::onPageChange()
{
    // Reset all fields.
    vlnvEditor_->setVLNV(VLNV());
    typeButtons_[0]->setChecked(true);
    return true;
}

//-----------------------------------------------------------------------------
// Function: updateDirectory()
//-----------------------------------------------------------------------------
void NewSWComponentPage::updateDirectory()
{
    QSettings settings;
    QString dir = settings.value("general/defaultDir", QCoreApplication::applicationDirPath()).toString();

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
