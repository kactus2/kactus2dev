//-----------------------------------------------------------------------------
// File: newobjetdialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 
//
// Description:
// NewObjectDialog is used to query the VLNV and path of a newly created document.
//-----------------------------------------------------------------------------

#include "newobjectdialog.h"

#include <common/widgets/kactusAttributeEditor/KactusAttributeEditor.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <library/LibraryInterface.h>

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: NewObjectDialog::NewObjectDialog()
//-----------------------------------------------------------------------------
NewObjectDialog::NewObjectDialog(LibraryInterface* libInterface, VLNV::IPXactType type, bool showAttributes,
    QWidget *parent):
QDialog(parent), 
    library_(libInterface),
    attributeEditor_(new KactusAttributeEditor(this)), 
    vlnvEditor_(new VLNVEditor(type, libInterface, this, this)),
    directoryEditor_(new LibrarySelectorWidget(this)), 
    okButton_(new QPushButton(tr("&OK")))
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setWindowTitle("New HW Design");

    attributeEditor_->setVisible(showAttributes);
   
    directoryEditor_->layout()->setContentsMargins(0,0,0,0);
    
    okButton_->setDefault(true);
    okButton_->setEnabled(false);

    setupLayout();

    onProductHierarchyChanged();

    connect(attributeEditor_, SIGNAL(productHierarchyChanged()), this, SLOT(onProductHierarchyChanged()));

    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));
    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(updateDirectory()));
    connect(directoryEditor_, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));

    connect(okButton_, SIGNAL(released()), this, SLOT(accept()));    
}

//-----------------------------------------------------------------------------
// Function: NewObjectDialog::~NewObjectDialog()
//-----------------------------------------------------------------------------
NewObjectDialog::~NewObjectDialog()
{

}

//-----------------------------------------------------------------------------
// Function: NewObjectDialog::getVLNV()
//-----------------------------------------------------------------------------
VLNV NewObjectDialog::getVLNV()
{
    return vlnvEditor_->getVLNV();
}

//-----------------------------------------------------------------------------
// Function: NewObjectDialog::getPath()
//-----------------------------------------------------------------------------
QString NewObjectDialog::getPath()
{
    return directoryEditor_->getPath();
}

//-----------------------------------------------------------------------------
// Function: NewObjectDialog::onContentChanged()
//-----------------------------------------------------------------------------
void NewObjectDialog::onContentChanged()
{
    okButton_->setEnabled(directoryEditor_->isValid() && vlnvEditor_->isValid());
}

