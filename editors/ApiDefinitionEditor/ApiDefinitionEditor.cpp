//-----------------------------------------------------------------------------
// File: ApiDefinitionEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 18.4.2012
//
// Description:
// Editor for API definitions.
//-----------------------------------------------------------------------------

#include "ApiDefinitionEditor.h"

#include <common/dialogs/newObjectDialog/newobjectdialog.h>

#include <IPXACTmodels/kactusExtensions/ComDefinition.h>
#include <IPXACTmodels/kactusExtensions/ApiDefinition.h>
#include <IPXACTmodels/kactusExtensions/validators/ApiDefinitionValidator.h>

#include <QGridLayout>

//-----------------------------------------------------------------------------
// Function: ApiDefinitionEditor::ApiDefinitionEditor()
//-----------------------------------------------------------------------------
ApiDefinitionEditor::ApiDefinitionEditor(QWidget *parent, LibraryInterface* libHandler,
    QSharedPointer<ApiDefinition> apiDef) :
    TabDocument(parent, libHandler, DOC_PROTECTION_SUPPORT),
    nameGroup_(this),
    apiDef_(apiDef),
    comDefVLNVEdit_(VLNV::COMDEFINITION, libHandler, parent, this),
    dataTypeList_(tr("Data types"), this),
    functionEditor_(this)
{
    // Initialize the editors.
    nameGroup_.setTitle(tr("API definition"));
    nameGroup_.setDocumentNameGroup(apiDef_, getLibHandler()->getPath(apiDef_->getVlnv()));
    nameGroup_.setFlat(true);
    dataTypeList_.initialize(*apiDef->getDataTypes());

    functionEditor_.updateDataTypes(*apiDef->getDataTypes());
    functionEditor_.restore(*apiDef);
    functionEditor_.setFlat(true);

    comDefVLNVEdit_.setTitle(tr("COM definition reference"));
    comDefVLNVEdit_.setVLNV(apiDef->getComDefinitionRef());
    comDefVLNVEdit_.setFlat(true);
    comDefVLNVEdit_.setRevisionFilter(true, apiDef->getRevision());
    updateComDefinition();

    connect(&nameGroup_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&dataTypeList_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&dataTypeList_, SIGNAL(contentChanged()), this, SLOT(updateDataTypeLists()), Qt::UniqueConnection);
    connect(&comDefVLNVEdit_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&comDefVLNVEdit_, SIGNAL(contentChanged()), this, SLOT(updateComDefinition()), Qt::UniqueConnection);
    connect(&functionEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    // Setup the layout.
    auto layout = new QGridLayout(this);
    layout->addWidget(&nameGroup_, 0, 0, 2, 1);
    layout->addWidget(&dataTypeList_, 1, 1, 1, 1);
    layout->addWidget(&comDefVLNVEdit_, 0, 1, 1, 1, Qt::AlignTop);
    layout->addWidget(&functionEditor_, 2, 0, 1, 2);


    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 3);

    setModified(false);

    // Set the document name and type.
    VLNV vlnv = apiDef_->getVlnv();
    setDocumentName(vlnv.getName() + " (" + vlnv.getVersion() + ")");
    setDocumentType(DocumentType::API_DEFINITION);

    // Open in unlocked mode by default only if the version is draft.
    setProtection(vlnv.getVersion() != "draft");
}


//-----------------------------------------------------------------------------
// Function: ApiDefinitionEditor::setProtection()
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
    QSharedPointer<Document> libComp = getLibHandler()->getModel(apiDef_->getVlnv());
    apiDef_ = libComp.staticCast<ApiDefinition>();

    // Update the editors.
    nameGroup_.setDocumentNameGroup(apiDef_, getLibHandler()->getPath(apiDef_->getVlnv()));
    comDefVLNVEdit_.setVLNV(apiDef_->getComDefinitionRef());
    dataTypeList_.initialize(*apiDef_->getDataTypes());
    functionEditor_.restore(*apiDef_);

    setModified(false);
    TabDocument::refresh();
}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionEditor::getDocumentVLNV()
//-----------------------------------------------------------------------------
VLNV ApiDefinitionEditor::getDocumentVLNV() const
{
    return apiDef_->getVlnv();
}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionEditor::validate()
//-----------------------------------------------------------------------------
bool ApiDefinitionEditor::validate(QVector<QString>& errorList)
{
    applyChanges();

	ApiDefinitionValidator validator;
    validator.findErrorsIn( errorList, apiDef_ );

    return errorList.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionEditor::save()
//-----------------------------------------------------------------------------
bool ApiDefinitionEditor::save()
{
    applyChanges();
    getLibHandler()->writeModelToFile(apiDef_);

    return TabDocument::save();
}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionEditor::saveAs()
//-----------------------------------------------------------------------------
bool ApiDefinitionEditor::saveAs()
{
    // Ask the user for a new VLNV and directory.
    VLNV vlnv;
    QString directory;

    if (!NewObjectDialog::saveAsDialog(parentWidget(), getLibHandler(), apiDef_->getVlnv(), vlnv, directory))
    {
        return false;
    }

    // Create a copy of the object and update its VLNV.
    apiDef_ = QSharedPointer<ApiDefinition>(new ApiDefinition(*apiDef_));

    vlnv.setType(VLNV::APIDEFINITION);
    apiDef_->setVlnv(vlnv);

    // Apply changes to the copy.
    applyChanges();

    if (getLibHandler()->writeModelToFile(directory, apiDef_))
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
        QSharedPointer<Document const> libComp = getLibHandler()->getModelReadOnly(vlnv);
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

//-----------------------------------------------------------------------------
// Function: ApiDefinitionEditor::getIdentifyingVLNV()
//-----------------------------------------------------------------------------
VLNV ApiDefinitionEditor::getIdentifyingVLNV() const
{
	return getDocumentVLNV();
}

