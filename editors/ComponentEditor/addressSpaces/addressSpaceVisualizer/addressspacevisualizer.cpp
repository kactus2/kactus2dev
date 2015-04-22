/* 
 *  	Created on: 19.12.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacevisualizer.cpp
 *		Project: Kactus 2
 */

#include "addressspacevisualizer.h"
#include <common/graphicsItems/visualizeritem.h>
#include <common/widgets/summaryLabel/summarylabel.h>

#include <QHBoxLayout>
#include <QVBoxLayout>

AddressSpaceVisualizer::AddressSpaceVisualizer(QSharedPointer<AddressSpace> addrSpace,
											   QWidget *parent):
ItemVisualizer(parent),
addrSpace_(addrSpace),
view_(new QGraphicsView(this)),
scene_(new AddressSpaceScene(addrSpace, this)) {

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

AddressSpaceVisualizer::~AddressSpaceVisualizer() {
}

void AddressSpaceVisualizer::refresh() {
	scene_->refresh();
}

QSize AddressSpaceVisualizer::minimumSizeHint() const {
	return QSize(VisualizerItem::DEFAULT_WIDTH * 2 + 3, VisualizerItem::DEFAULT_HEIGHT * 6);
}

QSize AddressSpaceVisualizer::sizeHint() const {
	return minimumSizeHint();
}
