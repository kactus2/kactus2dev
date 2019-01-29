//-----------------------------------------------------------------------------
// File: ComponentItemAutoConnector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.01.2019
//
// Description:
// Automatically connects the ports and bus interfaces of two component items.
//-----------------------------------------------------------------------------

#include "ComponentItemAutoConnector.h"

#include <common/graphicsItems/ComponentItem.h>

#include <editors/common/ComponentItemAutoConnector/PortList.h>

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>

//-----------------------------------------------------------------------------
// Function: ComponentItemAutoConnector::ComponentItemAutoConnector()
//-----------------------------------------------------------------------------
ComponentItemAutoConnector::ComponentItemAutoConnector(ComponentItem* firstItem, ComponentItem* secondItem,
    QWidget* parent):
QDialog(parent),
clearButton_(new QPushButton(QIcon(":/icons/common/graphics/cleanup.png"), tr("Clear"), this)),
firstItem_(firstItem),
secondItem_(secondItem),
portsTable_()
{
    setMinimumWidth(1000);
    setMinimumHeight(600);
    
    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ComponentItemAutoConnector::~ComponentItemAutoConnector()
//-----------------------------------------------------------------------------
ComponentItemAutoConnector::~ComponentItemAutoConnector()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentItemAutoConnector::getConnectedPorts()
//-----------------------------------------------------------------------------
QVector<QPair<ConnectedPortsTable::ConnectedPort, ConnectedPortsTable::ConnectedPort> >
    ComponentItemAutoConnector::getConnectedPorts() const
{
    return portsTable_->getConnectedPorts();
}

//-----------------------------------------------------------------------------
// Function: ComponentItemAutoConnector::setupLayout()
//-----------------------------------------------------------------------------
void ComponentItemAutoConnector::setupLayout()
{
    QPushButton* okButton(new QPushButton(tr("Ok"), this));
    connect(okButton, SIGNAL(released()), this, SLOT(accept()), Qt::UniqueConnection);

    QPushButton* cancelButton (new QPushButton(tr("Cancel"), this));
    connect(cancelButton, SIGNAL(released()), this, SLOT(reject()), Qt::UniqueConnection);

    QDialogButtonBox* buttonBox (new QDialogButtonBox(Qt::Horizontal));
    buttonBox->addButton(clearButton_, QDialogButtonBox::ActionRole);
    buttonBox->addButton(okButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);

    setWindowTitle("Auto connect (experimental)");

    PortList* firstComponentPortList(new PortList(firstItem_->componentModel(), this));

    QVBoxLayout* firstComponentLayout(new QVBoxLayout());
    firstComponentLayout->addWidget(firstComponentPortList);

    QGroupBox* firstComponentGroup(new QGroupBox(getComponentItemName(firstItem_), this));
    firstComponentGroup->setLayout(firstComponentLayout);

    PortList* secondComponentPortList(new PortList(secondItem_->componentModel(), this));
    
    QVBoxLayout* secondComponentLayout(new QVBoxLayout());
    secondComponentLayout->addWidget(secondComponentPortList);

    QGroupBox* secondComponentGroup(new QGroupBox(getComponentItemName(secondItem_), this));
    secondComponentGroup->setLayout(secondComponentLayout);

    portsTable_ = new ConnectedPortsTable(firstItem_, secondItem_, this);
    connect(clearButton_, SIGNAL(released()), portsTable_, SLOT(clearConnectedPorts()), Qt::UniqueConnection);

    QVBoxLayout* connectedPortsLayout(new QVBoxLayout());
    connectedPortsLayout->addWidget(portsTable_);

    QGroupBox* connectedPortsGroup(new QGroupBox(tr("Connected ports")));
    connectedPortsGroup->setAlignment(Qt::AlignHCenter);
    connectedPortsGroup->setLayout(connectedPortsLayout);

    QHBoxLayout* portsLayout(new QHBoxLayout());
    portsLayout->addWidget(firstComponentGroup);
    portsLayout->addWidget(connectedPortsGroup, 2);
    portsLayout->addWidget(secondComponentGroup);

    QVBoxLayout* mainLayout (new QVBoxLayout(this));
    mainLayout->addLayout(portsLayout, 1);
    mainLayout->addWidget(buttonBox);
}

//-----------------------------------------------------------------------------
// Function: ComponentItemAutoConnector::getComponentItemName()
//-----------------------------------------------------------------------------
QString ComponentItemAutoConnector::getComponentItemName(ComponentItem* componentItem) const
{
    if (!componentItem->displayName().isEmpty())
    {
        return componentItem->displayName();
    }
    else
    {
        return componentItem->name();
    }
}
