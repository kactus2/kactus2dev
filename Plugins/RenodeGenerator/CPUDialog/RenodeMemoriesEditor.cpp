//-----------------------------------------------------------------------------
// File: RenodeMemoriesEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 20.03.2023
//
// Description:
// Editor for CPU memory details in renode generation.
//-----------------------------------------------------------------------------

#include "RenodeMemoriesEditor.h"

#include <Plugins/RenodeGenerator/CPUDialog/RenodeMemoriesModel.h>

#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: RenodeMemoriesEditor::RenodeMemoriesEditor()
//-----------------------------------------------------------------------------
RenodeMemoriesEditor::RenodeMemoriesEditor(QWidget *parent):
QWidget(parent),
view_(new QTableView(this)),
model_(new RenodeMemoriesModel(this))
{
    QSortFilterProxyModel* proxy(new QSortFilterProxyModel(this));
    proxy->setSourceModel(model_);

    view_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    view_->horizontalHeader()->setStretchLastSection(true);
    view_->verticalHeader()->hide();
    view_->verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);

    view_->setModel(proxy);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(view_, 1);
    layout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: RenodeMemoriesEditor::setupMemories()
//-----------------------------------------------------------------------------
void RenodeMemoriesEditor::setupMemories(QVector<QSharedPointer<RenodeStructs::cpuMemories>> memories)
{
    model_->setupMemories(memories);
    view_->resizeColumnsToContents();
}
