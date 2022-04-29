//-----------------------------------------------------------------------------
// File: InstantiationsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 25.01.2016
//
// Description:
// The Instantiations-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "InstantiationsItem.h"

#include "ComponentInstantiationsItem.h"
#include "DesignInstantiationsItem.h"
#include "DesignConfigurationInstantiationsItem.h"

#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/MultipleParameterFinder.h>
#include <KactusAPI/include/ComponentInstantiationParameterFinder.h>
#include <KactusAPI/include/ParametersInterface.h>
#include <KactusAPI/include/FileBuilderInterface.h>
#include <KactusAPI/include/FileSetInterface.h>
#include <KactusAPI/include/FileInterface.h>
#include <editors/ComponentEditor/instantiations/InstantiationsEditor.h>
#include <KactusAPI/include/ComponentInstantiationInterface.h>
#include <KactusAPI/include/ModuleParameterInterface.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>
#include <IPXACTmodels/Component/validators/FileValidator.h>
#include <IPXACTmodels/Component/validators/FileSetValidator.h>

//-----------------------------------------------------------------------------
// Function: InstantiationsItem::InstantiationsItem()
//-----------------------------------------------------------------------------
InstantiationsItem::InstantiationsItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, ComponentEditorItem* parent) :
ComponentEditorItem(model, libHandler, component, parent),
expressionParser_(expressionParser),
validator_(new InstantiationsValidator(expressionParser, component->getFileSets(),
    QSharedPointer<ParameterValidator>(new ParameterValidator(expressionParser, component->getChoices())),
    libHandler)),
componentInstantiationsItem_(0),
designConfigurationInstantiationsItem_(
    new DesignConfigurationInstantiationsItem(model, libHandler, component, validator_, referenceCounter,
        parameterFinder, expressionFormatter, expressionParser, this)),
designInstantiationsItem_(new DesignInstantiationsItem(model, libHandler, component, validator_, parameterFinder,
    referenceCounter, this)),
componentInstantiationInterface_()
{
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

    setObjectName(tr("InstantiationsItem"));

    setReferenceCounter(referenceCounter);

    constructInterfaces();

    componentInstantiationsItem_ = QSharedPointer<ComponentInstantiationsItem>(new ComponentInstantiationsItem(
        model, libHandler, component, validator_, referenceCounter, parameterFinder, expressionFormatter,
        expressionParser, componentInstantiationInterface_, this));

    connect(componentInstantiationsItem_.data(), SIGNAL(openReferenceTree(QString const&, QString const&)),
        this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

    connect(designConfigurationInstantiationsItem_.data(), SIGNAL(openReferenceTree(QString const&, QString const&)),
        this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

    childItems_.append(componentInstantiationsItem_);
    childItems_.append(designConfigurationInstantiationsItem_);
    childItems_.append(designInstantiationsItem_);
}

//-----------------------------------------------------------------------------
// Function: InstantiationsItem::~InstantiationsItem()
//-----------------------------------------------------------------------------
InstantiationsItem::~InstantiationsItem()
{

}

//-----------------------------------------------------------------------------
// Function: InstantiationsItem::getTooltip()
//-----------------------------------------------------------------------------
QString InstantiationsItem::getTooltip() const
{
    return tr("Contains the instantiations of the component");
}

//-----------------------------------------------------------------------------
// Function: InstantiationsItem::getFont()
//-----------------------------------------------------------------------------
QFont InstantiationsItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!component_->getComponentInstantiations()->isEmpty() || 
        !component_->getDesignConfigurationInstantiations()->isEmpty() ||
        !component_->getDesignInstantiations()->isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsItem::text()
//-----------------------------------------------------------------------------
QString InstantiationsItem::text() const
{
	return tr("Instantiations");
}

//-----------------------------------------------------------------------------
// Function: InstantiationsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* InstantiationsItem::editor()
{
	if (!editor_)
    {
        editor_ = new InstantiationsEditor(
            component_, parameterFinder_, libHandler_, validator_, componentInstantiationInterface_);
        editor_->setProtection(locked_);
 
 		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
  		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connect(editor_, SIGNAL(componentInstanceAdded(int)), 
            componentInstantiationsItem_.data(), SLOT(onAddChild(int)));
        connect(editor_, SIGNAL(componentInstanceRemoved(int)),
            componentInstantiationsItem_.data(), SLOT(onRemoveChild(int)));

        connect(editor_, SIGNAL(designConfigurationInstanceAdded(int)), 
            designConfigurationInstantiationsItem_.data(), SLOT(onAddChild(int)));
        connect(editor_, SIGNAL(designConfigurationInstanceRemoved(int)),
            designConfigurationInstantiationsItem_.data(), SLOT(onRemoveChild(int)));

        connect(editor_, SIGNAL(designInstanceAdded(int)), designInstantiationsItem_.data(), 
            SLOT(onAddChild(int)));
        connect(editor_, SIGNAL(designInstanceRemoved(int)), designInstantiationsItem_.data(), 
            SLOT(onRemoveChild(int)));

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsItem::createChild()
//-----------------------------------------------------------------------------
void InstantiationsItem::createChild(int)
{
    // No new children allowed.
}

//-----------------------------------------------------------------------------
// Function: InstantiationsItem::constructInterfaces()
//-----------------------------------------------------------------------------
void InstantiationsItem::constructInterfaces()
{
    QSharedPointer<ComponentInstantiationParameterFinder> instantiationParameterFinder(
        new ComponentInstantiationParameterFinder(QSharedPointer<ComponentInstantiation>()));

    QSharedPointer<MultipleParameterFinder> cimpFinder =
        QSharedPointer<MultipleParameterFinder>(new MultipleParameterFinder);
    cimpFinder->addFinder(parameterFinder_);
    cimpFinder->addFinder(instantiationParameterFinder);

    QSharedPointer<IPXactSystemVerilogParser> cimpParser =
        QSharedPointer<IPXactSystemVerilogParser>(new IPXactSystemVerilogParser(cimpFinder));

    QSharedPointer<ParameterValidator> validator(
        new ParameterValidator(cimpParser, component_->getChoices()));

    QSharedPointer<ExpressionFormatter> moduleFormatter(new ExpressionFormatter(cimpFinder));

    ParametersInterface* parameterInterface =
        new ParametersInterface(validator, cimpParser, moduleFormatter);


    ModuleParameterInterface* moduleParameterInterface =
        new ModuleParameterInterface(validator, cimpParser, moduleFormatter, instantiationParameterFinder);

    FileBuilderInterface* fileBuilderInterface = new FileBuilderInterface(cimpParser, moduleFormatter);

    QSharedPointer<FileValidator> fileValidator(new FileValidator(expressionParser_));
    FileInterface* fileInterface = new FileInterface(fileValidator, expressionParser_, moduleFormatter);

    QSharedPointer<FileSetValidator> fileSetValidator(new FileSetValidator(fileValidator, expressionParser_));
    FileSetInterface* fileSetInterface = new FileSetInterface(
        fileSetValidator, expressionParser_, moduleFormatter, fileInterface, fileBuilderInterface);

    fileSetInterface->setFileSets(component_->getFileSets());

    componentInstantiationInterface_ = new ComponentInstantiationInterface(validator_, cimpParser,
        moduleFormatter, parameterInterface, moduleParameterInterface, fileBuilderInterface, fileSetInterface);
}
