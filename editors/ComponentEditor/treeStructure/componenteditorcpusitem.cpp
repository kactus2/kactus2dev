//-----------------------------------------------------------------------------
// File: componenteditorcpusitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author:Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Cpus-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorcpusitem.h"
#include "SingleCpuItem.h"

#include <editors/ComponentEditor/cpus/cpuseditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/validators/CPUValidator.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorCpusItem::ComponentEditorCpusItem()
//-----------------------------------------------------------------------------
ComponentEditorCpusItem::ComponentEditorCpusItem(ComponentEditorTreeModel* model, 
    LibraryInterface* libHandler,
    QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder, 
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser,
    ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
    cpus_(component->getCpus()),
    expressionParser_(expressionParser),
    validator_(new CPUValidator(
        QSharedPointer<ParameterValidator>(new ParameterValidator(expressionParser, component->getChoices())),
        expressionParser, component->getAddressSpaces()))
{
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
    setReferenceCounter(referenceCounter);

    const int CHILD_COUNT = cpus_->count();
    for (int i = 0; i < CHILD_COUNT; ++i)
    {
        createChild(i);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorCpusItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorCpusItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!cpus_->isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorCpusItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorCpusItem::text() const
{
	return tr("CPUs");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorCpusItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorCpusItem::editor()
{
	if (!editor_)
    {
		editor_ = new CpusEditor(component_, libHandler_, validator_, expressionParser_,
            parameterFinder_, expressionFormatter_);
        editor_->setProtection(locked_);
        connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
	}

	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorCpusItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorCpusItem::getTooltip() const
{
	return tr("Contains the programmable cores of the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorCpusItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorCpusItem::isValid() const
{
    QVector<QString> cpuNames;

    for (QSharedPointer<Cpu> cpu : *cpus_) 
    {
        if (cpuNames.contains(cpu->name()) || !validator_->validate(cpu))
        {
            return false;
        }

        cpuNames.append(cpu->name());
    }

	return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorCpusItem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorCpusItem::createChild(int index)
{
    QSharedPointer<SingleCpuItem> cpuItem(new SingleCpuItem(cpus_->at(index), model_, libHandler_,
        component_, referenceCounter_, parameterFinder_, expressionFormatter_, expressionParser_, 
        validator_, this));

    cpuItem->setLocked(locked_);

    childItems_.insert(index, cpuItem);
}
