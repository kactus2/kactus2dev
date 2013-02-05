/* 
 * 		filename: newobjectdialog.cpp
 *		Project: Kactus 2
 */

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QDialog>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QCoreApplication>

#include <common/widgets/kactusAttributeEditor/KactusAttributeEditor.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <LibraryManager/libraryinterface.h>

#include "newobjectdialog.h"

NewObjectDialog::NewObjectDialog(LibraryInterface* libInterface, VLNV::IPXactType type,
                                 bool showAttributes, QWidget *parent):
QDialog(parent), 
lh_(libInterface),
attributeEditor_(0), 
vlnvEditor_(0),
directoryEdit_(0), 
okButton_(0)
{
    attributeEditor_ = new KactusAttributeEditor(this);
    attributeEditor_->setVisible(showAttributes);
    connect(attributeEditor_, SIGNAL(productHierarchyChanged()), this, SLOT(onProductHierarchyChanged()));

    QLabel *directoryLabel = new QLabel(tr("Directory:"));
    
	directoryEdit_ = new LibraryPathSelector(this);
	connect(directoryEdit_, SIGNAL(editTextChanged(QString const&)), this, SLOT(onContentChanged()));

    okButton_ = new QPushButton(tr("&OK"));
    okButton_->setEnabled(false);
    connect(okButton_, SIGNAL(released()), this, SLOT(accept()));

    QPushButton *cancelButton = new QPushButton(tr("&Cancel"));
    connect(cancelButton, SIGNAL(released()), this, SLOT(reject()));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(okButton_, QDialogButtonBox::ActionRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);

    vlnvEditor_ = new VLNVEditor(type, libInterface, this, this, true);
    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));
    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(updateDirectory()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(attributeEditor_);
    mainLayout->addWidget(vlnvEditor_);
    mainLayout->addWidget(directoryLabel);
    mainLayout->addWidget(directoryEdit_);
    mainLayout->addWidget(buttonBox);
    mainLayout->addStretch(1);
    setLayout(mainLayout);

    setWindowTitle("New Design");
    setFixedHeight(sizeHint().height());
    resize(400, sizeHint().height());

    okButton_->setDefault(true);

    onProductHierarchyChanged();
}

VLNV NewObjectDialog::getVLNV()
{
    return vlnvEditor_->getVLNV();
}

QString NewObjectDialog::getPath()
{
    return directoryEdit_->currentText();
}

//-----------------------------------------------------------------------------
// Function: onContentChanged()
//-----------------------------------------------------------------------------
void NewObjectDialog::onContentChanged()
{
    // Enable/disable the ok button if the contents are valid/invalid.
    okButton_->setEnabled(!directoryEdit_->currentText().isEmpty() && vlnvEditor_->isValid());
}

//-----------------------------------------------------------------------------
// Function: accept()
//-----------------------------------------------------------------------------
void NewObjectDialog::accept()
{
    // Make sure that the VLNV is not already in use.
    if (lh_->contains(vlnvEditor_->getVLNV()))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
                           tr("The design cannot be created because the VLNV already exists."),
                           QMessageBox::Ok, this);
        
        msgBox.exec();
        return;
    }

    QDialog::accept();
}

void NewObjectDialog::setVLNV( const VLNV& vlnv ) {
	vlnvEditor_->setVLNV(vlnv);
    updateDirectory();
}

//-----------------------------------------------------------------------------
// Function: setKactusAttributes()
//-----------------------------------------------------------------------------
void NewObjectDialog::setKactusAttributes(KactusAttribute::ProductHierarchy prodHier,
                                          KactusAttribute::Firmness firmness)
{
    attributeEditor_->setAttributes(prodHier, firmness);
}

//-----------------------------------------------------------------------------
// Function: getProductHierarchy()
//-----------------------------------------------------------------------------
KactusAttribute::ProductHierarchy NewObjectDialog::getProductHierarchy() const
{
    return attributeEditor_->getProductHierarchy();
}

//-----------------------------------------------------------------------------
// Function: getFirmness()
//-----------------------------------------------------------------------------
KactusAttribute::Firmness NewObjectDialog::getFirmness() const
{
    return attributeEditor_->getFirmness();
}

//-----------------------------------------------------------------------------
// Function: showDialog()
//-----------------------------------------------------------------------------
bool NewObjectDialog::saveAsDialog(QWidget* parent, LibraryInterface* lh, VLNV const& oldVLNV,
                                   KactusAttribute::ProductHierarchy& prodHier,
                                   KactusAttribute::Firmness& firmness,
                                   VLNV& vlnv, QString& directory)
{
    // Create a suggestion for the new vlnv without the version number.
    VLNV suggestion = oldVLNV;
    suggestion.setVersion(QString());

    NewObjectDialog dialog(lh, oldVLNV.getType(), oldVLNV.getType() != VLNV::BUSDEFINITION, parent);
    dialog.setWindowTitle(tr("Save As"));
    dialog.setVLNV(suggestion);
    dialog.setKactusAttributes(prodHier, firmness);

    if (dialog.exec() == QDialog::Rejected)
    {
        return false;
    }

    prodHier = dialog.getProductHierarchy();
    firmness = dialog.getFirmness();
    vlnv = dialog.getVLNV();
    directory = dialog.getPath();
    return true;
}

//-----------------------------------------------------------------------------
// Function: showDialog()
//-----------------------------------------------------------------------------
bool NewObjectDialog::saveAsDialog(QWidget* parent, LibraryInterface* lh,
                                   VLNV const& oldVLNV, VLNV& vlnv, QString& directory,
								   const QString& windowTitle)
{
    // Create a suggestion for the new vlnv without the version number.
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
// Function: updateDirectory()
//-----------------------------------------------------------------------------
void NewObjectDialog::updateDirectory()
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

//-----------------------------------------------------------------------------
// Function: onProductHierarchyChanged()
//-----------------------------------------------------------------------------
void NewObjectDialog::onProductHierarchyChanged()
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