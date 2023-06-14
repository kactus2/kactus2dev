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
#include <common/widgets/LibrarySelectorWidget/LibrarySelectorWidget.h>

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/VLNV.h>
#include <KactusAPI/include/LibraryInterface.h>

#include <QVBoxLayout>
#include <QFont>
#include <QCoreApplication>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: NewPage()
//-----------------------------------------------------------------------------
NewPage::NewPage(LibraryInterface* libInterface, VLNV::IPXactType vlnvType, QString title, QString description,
    QWidget* parentDlg):
PropertyPageView(),
vlnvEditor_(new VLNVEditor(vlnvType, libInterface, parentDlg, this)),
libInterface_(libInterface),
revisionSelector_(new QComboBox(this)),
librarySelector_(new LibrarySelectorWidget(this)),
titleLabel_(new QLabel(title,this)),
descLabel_(new QLabel(description,this))
{   
    QFont font = titleLabel_->font();
    font.setPointSize(12);
    font.setBold(true);
    titleLabel_->setFont(font);

    revisionSelector_->addItem(Document::toString(Document::Revision::Std22));
    revisionSelector_->addItem(Document::toString(Document::Revision::Std14));

    QSettings settings;
    auto defaultRevision = settings.value("General/Revision", Document::toString(Document::Revision::Std22)).toString();
    revisionSelector_->setCurrentText(defaultRevision);

    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(updateDirectory()));

	connect(librarySelector_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    setupLayout();
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

    return isUnusedVLNV(vlnvEditor_->getVLNV());
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
// Function: NewPage::onProductHierarchyChanged()
//-----------------------------------------------------------------------------
void NewPage::onProductHierarchyChanged()
{
    // Update the VLNV's library field if it is either empty or any of the predefined ones.
    VLNV vlnv = vlnvEditor_->getVLNV();

    if (vlnv.getLibrary().isEmpty())
    {
        vlnv.setLibrary(KactusAttribute::hierarchyToString(getProductHierarchy()).toLower());
    }
    else if (KactusAttribute::hierarchyFrom(vlnv.getLibrary().toLower()) != KactusAttribute::KTS_PRODHIER_COUNT)
    {
        vlnv.setLibrary(KactusAttribute::hierarchyToString(getProductHierarchy()).toLower());
    }

    vlnvEditor_->setVLNV(vlnv);
}

//-----------------------------------------------------------------------------
// Function: NewPage::getProductHierarchy()
//-----------------------------------------------------------------------------
KactusAttribute::ProductHierarchy NewPage::getProductHierarchy() const
{
    return KactusAttribute::KTS_PRODHIER_COUNT;
}

//-----------------------------------------------------------------------------
// Function: NewPage::isUnusedVLNV()
//-----------------------------------------------------------------------------
bool NewPage::isUnusedVLNV(VLNV const& vlnv) const
{
    return !libInterface_->contains(vlnv);
}

//-----------------------------------------------------------------------------
// Function: NewPage::selectedPath()
//-----------------------------------------------------------------------------
QString NewPage::selectedPath() const
{
    return librarySelector_->getPath();
}

//-----------------------------------------------------------------------------
// Function: NewPage::showErrorForReservedVLVN()
//-----------------------------------------------------------------------------
void NewPage::showErrorForReservedVLVN(VLNV const& vlnv)
{
    QString type = type2Show(vlnv.getType());
    QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
        tr("The %1 cannot be created because the VLNV %2" 
        " already exists in the library.").arg(type, vlnv.toString()), QMessageBox::Ok, this);
    msgBox.exec();
}

//-----------------------------------------------------------------------------
// Function: NewPage::type2Show()
//-----------------------------------------------------------------------------
QString NewPage::type2Show(VLNV::IPXactType const& type)
{
    return VLNV::IPXactType2String(type);
}

//-----------------------------------------------------------------------------
// Function: NewPage::selectedRevision()
//-----------------------------------------------------------------------------
Document::Revision NewPage::selectedRevision() const
{
    return Document::toRevision(revisionSelector_->currentText());
}

//-----------------------------------------------------------------------------
// Function: setupLayout()
//-----------------------------------------------------------------------------
void NewPage::setupLayout()
{
    auto revisionLayout = new QHBoxLayout();
    revisionLayout->addWidget(new QLabel(tr("Revision: "), this));
    revisionLayout->addWidget(revisionSelector_, 1);
  
    auto layout = new QVBoxLayout(this);
    layout->addWidget(titleLabel_);
    layout->addWidget(descLabel_);
    layout->addSpacing(12);
    layout->addWidget(vlnvEditor_);
    layout->addLayout(revisionLayout);
    layout->addWidget(librarySelector_);
}
