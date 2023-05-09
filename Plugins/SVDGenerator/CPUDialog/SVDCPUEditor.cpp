//-----------------------------------------------------------------------------
// File: SVDCPUEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 18.05.2021
//
// Description:
// Editor for CPU details in SVD generation.
//-----------------------------------------------------------------------------

#include "SVDCPUEditor.h"

#include <Plugins/common/ConnectivityGraphUtilities.h>
#include <Plugins/SVDGenerator/CPUDialog/SVDCPUModel.h>
#include <Plugins/SVDGenerator/CPUDialog/SVDCPUDelegate.h>
#include <Plugins/SVDGenerator/CPUDialog/SVDCpuRoutesContainer.h>
#include <Plugins/SVDGenerator/CPUDialog/SVDUtilities.h>

#include <QJsonArray>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: SVDCPUEditor::SVDCPUEditor()
//-----------------------------------------------------------------------------
SVDCPUEditor::SVDCPUEditor(QJsonObject const& configurationObject, QWidget *parent):
CPUEditor(parent),
view_(new QTableView(this)),
model_(new SVDCPUModel(this)),
configurationObject_(configurationObject)
{
    SVDCPUDelegate* cpuDelegate(new SVDCPUDelegate(parent));

    QSortFilterProxyModel* proxy(new QSortFilterProxyModel(this));
    proxy->setSourceModel(model_);

    view_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    view_->horizontalHeader()->setStretchLastSection(true);
    view_->verticalHeader()->hide();
    view_->verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);

    view_->setModel(proxy);
    view_->setItemDelegate(cpuDelegate);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(view_, 1);
    layout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: SVDCPUEditor::setCPUDetails()
//-----------------------------------------------------------------------------
void SVDCPUEditor::setupCPUDetails(LibraryInterface* library, QSharedPointer<Component> component, QString const& activeView)
{
    QVector<QSharedPointer<SVDCpuRoutesContainer> > cpuDetails = getSVDCPURoutes(library, component, activeView);

    model_->setupCPUDetails(cpuDetails);
    view_->resizeColumnsToContents();
}

//-----------------------------------------------------------------------------
// Function: SVDCPUEditor::getSVDCPURoutes()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<SVDCpuRoutesContainer> > SVDCPUEditor::getSVDCPURoutes(LibraryInterface* library, QSharedPointer<Component> component, QString const& activeView)
{
    QJsonValue cpusValue = configurationObject_.value(SVDConstants::CONFIGURATIONCPUS);
    QJsonArray cpuArray;
    if (cpusValue.isArray())
    {
        cpuArray = cpusValue.toArray();
    }

    QVector<QSharedPointer<SVDCpuRoutesContainer> > cpuDetails;
    for (auto defaultCPU : ConnectivityGraphUtilities::getDefaultCPUs(library, component, activeView))
    {
        QSharedPointer<SVDCpuRoutesContainer> svdCPU(new SVDCpuRoutesContainer(*defaultCPU.data()));

        for (auto cpuConfiguration : cpuArray)
        {
            if (cpuConfiguration.isObject())
            {
                QJsonObject cpuObject = cpuConfiguration.toObject();

                QString configurationName = cpuObject.value(SVDConstants::NAME).toString();

                if (configurationName == svdCPU->getFileName())
                {
                    svdCPU->setupConfiguration(cpuObject);
                }
            }
        }

        cpuDetails.append(svdCPU);
    }

    return cpuDetails;
}

//-----------------------------------------------------------------------------
// Function: SVDCPUEditor::getSelectedCPUs()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<CpuRoutesContainer> > SVDCPUEditor::getSelectedCPUs() const
{
    QVector<QSharedPointer<CpuRoutesContainer> > defaultCPUs;

    for (auto svdCPU : model_->getCPUDetails())
    {
        defaultCPUs.append(svdCPU);
    }

    return defaultCPUs;
}
