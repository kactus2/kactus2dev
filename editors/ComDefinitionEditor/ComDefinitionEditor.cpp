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

#include <common/dialogs/newObjectDialog/newobjectdialog.h>

#include <IPXACTmodels/ComProperty.h>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: ComDefinitionEditor()
//-----------------------------------------------------------------------------
ComDefinitionEditor::ComDefinitionEditor(QWidget *parent,
                                         LibraryInterface* libHandler,
                                         QSharedPointer<ComDefinition> comDef)
    : TabDocument(parent, DOC_PROTECTION_SUPPORT),
      libHandler_(libHandler),
      comDef_(comDef),
      dataTypeList_(tr("Transfer types"), this),
      propertyEditor_(this)
{
    // Initialize the editors.
    dataTypeList_.initialize(comDef_->getTransferTypes());
    propertyEditor_.setProperties(comDef_->getProperties());

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
    setEnabled(!locked);
}

//-----------------------------------------------------------------------------
// Function: ComDefinitionEditor::refresh()
//-----------------------------------------------------------------------------
void ComDefinitionEditor::refresh()
{
    QSharedPointer<LibraryComponent> libComp = libHandler_->getModel(*comDef_->getVlnv());
    comDef_ = libComp.staticCast<ComDefinition>();

    // Initialize the editors.
    dataTypeList_.initialize(comDef_->getTransferTypes());
    propertyEditor_.setProperties(comDef_->getProperties());

    setModified(false);
    TabDocument::refresh();
}

//-----------------------------------------------------------------------------
// Function: getDocumentVLNV()
//-----------------------------------------------------------------------------
VLNV ComDefinitionEditor::getDocumentVLNV() const
{
    return *comDef_->getVlnv();
}

//-----------------------------------------------------------------------------
// Function: ComDefinitionEditor::validate()
//-----------------------------------------------------------------------------
bool ComDefinitionEditor::validate(QStringList& errorList)
{
    applyChanges();
    return comDef_->isValid(errorList);
}

//-----------------------------------------------------------------------------
// Function: save()
//-----------------------------------------------------------------------------
bool ComDefinitionEditor::save()
{
    applyChanges();
    libHandler_->writeModelToFile(comDef_);

    return TabDocument::save();
}

//-----------------------------------------------------------------------------
// Function: saveAs()
//-----------------------------------------------------------------------------
bool ComDefinitionEditor::saveAs()
{
    // Ask the user for a new VLNV and directory.
    VLNV vlnv;
    QString directory;

    if (!NewObjectDialog::saveAsDialog(parentWidget(), libHandler_, *comDef_->getVlnv(), vlnv, directory))
    {
        return false;
    }

    // Create a copy of the object and update its VLNV.
    comDef_ = QSharedPointer<ComDefinition>(new ComDefinition(*comDef_));

    vlnv.setType(VLNV::COMDEFINITION);
    comDef_->setVlnv(vlnv);

    // Apply changes to the copy.
    applyChanges();

    if (libHandler_->writeModelToFile(directory, comDef_))
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
// Function: ComDefinitionEditor::applyChanges()
//-----------------------------------------------------------------------------
void ComDefinitionEditor::applyChanges()
{
    QList< QSharedPointer<ComProperty> > properties;

    foreach (QSharedPointer<ComProperty> prop, propertyEditor_.getProperties())
    {
        properties.append(QSharedPointer<ComProperty>(new ComProperty(*prop.data())));
    }

    comDef_->setProperties(properties);
    comDef_->setTransferTypes(dataTypeList_.items());
}

//-----------------------------------------------------------------------------
// Function: ComDefinitionEditor::showEvent()
//-----------------------------------------------------------------------------
void ComDefinitionEditor::showEvent(QShowEvent* event)
{
    TabDocument::showEvent(event);
    emit helpUrlRequested("definitions/comdefinition.html");
}

VLNV ComDefinitionEditor::getIdentifyingVLNV() const {
	return getDocumentVLNV();
}

