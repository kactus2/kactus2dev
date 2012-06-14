//-----------------------------------------------------------------------------
// File: generaleditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 7.2.2011
//
// Description:
// General editor for editing the component's general data.
//-----------------------------------------------------------------------------

#include "generaleditor.h"

#include <common/widgets/kactusAttributeEditor/KactusAttributeEditor.h>
#include <common/widgets/componentPreviewBox/ComponentPreviewBox.h>

#include <models/component.h>
#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDebug>

//-----------------------------------------------------------------------------
// Function: GeneralEditor()
//-----------------------------------------------------------------------------
GeneralEditor::GeneralEditor(LibraryInterface* libHandler,
							 QSharedPointer<Component> component,
							 QWidget *parent):
ItemEditor(component, parent),
vlnvDisplayer_(0),
attributeEditor_(0),
previewBox_(0),
component_()
{
    Q_ASSERT(libHandler != 0);
    Q_ASSERT(component != 0);

    component_ = component;

    // Create the VLNV displayer and attribute & description editors.
    QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	const QString xmlPath = libHandler->getPath(*component->getVlnv());

    vlnvDisplayer_ = new VLNVDisplayer(this, *component->getVlnv());
	vlnvDisplayer_->setPath(xmlPath);

    attributeEditor_ = new KactusAttributeEditor(this);

    descEditor_ = new DescEditor();

    previewBox_ = new ComponentPreviewBox(libHandler);
    previewBox_->setComponent(component_);

    layout->addWidget(vlnvDisplayer_);
    layout->addWidget(attributeEditor_);
    layout->addWidget(descEditor_);
    layout->addWidget(previewBox_);

    // Connect the contentChanged() signals.
    connect(attributeEditor_, SIGNAL(contentChanged()),
            this, SLOT(onAttributesChange()), Qt::UniqueConnection);
    connect(descEditor_, SIGNAL(contentChanged()),
            this, SLOT(onDescriptionChange()), Qt::UniqueConnection);

    refresh();
}

//-----------------------------------------------------------------------------
// Function: ~GeneralEditor()
//-----------------------------------------------------------------------------
GeneralEditor::~GeneralEditor()
{
}

//-----------------------------------------------------------------------------
// Function: isValid()
//-----------------------------------------------------------------------------
bool GeneralEditor::isValid() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: makeChanges()
//-----------------------------------------------------------------------------
void GeneralEditor::makeChanges()
{
	// TODO remove this in final    
}

void GeneralEditor::refresh() {
	if (component_->getComponentImplementation() != KactusAttribute::KTS_SW)
	{
		KactusAttribute::ProductHierarchy prodHier = component_->getComponentHierarchy();
		KactusAttribute::Firmness firmness = component_->getComponentFirmness();
		attributeEditor_->setAttributes(prodHier, firmness);
	}
	else
	{
		attributeEditor_->hideAttributes();
	}

	attributeEditor_->setImplementation(component_->getComponentImplementation());


	descEditor_->setDescription(component_->getDescription());
}

void GeneralEditor::onAttributesChange() {
	if (component_->getComponentImplementation() != KactusAttribute::KTS_SW)
	{
		component_->setComponentHierarchy(attributeEditor_->getProductHierarchy());
		component_->setComponentFirmness(attributeEditor_->getFirmness());
	}
	emit contentChanged();
}

void GeneralEditor::onDescriptionChange() {
	component_->setDescription(descEditor_->getDescription());
	emit contentChanged();
}
