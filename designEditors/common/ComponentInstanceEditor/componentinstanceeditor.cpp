/* 
 *  	Created on: 11.8.2011
 *      Author: Antti Kamppi
 * 		filename: componentinstanceeditor.cpp
 *		Project: Kactus 2
 */

#include "componentinstanceeditor.h"

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/MultipleParameterFinder.h>
#include <designEditors/common/TopComponentParameterFinder.h>

#include <designEditors/HWDesign/HWComponentItem.h>
#include <designEditors/HWDesign/HWDesignWidget.h>
#include <designEditors/HWDesign/HWChangeCommands.h>
#include <designEditors/SystemDesign/SystemChangeCommands.h>
#include <designEditors/SystemDesign/SystemComponentItem.h>
#include <designEditors/SystemDesign/SWComponentItem.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/designconfiguration.h>

#include <IPXACTmodels/vlnv.h>

#include <QVBoxLayout>
#include <QDockWidget>

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::ComponentInstanceEditor()
//-----------------------------------------------------------------------------
ComponentInstanceEditor::ComponentInstanceEditor(QWidget *parent)
    : QWidget(parent),
      component_(0),
      vlnvDisplayer_(new VLNVDisplayer(this)),
      nameGroup_(new NameGroupBox(this, tr("Instance name"))),
      configurableElements_(0),
      swGroup_(new QGroupBox(tr("SW"), this)),
      fileSetRefCombo_(new QComboBox(this)),
      propertyValueEditor_(new PropertyValueEditor(this)),
      editProvider_(0),
      instanceFinder_(new ComponentParameterFinder(QSharedPointer<Component>(0))),
      topFinder_(new TopComponentParameterFinder(QSharedPointer<Component>(0)))
{
    QSharedPointer<MultipleParameterFinder> multiFinder(new MultipleParameterFinder());
    multiFinder->addFinder(instanceFinder_);
    multiFinder->addFinder(topFinder_);

    configurableElements_ = new ConfigurableElementEditor(multiFinder,
        QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(multiFinder)), this);

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	vlnvDisplayer_->hide();
	nameGroup_->hide();
    swGroup_->hide();
	configurableElements_->hide();
    propertyValueEditor_->hide();

	vlnvDisplayer_->setTitle(tr("Instance model VLNV"));
	vlnvDisplayer_->setFlat(false);

    QVBoxLayout* swGroupLayout = new QVBoxLayout(swGroup_);
    swGroupLayout->addWidget(new QLabel(tr("File set reference:"), this));
    swGroupLayout->addWidget(fileSetRefCombo_);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(vlnvDisplayer_);
	layout->addWidget(nameGroup_);
	layout->addWidget(configurableElements_);
    layout->addWidget(swGroup_);
    layout->addWidget(propertyValueEditor_);
	layout->addStretch();

	connect(nameGroup_, SIGNAL(nameChanged(QString const&)),
		    this, SLOT(onNameChanged(QString const&)), Qt::UniqueConnection);
	connect(nameGroup_, SIGNAL(displayNameChanged(const QString)),
		    this, SLOT(onDisplayNameChanged(const QString)), Qt::UniqueConnection);
	connect(nameGroup_, SIGNAL(descriptionChanged(const QString)),
		    this, SLOT(onDescriptionChanged(QString const&)), Qt::UniqueConnection);
    connect(propertyValueEditor_, SIGNAL(contentChanged()),
            this, SLOT(onPropertyValuesChanged()), Qt::UniqueConnection);            

    clear();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::~ComponentInstanceEditor()
