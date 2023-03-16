//-----------------------------------------------------------------------------
// File: RenodePeripheralsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.02.2023
//
// Description:
// Editor for CPU details in renode generation.
//-----------------------------------------------------------------------------

#include "RenodePeripheralsEditor.h"

#include <Plugins/common/CPUDetailRoutes.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodePeripheralsDelegate.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeCPUDetailRoutes.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodePeripheralsModel.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeUtilities.h>

#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsEditor::RenodePeripheralsEditor()
//-----------------------------------------------------------------------------
RenodePeripheralsEditor::RenodePeripheralsEditor(QWidget *parent):
CPUEditor(parent),
model_(new RenodePeripheralsModel(this)),
peripheralDelegate_(new RenodePeripheralsDelegate(parent)),
renodeCPU_()
{
    QSortFilterProxyModel* proxy(new QSortFilterProxyModel(this));
    proxy->setSourceModel(model_);

    getView()->setModel(proxy);
    getView()->setItemDelegate(peripheralDelegate_);
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsEditor::setCPUDetails()
//-----------------------------------------------------------------------------
void RenodePeripheralsEditor::setupCPUDetails(LibraryInterface* library, QSharedPointer<Component> component, QString const& activeView)
{
    QVector<QSharedPointer<RenodeCPUDetailRoutes> > cpuDetails = RenodeUtilities::getRenodeCpuRoutes(library, component, activeView);
    renodeCPU_ = cpuDetails.first();

    model_->setupPeripherals(renodeCPU_->getPeripherals());

    getView()->resizeColumnsToContents();
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsEditor::getSelectedCPUs()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<CPUDetailRoutes> > RenodePeripheralsEditor::getSelectedCPUs() const
{
    QVector<QSharedPointer<CPUDetailRoutes> > defaultCPUs;
    defaultCPUs.append(renodeCPU_);

    return defaultCPUs;
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsEditor::setupFolderPath()
//-----------------------------------------------------------------------------
void RenodePeripheralsEditor::setupFolderPath(QString const& newFolderPath)
{
    peripheralDelegate_->onFolderChanged(newFolderPath);
}
