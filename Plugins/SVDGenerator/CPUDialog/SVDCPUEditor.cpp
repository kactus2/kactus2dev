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

#include <Plugins/SVDGenerator/CPUDialog/SVDCPUModel.h>
#include <Plugins/SVDGenerator/CPUDialog/SVDCPUDelegate.h>

#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: SVDCPUEditor::SVDCPUEditor()
//-----------------------------------------------------------------------------
SVDCPUEditor::SVDCPUEditor(QWidget *parent):
QWidget(parent),
view_(new QTableView(this)),
model_(new SVDCPUModel(this))
{
    SVDCPUDelegate* cpuDelegate(new SVDCPUDelegate(parent));

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
// Function: SVDCPUEditor::setCPUDetails()
//-----------------------------------------------------------------------------
void SVDCPUEditor::setupCPUDetails(
    QVector<QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes>> cpuDetails)
{
    model_->setupCPUDetails(cpuDetails);
    view_->resizeColumnsToContents();
}
