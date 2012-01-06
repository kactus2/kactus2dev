//-----------------------------------------------------------------------------
// File: ImportEndpointsDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.1.2012
//
// Description:
// Dialog for selecting from which component to import endpoints.
//-----------------------------------------------------------------------------

#include "ImportEndpointsDialog.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>

//-----------------------------------------------------------------------------
// Function: ImportEndpointsDialog()
//-----------------------------------------------------------------------------
ImportEndpointsDialog::ImportEndpointsDialog(LibraryInterface* lh, QWidget* parentWnd,
                                             QWidget* parent)
    : QDialog(parent),
      sourceVLNV_(VLNV::COMPONENT, lh, parentWnd, parent),
      importOptionsGroup_(tr("Import Options"), this),
      okButton_(tr("&OK"), this),
      cancelButton_(tr("&Cancel"), this)
{
    setWindowTitle(tr("Import Endpoints"));

    sourceVLNV_.setTitle(tr("Source Endpoints/Application"));
    sourceVLNV_.setImplementationFilter(true, KactusAttribute::KTS_SW);
    sourceVLNV_.updateFiltering();

    // Initialize import options.
    importOptions_[IMPORT_SYNCHRONIZE].setText(tr("Synchronize\nALL existing endpoints are removed."));
    importOptions_[IMPORT_SYNCHRONIZE].setParent(this);
    importOptions_[IMPORT_SYNCHRONIZE].setChecked(true);

    importOptions_[IMPORT_KEEP_EXISTING].setText(tr("Keep existing\nNew endpoints are added/updated and existing ones are kept."));
    importOptions_[IMPORT_KEEP_EXISTING].setParent(this);

    // Create the layout for the import options group.
    QVBoxLayout* optionsLayout = new QVBoxLayout(&importOptionsGroup_);
    optionsLayout->addWidget(&importOptions_[IMPORT_SYNCHRONIZE]);
    optionsLayout->addWidget(&importOptions_[IMPORT_KEEP_EXISTING]);

    // Create the buttons and their layout.
    okButton_.setEnabled(false);
    connect(&okButton_, SIGNAL(released()), this, SLOT(accept()));
    connect(&cancelButton_, SIGNAL(released()), this, SLOT(reject()));

    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(&okButton_, QDialogButtonBox::ActionRole);
    buttonBox->addButton(&cancelButton_, QDialogButtonBox::ActionRole);

    // Create the main layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&sourceVLNV_, Qt::AlignTop);
    layout->addWidget(&importOptionsGroup_, Qt::AlignTop);
    layout->addWidget(buttonBox, Qt::AlignTop);

    // Connect the VLNV edited signal.
    connect(&sourceVLNV_, SIGNAL(vlnvEdited()), this, SLOT(onVlnvEdited()));

    // Fix the dialog size.
    setFixedSize(sizeHint());
}

//-----------------------------------------------------------------------------
// Function: ~ImportEndpointsDialog()
//-----------------------------------------------------------------------------
ImportEndpointsDialog::~ImportEndpointsDialog()
{
}

//-----------------------------------------------------------------------------
// Function: getSourceVLNV()
//-----------------------------------------------------------------------------
VLNV ImportEndpointsDialog::getSourceVLNV() const
{
    return sourceVLNV_.getVLNV();
}

//-----------------------------------------------------------------------------
// Function: getImportOption()
//-----------------------------------------------------------------------------
ImportEndpointsDialog::ImportOption ImportEndpointsDialog::getImportOption() const
{
    if (importOptions_[IMPORT_SYNCHRONIZE].isChecked())
    {
        return IMPORT_SYNCHRONIZE;
    }
    else
    {
        return IMPORT_KEEP_EXISTING;
    }
}

//-----------------------------------------------------------------------------
// Function: onVlnvEdited()
//-----------------------------------------------------------------------------
void ImportEndpointsDialog::onVlnvEdited()
{
    okButton_.setEnabled(sourceVLNV_.isValid());
}
