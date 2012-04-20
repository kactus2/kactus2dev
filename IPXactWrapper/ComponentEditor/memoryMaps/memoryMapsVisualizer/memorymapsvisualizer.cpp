/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapsvisualizer.cpp
 *		Project: Kactus 2
 */

#include "memorymapsvisualizer.h"

#include <QHBoxLayout>

MemoryMapsVisualizer::MemoryMapsVisualizer(QWidget *parent):
QWidget(parent),
view_(this),
scene_(this) {

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(&view_);

	view_.setScene(&scene_);
}

MemoryMapsVisualizer::~MemoryMapsVisualizer() {
}

void MemoryMapsVisualizer::setMemoryMaps( QList<QSharedPointer<MemoryMap> >& memoryMaps ) {
	scene_.setMemoryMaps(memoryMaps);
}
