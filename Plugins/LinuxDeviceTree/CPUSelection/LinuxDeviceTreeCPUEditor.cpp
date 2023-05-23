//-----------------------------------------------------------------------------
// File: LinuxDeviceTreeCPUEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.10.2021
//
// Description:
// Editor for CPU details in Linux Device Tree generation.
//-----------------------------------------------------------------------------

#include "LinuxDeviceTreeCPUEditor.h"

#include <Plugins/LinuxDeviceTree/CPUSelection/LinuxDeviceTreeCPUModel.h>
#include <Plugins/LinuxDeviceTree/CPUSelection/LinuxDeviceTreeCPUDelegate.h>

#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUEditor::LinuxDeviceTreeCPUEditor()
//-----------------------------------------------------------------------------
LinuxDeviceTreeCPUEditor::LinuxDeviceTreeCPUEditor(QWidget *parent):
QWidget(parent),
view_(new QTableView(this)),
model_(new LinuxDeviceTreeCPUModel(this))
{
    LinuxDeviceTreeCPUDelegate* cpuDelegate(new LinuxDeviceTreeCPUDelegate(parent));

    QSortFilterProxyModel* proxy(new QSortFilterProxyModel(this));
    proxy->setSourceModel(model_);

    view_->setModel(proxy);
    view_->setItemDelegate(cpuDelegate),

    view_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    view_->horizontalHeader()->setStretchLastSection(true);
    view_->verticalHeader()->hide();
    view_->verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(view_, 1);
    layout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUEditor::setCPUDetails()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeCPUEditor::setupCPUDetails(
    QVector<QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> > cpuDetails)
{
    model_->setupCPUDetails(cpuDetails);
    view_->resizeColumnsToContents();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUEditor::getAcceptedContainers()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> > LinuxDeviceTreeCPUEditor::getAcceptedContainers()
const
{
    return model_->getCPUDetails();
}
