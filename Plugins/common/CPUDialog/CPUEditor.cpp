//-----------------------------------------------------------------------------
// File: CPUEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 17.02.2023
//
// Description:
// Editor for CPU details in generators.
//-----------------------------------------------------------------------------

#include "CPUEditor.h"

#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: CPUEditor::CPUEditor()
//-----------------------------------------------------------------------------
CPUEditor::CPUEditor(QWidget *parent):
QWidget(parent),
view_(new QTableView(this))
{
    view_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    view_->horizontalHeader()->setStretchLastSection(true);
    view_->verticalHeader()->hide();
    view_->verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(view_, 1);
    layout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: CPUEditor::setupFolderPath()
//-----------------------------------------------------------------------------
void CPUEditor::setupFolderPath(QString const& /*newFolderPath*/)
{

}

//-----------------------------------------------------------------------------
// Function: CPUEditor::getView()
//-----------------------------------------------------------------------------
QTableView* CPUEditor::getView() const
{
    return view_;
}
