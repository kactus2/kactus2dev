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
#include <designwidget/designwidget.h>

#include <EndpointDesign/EndpointDesignDiagram.h>
#include <EndpointDesign/EndpointDesignWidget.h>

#include <designwidget/DiagramChangeCommands.h>

#include <QVBoxLayout>
#include <QDockWidget>
#include <QHeaderView>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: AdHocEditor()
//-----------------------------------------------------------------------------
AdHocEditor::AdHocEditor(QWidget *parent): QWidget(parent),
                                           component_(0),
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
    layout->addWidget(&portAdHocTable_, 1);
}

//-----------------------------------------------------------------------------
// Function: ~AdHocEditor()
//-----------------------------------------------------------------------------
AdHocEditor::~AdHocEditor()
{
    adHocModel_.setComponent(0);
}

//-----------------------------------------------------------------------------
// Function: AdHocEditor::setComponent()
//-----------------------------------------------------------------------------
void AdHocEditor::setComponent( ComponentItem* component ) {
	Q_ASSERT(component);

	parentWidget()->raise();

	// if previous component has been specified, then disconnect signals to this editor.
	if (component_) {
		component_->disconnect(this);
	}

	component_ = component;

    bool locked = false;

	if (dynamic_cast<BlockDiagram*>(component->scene()) != 0)
    {
	    BlockDiagram* diagram = static_cast<BlockDiagram*>(component->scene());
        locked = diagram->isProtected();
    }
    else
    {
        EndpointDesignDiagram* diagram = static_cast<EndpointDesignDiagram*>(component->scene());
        locked = diagram->isProtected();
    }

    portAdHocTable_.setEnabled(!locked);

    adHocModel_.setComponent(static_cast<DiagramComponent*>(component_));
    portAdHocTable_.resizeRowsToContents();
    portAdHocTable_.show();

    // if the connected component is destroyed then clear this editor
	connect(component_, SIGNAL(destroyed(ComponentItem*)),
		    this, SLOT(clear()), Qt::UniqueConnection);

	parentWidget()->setMaximumHeight(QWIDGETSIZE_MAX);
}

//-----------------------------------------------------------------------------
// Function: AdHocEditor::clear()
//-----------------------------------------------------------------------------
void AdHocEditor::clear() {

	// If a previous component has been specified, disconnect signals to this editor.
	if (component_)
    {
		component_->disconnect(this);
	}

	component_ = 0;
    adHocModel_.setComponent(0);
    portAdHocTable_.hide();

	parentWidget()->setMaximumHeight(20);
}
