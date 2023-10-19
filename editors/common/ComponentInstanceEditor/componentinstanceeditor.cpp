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

#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/MultipleParameterFinder.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>

#include <editors/common/DesignCompletionModel.h>
#include <editors/common/ExpressionSet.h>

#include <editors/HWDesign/HWChangeCommands.h>
#include <editors/HWDesign/HWComponentItem.h>

#include <editors/SystemDesign/SystemComponentItem.h>
#include <editors/SystemDesign/SWComponentItem.h>
#include <editors/SystemDesign/UndoCommands/SystemChangeCommands.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/ComponentInstance.h>
#include <IPXACTmodels/DesignConfiguration/ViewConfiguration.h>

#include <QVBoxLayout>
#include <QDockWidget>

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::ComponentInstanceEditor()
//-----------------------------------------------------------------------------
ComponentInstanceEditor::ComponentInstanceEditor(QWidget *parent):
QWidget(parent)
{
    QSharedPointer<MultipleParameterFinder> parameterFinder(new MultipleParameterFinder());
    parameterFinder->addFinder(instanceFinder_);
    parameterFinder->addFinder(designParameterFinder_);

    QSharedPointer<IPXactSystemVerilogParser> parameterParser(new IPXactSystemVerilogParser(parameterFinder));
    ExpressionSet parameterExpressions({ parameterFinder , parameterParser,
        QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(parameterFinder)) });

    QSharedPointer<IPXactSystemVerilogParser> defaultParser (new IPXactSystemVerilogParser(instanceFinder_));
    ExpressionSet defaultExpressions({ instanceFinder_ , defaultParser,
        QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(instanceFinder_)) });

    QSharedPointer<IPXactSystemVerilogParser> designParameterParser(
        new IPXactSystemVerilogParser(parameterFinder));

    auto completionModel = new ComponentParameterModel(designParameterFinder_, this);
    completionModel->setExpressionParser(designParameterParser);

    configurableElements_ = new ComponentInstanceConfigurableElementsEditor(parameterExpressions, 
        defaultExpressions, defaultExpressions, completionModel, this);

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    vlnvDisplayer_->setFlat(true);
    nameGroup_->setFlat(true);
    swGroup_->setFlat(true);
    propertyValueEditor_->setFlat(true);

	vlnvDisplayer_->hide();
	nameGroup_->hide();
    swGroup_->hide();
	configurableElements_->hide();
    propertyValueEditor_->hide();

	vlnvDisplayer_->setTitle(tr("Component VLNV"));

    setupLayout();

    connect(configurableElements_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(nameGroup_, SIGNAL(nameChanged()), this, SLOT(onNameChanged()), Qt::UniqueConnection);
    connect(nameGroup_, SIGNAL(displayNameChanged()), this, SLOT(onDisplayNameChanged()), Qt::UniqueConnection);
    connect(nameGroup_, SIGNAL(descriptionChanged()), this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);
    connect(propertyValueEditor_, SIGNAL(contentChanged()),
            this, SLOT(onPropertyValuesChanged()), Qt::UniqueConnection);            

    connect(configurableElements_, SIGNAL(increaseReferences(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(configurableElements_, SIGNAL(decreaseReferences(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);

    clear();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::setComponentInstance()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::setComponentInstance(ComponentItem* componentItem,
    QSharedPointer<IEditProvider> editProvider, QSharedPointer<Design> design,
    QSharedPointer<DesignConfiguration> designConfiguration, QString const& activeViewName)
{
	Q_ASSERT(componentItem);

	// if previous component has been specified, then disconnect signals to this editor.
	if (component_)
    {
		component_->disconnect(this);
        component_->disconnect(propertyValueEditor_);
		component_->disconnect(nameGroup_);
        component_->disconnect(fileSetRefCombo_);

        disconnect(fileSetRefCombo_, SIGNAL(currentTextChanged(QString const&)),
                   this, SLOT(onFileSetRefChanged(QString const&)));
	}

    designParameterFinder_->setParameterList(design->getParameters());
    containingDesign_ = design;
	component_ = componentItem;
	instanceFinder_->setComponent(componentItem->componentModel());

    auto instanceViewName = QString();

    if (designConfiguration)
    {
        instanceViewName = designConfiguration->getActiveView(componentItem->name());
    }

    activeViewLabel_->parentWidget()->show();
    activeViewLabel_->setText(instanceViewName);
    
    topFinder_->setActiveView(topComponent_->getModel()->findView(activeViewName));    

	// set the vlnv of the component to be displayed
    vlnvDisplayer_->setVLNV(componentItem->componentModel()->getVlnv());
	vlnvDisplayer_->show();

	nameGroup_->setName(componentItem->name());
	nameGroup_->setDisplayName(componentItem->displayName());

    disconnect(nameGroup_, SIGNAL(descriptionChanged()),
        this, SLOT(onDescriptionChanged()));
    nameGroup_->setDescription(componentItem->description());
    connect(nameGroup_, SIGNAL(descriptionChanged()), this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);

	nameGroup_->show();

    // Show the file set reference if the component is software.
    if (auto swComponent = dynamic_cast<SWComponentItem*>(componentItem);
        swComponent != nullptr)
    {
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
	if (auto swComponent = dynamic_cast<SystemComponentItem*>(componentItem);
        swComponent != nullptr)
    {
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

    connect(fileSetRefCombo_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(onFileSetRefChanged(QString const&)), Qt::UniqueConnection);

	// if the connected component is destroyed then clear this editor
	connect(component_, SIGNAL(destroyed(ComponentItem*)), this, SLOT(clear()), Qt::UniqueConnection);

	parentWidget()->setMaximumHeight(QWIDGETSIZE_MAX);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::setContext()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::setContext(QSharedPointer<Component> topComponent,
     QSharedPointer<IEditProvider> editProvider)
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

        disconnect(fileSetRefCombo_, SIGNAL(currentTextChanged(QString const&)),
                   this, SLOT(onFileSetRefChanged(QString const&)));
	}

	component_ = nullptr;
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
    if (component_)
    {
        auto newName = nameGroup_->name();

        QSharedPointer<ComponentChangeNameCommand> cmd(
            new ComponentChangeNameCommand(component_, newName, containingDesign_));

        disconnect(component_, SIGNAL(nameChanged(QString const&, QString const&)),
            nameGroup_, SLOT(setName(QString const&)));

        editProvider_->addCommand(cmd);
        cmd->redo();

        connect(component_, SIGNAL(nameChanged(QString const&, QString const&)),
            nameGroup_, SLOT(setName(QString const&)), Qt::UniqueConnection);
    }	
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
    if (component_)
    {
        QString newDescription = nameGroup_->description();

        QSharedPointer<ComponentChangeDescriptionNameCommand> cmd(new ComponentChangeDescriptionNameCommand(
            component_, newDescription));

        disconnect(component_, SIGNAL(descriptionChanged(QString const&)),
            nameGroup_, SLOT(setDescription(QString const&)));
        editProvider_->addCommand(cmd);
        cmd->redo();
        connect(component_, SIGNAL(descriptionChanged(QString const&)),
            nameGroup_, SLOT(setDescription(QString const&)), Qt::UniqueConnection);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceEditor::onPropertyValuesChanged()
//-----------------------------------------------------------------------------
void ComponentInstanceEditor::onPropertyValuesChanged()
{
    disconnect(component_, SIGNAL(propertyValuesChanged(QMap<QString, QString> const&)),
               propertyValueEditor_, SLOT(setData(QMap<QString, QString> const&)));

    auto swComp = static_cast<SystemComponentItem*>(component_);
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

    auto swComp = static_cast<SWComponentItem*>(component_);
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
    auto swGroupLayout = new QHBoxLayout(swGroup_);
    swGroupLayout->addWidget(new QLabel(tr("File set reference:"), this));
    swGroupLayout->addWidget(fileSetRefCombo_, 1);

    auto configurationBox = new QGroupBox(tr("Configuration"), this);
    configurationBox->setFlat(true);

    auto configurationLayout = new QFormLayout(configurationBox);
    configurationLayout->addRow(tr("Active view:"), activeViewLabel_);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(vlnvDisplayer_);
    layout->addWidget(nameGroup_);
    layout->addWidget(configurationBox);
    layout->addWidget(configurableElements_, 1);
    layout->addWidget(swGroup_);
    layout->addWidget(propertyValueEditor_);
    layout->addStretch();
}
