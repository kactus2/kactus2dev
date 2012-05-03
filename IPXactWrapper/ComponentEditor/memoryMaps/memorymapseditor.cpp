/* 
 *  	Created on: 11.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapseditor.cpp
 *		Project: Kactus 2
 */

#include "memorymapseditor.h"

#include <QHBoxLayout>

MemoryMapsEditor::MemoryMapsEditor( QSharedPointer<Component> component,
								   void* dataPointer, 
								   LibraryInterface* handler,
								   QWidget *parent ):
ItemEditor(component, parent),
handler_(handler),
memoryMaps_(static_cast<QList<QSharedPointer<MemoryMap> >* >(dataPointer)),
splitter_(this),
visualizer_(&splitter_),
view_(&splitter_),
model_(this) {

	Q_ASSERT(memoryMaps_);

	view_.setModel(&model_);

	splitter_.addWidget(&view_);
	splitter_.addWidget(&visualizer_);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(&splitter_);

	visualizer_.setMemoryMaps(*memoryMaps_);
}

MemoryMapsEditor::~MemoryMapsEditor() {
}

bool MemoryMapsEditor::isValid() const {
	// if at least one memory map is not in valid state
	foreach (QSharedPointer<MemoryMap> memMap, *memoryMaps_) {
		if (!memMap->isValid()) {
			return false;
		}
	}

	return true;
}
