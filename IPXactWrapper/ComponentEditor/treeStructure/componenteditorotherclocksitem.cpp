/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorotherclocksitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorotherclocksitem.h"

ComponentEditorOtherClocksItem::ComponentEditorOtherClocksItem(ComponentEditorTreeModel* model, 
															   LibraryInterface* libHandler,
															   QSharedPointer<Component> component,
															   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
otherClocks_(component->getOtherClockDrivers()),
editor_(component) {
	editor_.hide();
	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
}

ComponentEditorOtherClocksItem::~ComponentEditorOtherClocksItem() {
}

QString ComponentEditorOtherClocksItem::text() const {
	return tr("Other clock drivers");
}

bool ComponentEditorOtherClocksItem::isValid() const {
	foreach (QSharedPointer<OtherClockDriver> otherClock, otherClocks_) {
		if (!otherClock->isValid()) {
			return false;
		}
	}
	return editor_.isValid();
}

ItemEditor* ComponentEditorOtherClocksItem::editor() {
	return &editor_;
}

QString ComponentEditorOtherClocksItem::getTooltip() const {
	return tr("Contains the details that define clock waveforms");
}
