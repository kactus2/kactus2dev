//-----------------------------------------------------------------------------
// File: TableViewDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 04.09.2012
//
// Description:
// Generic table view dialog with close button.
//-----------------------------------------------------------------------------

#include "TableViewDialog.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSizePolicy>

//-----------------------------------------------------------------------------
// Function: TableViewDialog::TableViewDialog()
//-----------------------------------------------------------------------------
TableViewDialog::TableViewDialog(QWidget* parent):
QDialog(parent),
    label_(new QLabel("", this)),
    view_(new QTableView(this)),
    sortProxy_(new QSortFilterProxyModel(this))
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    view_->setSortingEnabled(true);
    view_->setModel(sortProxy_);
    view_->horizontalHeader()->setStretchLastSection(true);
    view_->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    view_->setWordWrap(true);    
    view_->verticalHeader()->hide();

    setupLayout();
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

    view_->resizeColumnToContents(0);
    view_->setColumnWidth(1, 500);
    view_->resizeColumnToContents(2);
    view_->resizeRowsToContents();
}

//-----------------------------------------------------------------------------
// Function: TableViewDialog::setupLayout()
//-----------------------------------------------------------------------------
void TableViewDialog::setupLayout()
{
    QWidget* introWidget = new QWidget(this);
    QHBoxLayout* introLayout = new QHBoxLayout(introWidget);
    introLayout->addWidget(label_, 9);
    QLabel* errorIcon = new QLabel(this);
    errorIcon->setPixmap(QPixmap(":/icons/common/graphics/checkIntegrity.png"));
    introLayout->addWidget(errorIcon, 1, Qt::AlignRight);    

    QPalette introPalette = introWidget->palette();
    introPalette.setColor(QPalette::Background, Qt::white);
    introWidget->setPalette(introPalette);
    introWidget->setAutoFillBackground(true);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    connect(buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()),
        this, SLOT(accept()), Qt::UniqueConnection);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(introWidget);
    layout->addWidget(view_, 1);
    layout->addWidget(buttonBox);
}
