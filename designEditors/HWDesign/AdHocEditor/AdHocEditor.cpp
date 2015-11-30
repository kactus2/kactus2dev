//-----------------------------------------------------------------------------
// File: AdHocEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 16.2.2012
//
// Description:
// Ad-hoc visibility editor.
//-----------------------------------------------------------------------------

#include "AdHocEditor.h"
#include "AdHocDelegate.h"
#include "AdHocColumns.h"

#include <common/IEditProvider.h>
#include <designEditors/HWDesign/AdHocEnabled.h>

#include <designEditors/HWDesign/HWChangeCommands.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QVBoxLayout>
#include <QHeaderView>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: AdHocEditor()
//-----------------------------------------------------------------------------
AdHocEditor::AdHocEditor(QWidget *parent): QWidget(parent), dataSource_(0), nameLabel_(tr("Component: "), this),
                                           portAdHocTable_(this), adHocModel_(this)
{
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    // Set settings for the table view.
    portAdHocTable_.setSortingEnabled(true);
    portAdHocTable_.setSelectionMode(QAbstractItemView::SingleSelection);
    portAdHocTable_.setItemDelegate(new AdHocDelegate(this));
    portAdHocTable_.verticalHeader()->hide();

    QSortFilterProxyModel* proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(&adHocModel_);
    portAdHocTable_.setModel(proxy);

    // Sort the table.
    portAdHocTable_.sortByColumn(0, Qt::AscendingOrder);

    portAdHocTable_.setColumnWidth(AdHocColumns::ADHOC_COL_DIRECTION, 70);
    portAdHocTable_.setColumnWidth(AdHocColumns::ADHOC_COL_VISIBILITY, 70);
    portAdHocTable_.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    portAdHocTable_.horizontalHeader()->setSectionResizeMode(
        AdHocColumns::ADHOC_COL_DIRECTION, QHeaderView::Fixed);
    portAdHocTable_.horizontalHeader()->setSectionResizeMode(
        AdHocColumns::ADHOC_COL_VISIBILITY, QHeaderView::Fixed);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&nameLabel_);
    layout->addWidget(&portAdHocTable_, 1);
}

//-----------------------------------------------------------------------------
// Function: ~AdHocEditor()
//-----------------------------------------------------------------------------
AdHocEditor::~AdHocEditor()
{
    adHocModel_.setDataSource(0, QSharedPointer<IEditProvider>());
}

//-----------------------------------------------------------------------------
// Function: AdHocEditor::setDataSource()
//-----------------------------------------------------------------------------
void AdHocEditor::setDataSource(AdHocEnabled* dataSource, QSharedPointer<IEditProvider> editProvider,
    bool lockEditor)
{
	Q_ASSERT(dataSource);
	parentWidget()->raise();

	// If a previous data source has been specified, detach it from the editor.
	if (dataSource_ != 0)
    {
        dataSource_->detach(this);
	}

	dataSource_ = dataSource;

    nameLabel_.setText(tr("Component: %1").arg(dataSource_->adHocIdentifier()));

    portAdHocTable_.setEnabled(!lockEditor);

    adHocModel_.setDataSource(dataSource_, editProvider);
    portAdHocTable_.resizeRowsToContents();
    portAdHocTable_.show();

    // Attach the data source to the editor.
    dataSource_->attach(this);
	parentWidget()->setMaximumHeight(QWIDGETSIZE_MAX);
}

//-----------------------------------------------------------------------------
// Function: AdHocEditor::clear()
//-----------------------------------------------------------------------------
void AdHocEditor::clear()
{
	// If a previous data source has been specified, detach it from the editor.
	if (dataSource_ != 0)
    {
		dataSource_->detach(this);
	}

	dataSource_ = 0;
    adHocModel_.setDataSource(0, QSharedPointer<IEditProvider>());
    portAdHocTable_.hide();

	parentWidget()->setMaximumHeight(20);
}

//-----------------------------------------------------------------------------
// Function: AdHocEditor::onContentChanged()
//-----------------------------------------------------------------------------
void AdHocEditor::onContentChanged()
{
    adHocModel_.updateVisibilities();
}
