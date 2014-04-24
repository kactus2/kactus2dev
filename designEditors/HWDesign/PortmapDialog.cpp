//-----------------------------------------------------------------------------
// File: PortmapDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 10.6.2011
//
// Description:
// Port map dialog.
//-----------------------------------------------------------------------------

#include "PortmapDialog.h"

#include <editors/ComponentEditor/busInterfaces/busifportmaptab.h>
#include <library/LibraryManager/libraryinterface.h>
#include <models/businterface.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QCoreApplication>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: PortmapDialog()
//-----------------------------------------------------------------------------
PortmapDialog::PortmapDialog(LibraryInterface* libInterface, QSharedPointer<Component> component,
                             BusInterface* busIf, BusInterface* otherBusIf,
                             QWidget* parent) : QDialog(parent), busIf_(busIf), otherBusIf_(otherBusIf)
{
    Q_ASSERT(libInterface != 0);
    Q_ASSERT(component != 0);
    Q_ASSERT(busIf != 0);
    Q_ASSERT(otherBusIf != 0);

    setWindowTitle(tr("Define Port Maps"));

    // Create the port map widget.
    portmapWidget_ = new BusIfPortmapTab(libInterface, component, busIf, this);
    portmapWidget_->setAbsType(busIf->getAbstractionType(), busIf->getInterfaceMode());

    // Create a separator.
    QGroupBox* separator = new QGroupBox(this);
    separator->setFlat(true);

    // Create the buttons and their layout.
    QPushButton* btnOk = new QPushButton(tr("OK") , this);
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
    // Check if the port map settings are invalid.
    if (!portmapWidget_->isValid())
    {
        return;
    }

	// this has changed, now port map tab makes the changes immediately
    // Apply the changes to the bus interface.
    //portmapWidget_->applyChanges();

    // Check if all required logical ports are not mapped.
    QList<QString> missingMappings;

    foreach (QSharedPointer<General::PortMap> portMap, otherBusIf_->getPortMaps())
    {
        bool found = false;
        
        foreach (QSharedPointer<General::PortMap> localPortMap, busIf_->getPortMaps())
        {
            // The logical port was mapped correctly if the bus interface has a
            // port map that utilizes the same logical port.
            if (localPortMap->logicalPort_ == portMap->logicalPort_)
            {
                found = true;
                break;
            }
        }

        // If the corresponding port map was not found, add the logical port to the list
        // of missing mappings.
        if (!found)
        {
            missingMappings.append(portMap->logicalPort_);
        }
    }

    // If there were missing mapping, show an error message.
    if (!missingMappings.empty())
    {
        QString detailMessage = tr("The following logical ports must be mapped:");

        foreach (QString const& name, missingMappings)
        {
            detailMessage += "\n * " + name;
        }

        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
                           tr("Some required port maps are missing."), QMessageBox::Ok, this);
        msgBox.setDetailedText(detailMessage);
        msgBox.exec();
        return;
    }

    QDialog::accept();
}
