//-----------------------------------------------------------------------------
// File: otherclockdriverseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 04.04.2011
//
// Description:
// Editor for otherClockDrivers of a component.
//-----------------------------------------------------------------------------

#include "otherclockdriverseditor.h"

#include "clockdriversdelegate.h"

#include <common/widgets/summaryLabel/summarylabel.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: OtherClockDriversEditor::OtherClockDriversEditor()
//-----------------------------------------------------------------------------
OtherClockDriversEditor::OtherClockDriversEditor(QSharedPointer<Component> component, LibraryInterface* handler,
    QSharedPointer<OtherClockDriverValidator> clockValidator, QWidget *parent /* = 0 */):
ItemEditor(component, handler, parent),
view_(this),
model_(component, clockValidator, this)
{
    connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	const QString componentPath = handler->getDirectoryPath(component->getVlnv());
	QString defaultPath = QString("%1/otherClocksListing.csv").arg(componentPath);
	view_.setDefaultImportExportPath(defaultPath);
	view_.setAllowImportExport(true);

	view_.setSortingEnabled(true);
	view_.setItemsDraggable(false);
	view_.setItemDelegate(new ClockDriversDelegate(this));

	// Set proxy to do the sorting automatically.
	proxy_ = new QSortFilterProxyModel(this);	
	proxy_->setSourceModel(&model_);
	view_.setModel(proxy_);

    // Set case-insensitive sorting.
    proxy_->setSortCaseSensitivity(Qt::CaseInsensitive);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Other clock drivers"), this);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	refresh();
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriversEditor::~OtherClockDriversEditor()
//-----------------------------------------------------------------------------
OtherClockDriversEditor::~OtherClockDriversEditor()
{
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriversEditor::refresh()
//-----------------------------------------------------------------------------
void OtherClockDriversEditor::refresh()
{
	view_.update();
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriversEditor::showEvent()
//-----------------------------------------------------------------------------
void OtherClockDriversEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/otherclockdrivers.html");
}
