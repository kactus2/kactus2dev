/* 
 *
 *  Created on: 22.2.2011
 *      Author: Antti Kamppi
 * 		filename: propertywidget.cpp
 */

#include "propertywidget.h"
#include <designwidget/diagramcomponent.h>
#include <designwidget/diagramport.h>
#include <designwidget/diagraminterface.h>

#include <models/businterface.h>

//#include "generalcomponenttab.h"
#include "generalinterfacetab.h"
#include "portmapinterfacetab.h"

#include <QGraphicsItem>

#include <QDebug>

PropertyWidget::PropertyWidget(QWidget *parent): QTabWidget(parent), 
console_(this) {
	
	setTabsClosable(false);
	setTabPosition(QTabWidget::South);

	// set up the message console
	addTab(&console_, tr("Output"));
	connect(this, SIGNAL(errorMessage(const QString&)),
		&console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(noticeMessage(const QString&)),
		&console_, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);
}

PropertyWidget::~PropertyWidget() {
}

void PropertyWidget::onComponentSelected( DiagramComponent* component ) {

	// remove the old editors
	onClearItemSelection();

	// create a component editor for the component and set it in the 
	// selectionDock
// 	GeneralComponentTab* editor = new GeneralComponentTab(component, this);
// 	setCurrentIndex(addTab(editor, tr("General")));
// 	connect(editor, SIGNAL(contentChanged()),
// 		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

}

void PropertyWidget::onPortSelected( DiagramPort* port ) {
	
	// remove the old editors
	onClearItemSelection();

	GeneralInterfaceTab* generalEditor = new GeneralInterfaceTab(port, this);
	setCurrentIndex(addTab(generalEditor, tr("General")));
	connect(generalEditor, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	PortmapInterfaceTab* portmapEditor = new PortmapInterfaceTab(port, this);
	addTab(portmapEditor, tr("Portmap"));
	connect(portmapEditor, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

void PropertyWidget::onInterfaceSelected(DiagramInterface* interface)
{
    // remove the old editors
    onClearItemSelection();

    if (interface->getBusInterface() != 0)
    {
        GeneralInterfaceTab* generalEditor = new GeneralInterfaceTab(interface, this);
        setCurrentIndex(addTab(generalEditor, tr("General")));
        connect(generalEditor, SIGNAL(contentChanged()),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);

        PortmapInterfaceTab* portmapEditor = new PortmapInterfaceTab(interface, this);
        addTab(portmapEditor, tr("Portmap"));
        connect(portmapEditor, SIGNAL(contentChanged()),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    }
}

void PropertyWidget::onClearItemSelection() {

	while (count() > 1) {
		// remove the current tab
		QWidget* editor = widget(1);
		removeTab(1);
		delete editor;
	}

	// set the output tab as the viewed tab
	setCurrentIndex(0);
}
