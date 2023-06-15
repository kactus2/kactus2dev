//-----------------------------------------------------------------------------
// File: bridgeseditor.app
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.06.2012
//
// Description:
// Editor to edit the transparent bridge-elements within a slave interface mode or indirect interface.
//-----------------------------------------------------------------------------

#include "bridgeseditor.h"
#include "bridgesdelegate.h"
#include "BridgeColumns.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/TransparentBridge.h>

#include <KactusAPI/include/BusInterfaceInterface.h>
#include <KactusAPI/include/TransparentBridgeInterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: BridgesEditor::BridgesEditor()
//-----------------------------------------------------------------------------
BridgesEditor::BridgesEditor(BusInterfaceInterface* busInterface,
    QSharedPointer<QList<QSharedPointer<TransparentBridge>>> bridges, QWidget* parent) :
QGroupBox(tr("Transparent bridge(s)"), parent),
view_(this),
proxy_(this),
model_(busInterface->getBridgeInterface(), this),
bridgeInterface_(busInterface->getBridgeInterface()),
bridges_(bridges)
{
    view_.setItemDelegate(new BridgesDelegate(busInterface, this));

    // items can not be dragged
    view_.setItemsDraggable(false);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	view_.setColumnWidth(BridgeColumns::INITIATOR_COLUMN, 200);    

    setFlat(true);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BridgesEditor::~BridgesEditor()
//-----------------------------------------------------------------------------
BridgesEditor::~BridgesEditor()
{

}

//-----------------------------------------------------------------------------
// Function: bridgeseditor::setupBridges()
//-----------------------------------------------------------------------------
void BridgesEditor::setupBridges(QSharedPointer<QList<QSharedPointer<TransparentBridge> > > newBridges)
{
    bridges_ = newBridges;

    refresh();
}

//-----------------------------------------------------------------------------
// Function: BridgesEditor::refresh()
//-----------------------------------------------------------------------------
void BridgesEditor::refresh()
{
    bridgeInterface_->setBridges(bridges_);

    model_.refresh();
}
