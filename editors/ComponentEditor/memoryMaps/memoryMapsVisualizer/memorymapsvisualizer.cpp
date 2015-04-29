//-----------------------------------------------------------------------------
// File: memorymapsvisualizer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 17.04.2012
//
// Description:
// A widget to visualize a set of memory maps.
//-----------------------------------------------------------------------------

#include "memorymapsvisualizer.h"
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapview.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapscene.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapgraphitem.h>

#include <common/graphicsItems/visualizeritem.h>

#include <common/widgets/summaryLabel/summarylabel.h>

#include <QVBoxLayout>
#include <QPointF>

//-----------------------------------------------------------------------------
// Function: MemoryMapsVisualizer::MemoryMapsVisualizer()
//-----------------------------------------------------------------------------
MemoryMapsVisualizer::MemoryMapsVisualizer(QWidget *parent):
ItemVisualizer(parent),
view_(new MemoryMapView(this)),
scene_(new MemoryMapScene(this))
{
	// display a label on top the table
	SummaryLabel* visualizationLabel = new SummaryLabel(tr("Memory maps visualization"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(visualizationLabel, 0, Qt::AlignCenter);
	layout->addWidget(view_);
	layout->setContentsMargins(0, 0, 0, 0);

	view_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	view_->setScene(scene_);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsVisualizer::~MemoryMapsVisualizer()
//-----------------------------------------------------------------------------
MemoryMapsVisualizer::~MemoryMapsVisualizer() 
{
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsVisualizer::addMemoryMapItem()
//-----------------------------------------------------------------------------
void MemoryMapsVisualizer::addMemoryMapItem( MemoryMapGraphItem* memMapItem ) 
{
	scene_->addMemGraphItem(memMapItem);

	QPointF position = memMapItem->pos();
	view_->centerOn(position.x() - MemoryVisualizationItem::CHILD_INDENTATION, position.y());
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsVisualizer::removeMemoryMapItem()
//-----------------------------------------------------------------------------
void MemoryMapsVisualizer::removeMemoryMapItem( MemoryMapGraphItem* memMapItem )
{
	scene_->removeMemGraphItem(memMapItem);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsVisualizer::minimumSizeHint()
//-----------------------------------------------------------------------------
QSize MemoryMapsVisualizer::minimumSizeHint() const
{
	return QSize(VisualizerItem::DEFAULT_WIDTH + 80, VisualizerItem::DEFAULT_HEIGHT * 2);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsVisualizer::sizeHint()
//-----------------------------------------------------------------------------
QSize MemoryMapsVisualizer::sizeHint() const
{
	return minimumSizeHint();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsVisualizer::showEvent()
//-----------------------------------------------------------------------------
void MemoryMapsVisualizer::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit displayed();
}
