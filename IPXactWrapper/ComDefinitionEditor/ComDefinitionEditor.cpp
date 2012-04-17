//-----------------------------------------------------------------------------
// File: ComDefinitionEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.4.2012
//
// Description:
// Editor for editing COM definitions.
//-----------------------------------------------------------------------------

#include "ComDefinitionEditor.h"

#include <models/ComProperty.h>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: ComDefinitionEditor()
//-----------------------------------------------------------------------------
ComDefinitionEditor::ComDefinitionEditor(QWidget *parent, QWidget* parentWnd,
                                         LibraryInterface* libHandler,
                                         QSharedPointer<ComDefinition> comDef)
    : TabDocument(parent, DOC_PROTECTION_SUPPORT),
      libHandler_(libHandler),
      comDef_(comDef),
      dataTypeList_(tr("Data types"), this),
      propertyEditor_(this)
{
    // Initialize the editors.
    dataTypeList_.initialize(comDef->getDataTypes());
    propertyEditor_.setProperties(comDef->getProperties());

    connect(&dataTypeList_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&propertyEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    
    // Setup the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&dataTypeList_);
    layout->addWidget(&propertyEditor_, 1);
    
    setModified(false);

    // Set the document name and type.
    VLNV const* vlnv = comDef_->getVlnv();
    setDocumentName(vlnv->getName() + " (" + vlnv->getVersion() + ")");
    setDocumentType(tr("COM Definition"));

    // Open in unlocked mode by default only if the version is draft.
    setProtection(vlnv->getVersion() != "draft");
}

//-----------------------------------------------------------------------------
// Function: ~ComDefinitionEditor()
//-----------------------------------------------------------------------------
ComDefinitionEditor::~ComDefinitionEditor()
{
}

//-----------------------------------------------------------------------------
// Function: setProtection()
//-----------------------------------------------------------------------------
void ComDefinitionEditor::setProtection(bool locked)
{
    TabDocument::setProtection(locked);
}

//-----------------------------------------------------------------------------
// Function: getComponentVLNV()
//-----------------------------------------------------------------------------
VLNV ComDefinitionEditor::getComponentVLNV() const
{
    return *comDef_->getVlnv();
}

//-----------------------------------------------------------------------------
// Function: save()
//-----------------------------------------------------------------------------
bool ComDefinitionEditor::save()
{
    QList< QSharedPointer<ComProperty> > properties;

    foreach (QSharedPointer<ComProperty> prop, propertyEditor_.getProperties())
    {
        properties.append(QSharedPointer<ComProperty>(new ComProperty(*prop.data())));
    }

    comDef_->setProperties(properties);
    comDef_->setDataTypes(dataTypeList_.items());

    libHandler_->writeModelToFile(comDef_);

    return TabDocument::save();
}

//-----------------------------------------------------------------------------
// Function: saveAs()
//-----------------------------------------------------------------------------
bool ComDefinitionEditor::saveAs()
{
    return TabDocument::saveAs();
}

