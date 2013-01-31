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
editor_(component, libHandler) {
	editor_.hide();
	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(helpUrlRequested(QString const&)),
		this, SIGNAL(helpUrlRequested(QString const&)));
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
	return true;
}

ItemEditor* ComponentEditorOtherClocksItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorOtherClocksItem::editor() const {
	return &editor_;
}

QString ComponentEditorOtherClocksItem::getTooltip() const {
	return tr("Contains the details that define clock waveforms");
}
