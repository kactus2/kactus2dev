//-----------------------------------------------------------------------------
// File: ComDefinitionEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 17.4.2012
//
// Description:
// Editor for editing COM definitions.
//-----------------------------------------------------------------------------

#include "ComDefinitionEditor.h"

#include <common/dialogs/newObjectDialog/newobjectdialog.h>

#include <IPXACTmodels/kactusExtensions/ComProperty.h>
#include <IPXACTmodels/kactusExtensions/validators/ComDefinitionValidator.h>

#include <QGridLayout>
#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: ComDefinitionEditor()
//-----------------------------------------------------------------------------
ComDefinitionEditor::ComDefinitionEditor(QWidget *parent, LibraryInterface* libHandler,
    QSharedPointer<ComDefinition> comDef):
TabDocument(parent, DOC_PROTECTION_SUPPORT),
    libHandler_(libHandler),
    comDef_(comDef),
    nameGroup_(this),
    dataTypeList_(tr("Transfer types"), this),
    propertyEditor_(this)
{
    // Initialize the editors.
    nameGroup_.setDocumentNameGroup(comDef_, libHandler_->getPath(comDef_->getVlnv()));
    nameGroup_.setTitle(tr("COM definition"));
    dataTypeList_.initialize(*comDef_->getTransferTypes());
    propertyEditor_.setProperties(comDef_->getProperties());

    nameGroup_.setFlat(true);
    propertyEditor_.setFlat(true);

    connect(&nameGroup_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&dataTypeList_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&propertyEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    
    // Setup the layout.
    auto layout = new QGridLayout(this);
    layout->addWidget(&nameGroup_, 0, 0, 1, 1);
    layout->addWidget(&dataTypeList_, 0, 1, 1, 1);
    layout->addWidget(&propertyEditor_, 1, 0, 1, 2);
    
    setModified(false);

    // Set the document name and type.
    VLNV const vlnv = comDef_->getVlnv();
    setDocumentName(vlnv.getName() + " (" + vlnv.getVersion() + ")");
    setDocumentType(DocumentType(DocumentTypes::COM_DEFINITION));

    // Open in unlocked mode by default only if the version is draft.
    setProtection(vlnv.getVersion() != "draft");
}

//-----------------------------------------------------------------------------
// Function: ComDefinitionEditor::getIdentifyingVLNV()
//-----------------------------------------------------------------------------
VLNV ComDefinitionEditor::getIdentifyingVLNV() const
{
    return getDocumentVLNV();
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
    QSharedPointer<Document> libComp = libHandler_->getModel(comDef_->getVlnv());
    comDef_ = libComp.staticCast<ComDefinition>();

    // Initialize the editors.
    nameGroup_.setDocumentNameGroup(comDef_, libHandler_->getPath(comDef_->getVlnv()));
    dataTypeList_.initialize(*comDef_->getTransferTypes());
    propertyEditor_.setProperties(comDef_->getProperties());

    setModified(false);
    TabDocument::refresh();
}

//-----------------------------------------------------------------------------
// Function: getDocumentVLNV()
//-----------------------------------------------------------------------------
VLNV ComDefinitionEditor::getDocumentVLNV() const
{
    return comDef_->getVlnv();
}

//-----------------------------------------------------------------------------
// Function: ComDefinitionEditor::validate()
//-----------------------------------------------------------------------------
bool ComDefinitionEditor::validate(QVector<QString>& errorList)
{
    applyChanges();

	ComDefinitionValidator validator;
	validator.findErrorsIn(errorList, comDef_);

    return errorList.isEmpty();
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

    if (!NewObjectDialog::saveAsDialog(parentWidget(), libHandler_, comDef_->getVlnv(), vlnv, directory))
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

    for (QSharedPointer<ComProperty> prop : propertyEditor_.getProperties())
    {
        properties.append(QSharedPointer<ComProperty>(new ComProperty(*prop)));
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

