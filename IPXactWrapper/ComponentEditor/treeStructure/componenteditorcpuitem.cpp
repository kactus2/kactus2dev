/* 
 *  	Created on: 15.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorcpuitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorcpuitem.h"

ComponentEditorCpuItem::ComponentEditorCpuItem(QSharedPointer<Cpu> cpu,
											   ComponentEditorTreeModel* model,
											   LibraryInterface* libHandler, 
											   QSharedPointer<Component> component, 
											   QWidget* widget, 
											   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
cpu_(cpu),
editor_(component, cpu, widget) {

}

ComponentEditorCpuItem::~ComponentEditorCpuItem() {
}

QString ComponentEditorCpuItem::text() const {
	return cpu_->getName();
}

bool ComponentEditorCpuItem::isValid() const {
	if (!cpu_->isValid(component_->getAddressSpaceNames())) {
		return false;
	}
	return editor_.isValid();
}

ItemEditor* ComponentEditorCpuItem::editor() {
	return &editor_;
}