//-----------------------------------------------------------------------------
// Function: NewObjectDialog::accept()
//-----------------------------------------------------------------------------
void NewObjectDialog::accept()
{
    // Make sure that the VLNV is not already in use.
    if (library_->contains(vlnvEditor_->getVLNV()))
    {
        QMessageBox::critical(this, QCoreApplication::applicationName(),
            tr("The document cannot be created because the VLNV already exists."),
            QMessageBox::Ok);
        
        return;
    }

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: NewObjectDialog::setVLNV()
//-----------------------------------------------------------------------------
void NewObjectDialog::setVLNV( const VLNV& vlnv )
{
	vlnvEditor_->setVLNV(vlnv);
    updateDirectory();
}

//-----------------------------------------------------------------------------
// Function: NewObjectDialog::setKactusAttributes()
//-----------------------------------------------------------------------------
void NewObjectDialog::setKactusAttributes(KactusAttribute::ProductHierarchy prodHier,
    KactusAttribute::Firmness firmness, QVector<QPair<QString, QString> > tags)
{
    attributeEditor_->setAttributes(prodHier, firmness, tags);
}

//-----------------------------------------------------------------------------
// Function: NewObjectDialog::getProductHierarchy()
//-----------------------------------------------------------------------------
KactusAttribute::ProductHierarchy NewObjectDialog::getProductHierarchy() const
{
    return attributeEditor_->getProductHierarchy();
}

//-----------------------------------------------------------------------------
// Function: NewObjectDialog::getFirmness()
//-----------------------------------------------------------------------------
KactusAttribute::Firmness NewObjectDialog::getFirmness() const
{
    return attributeEditor_->getFirmness();
}

//-----------------------------------------------------------------------------
// Function: newobjectdialog::getTags()
//-----------------------------------------------------------------------------
QVector<QPair<QString, QString> > NewObjectDialog::getTags() const
{
    return attributeEditor_->getTags();
}

//-----------------------------------------------------------------------------
// Function: NewObjectDialog::saveAsDialog()
//-----------------------------------------------------------------------------
bool NewObjectDialog::saveAsDialog(QWidget* parent, LibraryInterface* lh, VLNV const& oldVLNV,
    KactusAttribute::ProductHierarchy& prodHier, KactusAttribute::Firmness& firmness,
    QVector<QPair<QString, QString> >& tags, VLNV& vlnv, QString& directory)
{
    VLNV suggestion = oldVLNV;
    suggestion.setVersion(QString());

    NewObjectDialog dialog(lh, oldVLNV.getType(), oldVLNV.getType() != VLNV::BUSDEFINITION, parent);
    dialog.setWindowTitle(tr("Save As"));
    dialog.setVLNV(suggestion);
    dialog.setKactusAttributes(prodHier, firmness, tags);

    if (dialog.exec() == QDialog::Rejected)
    {
        return false;
    }

    prodHier = dialog.getProductHierarchy();
    firmness = dialog.getFirmness();
    tags = dialog.getTags();

    vlnv = dialog.getVLNV();
    directory = dialog.getPath();
    return true;
}

//-----------------------------------------------------------------------------
// Function: NewObjectDialog::saveAsDialog()
//-----------------------------------------------------------------------------
bool NewObjectDialog::saveAsDialog(QWidget* parent, LibraryInterface* lh,
    VLNV const& oldVLNV, VLNV& vlnv, QString& directory, QString const& windowTitle)
{
    VLNV suggestion = oldVLNV;
    suggestion.setVersion(QString());

    NewObjectDialog dialog(lh, oldVLNV.getType(), false, parent);
    dialog.setWindowTitle(windowTitle);
    dialog.setVLNV(suggestion);

    if (dialog.exec() == QDialog::Rejected)
    {
        return false;
    }

    vlnv = dialog.getVLNV();
    directory = dialog.getPath();
    return true;
}

//-----------------------------------------------------------------------------
// Function: NewObjectDialog::updateDirectory()
//-----------------------------------------------------------------------------
void NewObjectDialog::updateDirectory()
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

    directoryEditor_->updatePath(vlnvDir);
}

//-----------------------------------------------------------------------------
// Function: NewObjectDialog::onProductHierarchyChanged()
//-----------------------------------------------------------------------------
void NewObjectDialog::onProductHierarchyChanged()
{
    // Update the VLNV's library field if it is either empty or any of the predefined ones.
    VLNV vlnv = vlnvEditor_->getVLNV();

    if (vlnv.getLibrary().isEmpty())
    {
        vlnv.setLibrary(KactusAttribute::hierarchyToString(attributeEditor_->getProductHierarchy()).toLower());
    }
    else
    {
        for (unsigned int i = 0; i < KactusAttribute::KTS_PRODHIER_COUNT; ++i)
        {
            if (vlnv.getLibrary().toLower() ==
                KactusAttribute::hierarchyToString(static_cast<KactusAttribute::ProductHierarchy>(i)).toLower())
            {
                vlnv.setLibrary(KactusAttribute::hierarchyToString(attributeEditor_->getProductHierarchy()).toLower());
                break;
            }
        }
    }

    vlnvEditor_->setVLNV(vlnv);
}

//-----------------------------------------------------------------------------
// Function: NewObjectDialog::setupLayout()
//-----------------------------------------------------------------------------
void NewObjectDialog::setupLayout()
{
    QGroupBox* separator = new QGroupBox(this);
    separator->setFlat(true);

    QPushButton* cancelButton = new QPushButton(tr("&Cancel"));
    connect(cancelButton, SIGNAL(released()), this, SLOT(reject()));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(okButton_, QDialogButtonBox::ActionRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(attributeEditor_);
    mainLayout->addWidget(vlnvEditor_);
    mainLayout->addWidget(directoryEditor_);
    mainLayout->addWidget(separator);
    mainLayout->addWidget(buttonBox);
    mainLayout->addStretch(1);

    setFixedHeight(sizeHint().height());
    resize(400, sizeHint().height());
}
