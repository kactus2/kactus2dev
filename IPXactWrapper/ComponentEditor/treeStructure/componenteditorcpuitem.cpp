/* 
 *  	Created on: 15.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorcpuitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorcpuitem.h"

#include <QFont>
#include <QApplication>

ComponentEditorCpuItem::ComponentEditorCpuItem(QSharedPointer<Cpu> cpu,
											   ComponentEditorTreeModel* model,
											   LibraryInterface* libHandler, 
											   QSharedPointer<Component> component,
											   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
cpu_(cpu),
editor_(component, cpu) {
	editor_.hide();
	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
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

const ItemEditor* ComponentEditorCpuItem::editor() const {
	return &editor_;
}

QFont ComponentEditorCpuItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorCpuItem::getTooltip() const {
	return tr("Describes the containing component with a programmable core");
}
