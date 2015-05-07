//-----------------------------------------------------------------------------
// File: addressspacevisualizer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 19.12.2012
//
// Description:
// The visualizer to display the contents of an address space.
//-----------------------------------------------------------------------------

#include "addressspacevisualizer.h"

#include <common/graphicsItems/visualizeritem.h>
#include <common/widgets/summaryLabel/summarylabel.h>

#include <QHBoxLayout>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: AddressSpaceVisualizer::AddressSpaceVisualizer()
//-----------------------------------------------------------------------------
AddressSpaceVisualizer::AddressSpaceVisualizer(QSharedPointer<AddressSpace> addrSpace,
                                               QSharedPointer<ExpressionParser> expressionParser,
											   QWidget *parent):
ItemVisualizer(parent),
addrSpace_(addrSpace),
view_(new QGraphicsView(this)),
scene_(new AddressSpaceScene(addrSpace, expressionParser, this))
{
	// display a label on top the table
	SummaryLabel* segmentLabel = new SummaryLabel(tr("Segments"), this);
	SummaryLabel* localAddrBlockLabel = new SummaryLabel(tr("Address blocks"), this);

	QHBoxLayout* labelLayout = new QHBoxLayout();
	labelLayout->addWidget(segmentLabel, 0, Qt::AlignCenter);
	labelLayout->addWidget(localAddrBlockLabel, 0, Qt::AlignCenter);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addLayout(labelLayout);
	layout->addWidget(view_);
	layout->setContentsMargins(0, 0, 0, 0);

	view_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	view_->setScene(scene_);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceVisualizer::~AddressSpaceVisualizer()
//-----------------------------------------------------------------------------
AddressSpaceVisualizer::~AddressSpaceVisualizer()
{
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceVisualizer::refresh()
//-----------------------------------------------------------------------------
void AddressSpaceVisualizer::refresh()
{
	scene_->refresh();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceVisualizer::minimumSizeHint()
//-----------------------------------------------------------------------------
QSize AddressSpaceVisualizer::minimumSizeHint() const
{
	return QSize(VisualizerItem::DEFAULT_WIDTH * 2 + 3, VisualizerItem::DEFAULT_HEIGHT * 6);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceVisualizer::sizeHint()
//-----------------------------------------------------------------------------
QSize AddressSpaceVisualizer::sizeHint() const
{
	return minimumSizeHint();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceVisualizer::showEvent()
//-----------------------------------------------------------------------------
void AddressSpaceVisualizer::showEvent(QShowEvent* event)
{
    ItemVisualizer::showEvent(event);
    refresh();
}