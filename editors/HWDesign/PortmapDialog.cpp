//-----------------------------------------------------------------------------
// File: PortmapDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 10.6.2011
//
// Description:
// Port map dialog.
//-----------------------------------------------------------------------------

#include "PortmapDialog.h"

#include <editors/ComponentEditor/busInterfaces/portmaps/BusInterfacePortMapTab.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/ComponentParameterFinder.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/BusInterfaceInterface.h>
#include <KactusAPI/include/BusInterfaceInterfaceFactory.h>
#include <KactusAPI/include/AbstractionTypeInterface.h>
#include <KactusAPI/include/PortMapInterface.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QCoreApplication>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: PortmapDialog()
//-----------------------------------------------------------------------------
PortmapDialog::PortmapDialog(LibraryInterface* library, QSharedPointer<Component> component,
    QSharedPointer<BusInterface> busIf, QSharedPointer<BusInterface> otherBusIf, QWidget* parent) :
QDialog(parent),
busIf_(busIf),
otherBusIf_(otherBusIf),
designComponent_(component)
{
    Q_ASSERT(library != 0);
    Q_ASSERT(component != 0);
    Q_ASSERT(busIf != 0);
    Q_ASSERT(otherBusIf != 0);

    setWindowTitle(tr("Define Port Maps"));

    QSharedPointer<ParameterFinder> parameterFinder(new ComponentParameterFinder(component));
    QSharedPointer<ExpressionFormatter> expressionFormatter(new ExpressionFormatter(parameterFinder));
    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));


    BusInterfaceInterface* busInterface = BusInterfaceInterfaceFactory::createBusInterface(
        parameterFinder, expressionFormatter, expressionParser, component, library);

    // Temporarily add businterface to design component
    component->getBusInterfaces()->append(busIf);

    AbstractionTypeInterface* absTypeInterface = busInterface->getAbstractionTypeInterface();
    PortMapInterface* portMapInterface = absTypeInterface->getPortMapInterface();
    if (portMapInterface)
    {
        // Create the port map widget.
        portmapWidget_ = new BusInterfacePortMapTab(library, component, busInterface, busIf->name().toStdString(),
            expressionParser, parameterFinder, portMapInterface, this);

        portmapWidget_->setAbstractionDefinitions();

        // Create a separator.
        QGroupBox* separator = new QGroupBox(this);
        separator->setFlat(true);

        // Create the buttons and their layout.
        QPushButton* btnOk = new QPushButton(tr("OK"), this);
        QPushButton* btnCancel = new QPushButton(tr("Cancel"), this);

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->addStretch(1);
        buttonLayout->addWidget(btnOk);
        buttonLayout->addWidget(btnCancel);

        // Create the main layout.
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(portmapWidget_);
        mainLayout->addWidget(separator);
        mainLayout->addLayout(buttonLayout);

        connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
        connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    }
}

//-----------------------------------------------------------------------------
// Function: ~PortmapDialog()
//-----------------------------------------------------------------------------
PortmapDialog::~PortmapDialog()
{

}

//-----------------------------------------------------------------------------
// Function: accept()
//-----------------------------------------------------------------------------
void PortmapDialog::accept()
{
    // Remove temporary bus interface. Results in duplicate component when saved if not deleted
    designComponent_->getBusInterfaces()->removeOne(busIf_);
    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: PortmapDialog::reject()
//-----------------------------------------------------------------------------
void PortmapDialog::reject()
{
    // Remove added temporary interface, if operation is cancelled.
    auto componentBuses = designComponent_->getBusInterfaces();
    for (auto const& bus : *componentBuses)
    {
        if (bus->name() == busIf_->name())
        {
            bus->clearAllPortMaps();
            componentBuses->removeOne(bus);
            break;
        }
    }

    QDialog::reject();
}
