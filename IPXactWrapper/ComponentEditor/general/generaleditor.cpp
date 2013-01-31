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
#include <models/librarycomponent.h>

#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QStringList>

#include <QDebug>

//-----------------------------------------------------------------------------
// Function: GeneralEditor()
//-----------------------------------------------------------------------------
GeneralEditor::GeneralEditor(LibraryInterface* libHandler,
							 QSharedPointer<Component> component,
							 QWidget *parent):
ItemEditor(component, libHandler, parent),
vlnvDisplayer_(0),
attributeEditor_(0),
descEditor_(NULL),
headerEditor_(NULL),
previewBox_(0) {
    Q_ASSERT(libHandler != 0);
    Q_ASSERT(component != 0);

    // Create the VLNV displayer and attribute & description editors.
    QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	const QString xmlPath = libHandler->getPath(*component->getVlnv());

    vlnvDisplayer_ = new VLNVDisplayer(this, *component->getVlnv());
	vlnvDisplayer_->setPath(xmlPath);

    attributeEditor_ = new KactusAttributeEditor(this);

    descEditor_ = new DescEditor();

	headerEditor_ = new DescEditor();
	headerEditor_->setTitle(tr("XML header"));

    previewBox_ = new ComponentPreviewBox(libHandler);
    previewBox_->setComponent(component);

    layout->addWidget(vlnvDisplayer_);
    layout->addWidget(attributeEditor_);
    layout->addWidget(descEditor_);
	layout->addWidget(headerEditor_);
    layout->addWidget(previewBox_);

    // Connect the contentChanged() signals.
    connect(attributeEditor_, SIGNAL(contentChanged()),
            this, SLOT(onAttributesChange()), Qt::UniqueConnection);
    connect(descEditor_, SIGNAL(contentChanged()),
            this, SLOT(onDescriptionChange()), Qt::UniqueConnection);
	connect(headerEditor_, SIGNAL(contentChanged()),
		this, SLOT(onHeaderChange()), Qt::UniqueConnection);

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

void GeneralEditor::refresh() {
	disconnect(attributeEditor_, SIGNAL(contentChanged()),
		this, SLOT(onAttributesChange()));
	if (component()->getComponentImplementation() == KactusAttribute::KTS_HW)
	{
		KactusAttribute::ProductHierarchy prodHier = component()->getComponentHierarchy();
		KactusAttribute::Firmness firmness = component()->getComponentFirmness();
		attributeEditor_->setAttributes(prodHier, firmness);
	}
	else
	{
		attributeEditor_->hideAttributes();
	}
	connect(attributeEditor_, SIGNAL(contentChanged()),
		this, SLOT(onAttributesChange()), Qt::UniqueConnection);

	attributeEditor_->setImplementation(component()->getComponentImplementation());

	disconnect(descEditor_, SIGNAL(contentChanged()),
		this, SLOT(onDescriptionChange()));
	descEditor_->setDescription(component()->getDescription());
	connect(descEditor_, SIGNAL(contentChanged()),
		this, SLOT(onDescriptionChange()), Qt::UniqueConnection);

	disconnect(headerEditor_, SIGNAL(contentChanged()),
		this, SLOT(onDescriptionChange()));
	QStringList comments = component()->getTopComments();
	QString comment = comments.join("\n");
	headerEditor_->setDescription(comment);
	connect(headerEditor_, SIGNAL(contentChanged()),
		this, SLOT(onDescriptionChange()), Qt::UniqueConnection);
}

void GeneralEditor::onAttributesChange() {
	if (component()->getComponentImplementation() == KactusAttribute::KTS_HW)
	{
		component()->setComponentHierarchy(attributeEditor_->getProductHierarchy());
		component()->setComponentFirmness(attributeEditor_->getFirmness());
	}
	emit contentChanged();
}

void GeneralEditor::onDescriptionChange() {
	component()->setDescription(descEditor_->getDescription());
	emit contentChanged();
}

void GeneralEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/general.html");
}

void GeneralEditor::onHeaderChange() {
	component()->setTopComments(headerEditor_->getDescription());
	emit contentChanged();
}
