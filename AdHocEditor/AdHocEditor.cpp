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

#include <designwidget/diagramcomponent.h>
#include <models/component.h>

#include <LibraryManager/vlnv.h>

#include <designwidget/blockdiagram.h>
#include <designwidget/diagramcomponent.h>

#include <designwidget/DiagramChangeCommands.h>

#include <QVBoxLayout>
#include <QDockWidget>
#include <QHeaderView>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: AdHocEditor()
//-----------------------------------------------------------------------------
AdHocEditor::AdHocEditor(QWidget *parent): QWidget(parent),
                                           dataSource_(0),
                                           nameLabel_(tr("Component: "), this),
                                           portAdHocTable_(this),
                                           adHocModel_(this)
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

    portAdHocTable_.setColumnWidth(ADHOC_COL_DIRECTION, 70);
    portAdHocTable_.setColumnWidth(ADHOC_COL_VISIBILITY, 70);
    portAdHocTable_.horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    portAdHocTable_.horizontalHeader()->setResizeMode(ADHOC_COL_DIRECTION, QHeaderView::Fixed);
    portAdHocTable_.horizontalHeader()->setResizeMode(ADHOC_COL_VISIBILITY, QHeaderView::Fixed);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&nameLabel_);
    layout->addWidget(&portAdHocTable_, 1);
}

//-----------------------------------------------------------------------------
// Function: ~AdHocEditor()
//-----------------------------------------------------------------------------
AdHocEditor::~AdHocEditor()
{
    adHocModel_.setDataSource(0);
}

//-----------------------------------------------------------------------------
// Function: AdHocEditor::setDataSource()
//-----------------------------------------------------------------------------
void AdHocEditor::setDataSource(AdHocEnabled* dataSource)
{
	Q_ASSERT(dataSource);
	parentWidget()->raise();

	// If a previous data source has been specified, detach it from the editor.
	if (dataSource_ != 0)
    {
        dataSource_->detach(this);
	}

	dataSource_ = dataSource;

    if (dynamic_cast<DesignDiagram*>(dataSource_) != 0)
    {
        nameLabel_.setText(tr("Component: top-level"));
    }
    else
    {
        DiagramComponent* compItem = dynamic_cast<DiagramComponent*>(dataSource_);
        Q_ASSERT(compItem != 0);
        nameLabel_.setText(tr("Component: %1").arg(compItem->name()));
    }

    bool locked = dataSource_->isProtected();
    portAdHocTable_.setEnabled(!locked);

    adHocModel_.setDataSource(dataSource_);
    portAdHocTable_.resizeRowsToContents();
    portAdHocTable_.show();

    // if the connected dataSource is destroyed then clear this editor
    // 	connect(dataSource_, SIGNAL(destroyed(ComponentItem*)),
    // 		    this, SLOT(clear()), Qt::UniqueConnection);

    // Attach the data source to the editor.
    dataSource_->attach(this);
	parentWidget()->setMaximumHeight(QWIDGETSIZE_MAX);
}

//-----------------------------------------------------------------------------
// Function: AdHocEditor::clear()
//-----------------------------------------------------------------------------
void AdHocEditor::clear() {

	// If a previous data source has been specified, detach it from the editor.
	if (dataSource_ != 0)
    {
		dataSource_->detach(this);
	}

	dataSource_ = 0;
    adHocModel_.setDataSource(0);
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
