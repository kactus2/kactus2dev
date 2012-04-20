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

#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: ApiDefinitionEditor()
//-----------------------------------------------------------------------------
ApiDefinitionEditor::ApiDefinitionEditor(QWidget *parent, QWidget* parentWnd,
                                         LibraryInterface* libHandler,
                                         QSharedPointer<ApiDefinition> apiDef)
    : TabDocument(parent, DOC_PROTECTION_SUPPORT),
      libHandler_(libHandler),
      apiDef_(apiDef),
      dataTypeList_(tr("Data types"), this),
      functionEditor_(this)
{
    // Initialize the editors.
    dataTypeList_.initialize(apiDef->getDataTypes());
    functionEditor_.restore(*apiDef);

    connect(&dataTypeList_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&functionEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    // Setup the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&dataTypeList_);
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
    dataTypeList_.setEnabled(!locked);
    functionEditor_.setEnabled(!locked);
}

//-----------------------------------------------------------------------------
// Function: getComponentVLNV()
//-----------------------------------------------------------------------------
VLNV ApiDefinitionEditor::getComponentVLNV() const
{
    return *apiDef_->getVlnv();
}

//-----------------------------------------------------------------------------
// Function: save()
//-----------------------------------------------------------------------------
bool ApiDefinitionEditor::save()
{
    apiDef_->setDataTypes(dataTypeList_.items());
    functionEditor_.save(*apiDef_);

    libHandler_->writeModelToFile(apiDef_);

    return TabDocument::save();
}

//-----------------------------------------------------------------------------
// Function: saveAs()
//-----------------------------------------------------------------------------
bool ApiDefinitionEditor::saveAs()
{
    return TabDocument::saveAs();
}

