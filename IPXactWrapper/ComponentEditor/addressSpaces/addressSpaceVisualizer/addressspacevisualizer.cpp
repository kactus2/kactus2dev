/* 
 *  	Created on: 19.12.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacevisualizer.cpp
 *		Project: Kactus 2
 */

#include "addressspacevisualizer.h"

#include <QHBoxLayout>

AddressSpaceVisualizer::AddressSpaceVisualizer(QSharedPointer<AddressSpace> addrSpace,
											   QSharedPointer<Component> component,
											   QWidget *parent):
ItemVisualizer(component, parent),
addrSpace_(addrSpace),
view_(new QGraphicsView(this)),
scene_(new AddressSpaceScene(addrSpace, this)) {

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(view_);
	layout->setContentsMargins(0, 0, 0, 0);

	view_->setScene(scene_);
}

AddressSpaceVisualizer::~AddressSpaceVisualizer() {
}
