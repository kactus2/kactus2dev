//-----------------------------------------------------------------------------
// File: TableViewDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 04.09.2012
//
// Description:
// Generic table view dialog with OK button.
//-----------------------------------------------------------------------------

#include "TableViewDialog.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QDialogButtonBox>
#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: TableViewDialog::TableViewDialog()
//-----------------------------------------------------------------------------
TableViewDialog::TableViewDialog(QWidget* parent)
    : QDialog(parent),
      label_(new QLabel("", this)),
      view_(new QTableView(this)),
      sortProxy_(new QSortFilterProxyModel(this))
{
    view_->setSortingEnabled(true);
    view_->setModel(sortProxy_);
    view_->horizontalHeader()->setStretchLastSection(true);
    view_->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    view_->verticalHeader()->hide();

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(accept()), Qt::UniqueConnection);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(label_);
    layout->addWidget(view_);
    layout->addWidget(buttonBox);
}

//-----------------------------------------------------------------------------
// Function: TableViewDialog::~TableViewDialog()
//-----------------------------------------------------------------------------
TableViewDialog::~TableViewDialog()
{
}

//-----------------------------------------------------------------------------
// Function: TableViewDialog::setDescription()
//-----------------------------------------------------------------------------
void TableViewDialog::setDescription(QString const& desc)
{
    label_->setText(desc);
}

//-----------------------------------------------------------------------------
// Function: TableViewDialog::setModel()
//-----------------------------------------------------------------------------
void TableViewDialog::setModel(QAbstractTableModel* model)
{
    sortProxy_->setSourceModel(model);
    view_->resizeRowsToContents();
    view_->resizeColumnsToContents();
}
