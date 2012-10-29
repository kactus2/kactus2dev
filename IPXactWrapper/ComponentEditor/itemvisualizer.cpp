/* 
 *  	Created on: 22.5.2012
 *      Author: Antti Kamppi
 * 		filename: itemvisualizer.cpp
 *		Project: Kactus 2
 */

#include "itemvisualizer.h"

ItemVisualizer::ItemVisualizer(QSharedPointer<Component> component, QWidget *parent):
QWidget(parent),
component_(component) {
	Q_ASSERT(component_);
}

ItemVisualizer::~ItemVisualizer() {
}
