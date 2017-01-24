//-----------------------------------------------------------------------------
// File: AdHocVisibilityEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 16.2.2012
//
// Description:
// Ad-hoc visibility editor.
//-----------------------------------------------------------------------------

#include "AdHocVisibilityEditor.h"
#include "AdHocVisibilityDelegate.h"
#include "AdHocVisibilityColumns.h"

#include <common/IEditProvider.h>

#include <designEditors/HWDesign/AdHocEnabled.h>
#include <designEditors/HWDesign/HWChangeCommands.h>
#include <designEditors/HWDesign/AdHocVisibilityEditor/AdHocVisibilityPolicy.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Design/Design.h>

#include <QVBoxLayout>
#include <QHeaderView>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityEditor::AdHocVisibilityEditor()
//-----------------------------------------------------------------------------
AdHocVisibilityEditor::AdHocVisibilityEditor(QWidget *parent):
QWidget(parent),
dataSource_(0),
visibilityPolicy_(new AdHocVisibilityPolicy()),
nameLabel_(tr("Component: "), this),
portAdHocTable_(this),
adHocModel_(visibilityPolicy_, this)
{
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    // Set settings for the table view.
    portAdHocTable_.setSortingEnabled(true);
    portAdHocTable_.setSelectionMode(QAbstractItemView::SingleSelection);
    portAdHocTable_.setItemDelegate(new AdHocVisibilityDelegate(this));
    portAdHocTable_.verticalHeader()->hide();

    QSortFilterProxyModel* proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(&adHocModel_);
    portAdHocTable_.setModel(proxy);

    // Sort the table.
    portAdHocTable_.sortByColumn(0, Qt::AscendingOrder);

    portAdHocTable_.setColumnWidth(AdHocVisibilityColumns::ADHOC_COL_DIRECTION, 70);
    portAdHocTable_.setColumnWidth(AdHocVisibilityColumns::ADHOC_COL_VISIBILITY, 70);
    portAdHocTable_.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    portAdHocTable_.horizontalHeader()->setSectionResizeMode(
        AdHocVisibilityColumns::ADHOC_COL_DIRECTION, QHeaderView::Fixed);
    portAdHocTable_.horizontalHeader()->setSectionResizeMode(
        AdHocVisibilityColumns::ADHOC_COL_VISIBILITY, QHeaderView::Fixed);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&nameLabel_);
    layout->addWidget(&portAdHocTable_, 1);
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityEditor::~AdHocVisibilityEditor()
//-----------------------------------------------------------------------------
AdHocVisibilityEditor::~AdHocVisibilityEditor()
{
    adHocModel_.setDataSource(0, QSharedPointer<IEditProvider>());
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityEditor::setDataSource()
//-----------------------------------------------------------------------------
void AdHocVisibilityEditor::setDataSource(AdHocEnabled* dataSource, QSharedPointer<Design> design,
    QSharedPointer<IEditProvider> editProvider, bool lockEditor)
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

    visibilityPolicy_->setDataSource(dataSource, design->getAdHocConnections());

    adHocModel_.setDataSource(dataSource_, editProvider);
    portAdHocTable_.resizeRowsToContents();
    portAdHocTable_.show();

    // Attach the data source to the editor.
    dataSource_->attach(this);
	parentWidget()->setMaximumHeight(QWIDGETSIZE_MAX);
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityEditor::clear()
//-----------------------------------------------------------------------------
void AdHocVisibilityEditor::clear()
{
	dataSource_ = 0;
    adHocModel_.setDataSource(0, QSharedPointer<IEditProvider>());

    visibilityPolicy_->setDataSource(0, QSharedPointer<QList<QSharedPointer<AdHocConnection> > > ());

    portAdHocTable_.hide();

	parentWidget()->setMaximumHeight(20);
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityEditor::onContentChanged()
//-----------------------------------------------------------------------------
void AdHocVisibilityEditor::onContentChanged()
{
    adHocModel_.updateVisibilities();
}