//-----------------------------------------------------------------------------
ComponentInstanceEditor::~ComponentInstanceEditor()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::setComponentInstance()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::setComponentInstance(ComponentItem* component)
{
	Q_ASSERT(component);

	parentWidget()->raise();

	// if previous component has been specified, then disconnect signals to this editor.
	if (component_)
    {
		component_->disconnect(this);
        component_->disconnect(propertyValueEditor_);
		component_->disconnect(nameGroup_);
        component_->disconnect(fileSetRefCombo_);

        disconnect(fileSetRefCombo_, SIGNAL(currentIndexChanged(QString const&)),
                   this, SLOT(onFileSetRefChanged(QString const&)));
	}

	component_ = component;
    instanceFinder_->setComponent(component->componentModel());

	// set the vlnv of the component to be displayed
	vlnvDisplayer_->setVLNV(*component->componentModel()->getVlnv(), true);
	vlnvDisplayer_->show();

	nameGroup_->setName(component->name());
	nameGroup_->setDisplayName(component->displayName());
	nameGroup_->setDescription(component->description());

	nameGroup_->show();

    // Show the file set reference if the component is software.
    if (dynamic_cast<SWComponentItem*>(component) != 0)
    {
        SWComponentItem* swComponent = static_cast<SWComponentItem*>(component);

        fileSetRefCombo_->clear();
        fileSetRefCombo_->addItem("");
        fileSetRefCombo_->addItems(topComponent_->getFileSetNames());

        int index = fileSetRefCombo_->findText(swComponent->getFileSetRef());

        if (index != -1)
        {
            fileSetRefCombo_->setCurrentIndex(index);
        }

        swGroup_->show();

        connect(swComponent, SIGNAL(fileSetRefChanged(QString const&)),
                this, SLOT(updateFileSetRef(QString const&)), Qt::UniqueConnection);
    }
    else
    {
        swGroup_->hide();
    }

    // Show the component's property values in case of SW/HW mapping.
	if (dynamic_cast<SystemComponentItem*>(component) != 0)
    {
        SystemComponentItem* swComponent = static_cast<SystemComponentItem*>(component);

        propertyValueEditor_->setData(swComponent->getPropertyValues());
        propertyValueEditor_->setAllowedProperties(&swComponent->componentModel()->getSWProperties());

        propertyValueEditor_->show();
        configurableElements_->hide();

        connect(swComponent, SIGNAL(propertyValuesChanged(QMap<QString, QString> const&)),
                propertyValueEditor_, SLOT(setData(QMap<QString, QString> const&)), Qt::UniqueConnection);
    }
    else
    {
        propertyValueEditor_->hide();

        // Show the component's configurable elements in case of HW.
	    configurableElements_->setComponent(component);
	    configurableElements_->show();
    }

	connect(component_, SIGNAL(nameChanged(QString const&, QString const&)),
		    nameGroup_, SLOT(setName(QString const&)), Qt::UniqueConnection);
	connect(component_, SIGNAL(displayNameChanged(QString const&)),
		    nameGroup_, SLOT(setDisplayName(QString const&)), Qt::UniqueConnection);
	connect(component_, SIGNAL(descriptionChanged(QString const&)),
		    nameGroup_, SLOT(setDescription(QString const&)), Qt::UniqueConnection);

    connect(fileSetRefCombo_, SIGNAL(currentIndexChanged(QString const&)),
        this, SLOT(onFileSetRefChanged(QString const&)), Qt::UniqueConnection);

	// if the connected component is destroyed then clear this editor
	connect(component_, SIGNAL(destroyed(ComponentItem*)), this, SLOT(clear()), Qt::UniqueConnection);

	parentWidget()->setMaximumHeight(QWIDGETSIZE_MAX);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::setContext()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::setContext(QSharedPointer<Component> topComponent, GenericEditProvider* editProvider)
{
    topFinder_->setComponent(topComponent);
    editProvider_ = editProvider;
    topComponent_ = topComponent;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::setProtection()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::setProtection(bool locked)
{
    nameGroup_->setEnabled(!locked);
    fileSetRefCombo_->setEnabled(!locked);
    propertyValueEditor_->setEnabled(!locked);
    configurableElements_->setEnabled(!locked);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::setTopComponentActiveView()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::setTopComponentActiveView(QString const& activeView)
{
    topFinder_->setActiveView(activeView);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::clear()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::clear()
{
	// if previous component has been specified, then disconnect signals to this editor.
	if (component_)
    {
        component_->disconnect(this);
        component_->disconnect(propertyValueEditor_);
        component_->disconnect(nameGroup_);
        component_->disconnect(fileSetRefCombo_);

        disconnect(fileSetRefCombo_, SIGNAL(currentIndexChanged(QString const&)),
                   this, SLOT(onFileSetRefChanged(QString const&)));
	}

	component_ = 0;
	vlnvDisplayer_->hide();
	nameGroup_->hide();
    swGroup_->hide();
    fileSetRefCombo_->clear();
    propertyValueEditor_->hide();
	configurableElements_->hide();
	configurableElements_->clear();

	parentWidget()->setMaximumHeight(20);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::onNameChanged()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::onNameChanged( QString const& newName )
{
	// create command to the undo/redo stack
	QSharedPointer<ComponentChangeNameCommand> cmd(new ComponentChangeNameCommand(component_, newName));

	disconnect(component_, SIGNAL(nameChanged(QString const&, QString const&)),
		       nameGroup_, SLOT(setName(QString const&)));

	editProvider_->addCommand(cmd);
    cmd->redo();

	connect(component_, SIGNAL(nameChanged(QString const&, QString const&)),
		    nameGroup_, SLOT(setName(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::onDisplayNameChanged()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::onDisplayNameChanged( QString const& newDisplayName )
{
	// create command to the undo/redo stack
	QSharedPointer<ComponentChangeDisplayNameCommand> cmd(new ComponentChangeDisplayNameCommand(component_, 
        newDisplayName));

	disconnect(component_, SIGNAL(displayNameChanged(QString const&)),
        nameGroup_, SLOT(setDisplayName(QString const&)));

	editProvider_->addCommand(cmd);
    cmd->redo();

	connect(component_, SIGNAL(displayNameChanged(QString const&)),
		    nameGroup_, SLOT(setDisplayName(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::onDescriptionChanged()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::onDescriptionChanged( QString const& newDescription ) {
	// create command to the undo/redo stack
	QSharedPointer<ComponentChangeDescriptionNameCommand> cmd(new ComponentChangeDescriptionNameCommand(component_, newDescription));

	disconnect(component_, SIGNAL(descriptionChanged(QString const&)),
		nameGroup_, SLOT(setDescription(QString const&)));
	editProvider_->addCommand(cmd);
    cmd->redo();
	connect(component_, SIGNAL(descriptionChanged(QString const&)),
		nameGroup_, SLOT(setDescription(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::onPropertyValuesChanged()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::onPropertyValuesChanged()
{
    disconnect(component_, SIGNAL(propertyValuesChanged(QMap<QString, QString> const&)),
               propertyValueEditor_, SLOT(setData(QMap<QString, QString> const&)));

    SystemComponentItem* swComp = static_cast<SystemComponentItem*>(component_);
    QSharedPointer<PropertyValuesChangeCommand> cmd(new PropertyValuesChangeCommand(swComp,
        propertyValueEditor_->getData()));
    editProvider_->addCommand(cmd);
    cmd->redo();

    connect(component_, SIGNAL(propertyValuesChanged(QMap<QString, QString> const&)),
            propertyValueEditor_, SLOT(setData(QMap<QString, QString> const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::onFileSetRefChanged()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::onFileSetRefChanged(QString const& fileSetRef)
{
    disconnect(component_, SIGNAL(fileSetRefChanged(QString const&)),
               this, SLOT(updateFileSetRef(QString const&)));

    SWComponentItem* swComp = static_cast<SWComponentItem*>(component_);
    QSharedPointer<FileSetRefChangeCommand> cmd(new FileSetRefChangeCommand(swComp, fileSetRef));
    editProvider_->addCommand(cmd);
    cmd->redo();

    connect(component_, SIGNAL(fileSetRefChanged(QString const&)),
            this, SLOT(updateFileSetRef(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::updateFileSetRef()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::updateFileSetRef(QString const& fileSetRef)
{
    int index = fileSetRefCombo_->findText(fileSetRef);

    if (index != -1)
    {
        fileSetRefCombo_->setCurrentIndex(index);
    }
    else
    {
        fileSetRefCombo_->setCurrentIndex(0);
    }
}
