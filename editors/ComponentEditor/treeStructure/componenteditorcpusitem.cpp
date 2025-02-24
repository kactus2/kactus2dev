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

#include <editors/ComponentEditor/treeStructure/SingleCpuItem.h>
#include <editors/ComponentEditor/cpus/cpuseditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/validators/CPUValidator.h>
#include <IPXACTmodels/Component/validators/CollectionValidators.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorCpusItem::ComponentEditorCpusItem()
//-----------------------------------------------------------------------------
ComponentEditorCpusItem::ComponentEditorCpusItem(ComponentEditorTreeModel* model, 
    LibraryInterface* libHandler,
    QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter,
    ExpressionSet expressions,
    ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
    cpus_(component->getCpus()),
    singleCpuValidator_(new CPUValidator(
        QSharedPointer<ParameterValidator>(new ParameterValidator(expressions.parser, component->getChoices(), component->getRevision())),
        expressions.parser, component->getAddressSpaces(), component->getMemoryMaps(), component->getRevision())),
    cpusValidator_(new CPUsValidator(singleCpuValidator_)),
    expressions_(expressions)
{
    setParameterFinder(expressions.finder);
    setExpressionFormatter(expressions.formatter);
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
		editor_ = new CpusEditor(component_, libHandler_, singleCpuValidator_, expressions_);
        editor_->setProtection(locked_);
        connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
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
    auto cpusNameGroup = CollectionValidators::itemListToNameGroupList(cpus_);
    cpusValidator_->childrenHaveUniqueNames(cpusNameGroup);
    return ComponentEditorItem::isValid();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorCpusItem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorCpusItem::createChild(int index)
{
    QSharedPointer<SingleCpuItem> cpuItem(new SingleCpuItem(cpus_->at(index), model_, libHandler_,
        component_, referenceCounter_, expressions_, singleCpuValidator_, this));

    cpuItem->setLocked(locked_);

    childItems_.insert(index, cpuItem);
}
