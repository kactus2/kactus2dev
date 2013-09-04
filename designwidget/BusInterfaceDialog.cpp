//-----------------------------------------------------------------------------
// File: BusInterfaceDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 30.5.2011
//
// Description:
// Interface mode selection dialog.
//-----------------------------------------------------------------------------

#include "BusInterfaceDialog.h"

#include <QLabel>
#include <QMessageBox>
#include <QCoreApplication>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QTableView>

#include <common/DesignDiagram.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/delegates/LineEditDelegate/lineeditdelegate.h>
#include <models/port.h>
#include <models/businterface.h>
#include <common/graphicsItems/ConnectionEndpoint.h>
#include <designwidget/views/CellEditTableView.h>
#include <designwidget/models/PortGenerationTableModel.h>
#include <LibraryManager/libraryinterface.h>

namespace
{
    QString const MODE_NAMES[] =
    {
        "Master",
        "Slave",
        "System",
        "Mirrored Master",
        "Mirrored Slave",
        "Mirrored System",
        "Monitor"
    }; 

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog()
//-----------------------------------------------------------------------------
BusInterfaceDialog::BusInterfaceDialog(bool enableNameEdit,
                                       QWidget* parent) : 
                                                          QDialog(parent), layout_(0),
                                                          nameLabel_(0),nameEdit_(0),
                                                          modeGroup_(0), btnOK_(0),
                                                          btnCancel_(0), tableGroup_(0), 
                                                          tableEnable_(false),portsView_(0),
                                                          portsModel_(0),
                                                          proxy_(0), modes_(0)
{
    setWindowTitle(tr("Define Bus Interface"));

    // Check if name editing is requested.
    if (enableNameEdit)
    {
        // Create the name edit and its label.
        nameLabel_ = new QLabel(tr("Name:"), this);
        nameEdit_ = new QLineEdit(this);
      }

    // Create the radio button group.
    modeGroup_ = new QGroupBox(tr("Select Interface Mode"), this);
    for (unsigned int i = 0; i < General::MONITOR + 1; ++i)
    {
        modeRadioButtons_[i] = new QRadioButton(MODE_NAMES[i], modeGroup_);
        modeRadioButtons_[i]->setVisible(false);       
    }

    // Create the OK and Cancel buttons.
    btnOK_ = new QPushButton(tr("&OK"),this);
    btnCancel_ = new QPushButton(tr("&Cancel"),this);

    // Connect the button signals to accept() and reject().
    connect(btnOK_, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel_, SIGNAL(clicked()), this, SLOT(reject()));

    if (nameEdit_ != 0)
    {
        connect(nameEdit_, SIGNAL(textChanged(QString const&)), this, SLOT(onNameChanged()));
        btnOK_->setEnabled(!nameEdit_->text().isEmpty());
    }

    tableGroup_ = new QGroupBox(tr("Define Port Names and Descriptions in Draft Component"), 
                                    this);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ~BusInterfaceDialog()
//-----------------------------------------------------------------------------
BusInterfaceDialog::~BusInterfaceDialog()
{
}

//-----------------------------------------------------------------------------
// Function: addMode()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::addMode(General::InterfaceMode mode)
{
    modeRadioButtons_[mode]->setVisible(true);
    modeRadioButtons_[mode]->setChecked(true);
    connect(modeRadioButtons_[mode], SIGNAL(clicked(bool)), this, SLOT(onModeSelected(bool)));
    modes_++;
}

//-----------------------------------------------------------------------------
// Function: getSelectedMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusInterfaceDialog::getSelectedMode() const
{
    for (unsigned int i = 0; i < General::MONITOR + 1; ++i)
    {
        if (modeRadioButtons_[i]->isChecked())
        {
            return static_cast<General::InterfaceMode>(i);
        }
    }

    // Should not be possible to get here.
    Q_ASSERT(false);
    return General::MASTER;
}

//-----------------------------------------------------------------------------
// Function: setBusPorts()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::setBusInterfaces(ConnectionEndpoint const* opposingBusPort, 
    ConnectionEndpoint const* draftBusPort, LibraryInterface* lh)
{
    lh_ = lh;
    opposingEnd_ = opposingBusPort;
    draftEnd_ = draftBusPort;

    if ( !tableEnable_ )
    {
        setupPortTable();      

        if ( modes_ == 1 )
        {
            btnOK_->setEnabled(portsModel_->isValid());
            portsView_->setEnabled(true);
            modeGroup_->setEnabled(false);
            updatePortsView();
        }
        else
        {
            // Clear initial mode selection.
            for (unsigned int i = 0; i < General::MONITOR + 1; ++i)
            {
                modeRadioButtons_[i]->setAutoExclusive(false);  
                modeRadioButtons_[i]->setChecked(false);
                modeRadioButtons_[i]->setAutoExclusive(true);
            }

            btnOK_->setEnabled(false);
            portsView_->setEnabled(false);
        }
    }

    ComponentItem* sourceComponent = opposingBusPort->encompassingComp();    
    QString sourceName;   
    if (sourceComponent)
    {
        sourceName = sourceComponent->name();
    }
    else
    {
        sourceName = "Top-level";
    }

    QString sourceHeader = "Name in " + sourceName;
    portsModel_->setHeaderData(PortGenerationTableModel::SRC_NAME, Qt::Horizontal, sourceHeader);
    
    QString draftHeader = "Name in " + draftBusPort->encompassingComp()->name();
    portsModel_->setHeaderData(PortGenerationTableModel::DRAFT_NAME, Qt::Horizontal, draftHeader);
}

//-----------------------------------------------------------------------------
// Function: getPortMaps()
//-----------------------------------------------------------------------------
QList< QSharedPointer<General::PortMap> > BusInterfaceDialog::getPortMaps() const
{
    QList< QSharedPointer<General::PortMap> > portMaps;
   
    if ( tableEnable_ )
    {
        foreach ( QSharedPointer<General::PortMap> portMap, opposingEnd_->getBusInterface()->getPortMaps() )
        {
            int row = 0;
            QModelIndex index;
            for(row = 0; row < portsModel_->rowCount(); row++)
            {
                index = portsModel_->index(row,PortGenerationTableModel::SRC_NAME);
                QString name = portsModel_->data(index,Qt::DisplayRole).toString();
                if ( name == portMap->physicalPort_)
                {
                    break;
                }
            }
            index = portsModel_->index(row,PortGenerationTableModel::DRAFT_NAME);
            QString physDraft = portsModel_->data(index,Qt::DisplayRole).toString();

            QSharedPointer<General::PortMap> generated(new General::PortMap(*portMap));
            generated->physicalPort_ = physDraft;
            portMaps.append(generated);
        }
    }

    return portMaps;
}

//-----------------------------------------------------------------------------
// Function: getPorts()
//-----------------------------------------------------------------------------
QList< QSharedPointer<Port> > BusInterfaceDialog::getPorts() const
{
    QList< QSharedPointer<Port> > portList;
    if ( tableEnable_ )
    {
        for(int row = 0; row < portsModel_->rowCount(); row++)
        {
            QModelIndex index = portsModel_->index(row,PortGenerationTableModel::SRC_NAME);
            QString name = portsModel_->data(index,Qt::DisplayRole).toString();
            index = portsModel_->index(row,PortGenerationTableModel::DRAFT_NAME);
            QString generatedName = portsModel_->data(index,Qt::DisplayRole).toString();
            QSharedPointer<Port> port = opposingEnd_->getOwnerComponent()->getPort(name);
            QSharedPointer<Port> draftPort(new Port(generatedName,*port));
            index = portsModel_->index(row,PortGenerationTableModel::DRAFT_DIRECTION);
            QString draftDir = portsModel_->data(index,Qt::DisplayRole).toString();
            draftPort->setDirection(General::str2Direction(draftDir,General::DIRECTION_INVALID));
            portList.append(draftPort);
        }
    }

    return portList;
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::setName(QString const& name)
{
    Q_ASSERT(nameEdit_ != 0);
    nameEdit_->setText(name);
}

//-----------------------------------------------------------------------------
// Function: getName()
//-----------------------------------------------------------------------------
QString BusInterfaceDialog::getName() const
{
    Q_ASSERT(nameEdit_ != 0);
    return nameEdit_->text();
}

//-----------------------------------------------------------------------------
// Function: onNameChanged()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::onNameChanged()
{
    btnOK_->setEnabled(!nameEdit_->text().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: onTableDataChanged()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::onTableDataChanged()
{
    btnOK_->setEnabled(portsModel_->isValid());
}

//-----------------------------------------------------------------------------
// Function: onModeSelected()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::onModeSelected(bool const radioButtonChecked)
{
    if ( radioButtonChecked && tableEnable_ )
    {
        btnOK_->setDefault(false);
        portsView_->setEnabled(true);
        updatePortsView();        
    }
}

//-----------------------------------------------------------------------------
// Function: setupLayout()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::setupLayout()
{
    layout_ = new QVBoxLayout(this);

    // Create layout for name editing.
    if ( nameEdit_ != 0 )
    {
        QHBoxLayout* nameEditLayout = new QHBoxLayout();
        nameEditLayout->addWidget(nameLabel_);
        nameEditLayout->addWidget(nameEdit_);
        layout_->addLayout(nameEditLayout);
    }

    // Create vertical layout for radio buttons.
    QGridLayout* itemLayout = new QGridLayout(modeGroup_);
    for (unsigned int i = 0; i < General::MONITOR + 1; ++i)
    {
        itemLayout->addWidget(modeRadioButtons_[i], i, 0, 1, 1);
    }
    layout_->addWidget(modeGroup_);
  
    layout_->addWidget(tableGroup_);  
    tableGroup_->setVisible(false);

    // Create layout for OK and Cancel buttons.
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnOK_, 0, Qt::AlignRight);
    buttonLayout->addWidget(btnCancel_, 0, Qt::AlignRight);

    layout_->addLayout(buttonLayout);
}

//-----------------------------------------------------------------------------
// Function: setupPortTable()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::setupPortTable()
{
    portsView_ = new CellEditTableView(this);
    portsView_->setSortingEnabled(true);
    //portsView_->setEnabled(false);
    portsView_->setItemDelegate(new LineEditDelegate(this));

    portsModel_ = new PortGenerationTableModel(this);   

    proxy_ = new QSortFilterProxyModel(this);
    proxy_->setSourceModel(portsModel_);
    portsView_->setModel(proxy_);  
    portsView_->sortByColumn(1, Qt::AscendingOrder);

    // Create new, horizontal, layout for radio buttons.
    delete modeGroup_->layout();
    QGridLayout* itemLayout = new QGridLayout(modeGroup_);
    for (unsigned int i = 0; i < General::MONITOR + 1; ++i)
    {
        itemLayout->addWidget(modeRadioButtons_[i], 0, i, 1, 1);       
    }
    modeGroup_->setLayout(itemLayout);

    // Create layout for port generation table.
    QGridLayout* tableLayout = new QGridLayout(tableGroup_);          
    tableLayout->addWidget(portsView_,1,0,1,1);   
    tableGroup_->setVisible(true);

    connect(portsModel_,SIGNAL(dataChanged(const QModelIndex &,const QModelIndex &)),
        this, SLOT(onTableDataChanged()));

    setMinimumSize(QSize(800, 600));
    tableEnable_ = true;
}

//-----------------------------------------------------------------------------
// Function: updatePortsView()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::updatePortsView()
{
    if ( tableEnable_ && modes_ != 0 )
    {
        portsModel_->initialize(lh_,opposingEnd_, draftEnd_, getSelectedMode());        
        onTableDataChanged();
    }
}
