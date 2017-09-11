//-----------------------------------------------------------------------------
// File: componentinstanceeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 11.08.2011
//
// Description:
// Editor to edit the details of a component instance within a design.
//-----------------------------------------------------------------------------

#include "componentinstanceeditor.h"

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/MultipleParameterFinder.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ListParameterFinder.h>

#include <designEditors/common/DesignCompletionModel.h>
#include <designEditors/common/TopComponentParameterFinder.h>

#include <designEditors/HWDesign/HWChangeCommands.h>
#include <designEditors/HWDesign/HWComponentItem.h>

#include <designEditors/SystemDesign/SystemComponentItem.h>
#include <designEditors/SystemDesign/SWComponentItem.h>
#include <designEditors/SystemDesign/UndoCommands/SystemChangeCommands.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/ComponentInstance.h>
#include <IPXACTmodels/designConfiguration/ViewConfiguration.h>

#include <QVBoxLayout>
#include <QDockWidget>

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::ComponentInstanceEditor()
//-----------------------------------------------------------------------------
ComponentInstanceEditor::ComponentInstanceEditor(QWidget *parent):
QWidget(parent),
    component_(0),
    vlnvDisplayer_(new VLNVDisplayer(this)),
    nameGroup_(new NameGroupBox(this, tr("Component instance name"))),
    activeViewLabel_(new QLabel(this)),
    configurableElements_(0),
    swGroup_(new QGroupBox(tr("Software"), this)),
    fileSetRefCombo_(new QComboBox(this)),
    propertyValueEditor_(new PropertyValueEditor(this)),
    editProvider_(0),
    instanceFinder_(new ComponentParameterFinder(QSharedPointer<Component>(0))),
    elementFinder_(new ConfigurableElementFinder()),
    topFinder_(new TopComponentParameterFinder(QSharedPointer<Component>(0))),
    designParameterFinder_(new ListParameterFinder()),
    topComponent_(),
    containingDesign_()
{
    QSharedPointer<MultipleParameterFinder> multiFinder(new MultipleParameterFinder());
    multiFinder->addFinder(elementFinder_);
    multiFinder->addFinder(designParameterFinder_);

    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(multiFinder));
    QSharedPointer<IPXactSystemVerilogParser> instanceParser (new IPXactSystemVerilogParser(instanceFinder_));

    QSharedPointer<IPXactSystemVerilogParser> designParameterParser(
        new IPXactSystemVerilogParser(designParameterFinder_));

    ComponentParameterModel* completionModel = new ComponentParameterModel(designParameterFinder_, this);
    completionModel->setExpressionParser(designParameterParser);

    configurableElements_ = new ComponentInstanceConfigurableElementsEditor(elementFinder_, multiFinder, 
        QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(multiFinder)),
        QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(instanceFinder_)),
        expressionParser, instanceParser, completionModel, this);

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	vlnvDisplayer_->hide();
	nameGroup_->hide();
    swGroup_->hide();
	configurableElements_->hide();
    propertyValueEditor_->hide();

	vlnvDisplayer_->setTitle(tr("Component VLNV"));
	vlnvDisplayer_->setFlat(false);

    setupLayout();

    connect(configurableElements_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(nameGroup_, SIGNAL(nameChanged()), this, SLOT(onNameChanged()), Qt::UniqueConnection);
    connect(nameGroup_, SIGNAL(displayNameChanged()), this, SLOT(onDisplayNameChanged()), Qt::UniqueConnection);
    connect(nameGroup_, SIGNAL(descriptionChanged()), this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);
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
void ComponentInstanceEditor::setComponentInstance(ComponentItem* componentItem,
    QSharedPointer<IEditProvider> editProvider, QSharedPointer<Design> design,
    QSharedPointer<DesignConfiguration> designConfiguration, QString const& activeViewName)
{
	Q_ASSERT(componentItem);

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

    designParameterFinder_->setParameterList(design->getParameters());
    containingDesign_ = design;
	component_ = componentItem;
	instanceFinder_->setComponent(componentItem->componentModel());

    QString instanceViewName = QString();

    activeViewLabel_->parentWidget()->show();

    activeViewLabel_->setText(instanceViewName);
    
    topFinder_->setActiveView(topComponent_->getModel()->findView(activeViewName));    

	// set the vlnv of the component to be displayed
	vlnvDisplayer_->setVLNV(componentItem->componentModel()->getVlnv(), true);
	vlnvDisplayer_->show();

	nameGroup_->setName(componentItem->name());
	nameGroup_->setDisplayName(componentItem->displayName());

    disconnect(nameGroup_, SIGNAL(descriptionChanged()),
        this, SLOT(onDescriptionChanged()));
    nameGroup_->setDescription(componentItem->description());
    connect(nameGroup_, SIGNAL(descriptionChanged()), this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);

	nameGroup_->show();

    // Show the file set reference if the component is software.
    if (dynamic_cast<SWComponentItem*>(componentItem) != 0)
    {
        SWComponentItem* swComponent = static_cast<SWComponentItem*>(componentItem);

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
	if (dynamic_cast<SystemComponentItem*>(componentItem) != 0)
    {
        SystemComponentItem* swComponent = static_cast<SystemComponentItem*>(componentItem);

        propertyValueEditor_->setData(swComponent->getPropertyValues());
        propertyValueEditor_->setAllowedProperties(*swComponent->componentModel()->getSWProperties());

        propertyValueEditor_->show();
        configurableElements_->hide();

        connect(swComponent, SIGNAL(propertyValuesChanged(QMap<QString, QString> const&)),
                propertyValueEditor_, SLOT(setData(QMap<QString, QString> const&)), Qt::UniqueConnection);
    }
    else
    {
        propertyValueEditor_->hide();

        if (designConfiguration)
        {
            QSharedPointer<ViewConfiguration> matchingViewConfiguration =
                designConfiguration->getViewConfiguration(componentItem->getComponentInstance()->getInstanceName());

            // Show the component's configurable elements in case of HW.
            configurableElements_->setComponent(componentItem->componentModel(), componentItem->getComponentInstance(),
                matchingViewConfiguration, editProvider);
        }

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
void ComponentInstanceEditor::setContext(QSharedPointer<Component> topComponent,
    QSharedPointer<DesignConfiguration> designConfiguration, QSharedPointer<IEditProvider> editProvider)
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
void ComponentInstanceEditor::setTopComponentActiveView(QSharedPointer<View> activeView)
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
    activeViewLabel_->parentWidget()->hide();

	parentWidget()->setMaximumHeight(20);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::onNameChanged()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::onNameChanged()
{
    QString newName = nameGroup_->name();

	// create command to the undo/redo stack
	QSharedPointer<ComponentChangeNameCommand> cmd(
        new ComponentChangeNameCommand(component_, newName, containingDesign_));

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
void ComponentInstanceEditor::onDisplayNameChanged()
{
    if (component_)
    {
        QString newDisplayName = nameGroup_->displayName();

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
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::onDescriptionChanged()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::onDescriptionChanged()
{
    QString newDescription = nameGroup_->description();
	// create command to the undo/redo stack
	QSharedPointer<ComponentChangeDescriptionNameCommand> cmd(new ComponentChangeDescriptionNameCommand(component_,
        newDescription));

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
    int index = qMax(fileSetRefCombo_->findText(fileSetRef), 0);
    fileSetRefCombo_->setCurrentIndex(index);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::setupLayout()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::setupLayout()
{
    QHBoxLayout* swGroupLayout = new QHBoxLayout(swGroup_);
    swGroupLayout->addWidget(new QLabel(tr("File set reference:"), this));
    swGroupLayout->addWidget(fileSetRefCombo_, 1);

    QGroupBox* configurationBox = new QGroupBox(tr("Configuration"), this);
    QFormLayout* configurationLayout = new QFormLayout(configurationBox);
    configurationLayout->addRow(tr("Active view:"), activeViewLabel_);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(vlnvDisplayer_);
    layout->addWidget(nameGroup_);
    layout->addWidget(configurationBox);
    layout->addWidget(configurableElements_, 1);
    layout->addWidget(swGroup_);
    layout->addWidget(propertyValueEditor_);
    layout->addStretch();
}
