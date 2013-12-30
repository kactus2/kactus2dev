//-----------------------------------------------------------------------------
// File: busifportmaptab.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 5.4.2011
//
// Description:
// Tab for editing and viewing component port maps.
//-----------------------------------------------------------------------------

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
mode_(BusIfPortmapTab::ONE2MANY),
portMap_(),
busif_(busif),
component_(component), 
libHandler_(libHandler), 
model_(busif, component, libHandler, this),
view_(this),
logicalView_(this),
logicalModel_(libHandler, &model_, this),
mappingLabel_(tr("Bit-field mapping"),this),
mappingView_(component, this),
mappingProxy_(this),
mappingModel_(busif, component, libHandler, this),
physicalView_(this),
physProxy_(component, this),
physModel_(component, &model_, this),
cleanButton_(QIcon(":/icons/graphics/cleanup.png"), tr("Clean up"), this),
connectButton_(QIcon(":/icons/graphics/connect.png"), tr("Connect"), this),
showAllButton_(tr("Show all ports"), this),
// One to one and one to many removed as obsolete.
/*one2OneButton_(tr("1 to 1"), this),
one2ManyButton_(tr("1 to many"), this),*/
hideConnectedBox_(tr("Hide connected ports"), this),
showHideMappingButton_(tr("Show bit-field mapping"),this),
inButton_(QIcon(":/icons/graphics/control-180.png"), "", this),
outButton_(QIcon(":/icons/graphics/control.png"), "", this),
nameFilterEditor_(new QLineEdit(this)),
//directionFilterEditor_(new QComboBox(this)),
showBitMapping_(false)
{
    // One to one and one to many removed as obsolete.
	// mode buttons are checkable and mutually exclusive
	/*one2OneButton_.setCheckable(true);
	one2OneButton_.setChecked(true);
	one2OneButton_.setAutoExclusive(true);
	one2ManyButton_.setCheckable(true);
	one2ManyButton_.setAutoExclusive(true);*/

    view_.setModel(&model_);
    view_.setItemDelegate(new PortMapsDelegate(this));

    mappingView_.setDragDropMode(QAbstractItemView::DropOnly);
    mappingView_.setAcceptDrops(true); 
    mappingProxy_.setSourceModel(&mappingModel_);   
    mappingView_.setModel(&mappingProxy_);
    mappingLabel_.hide();
    mappingView_.hide();

    // Set view to be sortable.
    mappingProxy_.setSortCaseSensitivity(Qt::CaseInsensitive);
    mappingView_.setSortingEnabled(true);
    mappingView_.sortByColumn(BitMappingModel::INDEX, Qt::AscendingOrder);

    hideConnectedBox_.setCheckState(Qt::Checked);

	// Set the sources for views.
	logicalView_.setModel(&logicalModel_);
    physProxy_.setSourceModel(&physModel_);
	physicalView_.setModel(&physProxy_);

	// Remove the ports from the port lists that are already mapped.
	logicalModel_.removePorts(model_.logicalPorts());

	setupLayout();

    showAllButton_.setVisible(false);

    inButton_.setToolTip(tr("Filter ports by direction in"));
    outButton_.setToolTip(tr("Filter ports by direction out"));
    nameFilterEditor_->setToolTip(tr("Filter ports by name"));
    setTabOrder(&inButton_, &outButton_);
    setTabOrder(&outButton_, nameFilterEditor_);
    setTabOrder(nameFilterEditor_, &physicalView_);

	// Connect signals from model.
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
    connect(&model_, SIGNAL(physicalRemoved(const QString&)),
        &physProxy_, SLOT(onPortDisconnected(const QString&)), Qt::UniqueConnection);

    // Connect signals from logical view.
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

    // Connect signals from physical view.
	connect(&physicalView_, SIGNAL(makeConnection(const QStringList&, const QStringList&)),
		this, SLOT(onMakeConnections(const QStringList&, const QStringList&)), Qt::UniqueConnection);
	connect(&physicalView_, SIGNAL(moveItems(const QStringList&, const QModelIndex&)),
		&physModel_, SLOT(onMoveItems(const QStringList&, const QModelIndex&)), Qt::UniqueConnection);

    // Connect signals from logical model.
	connect(&logicalModel_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&logicalModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    // Connect signal from physical model.
	connect(&physModel_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&physModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    // Connect signals from physical port filters.
    connect(nameFilterEditor_, SIGNAL(textChanged(const QString&)), 
        &physProxy_, SLOT(setFilterPortName(const QString&)), Qt::UniqueConnection);
    connect(nameFilterEditor_, SIGNAL(textChanged(const QString&)), 
        &physicalView_, SLOT(onFilterNameChanged(const QString&)), Qt::UniqueConnection);
    connect(&inButton_, SIGNAL(clicked(bool)), 
        &physProxy_, SLOT(setFilterInDirection(bool)), Qt::UniqueConnection);
    connect(&outButton_, SIGNAL(clicked(bool)), 
        &physProxy_, SLOT(setFilterOutDirection(bool)), Qt::UniqueConnection);

    // Connect signals from bit-field mapping model.
    connect(&mappingModel_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);    
    connect(&mappingModel_, SIGNAL(logicalPortSet(const QString&)),
        &mappingView_, SLOT(onLogicalPortChanged(const QString&)), Qt::UniqueConnection);    

    // Connect signals from toolbar buttons.
	connect(&cleanButton_, SIGNAL(clicked(bool)),
		this, SLOT(onRefresh()), Qt::UniqueConnection);
	connect(&connectButton_, SIGNAL(clicked(bool)),
		this, SLOT(onConnect()), Qt::UniqueConnection);
    connect(&showHideMappingButton_, SIGNAL(clicked(bool)),
        this, SLOT(toggleMappingVisibility()), Qt::UniqueConnection);
    connect(&hideConnectedBox_, SIGNAL(toggled(bool)), 
        &physProxy_, SLOT(setFilterHideConnected(bool)), Qt::UniqueConnection);
    connect(&showAllButton_, SIGNAL(clicked(bool)), 
        this, SLOT(onShowAll()), Qt::UniqueConnection);

    // One to one and one to many removed as obsolete.
	/*connect(&one2OneButton_, SIGNAL(clicked(bool)),
		this, SLOT(onConnectionModeChange()), Qt::UniqueConnection);
	connect(&one2ManyButton_, SIGNAL(clicked(bool)),
		this, SLOT(onConnectionModeChange()), Qt::UniqueConnection);*/
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
// Function: BusIfPortmapTab::isValid()
//-----------------------------------------------------------------------------
bool BusIfPortmapTab::isValid(QStringList& errorList) const
{
	return model_.isValid(errorList);
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::refresh()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::refresh() {
	view_.update();

    model_.reset();    

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
	physicalLayout->addWidget(physLabel); 
    
    // Force filtering buttons to smaller size.
    inButton_.setFixedSize(QSize(13,20));
    inButton_.setContentsMargins(0,0,0,0);
    inButton_.setCheckable(true);
    inButton_.setFlat(true);
    
    outButton_.setFixedSize(QSize(13,20));
    outButton_.setContentsMargins(0,0,0,0);
    outButton_.setCheckable(true);
    outButton_.setFlat(true);

    QHBoxLayout* filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(0);
    filterLayout->setContentsMargins(0,0,0,0);
    filterLayout->addWidget(&inButton_, 0, Qt::AlignBottom);
    filterLayout->addWidget(&outButton_, 0, Qt::AlignBottom);
    filterLayout->addWidget(nameFilterEditor_, 0, Qt::AlignBottom);

    QVBoxLayout* physPortLayout = new QVBoxLayout();
    physPortLayout->setSpacing(0);
    physPortLayout->setContentsMargins(0,0,0,0);
    physPortLayout->addLayout(filterLayout);
    physPortLayout->addWidget(&physicalView_);

    physicalLayout->addLayout(physPortLayout);
    
	QHBoxLayout* buttonLayout = new QHBoxLayout();
    // One to one and one to many removed as obsolete.
	//buttonLayout->addWidget(&one2OneButton_, 0, Qt::AlignLeft);
	//buttonLayout->addWidget(&one2ManyButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&cleanButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&connectButton_, 0 , Qt::AlignLeft);
    buttonLayout->addWidget(&showHideMappingButton_, 0 , Qt::AlignLeft);
    buttonLayout->addWidget(&showAllButton_, 0 , Qt::AlignLeft);
    buttonLayout->addWidget(&hideConnectedBox_, 0 , Qt::AlignLeft);
	buttonLayout->addStretch();

    QVBoxLayout* mappingLayout = new QVBoxLayout();
    mappingLayout->addWidget(&mappingLabel_);    
    mappingLayout->addWidget(&mappingView_);

	QHBoxLayout* portNameLayout = new QHBoxLayout();
	portNameLayout->addLayout(logicalLayout);
    portNameLayout->addLayout(mappingLayout);
	portNameLayout->addLayout(physicalLayout);
	
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
    physProxy_.onConnectionsReset();
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

    if (portMaps.isEmpty())
    {        
        emit errorMessage("No physical ports mapped to " + logicalView_.getSelectedPorts(false).last());
        return; 
   }

    // Remove previous mappings of the logical port, if any.
    QList<QSharedPointer<General::PortMap> >& oldPortMaps = busif_->getPortMaps();
    foreach (QSharedPointer<General::PortMap> oldPortMap, oldPortMaps)
    {
        if (oldPortMap->logicalPort_ == portMaps.first()->logicalPort_)
        {
            oldPortMaps.removeAll(oldPortMap);
        }
    }
    
    foreach (QSharedPointer<General::PortMap> map, portMaps)
    {        
        model_.createMap(map);
                   
        logicalModel_.removePort(map->logicalPort_); 
        logicalView_.onPortRemoved();
        physProxy_.onPortConnected(map->physicalPort_);
    }    
}


//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::onMapPort()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::onMapPortToLastBit()
{
    if (showBitMapping_)
    {
        QStringList ports = physicalView_.getSelectedPorts(false);
        if (ports.size() > 0)
        {
            mappingModel_.mapToEnd(ports);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::keyPressEvent()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::keyPressEvent( QKeyEvent* event ) {

    // if enter / return was pressed
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        if (event->modifiers() == Qt::ShiftModifier)
        {
            onMapPortToLastBit();
        } 
        else
        {
            onConnect();
        }

    }
    // call also the default implementation
	QWidget::keyPressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::onConnectionModeChange()
//-----------------------------------------------------------------------------
/* One to one and one to many removed as obsolete.
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
*/

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
// Function: BusIfPortmapTab::setPhysicalPorts()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::setPhysicalPorts(QStringList const& ports)
{
    physProxy_.setFilterPorts(ports);
    showAllButton_.setVisible(!ports.isEmpty());
    onRefresh();
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::onMakeConnections()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::onMakeConnections( const QStringList& physicals, 
										const QStringList& logicals ) 
{
    // If bit mapping is visible, do not allow connection.
    if (showBitMapping_)
    {
        return;
    }

	if (physicals.isEmpty() || logicals.isEmpty())
		return;

    // One to one and one to many removed as obsolete.
	//if (mode_ == BusIfPortmapTab::ONE2ONE) {

    /*for (int i = 0; i < physicals.size() && i < logicals.size(); ++i) {

        // do not connect empty port names
        if (physicals.at(i).isEmpty() || logicals.at(i).isEmpty())
            continue;

        // if connection can be made between ports
        if (model_.canCreateMap(physicals.at(i), logicals.at(i))) {
            model_.createMap(physicals.at(i), logicals.at(i));                
            logicalModel_.removePort(logicals.at(i));
            logicalView_.onPortRemoved();
            physProxy_.onPortConnected(physicals.at(i));
        }
    }
	}
	// if mode is one to many
	else {*/

		foreach (QString physical, physicals) {
			if (physical.isEmpty())
				continue;



			foreach (QString logical, logicals) {
				if (logical.isEmpty())
					continue;

				if (model_.canCreateMap(physical, logical)) {
					model_.createMap(physical, logical);

					logicalModel_.removePort(logical);
                    physProxy_.onPortConnected(physical);
				}
			}
		}
	//}
}


//-----------------------------------------------------------------------------
// Function: PinMapTab::onLogicalChanged()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::onLogicalChanged(const QModelIndex& index)
{
    QString logicalName = logicalModel_.data(index).toString();

    mappingModel_.onSetLogicalSignal(logicalName);
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::onShowAll()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::onShowAll()
{    
    // Clear physical port filter.
    physProxy_.setFilterPorts(QStringList());
    showAllButton_.setVisible(false);
}

//-----------------------------------------------------------------------------
// Function: BusIfPortmapTab::focusInEvent()
//-----------------------------------------------------------------------------
void BusIfPortmapTab::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/portmaps.html");
}
