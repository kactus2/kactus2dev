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
												 QWidget* widget, 
												 ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
cpus_(component->getCpus()) {

	foreach (QSharedPointer<Cpu> cpu, cpus_) {
		ComponentEditorCpuItem* cpuItem = new ComponentEditorCpuItem(
			cpu, model, libHandler, component, widget, this);
		childItems_.append(cpuItem);
	}
}

ComponentEditorCpusItem::~ComponentEditorCpusItem() {
}

QString ComponentEditorCpusItem::text() const {
	return tr("Cpus");
}

bool ComponentEditorCpusItem::isValid() const {
	QStringList addrSpaceNames = component_->getAddressSpaceNames();
	foreach (QSharedPointer<Cpu> cpu, cpus_) {
		if (!cpu->isValid(addrSpaceNames)) {
			return false;
		}
	}
	return true;
}

ItemEditor* ComponentEditorCpusItem::editor() {
	return NULL;
}
