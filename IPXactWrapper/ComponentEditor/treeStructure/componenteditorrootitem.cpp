/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorrootitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorrootitem.h"

#include "componenteditorgeneralitem.h"
#include "componenteditorfilesetsitem.h"
#include "componenteditormodelparamsitem.h"
#include "componenteditorparametersitem.h"
#include "componenteditormemmapsitem.h"
#include "componenteditoraddrspacesitem.h"
#include "componenteditorviewsitem.h"
#include "componenteditorportsitem.h"
#include "componenteditorbusinterfacesitem.h"
#include "componenteditorchannelsitem.h"
#include "componenteditorcpusitem.h"
#include "componenteditorotherclocksitem.h"
#include "componenteditorsoftmapsitem.h"
#include "componenteditorcominterfacesitem.h"

ComponentEditorRootItem::ComponentEditorRootItem( LibraryInterface* libHandler,
												 QSharedPointer<Component> component,
												 QWidget* widget,
												 ComponentEditorTreeModel* parent ):
ComponentEditorItem(libHandler, component, parent) {

	childItems_.append(new ComponentEditorGeneralItem(parent, libHandler, component, widget, this));
	childItems_.append(new ComponentEditorFileSetsItem(parent, libHandler, component, widget, this));
	childItems_.append(new ComponentEditorModelParamsItem(parent, libHandler, component, widget, this));
	childItems_.append(new ComponentEditorParametersItem(parent, libHandler, component, widget, this));
	childItems_.append(new ComponentEditorMemMapsItem(parent, libHandler, component, widget, this));
	childItems_.append(new ComponentEditorAddrSpacesItem(parent, libHandler, component, widget, this));
	childItems_.append(new ComponentEditorViewsItem(parent, libHandler, component, widget, this));
	childItems_.append(new ComponentEditorPortsItem(parent, libHandler, component, widget, this));
	childItems_.append(new ComponentEditorBusInterfacesItem(parent, libHandler, component, widget, this));
	childItems_.append(new ComponentEditorChannelsItem(parent, libHandler, component, widget, this));
	childItems_.append(new ComponentEditorCpusItem(parent, libHandler, component, widget, this));
	childItems_.append(new ComponentEditorOtherClocksItem(parent, libHandler, component, widget, this));
	childItems_.append(new ComponentEditorSoftMapsItem(parent, libHandler, component, widget , this));
	childItems_.append(new ComponentEditorComInterfacesItem(parent, libHandler, component, widget, this));
}

ComponentEditorRootItem::~ComponentEditorRootItem() {

}

QString ComponentEditorRootItem::text() const {
	return tr("root");
}

bool ComponentEditorRootItem::isValid() const {
	return true;
}

ItemEditor* ComponentEditorRootItem::editor() {
	return NULL;
}
