/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorcpusitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorcpusitem.h"
#include "componenteditorcpuitem.h"

ComponentEditorCpusItem::ComponentEditorCpusItem(ComponentEditorTreeModel* model, 
												 LibraryInterface* libHandler,
												 QSharedPointer<Component> component,
												 ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
cpus_(component->getCpus()) {

	foreach (QSharedPointer<Cpu> cpu, cpus_) {
		QSharedPointer<ComponentEditorCpuItem> cpuItem(new ComponentEditorCpuItem(
			cpu, model, libHandler, component, this));
		childItems_.append(cpuItem);
	}
}

ComponentEditorCpusItem::~ComponentEditorCpusItem() {
}

QString ComponentEditorCpusItem::text() const {
	return tr("Cpus");
}

ItemEditor* ComponentEditorCpusItem::editor() {
	return NULL;
}

const ItemEditor* ComponentEditorCpusItem::editor() const {
	return NULL;
}

QString ComponentEditorCpusItem::getTooltip() const {
	return tr("Contains the programmable cores of the component");
}
