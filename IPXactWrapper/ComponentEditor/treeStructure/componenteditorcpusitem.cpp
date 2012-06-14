/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorcpusitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorcpusitem.h"

ComponentEditorCpusItem::ComponentEditorCpusItem(ComponentEditorTreeModel* model, 
												 LibraryInterface* libHandler,
												 QSharedPointer<Component> component,
												 ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
cpus_(component->getCpus()),
editor_(component) {

	editor_.hide();

	connect(&editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
}

ComponentEditorCpusItem::~ComponentEditorCpusItem() {
}

QString ComponentEditorCpusItem::text() const {
	return tr("Cpus");
}

ItemEditor* ComponentEditorCpusItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorCpusItem::editor() const {
	return &editor_;
}

QString ComponentEditorCpusItem::getTooltip() const {
	return tr("Contains the programmable cores of the component");
}
