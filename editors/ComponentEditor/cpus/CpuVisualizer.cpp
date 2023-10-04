//-----------------------------------------------------------------------------
// File: CpuVisualizer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.09.2023
//
// Description:
// The visualizer to display the regions of a CPU.
//-----------------------------------------------------------------------------

#include "CpuVisualizer.h"

#include <common/graphicsItems/visualizeritem.h>
#include <common/widgets/summaryLabel/summarylabel.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressSpace.h>

#include <QHBoxLayout>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: CpuVisualizer::CpuVisualizer()
//-----------------------------------------------------------------------------
CpuVisualizer::CpuVisualizer(QSharedPointer<Cpu> cpu,
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps,
    QSharedPointer<ExpressionParser> expressionParser,
    QWidget* parent) :
ItemVisualizer(parent),
    view_(new QGraphicsView(this)),
    scene_(new RegionScene(cpu, memoryMaps, expressionParser, this))
{
	// display a label on top the table
	auto leftLabel = new SummaryLabel(tr("Regions"), this);
	auto rightLabel = new SummaryLabel(tr("Memory map"), this);

	auto labelLayout = new QHBoxLayout();
	labelLayout->addWidget(leftLabel, 0, Qt::AlignCenter);
	labelLayout->addWidget(rightLabel, 0, Qt::AlignCenter);

	auto layout = new QVBoxLayout(this);
	layout->addLayout(labelLayout);
	layout->addWidget(view_);
	layout->setContentsMargins(0, 0, 0, 0);

	view_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	view_->setScene(scene_);
}

//-----------------------------------------------------------------------------
// Function: CpuVisualizer::refresh()
//-----------------------------------------------------------------------------
void CpuVisualizer::refresh()
{
	scene_->refresh();
}

//-----------------------------------------------------------------------------
// Function: CpuVisualizer::minimumSizeHint()
//-----------------------------------------------------------------------------
QSize CpuVisualizer::minimumSizeHint() const
{
	return QSize(VisualizerItem::DEFAULT_WIDTH * 2 + 3, VisualizerItem::DEFAULT_HEIGHT * 6);
}

//-----------------------------------------------------------------------------
// Function: CpuVisualizer::sizeHint()
//-----------------------------------------------------------------------------
QSize CpuVisualizer::sizeHint() const
{
	return minimumSizeHint();
}

//-----------------------------------------------------------------------------
// Function: CpuVisualizer::showEvent()
//-----------------------------------------------------------------------------
void CpuVisualizer::showEvent(QShowEvent* event)
{
    ItemVisualizer::showEvent(event);
    refresh();
}
