//-----------------------------------------------------------------------------
// File: ApiDefinitionEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.4.2012
//
// Description:
// Editor for API definitions.
//-----------------------------------------------------------------------------

#include "ApiDefinitionEditor.h"

#include <models/ComDefinition.h>

#include <common/dialogs/newObjectDialog/newobjectdialog.h>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: ApiDefinitionEditor()
//-----------------------------------------------------------------------------
ApiDefinitionEditor::ApiDefinitionEditor(QWidget *parent, LibraryInterface* libHandler,
                                         QSharedPointer<ApiDefinition> apiDef)
    : TabDocument(parent, DOC_PROTECTION_SUPPORT),
      libHandler_(libHandler),
      apiDef_(apiDef),
      comDefVLNVEdit_(VLNV::COMDEFINITION, libHandler, parent, this),
      dataTypeList_(tr("Data types"), this),
      functionEditor_(this)
{
    // Initialize the editors.
    dataTypeList_.initialize(apiDef->getDataTypes());

    functionEditor_.updateDataTypes(apiDef->getDataTypes());
    functionEditor_.restore(*apiDef);

    comDefVLNVEdit_.setTitle(tr("COM definition reference (optional)"));
    comDefVLNVEdit_.setVLNV(apiDef->getComDefinitionRef());
    updateComDefinition();

    connect(&dataTypeList_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&dataTypeList_, SIGNAL(contentChanged()), this, SLOT(updateDataTypeLists()), Qt::UniqueConnection);
    connect(&comDefVLNVEdit_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&comDefVLNVEdit_, SIGNAL(contentChanged()), this, SLOT(updateComDefinition()), Qt::UniqueConnection);
    connect(&functionEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    // Setup the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&dataTypeList_);
    layout->addWidget(&comDefVLNVEdit_);
    layout->addWidget(&functionEditor_, 1);

    setModified(false);

    // Set the document name and type.
    VLNV const* vlnv = apiDef_->getVlnv();
    setDocumentName(vlnv->getName() + " (" + vlnv->getVersion() + ")");
    setDocumentType(tr("API Definition"));

    // Open in unlocked mode by default only if the version is draft.
    setProtection(vlnv->getVersion() != "draft");
}

//-----------------------------------------------------------------------------
// Function: ~ApiDefinitionEditor()
//-----------------------------------------------------------------------------
ApiDefinitionEditor::~ApiDefinitionEditor()
{
}

//-----------------------------------------------------------------------------
// Function: setProtection()
//-----------------------------------------------------------------------------
void ApiDefinitionEditor::setProtection(bool locked)
{
    TabDocument::setProtection(locked);
    setEnabled(!locked);
}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionEditor::refresh()
//-----------------------------------------------------------------------------
void ApiDefinitionEditor::refresh()
{
    QSharedPointer<LibraryComponent> libComp = libHandler_->getModel(*apiDef_->getVlnv());
    apiDef_ = libComp.staticCast<ApiDefinition>();

    // Update the editors.
    comDefVLNVEdit_.setVLNV(apiDef_->getComDefinitionRef());
    dataTypeList_.initialize(apiDef_->getDataTypes());
    functionEditor_.restore(*apiDef_);

    setModified(false);
    TabDocument::refresh();
}

//-----------------------------------------------------------------------------
// Function: getDocumentVLNV()
//-----------------------------------------------------------------------------
VLNV ApiDefinitionEditor::getDocumentVLNV() const
{
    return *apiDef_->getVlnv();
}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionEditor::validate()
//-----------------------------------------------------------------------------
bool ApiDefinitionEditor::validate(QStringList& errorList)
{
    applyChanges();
    return apiDef_->isValid(errorList);
}

//-----------------------------------------------------------------------------
// Function: save()
//-----------------------------------------------------------------------------
bool ApiDefinitionEditor::save()
{
    applyChanges();
    libHandler_->writeModelToFile(apiDef_);

    return TabDocument::save();
}

//-----------------------------------------------------------------------------
// Function: saveAs()
//-----------------------------------------------------------------------------
bool ApiDefinitionEditor::saveAs()
{
    // Ask the user for a new VLNV and directory.
    VLNV vlnv;
    QString directory;

    if (!NewObjectDialog::saveAsDialog(parentWidget(), libHandler_, *apiDef_->getVlnv(), vlnv, directory))
    {
        return false;
    }

    // Create a copy of the object and update its VLNV.
    apiDef_ = QSharedPointer<ApiDefinition>(new ApiDefinition(*apiDef_));

    vlnv.setType(VLNV::APIDEFINITION);
    apiDef_->setVlnv(vlnv);

    // Apply changes to the copy.
    applyChanges();

    if (libHandler_->writeModelToFile(directory, apiDef_))
    {
        setDocumentName(vlnv.getName() + " (" + vlnv.getVersion() + ")");
        return TabDocument::saveAs();
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionEditor::updateDataTypeLists()
//-----------------------------------------------------------------------------
void ApiDefinitionEditor::updateDataTypeLists()
{
    functionEditor_.updateDataTypes(dataTypeList_.items());
}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionEditor::applyChanges()
//-----------------------------------------------------------------------------
void ApiDefinitionEditor::applyChanges()
{
    apiDef_->setComDefinitionRef(comDefVLNVEdit_.getVLNV());
    apiDef_->setDataTypes(dataTypeList_.items());
    functionEditor_.save(*apiDef_);
}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionEditor::updateComDefinition()
//-----------------------------------------------------------------------------
void ApiDefinitionEditor::updateComDefinition()
{
    VLNV vlnv = comDefVLNVEdit_.getVLNV();

    // Retrieve the new COM definition if the VLNV is valid.
    if (vlnv.isValid())
    {
        QSharedPointer<LibraryComponent const> libComp = libHandler_->getModelReadOnly(vlnv);
        QSharedPointer<ComDefinition const> comDef = libComp.dynamicCast<ComDefinition const>();
        functionEditor_.setComDefinition(comDef);
    }
    else
    {
        functionEditor_.setComDefinition(QSharedPointer<ComDefinition const>());
    }
}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionEditor::showEvent()
//-----------------------------------------------------------------------------
void ApiDefinitionEditor::showEvent(QShowEvent* event)
{
    TabDocument::showEvent(event);
    emit helpUrlRequested("definitions/apidefinition.html");
}

