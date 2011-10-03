/* $Id$
 *
 *  Created on: 21.1.2011
 *      Author: Antti Kamppi
 */

#include "attributeboxplugin.h"
#include "attributebox.h"

#include <QObject>
#include <QtPlugin>

AttributeBoxPlugin::AttributeBoxPlugin(QObject* parent): QObject(parent) {
}

AttributeBoxPlugin::~AttributeBoxPlugin() {
}

QString AttributeBoxPlugin::name() const {
	return "AttributeBox";
}

QString AttributeBoxPlugin::includeFile() const {
	return "attributebox.h";
}

QString AttributeBoxPlugin::group() const {
        return "IP-Xact widgets";
}

QIcon AttributeBoxPlugin::icon() const {
	return QIcon();
}

QString AttributeBoxPlugin::toolTip() const {
	return "Widget to edit attributes";
}

QString AttributeBoxPlugin::whatsThis() const {
	return "This widget can beu used to create/edit list of attributes";
}

bool AttributeBoxPlugin::isContainer() const {
	return false;
}

QWidget* AttributeBoxPlugin::createWidget(QWidget* parent) {
	return new AttributeBox(parent);
}

void AttributeBoxPlugin::initialize(QDesignerFormEditorInterface * /* core */) {
     if (initialized)
         return;

     initialized = true;
 }

 bool AttributeBoxPlugin::isInitialized() const {
     return initialized;
 }

Q_EXPORT_PLUGIN2(attributeboxplugin, AttributeBoxPlugin)
