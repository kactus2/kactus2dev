/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifportmaptab.cpp
 */

#include "busifportmaptab.h"

#include <models/component.h>
#include <models/abstractiondefinition.h>
#include <LibraryManager/vlnv.h>
#include <LibraryManager/libraryinterface.h>
#include <models/port.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QLabel>

BusIfPortmapTab::BusIfPortmapTab( LibraryInterface* libHandler,
								 QSharedPointer<Component> component,
								 BusInterface* busif, 
								 QWidget* parent ):
QWidget(parent), 
mode_(BusIfPortmapTab::ONE2ONE),
portMap_(),
busif_(busif),
component_(component), 
libHandler_(libHandler), 
mapProxy_(this),
model_(busif, component, libHandler, this),
view_(&mapProxy_, this),
logicalView_(this),
logicalModel_(libHandler, &model_, this),
physicalView_(this),
physModel_(component, &model_, this),
cleanButton_(QIcon(":/icons/graphics/cleanup.png"), tr("Clean up"), this),
connectButton_(QIcon(":/icons/graphics/connect.png"), tr("Connect"), this),
one2OneButton_(tr("1 to 1"), this),
one2ManyButton_(tr("1 to many"), this) {

	// mode buttons are checkable and mutually exclusive
	one2OneButton_.setCheckable(true);
	one2OneButton_.setChecked(true);
	one2OneButton_.setAutoExclusive(true);
	one2ManyButton_.setCheckable(true);
	one2ManyButton_.setAutoExclusive(true);

	mapProxy_.setSourceModel(&model_);
	view_.setModel(&mapProxy_);

	// set the sources for views
	logicalView_.setModel(&logicalModel_);
	physicalView_.setModel(&physModel_);

	// remove the ports from the port lists that are already mapped
	logicalModel_.removePorts(model_.logicalPorts());
	physModel_.removePorts(model_.physicalPorts());

	setupLayout();

	// connect signals from model
	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(logicalRemoved(const QString&)),
		&logicalModel_, SLOT(addPort(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(physicalRemoved(const QString&)),
		&physModel_, SLOT(addPort(const QString&)), Qt::UniqueConnection);

	// connect the signals from view to model that manipulate the items
	connect(&view_, SIGNAL(removeItems(const QModelIndex&)),
		&model_, SLOT(onRemoveItems(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(restoreItem(const QModelIndex&)),
		&model_, SLOT(onRestoreItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(&logicalView_, SIGNAL(removeItem(const QModelIndex&)),
		&logicalModel_, SLOT(removeItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&logicalView_, SIGNAL(removeItems(const QModelIndexList&)),
		&logicalModel_, SLOT(removeItems(const QModelIndexList&)), Qt::UniqueConnection);
	connect(&logicalView_, SIGNAL(makeConnection(const QStringList&, const QStringList&)),
		this, SLOT(onMakeConnections(const QStringList&, const QStringList&)), Qt::UniqueConnection);

	connect(&logicalView_, SIGNAL(moveItems(const QStringList&, const QModelIndex&)),
		&logicalModel_, SLOT(onMoveItems(const QStringList&, const QModelIndex&)), Qt::UniqueConnection);

	connect(&physicalView_, SIGNAL(removeItem(const QModelIndex&)),
		&physModel_, SLOT(removeItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&physicalView_, SIGNAL(removeItems(const QModelIndexList&)),
		&physModel_, SLOT(removeItems(const QModelIndexList&)), Qt::UniqueConnection);
	connect(&physicalView_, SIGNAL(makeConnection(const QStringList&, const QStringList&)),
		this, SLOT(onMakeConnections(const QStringList&, const QStringList&)), Qt::UniqueConnection);

	connect(&physicalView_, SIGNAL(moveItems(const QStringList&, const QModelIndex&)),
		&physModel_, SLOT(onMoveItems(const QStringList&, const QModelIndex&)), Qt::UniqueConnection);

	connect(&logicalModel_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&logicalModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&physModel_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&physModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(&cleanButton_, SIGNAL(clicked(bool)),
		this, SLOT(onRefresh()), Qt::UniqueConnection);
	connect(&connectButton_, SIGNAL(clicked(bool)),
		this, SLOT(onConnect()), Qt::UniqueConnection);

	connect(&one2OneButton_, SIGNAL(clicked(bool)),
		this, SLOT(onConnectionModeChange()), Qt::UniqueConnection);
	connect(&one2ManyButton_, SIGNAL(clicked(bool)),
		this, SLOT(onConnectionModeChange()), Qt::UniqueConnection);
}

BusIfPortmapTab::~BusIfPortmapTab() {
}

bool BusIfPortmapTab::isValid() const {
	return model_.isValid();
}

void BusIfPortmapTab::refresh() {
	view_.update();

	// the lists are refreshed
	onRefresh();
}

void BusIfPortmapTab::setupLayout() {

	QVBoxLayout* logicalLayout = new QVBoxLayout();
	QLabel* logicalLabel = new QLabel(tr("Logical ports"), this);
	logicalLayout->addWidget(logicalLabel);
	logicalLayout->addWidget(&logicalView_);

	QVBoxLayout* physicalLayout = new QVBoxLayout();
	QLabel* physLabel = new QLabel(tr("Physical ports"), this);
	physicalLayout->addWidget(physLabel);
	physicalLayout->addWidget(&physicalView_);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(&one2OneButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&one2ManyButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&cleanButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&connectButton_, 0 , Qt::AlignLeft);
	buttonLayout->addStretch();

	QHBoxLayout* portNameLayout = new QHBoxLayout();
	portNameLayout->addLayout(logicalLayout);
	portNameLayout->addLayout(physicalLayout);
	portNameLayout->addStretch();
	
	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addLayout(buttonLayout);
	topLayout->addLayout(portNameLayout);
	topLayout->addWidget(&view_);
}

void BusIfPortmapTab::onRemove() {
	
}

void BusIfPortmapTab::onRefresh() {
	logicalModel_.refresh();
	physModel_.refresh();

	// remove the ports from the port lists that are already mapped
	logicalModel_.removePorts(model_.logicalPorts());
	physModel_.removePorts(model_.physicalPorts());
}

void BusIfPortmapTab::onConnect() {

	// get lists of the selected ports
	QStringList logicals = logicalView_.getSelectedPorts(false);
	QStringList physicals = physicalView_.getSelectedPorts(false);

	onMakeConnections(physicals, logicals);
}

void BusIfPortmapTab::keyPressEvent( QKeyEvent* event ) {

	// if enter / return was pressed
	if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
		onConnect();

	// call also the default implementation
	QWidget::keyPressEvent(event);
}

void BusIfPortmapTab::onConnectionModeChange() {
	
	// if one to one button was pressed
	if (one2OneButton_.isChecked()) {
		mode_ = BusIfPortmapTab::ONE2ONE;
	}

	// if one to many button was pressed
	else {
		mode_ = BusIfPortmapTab::ONE2MANY;
	}
}

void BusIfPortmapTab::setAbsType( const VLNV& vlnv, General::InterfaceMode mode ) {
	// inform the model of logical signals that it should refresh itself
	logicalModel_.setAbsType(vlnv, mode);

	// tell port maps model that abstraction definition has changed
	model_.setAbsType(vlnv, mode);

	// remove the ports that already have connection
	onRefresh();
}

void BusIfPortmapTab::onMakeConnections( const QStringList& physicals, 
										const QStringList& logicals ) {

	if (physicals.isEmpty() || logicals.isEmpty())
		return;

	if (mode_ == BusIfPortmapTab::ONE2ONE) {

		for (int i = 0; i < physicals.size() && i < logicals.size(); ++i) {
			
			// do not connect empty port names
			if (physicals.at(i).isEmpty() || logicals.at(i).isEmpty())
				continue;

			// if connection can be made between ports
			if (model_.canCreateMap(physicals.at(i), logicals.at(i))) {
				model_.createMap(physicals.at(i), logicals.at(i));
				physModel_.removePort(physicals.at(i));
				logicalModel_.removePort(logicals.at(i));
			}
		}
	}
	// if mode is one to many
	else {

		foreach (QString physical, physicals) {
			if (physical.isEmpty())
				continue;

			bool connected = false;

			foreach (QString logical, logicals) {
				if (logical.isEmpty())
					continue;

				if (model_.canCreateMap(physical, logical)) {
					model_.createMap(physical, logical);
					connected = true;
					logicalModel_.removePort(logical);
				}
			}

			if (connected)
				physModel_.removePort(physical);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::focusInEvent()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/portmaps.html");
}
