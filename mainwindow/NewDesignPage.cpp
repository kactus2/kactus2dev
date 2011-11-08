//-----------------------------------------------------------------------------
// File: NewDesignPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.6.2011
//
// Description:
// New design property page.
//-----------------------------------------------------------------------------

#include "NewDesignPage.h"

#include <LibraryManager/libraryinterface.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/kactusAttributeEditor/KactusAttributeEditor.h>

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
// Function: NewDesignPage()
//-----------------------------------------------------------------------------
NewDesignPage::NewDesignPage(LibraryInterface* libInterface,
                             QWidget* parentDlg) : libInterface_(libInterface), attributeEditor_(0),
                                                   vlnvEditor_(0), directoryEdit_(0)
{
    // Create the title and description labels labels.
    QLabel* titleLabel = new QLabel(tr("New Design"), this);

    QFont font = titleLabel->font();
    font.setPointSize(12);
    font.setBold(true);
    titleLabel->setFont(font);

    QLabel* descLabel = new QLabel(tr("Creates a hierarchical design"), this);

    // Create the attribute editor.
    attributeEditor_ = new KactusAttributeEditor(this);
    connect(attributeEditor_, SIGNAL(productHierarchyChanged()), this, SLOT(onProductHierarchyChanged()));

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
    layout->addWidget(attributeEditor_);
    layout->addWidget(vlnvEditor_);
    layout->addSpacing(12);
    layout->addLayout(pathLayout);
    layout->addStretch(1);

    onProductHierarchyChanged();
}

//-----------------------------------------------------------------------------
// Function: ~NewDesignPage()
//-----------------------------------------------------------------------------
NewDesignPage::~NewDesignPage()
{
}

//-----------------------------------------------------------------------------
// Function: selectDirectory()
//-----------------------------------------------------------------------------
void NewDesignPage::selectDirectory()
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
bool NewDesignPage::prevalidate() const
{
    return (vlnvEditor_->isValid() && !directoryEdit_->text().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NewDesignPage::validate()
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
// Function: apply()
//-----------------------------------------------------------------------------
void NewDesignPage::apply()
{
    emit createDesign(attributeEditor_->getProductHierarchy(),
                      attributeEditor_->getFirmness(),
                      vlnvEditor_->getVLNV(), directoryEdit_->text());
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool NewDesignPage::onPageChange()
{
    // Discard the VLNV and reset the attributes.
    vlnvEditor_->setVLNV(VLNV());
    attributeEditor_->setAttributes(KactusAttribute::KTS_GLOBAL, KactusAttribute::KTS_TEMPLATE);
    onProductHierarchyChanged();
    return true;
}

//-----------------------------------------------------------------------------
// Function: onProductHierarchyChanged()
//-----------------------------------------------------------------------------
void NewDesignPage::onProductHierarchyChanged()
{
    // Update the VLNV's library field if it is either empty or any of the predefined ones.
    VLNV vlnv = vlnvEditor_->getVLNV();

    if (vlnv.getLibrary().isEmpty())
    {
        vlnv.setLibrary(KactusAttribute::valueToString(attributeEditor_->getProductHierarchy()).toLower());
    }
    else
    {
        for (unsigned int i = 0; i < KactusAttribute::KTS_PRODHIER_COUNT; ++i)
        {
            if (vlnv.getLibrary().toLower() ==
                KactusAttribute::valueToString(static_cast<KactusAttribute::ProductHierarchy>(i)).toLower())
            {
                vlnv.setLibrary(KactusAttribute::valueToString(attributeEditor_->getProductHierarchy()).toLower());
                break;
            }
        }
    }

    vlnvEditor_->setVLNV(vlnv);
}

//-----------------------------------------------------------------------------
// Function: updateDirectory()
//-----------------------------------------------------------------------------
void NewDesignPage::updateDirectory()
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
