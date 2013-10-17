/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifportmaptab.cpp
 */

#include "busifportmaptab.h"

#include <models/abstractiondefinition.h>
#include <models/component.h>
#include <models/port.h>
#include <LibraryManager/vlnv.h>
#include <LibraryManager/libraryinterface.h>
#include <IPXactWrapper/ComponentEditor/busInterfaces/portmaps/PortListSortProxyModel.h>
#include <IPXactWrapper/ComponentEditor/busInterfaces/portmaps/PortMapsDelegate.h>

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QFormLayout>

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::BusIfPortmapTab()
//-----------------------------------------------------------------------------
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
model_(busif, component, libHandler, this),
view_(this),
logicalView_(this),
logicalModel_(libHandler, &model_, this),
mappingLabel_(tr("Bit-field mapping"),this),
mappingView_(this),
mappingProxy_(this),
mappingModel_(busif, component, libHandler, this),
physicalView_(this),
physProxy_(component, this),
physModel_(component, &model_, this),
cleanButton_(QIcon(":/icons/graphics/cleanup.png"), tr("Clean up"), this),
connectButton_(QIcon(":/icons/graphics/connect.png"), tr("Connect"), this),
one2OneButton_(tr("1 to 1"), this),
one2ManyButton_(tr("1 to many"), this),
hideConnectedBox_(tr("Hide connected ports"), this),
showHideMappingButton_(tr("Show bit-field mapping"),this),
nameFilterEditor_(new QLineEdit(this)),
directionFilterEditor_(new QComboBox(this)),
showBitMapping_(false)
{

	// mode buttons are checkable and mutually exclusive
	one2OneButton_.setCheckable(true);
	one2OneButton_.setChecked(true);
	one2OneButton_.setAutoExclusive(true);
	one2ManyButton_.setCheckable(true);
	one2ManyButton_.setAutoExclusive(true);

    directionFilterEditor_->setEditable(false);
    directionFilterEditor_->addItem("any");
    directionFilterEditor_->addItem("in");
    directionFilterEditor_->addItem("inout");
    directionFilterEditor_->addItem("out");

    view_.setModel(&model_);
    view_.setItemDelegate(new PortMapsDelegate(this));

    mappingView_.setDragDropMode(QAbstractItemView::DropOnly);
    mappingView_.setAcceptDrops(true); 
    mappingProxy_.setSourceModel(&mappingModel_);   
    mappingView_.setModel(&mappingProxy_);
    mappingLabel_.hide();
    mappingView_.hide();

    // set view to be sortable
    mappingProxy_.setSortCaseSensitivity(Qt::CaseInsensitive);
    mappingView_.setSortingEnabled(true);
    mappingView_.sortByColumn(PinMappingModel::INDEX, Qt::AscendingOrder);

    hideConnectedBox_.setCheckState(Qt::Checked);

	// set the sources for views
	logicalView_.setModel(&logicalModel_);
    physProxy_.setSourceModel(&physModel_);
	physicalView_.setModel(&physProxy_);

	// remove the ports from the port lists that are already mapped
	logicalModel_.removePorts(model_.logicalPorts());

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
    connect(&model_, SIGNAL(logicalRemoved(const QString&)),
        &mappingModel_, SLOT(onRemoveMapping(const QString&)), Qt::UniqueConnection);
    connect(&model_, SIGNAL(logicalRestored(const QString&)),
        &logicalModel_, SLOT(addPort(const QString&)), Qt::UniqueConnection);
    connect(&model_, SIGNAL(logicalRestored(const QString&)),
        &logicalView_, SLOT(onPortRestored(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(physicalRemoved(const QString&)),
		&physModel_, SLOT(addPort(const QString&)), Qt::UniqueConnection);
    connect(&model_, SIGNAL(contentChanged()),
        &physProxy_, SLOT(onConnectionsChanged()), Qt::UniqueConnection);

	connect(&logicalView_, SIGNAL(removeItem(const QModelIndex&)),
		&logicalModel_, SLOT(removeItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&logicalView_, SIGNAL(removeItems(const QModelIndexList&)),
		&logicalModel_, SLOT(removeItems(const QModelIndexList&)), Qt::UniqueConnection);
	connect(&logicalView_, SIGNAL(makeConnection(const QStringList&, const QStringList&)),
		this, SLOT(onMakeConnections(const QStringList&, const QStringList&)), Qt::UniqueConnection);

	connect(&logicalView_, SIGNAL(moveItems(const QStringList&, const QModelIndex&)),
		&logicalModel_, SLOT(onMoveItems(const QStringList&, const QModelIndex&)), Qt::UniqueConnection);
    connect(&logicalView_, SIGNAL(clicked(const QModelIndex&)),
        this, SLOT(onLogicalChanged(const QModelIndex&)), Qt::UniqueConnection);

	connect(&physicalView_, SIGNAL(removeItem(const QModelIndex&)),
		&physProxy_, SLOT(onConnectionsChanged()), Qt::UniqueConnection);
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
    connect(nameFilterEditor_, SIGNAL(textChanged(const QString&)), 
        this, SLOT(onNameFilterChanged()), Qt::UniqueConnection);
    connect(directionFilterEditor_, SIGNAL(currentTextChanged(QString const&)), 
        &physProxy_, SLOT(setFilterDirection(QString const&)), Qt::UniqueConnection);

    connect(&hideConnectedBox_, SIGNAL(toggled(bool)), 
        &physProxy_, SLOT(setFilterHideConnected(bool)), Qt::UniqueConnection);

    connect(&mappingModel_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);    

    connect(&model_, SIGNAL(logicalRestored(const QString&)), 
        &mappingModel_, SLOT(onSetLogicalSignal(const QString&)), Qt::UniqueConnection);

	connect(&cleanButton_, SIGNAL(clicked(bool)),
		this, SLOT(onRefresh()), Qt::UniqueConnection);
	connect(&connectButton_, SIGNAL(clicked(bool)),
		this, SLOT(onConnect()), Qt::UniqueConnection);

	connect(&one2OneButton_, SIGNAL(clicked(bool)),
		this, SLOT(onConnectionModeChange()), Qt::UniqueConnection);
	connect(&one2ManyButton_, SIGNAL(clicked(bool)),
		this, SLOT(onConnectionModeChange()), Qt::UniqueConnection);

    connect(&showHideMappingButton_, SIGNAL(clicked(bool)),
        this, SLOT(toggleMappingVisibility()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::~BusIfPortmapTab()
//-----------------------------------------------------------------------------
BusIfPortmapTab::~BusIfPortmapTab() {
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::isValid()
//-----------------------------------------------------------------------------
bool BusIfPortmapTab::isValid() const {
	return model_.isValid();
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::refresh()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::refresh() {
	view_.update();

	// the lists are refreshed
	onRefresh();
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::setupLayout()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::setupLayout() {

	QVBoxLayout* logicalLayout = new QVBoxLayout();
	QLabel* logicalLabel = new QLabel(tr("Logical ports"), this);
	logicalLayout->addWidget(logicalLabel);
	logicalLayout->addWidget(&logicalView_);

	QVBoxLayout* physicalLayout = new QVBoxLayout();
	QLabel* physLabel = new QLabel(tr("Physical ports"), this);
    QGroupBox* filterGroup = new QGroupBox("Filters for Physical ports", this);
	QFormLayout* filterLayout = new QFormLayout(filterGroup);
    filterLayout->addRow("Name", nameFilterEditor_);
    filterLayout->addRow("Direction", directionFilterEditor_);
    filterLayout->addWidget(&hideConnectedBox_);    

	physicalLayout->addWidget(physLabel);    
	physicalLayout->addWidget(&physicalView_);
    

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(&one2OneButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&one2ManyButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&cleanButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&connectButton_, 0 , Qt::AlignLeft);
    buttonLayout->addWidget(&showHideMappingButton_, 0 , Qt::AlignLeft);
	buttonLayout->addStretch();

    QVBoxLayout* mappingLayout = new QVBoxLayout();
    mappingLayout->addWidget(&mappingLabel_);    
    mappingLayout->addWidget(&mappingView_);

	QHBoxLayout* portNameLayout = new QHBoxLayout();
	portNameLayout->addLayout(logicalLayout);
    portNameLayout->addLayout(mappingLayout);
	portNameLayout->addLayout(physicalLayout);
    portNameLayout->addWidget(filterGroup);
	//portNameLayout->addStretch();
	
	QVBoxLayout* topLayout = new QVBoxLayout(this);
    QLabel* portMapLabel = new QLabel(tr("Port Maps"), this);
	topLayout->addLayout(buttonLayout);
	topLayout->addLayout(portNameLayout);
    topLayout->addWidget(portMapLabel);
	topLayout->addWidget(&view_);
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::setMappingVisibility()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::toggleMappingVisibility()
{
    showBitMapping_ = !showBitMapping_;
    mappingLabel_.setVisible(showBitMapping_);
    mappingView_.setVisible(showBitMapping_);

    if (showBitMapping_)
    {
        showHideMappingButton_.setText("Hide bit-field mapping");
    }
    else
    {
        showHideMappingButton_.setText("Show bit-field mapping");
    }
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::onRemove()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::onRemove() {
	
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::onRefresh()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::onRefresh() {
	logicalModel_.refresh();
	physModel_.refresh();

	// remove the ports from the port lists that are already mapped
	logicalModel_.removePorts(model_.logicalPorts());
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::onConnect()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::onConnect() {

    if (!showBitMapping_)
    {
        // get lists of the selected ports
        QStringList logicals = logicalView_.getSelectedPorts(false);
        QStringList physicals = physicalView_.getSelectedPorts(false);

        onMakeConnections(physicals, logicals);
        QString empty = "";
        mappingModel_.onSetLogicalSignal(empty);
    }
    else
    {
        onBitConnect();
    }
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::onBitConnect()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::onBitConnect()
{

    if (mappingModel_.getLogicalSignal().isEmpty())
    {
        emit errorMessage("No logical port selected for mapping.");
        return;
    }

    QList<QSharedPointer<General::PortMap> > portMaps = mappingModel_.getPortMaps();

    if (portMaps.empty())
    {        
        emit errorMessage("No physical ports mapped to " + logicalView_.getSelectedPorts(false).last());
        return; 
   }

    if (!portMaps.isEmpty())
    {
        // Remove previous mappings of the logical port, if any.
        QList<QSharedPointer<General::PortMap> >& oldPortMaps = busif_->getPortMaps();
        foreach (QSharedPointer<General::PortMap> oldPortMap, oldPortMaps)
        {
            if (oldPortMap->logicalPort_ == portMaps.first()->logicalPort_)
            {
                oldPortMaps.removeAll(oldPortMap);
            }
        }
    }

    foreach (QSharedPointer<General::PortMap> map, portMaps)
    {        
        model_.createMap(map);
        logicalModel_.removePort(map->logicalPort_);
    }    
  
    QString empty = "";
    mappingModel_.onSetLogicalSignal(empty);
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::keyPressEvent()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::keyPressEvent( QKeyEvent* event ) {

	// if enter / return was pressed
	if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
		onConnect();

	// call also the default implementation
	QWidget::keyPressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::onConnectionModeChange()
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::setAbsType()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::setAbsType( const VLNV& vlnv, General::InterfaceMode mode ) {

    // inform the model of bit-level mapping that it should refresh itself
    mappingModel_.setAbsType(vlnv, mode);

	// inform the model of logical signals that it should refresh itself
	logicalModel_.setAbsType(vlnv, mode);

	// tell port maps model that abstraction definition has changed
	model_.setAbsType(vlnv, mode);

	// remove the ports that already have connection
	onRefresh();
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::onMakeConnections()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::onMakeConnections( const QStringList& physicals, 
										const QStringList& logicals ) 
{

    if (showBitMapping_)
    {
        return;
    }

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
		}
	}
}


//-----------------------------------------------------------------------------
// Function: PinMapTab::onLogicalChanged()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::onLogicalChanged(const QModelIndex& index)
{
    QString logicalName = logicalModel_.data(index).toString();

    if (!logicalName.isEmpty())
    {
        mappingModel_.onSetLogicalSignal(logicalName);
    }
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::onNameFilterChanged()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::onNameFilterChanged()
{
    QRegExp filterExp(nameFilterEditor_->text(), Qt::CaseInsensitive);
    physProxy_.setFilterRegExp(filterExp);
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::focusInEvent()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/portmaps.html");
}

