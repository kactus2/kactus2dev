//-----------------------------------------------------------------------------
// File: PortsView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 2.12.2013
//
// Description:
// View for port editor.
//-----------------------------------------------------------------------------

#include "PortsView.h"

#include <QMenu>
#include <QHeaderView>

#include <KactusAPI/include/BusInterfaceInterface.h>

//-----------------------------------------------------------------------------
// Function: PortsView::PortsView()
//-----------------------------------------------------------------------------
PortsView::PortsView(int const& nameColumn, BusInterfaceInterface* busInterface, QWidget *parent):
EditableTableView(parent),
createBus_(tr("Create new bus definition"), this),
createExistingBus_(tr("Use existing bus definition"), this),
nameColumn_(nameColumn),
busInterface_(busInterface)
{
    verticalHeader()->show();
    verticalHeader()->setMaximumWidth(300);
    verticalHeader()->setMinimumWidth(horizontalHeader()->fontMetrics().horizontalAdvance(tr("Name"))*2);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    connect(&createBus_, SIGNAL(triggered()), this, SLOT(onCreateNewBus()), Qt::UniqueConnection);

    connect(&createExistingBus_, SIGNAL(triggered()), this, SLOT(onCreateExistingBus()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: PortsView::~PortsView()
//-----------------------------------------------------------------------------
PortsView::~PortsView() 
{
}

//-----------------------------------------------------------------------------
// Function: PortsView::onCreateBus()
//-----------------------------------------------------------------------------
void PortsView::onCreateNewBus()
{
    QStringList selectedPorts;
    foreach(QModelIndex const& portIndex, selectedIndexes())
    {   
        QString portName = portIndex.sibling(portIndex.row(), nameColumn_).data().toString();
        if (!selectedPorts.contains(portName))
        {
            selectedPorts.append(portName);
        }
    }

    emit createBus(selectedPorts);
}

//-----------------------------------------------------------------------------
// Function: PortsView::onCreateExistingBus()
//-----------------------------------------------------------------------------
void PortsView::onCreateExistingBus()
{
    QStringList selectedPorts;
    foreach(QModelIndex const& portIndex, selectedIndexes())
    {   
        QString portName = portIndex.sibling(portIndex.row(), nameColumn_).data().toString();
        if (!selectedPorts.contains(portName))
        {
            selectedPorts.append(portName);
        }
    }

    emit createExistingBus(selectedPorts);
}

//-----------------------------------------------------------------------------
// Function: PortsView::contextMenuEvent()
//-----------------------------------------------------------------------------
void PortsView::contextMenuEvent(QContextMenuEvent* event)
{
    pressedPoint_ = event->pos();

    QModelIndex index = indexAt(pressedPoint_);

    QMenu menu(this);
    menu.addAction(&addAction_);

    // if at least one valid item is selected
    if (index.isValid())
    {
        menu.addAction(&removeAction_);
        menu.addAction(&clearAction_);
        menu.addAction(&cutAction_);
        menu.addAction(&copyAction_);
        menu.addAction(&pasteAction_);
    }
    
    if (index.isValid())
    {
        menu.addSeparator();
        QMenu* createMenu = menu.addMenu(tr("New bus interface"));
        createMenu->addAction(&createBus_);    
        createMenu->addAction(&createExistingBus_);           
    }

    if (busInterface_->itemCount() > 0)
    {
        menu.addSeparator();

        QMenu* portsFromBusMenu = menu.addMenu(tr("Create ports from..."));
        for (auto name : busInterface_->getItemNames())
        {
            QString busName = QString::fromStdString(name);
            std::vector<std::string> busAbstractions =
                busInterface_->getAbstractionReferences(busName.toStdString());
            if (busAbstractions.empty() || busAbstractions.size() == 1)
            {
                QAction* busAction(new QAction(busName, this));
                portsFromBusMenu->addAction(busAction);

                connect(busAction, SIGNAL(triggered()), this, SLOT(onCreatePortsFromBus()), Qt::UniqueConnection);
            }
            else
            {
                QMenu* selectAbstractionMenu = portsFromBusMenu->addMenu(busName);
                for (auto abstraction : busAbstractions)
                {
                    QAction* abstractionAction(new QAction(QString::fromStdString(abstraction), this));
                    abstractionAction->setData(QVariant(busName));

                    selectAbstractionMenu->addAction(abstractionAction);
                    connect(abstractionAction, SIGNAL(triggered()),
                        this, SLOT(onCreatePortsFromAbstraction()), Qt::UniqueConnection);
                }
            }
        }
    }

    if (importExportAllowed())
    {
        menu.addSeparator();
        menu.addAction(&importAction_);
        menu.addAction(&exportAction_);
    }

    menu.exec(event->globalPos());

    event->accept();
}

//-----------------------------------------------------------------------------
// Function: PortsView::onCreatePortsFromBus()
//-----------------------------------------------------------------------------
void PortsView::onCreatePortsFromBus()
{
    QAction* senderAction = dynamic_cast<QAction*>(QObject::sender());
    if (senderAction)
    {
        std::string busName = senderAction->text().toStdString();
        QString abstractionVLNV =
            QString::fromStdString(busInterface_->getAbstractionReferenceString(busName));

        emit createPortsFromAbstraction(busName, abstractionVLNV);
    }
}

//-----------------------------------------------------------------------------
// Function: PortsView::onCreatePortsFromAbstraction()
//-----------------------------------------------------------------------------
void PortsView::onCreatePortsFromAbstraction()
{
    QAction* senderAction = dynamic_cast<QAction*>(QObject::sender());
    if (senderAction)
    {
        std::string busName = senderAction->data().toString().toStdString();
        QString abstractionVLNV = senderAction->text();

        emit createPortsFromAbstraction(busName, abstractionVLNV);
    }
}

//-----------------------------------------------------------------------------
// Function: PortsView::mousePressEvent()
//-----------------------------------------------------------------------------
void PortsView::mousePressEvent(QMouseEvent* event)
{
    EditableTableView::mousePressEvent(event);

    QModelIndex currentIndex = indexAt(pressedPoint_);
    if (!currentIndex.isValid())
    {
        emit(changeExtensionsEditorItem(currentIndex));
    }
}

//-----------------------------------------------------------------------------
// Function: PortsView::currentChanged()
//-----------------------------------------------------------------------------
void PortsView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    emit(changeExtensionsEditorItem(current));

    EditableTableView::currentChanged(current, previous);
}
