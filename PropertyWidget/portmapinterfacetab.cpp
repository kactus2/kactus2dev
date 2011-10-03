/* 
 *
 *  Created on: 14.3.2011
 *      Author: Antti Kamppi
 * 		filename: portmapinterfacetab.cpp
 */

#include "portmapinterfacetab.h"

#include <designwidget/DiagramChangeCommands.h>
#include <designwidget/blockdiagram.h>

#include <models/generaldeclarations.h>
#include <models/businterface.h>

#include <common/GenericEditProvider.h>

#include <QList>
#include <QSharedPointer>
#include <QHeaderView>
#include <QHBoxLayout>

PortmapInterfaceTab::PortmapInterfaceTab(DiagramConnectionEndPoint* interface, QWidget *parent): 
QWidget(parent), mappings_(this), interface_(interface) {

	Q_ASSERT_X(interface, "PortmapInterfaceTab constructor",
		"Null DiagramConnectionEndPoint pointer given to constructor");

	// There are always 2 columns.
	mappings_.setColumnCount(2);
	mappings_.setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Logical name")));
	mappings_.setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Physical name")));
	mappings_.horizontalHeader()->setStretchLastSection(true);
	mappings_.horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	mappings_.verticalHeader()->hide();

	// set up the layout for the widget
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(&mappings_);

    onContentChanged();	
	connect(interface_, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));
}

PortmapInterfaceTab::~PortmapInterfaceTab() {
}

void PortmapInterfaceTab::onSave() {

	// if interface is not hierarchical the changes are not saved
	if (!interface_->isHierarchical())
		return;

	// save the bus interface name
	QSharedPointer<BusInterface> busIf = interface_->getBusInterface();

	Q_ASSERT_X(busIf, "PortmapInterfaceTab::onSave()",
		"Null BusInterface pointer");

	QList<QSharedPointer<General::PortMap> > portMaps;

	for (int i = 0; i < mappings_.rowCount(); i++) {
		QString logical = mappings_.item(i, 0)->text();
		QString physical = mappings_.item(i, 1)->text();

		QSharedPointer<General::PortMap> portMap(new General::PortMap);
		portMap->logicalPort_ = logical;
		portMap->physicalPort_ = physical;

		portMaps.append(portMap);
	}

    QSharedPointer<QUndoCommand> cmd(new EndPointPortMapCommand(interface_, portMaps));
    static_cast<BlockDiagram*>(interface_->scene())->getEditProvider().addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: onContentChanged()
//-----------------------------------------------------------------------------
void PortmapInterfaceTab::onContentChanged()
{
    // Disable content change signals.
    disconnect(&mappings_, SIGNAL(itemChanged(QTableWidgetItem*)),
               this, SLOT(onSave()));

    QSharedPointer<BusInterface> busIf = interface_->getBusInterface();
    QList<QSharedPointer<General::PortMap> > portMaps = busIf->getPortMaps();

    // as many rows as there are interface maps and always 2 columns
    mappings_.setRowCount(portMaps.size());

    // stop sorting when adding the ports to avoid sorting after each add
    mappings_.setSortingEnabled(false);
    int row = 0;
    foreach (QSharedPointer<General::PortMap> portMap, portMaps) {
        QTableWidgetItem *newItem;

        newItem = new QTableWidgetItem(portMap->logicalPort_);
        newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        mappings_.setItem(row, 0, newItem);

        mappings_.setItem(row, 1, new QTableWidgetItem(portMap->physicalPort_));

        row++;
    }

    // set sorting back after all items are added
    mappings_.setSortingEnabled(true);

    // if interface is not hierarchical then it can't be edited
    if (!interface_->isHierarchical())
        mappings_.setEditTriggers(QTableWidget::NoEditTriggers);

    // if interface is edited then emit a signal of content change
    connect(&mappings_, SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(onSave()), Qt::UniqueConnection);
}
