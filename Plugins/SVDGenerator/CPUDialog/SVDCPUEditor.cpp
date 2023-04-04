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
#include <Plugins/SVDGenerator/CPUDialog/SVDCPUDetailRoutes.h>

#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: SVDCPUEditor::SVDCPUEditor()
//-----------------------------------------------------------------------------
SVDCPUEditor::SVDCPUEditor(QWidget *parent):
CPUEditor(parent),
view_(new QTableView(this)),
model_(new SVDCPUModel(this))
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
    QVector<QSharedPointer<SVDCPUDetailRoutes> > cpuDetails = getSVDCPURoutes(library, component, activeView);

    model_->setupCPUDetails(cpuDetails);
    view_->resizeColumnsToContents();
}

//-----------------------------------------------------------------------------
// Function: SVDCPUEditor::getSVDCPURoutes()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<SVDCPUDetailRoutes> > SVDCPUEditor::getSVDCPURoutes(LibraryInterface* library, QSharedPointer<Component> component, QString const& activeView)
{
    QVector<QSharedPointer<SVDCPUDetailRoutes> > cpuDetails;
    for (auto defaultCPU : ConnectivityGraphUtilities::getDefaultCPUs(library, component, activeView))
    {
        QSharedPointer<SVDCPUDetailRoutes> svdCPU(new SVDCPUDetailRoutes(*defaultCPU.data()));
        cpuDetails.append(svdCPU);
    }

    return cpuDetails;
}

//-----------------------------------------------------------------------------
// Function: SVDCPUEditor::getSelectedCPUs()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<CPUDetailRoutes> > SVDCPUEditor::getSelectedCPUs() const
{
    QVector<QSharedPointer<CPUDetailRoutes> > defaultCPUs;

    for (auto svdCPU : model_->getCPUDetails())
    {
        defaultCPUs.append(svdCPU);
    }

    return defaultCPUs;
}
