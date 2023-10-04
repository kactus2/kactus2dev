//-----------------------------------------------------------------------------
// File: SingleCpuItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 15.6.2023
//
// Description:
// The single cpu item used in the component editor navigation tree.
//-----------------------------------------------------------------------------

#include "SingleCpuItem.h"


#include <editors/ComponentEditor/cpus/SingleCpuEditor.h>
#include <editors/common/ExpressionSet.h>

#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/validators/CPUValidator.h>

//-----------------------------------------------------------------------------
// Function: SingleCpuItem::SingleCpuItem()
//-----------------------------------------------------------------------------
SingleCpuItem::SingleCpuItem(QSharedPointer<Cpu> cpu, ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter, ExpressionSet expressions,
    QSharedPointer<CPUValidator> validator,
    ComponentEditorItem* parent) :
ComponentEditorItem(model, libHandler, component, parent),
    cpu_(cpu),
    validator_(validator),
    expressions_(expressions)
{
    setParameterFinder(expressions_.finder);
    setExpressionFormatter(expressions_.formatter);
    setReferenceCounter(referenceCounter);

    if (component->getRevision() == Document::Revision::Std22)
    {
        visualizer_ = new CpuVisualizer(cpu, component->getMemoryMaps(), expressions.parser, nullptr);
    }
}

//-----------------------------------------------------------------------------
// Function: SingleCpuItem::getTooltip()
//-----------------------------------------------------------------------------
QString SingleCpuItem::getTooltip() const
{
    return tr("Specifies a CPU of the component");
}

//-----------------------------------------------------------------------------
// Function: SingleCpuItem::text()
//-----------------------------------------------------------------------------
QString SingleCpuItem::text() const
{
    return cpu_->name();
}

//-----------------------------------------------------------------------------
// Function: SingleCpuItem::isValid()
//-----------------------------------------------------------------------------
bool SingleCpuItem::isValid() const
{
    return validator_->validate(cpu_);
}

//-----------------------------------------------------------------------------
// Function: SingleCpuItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* SingleCpuItem::editor()
{
    if (!editor_)
    {
        editor_ = new SingleCpuEditor(component_, cpu_, validator_->getRegionValidator(), libHandler_, expressions_);
        editor_->setProtection(locked_);

        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(errorMessage(const QString&)),
            this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
            this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);

        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
    }

    return editor_;
}

//-----------------------------------------------------------------------------
// Function: SingleCpuItem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* SingleCpuItem::visualizer()
{
    return visualizer_;
}

//-----------------------------------------------------------------------------
// Function: SingleCpuItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void SingleCpuItem::onGraphicsChanged()
{
    if (visualizer_)
    {
        visualizer_->refresh();
    }
}
