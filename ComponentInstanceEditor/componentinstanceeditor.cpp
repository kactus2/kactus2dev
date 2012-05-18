/* 
 *  	Created on: 11.8.2011
 *      Author: Antti Kamppi
 * 		filename: componentinstanceeditor.cpp
 *		Project: Kactus 2
 */

#include "componentinstanceeditor.h"

#include <designwidget/diagramcomponent.h>
#include <models/component.h>
#include <LibraryManager/vlnv.h>
#include <designwidget/blockdiagram.h>
#include <designwidget/designwidget.h>
#include <EndpointDesign/EndpointDesignDiagram.h>
#include <EndpointDesign/EndpointDesignWidget.h>
#include <designwidget/DiagramChangeCommands.h>

#include <common/validators/vhdlNameValidator/vhdlnamevalidator.h>

#include <QVBoxLayout>
#include <QDockWidget>

ComponentInstanceEditor::ComponentInstanceEditor(QWidget *parent):
QWidget(parent),
component_(0),
vlnvDisplayer_(this),
nameGroup_(this, tr("Instance name")),
configurableElements_(this),
editProvider_() {

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	vlnvDisplayer_.hide();
	nameGroup_.hide();
	configurableElements_.hide();

	vlnvDisplayer_.setTitle(tr("Instance model VLNV"));
	vlnvDisplayer_.setFlat(false);

	// set validator for the instance name
	VhdlNameValidator* vhdlNameValidator = new VhdlNameValidator(NULL);
	nameGroup_.setNameValidator(vhdlNameValidator);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&vlnvDisplayer_);
	layout->addWidget(&nameGroup_);
	layout->addWidget(&configurableElements_);
	layout->addStretch();

	connect(&nameGroup_, SIGNAL(nameChanged(const QString&)),
		this, SLOT(onNameChanged(const QString&)), Qt::UniqueConnection);
	connect(&nameGroup_, SIGNAL(displayNameChanged(const QString)),
		this, SLOT(onDisplayNameChanged(const QString)), Qt::UniqueConnection);
	connect(&nameGroup_, SIGNAL(descriptionChanged(const QString)),
		this, SLOT(onDescriptionChanged(const QString&)), Qt::UniqueConnection);
}

ComponentInstanceEditor::~ComponentInstanceEditor() {
}

void ComponentInstanceEditor::setComponent( ComponentItem* component ) {
	Q_ASSERT(component);

	parentWidget()->raise();

	// if previous component has been specified, then disconnect signals to this editor.
	if (component_) {
		component_->disconnect(this);
		component_->disconnect(&nameGroup_);
	}

	component_ = component;

    bool locked = false;

	// Get the edit provider that manages the undo/redo stack.
    DesignDiagram* diagram = static_cast<DesignDiagram*>(component->scene());
    editProvider_ = &diagram->getEditProvider();
    locked = diagram->isProtected();
    
	// set the vlnv of the component to be displayed
	vlnvDisplayer_.setVLNV(*component->componentModel()->getVlnv(), true);
	vlnvDisplayer_.show();

	nameGroup_.setName(component->name());
	nameGroup_.setDisplayName(component->displayName());
	nameGroup_.setDescription(component->description());
    nameGroup_.setEnabled(!locked);
	nameGroup_.show();

	// set the component's configurable elements
	configurableElements_.setComponent(component);
    configurableElements_.setEnabled(!locked);
	configurableElements_.show();

	connect(component_, SIGNAL(nameChanged(const QString&, const QString&)),
		&nameGroup_, SLOT(setName(const QString&)), Qt::UniqueConnection);
	connect(component_, SIGNAL(displayNameChanged(const QString&)),
		&nameGroup_, SLOT(setDisplayName(const QString&)), Qt::UniqueConnection);
	connect(component_, SIGNAL(descriptionChanged(const QString&)),
		&nameGroup_, SLOT(setDescription(const QString&)), Qt::UniqueConnection);

	// if the connected component is destroyed then clear this editor
	connect(component_, SIGNAL(destroyed(ComponentItem*)),
		    this, SLOT(clear()), Qt::UniqueConnection);

	parentWidget()->setMaximumHeight(QWIDGETSIZE_MAX);
}

void ComponentInstanceEditor::clear() {

	// if previous component has been specified, then disconnect signals to this editor.
	if (component_) {
		component_->disconnect(this);
	}

	component_ = 0;
	vlnvDisplayer_.hide();
	nameGroup_.hide();
	configurableElements_.hide();
	configurableElements_.clear();
	editProvider_ = 0;

	parentWidget()->setMaximumHeight(20);
}

void ComponentInstanceEditor::onNameChanged( const QString& newName ) {
	// create command to the undo/redo stack
	QSharedPointer<ComponentChangeNameCommand> cmd(new ComponentChangeNameCommand(component_, newName));

	disconnect(component_, SIGNAL(nameChanged(const QString&, const QString&)),
		&nameGroup_, SLOT(setName(const QString&)));
	editProvider_->addCommand(cmd);
	connect(component_, SIGNAL(nameChanged(const QString&, const QString&)),
		&nameGroup_, SLOT(setName(const QString&)), Qt::UniqueConnection);
}

void ComponentInstanceEditor::onDisplayNameChanged( const QString& newDisplayName ) {
	// create command to the undo/redo stack
	QSharedPointer<ComponentChangeDisplayNameCommand> cmd(new ComponentChangeDisplayNameCommand(component_, newDisplayName));

	disconnect(component_, SIGNAL(displayNameChanged(const QString&)),
		&nameGroup_, SLOT(setDisplayName(const QString&)));
	editProvider_->addCommand(cmd);
	connect(component_, SIGNAL(displayNameChanged(const QString&)),
		&nameGroup_, SLOT(setDisplayName(const QString&)), Qt::UniqueConnection);
}

void ComponentInstanceEditor::onDescriptionChanged( const QString& newDescription ) {
	// create command to the undo/redo stack
	QSharedPointer<ComponentChangeDescriptionNameCommand> cmd(new ComponentChangeDescriptionNameCommand(component_, newDescription));

	disconnect(component_, SIGNAL(descriptionChanged(const QString&)),
		&nameGroup_, SLOT(setDescription(const QString&)));
	editProvider_->addCommand(cmd);
	connect(component_, SIGNAL(descriptionChanged(const QString&)),
		&nameGroup_, SLOT(setDescription(const QString&)), Qt::UniqueConnection);
}
