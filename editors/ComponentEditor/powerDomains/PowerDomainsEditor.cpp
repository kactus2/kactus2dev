//-----------------------------------------------------------------------------
// File: PowerDomainsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 5.10.2023
//
// Description:
// Editor for power domains of a component.
//-----------------------------------------------------------------------------

#include "PowerDomainsEditor.h"

#include "PowerDomainsDelegate.h"

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <common/widgets/summaryLabel/summarylabel.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: PowerDomainsEditor::PowerDomainsEditor()
//-----------------------------------------------------------------------------
PowerDomainsEditor::PowerDomainsEditor(QSharedPointer<Component> component, LibraryInterface* handler,
    //QSharedPointer<OtherClockDriverValidator> clockValidator, 
	ExpressionSet expressions,
	QWidget *parent):
ItemEditor(component, handler, parent),
view_(this),
model_(component, expressions, this)
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
	QString defaultPath = QString("%1/powerDomainsListing.csv").arg(componentPath);
	view_.setDefaultImportExportPath(defaultPath);
	view_.setAllowImportExport(true);


    auto completionModel = new ComponentParameterModel(expressions.finder, this);
    completionModel->setExpressionParser(expressions.parser);

	view_.setSortingEnabled(true);
	view_.setItemsDraggable(false);
	view_.setItemDelegate(new PowerDomainsDelegate(component->getPowerDomains(), completionModel,
		expressions.finder, this));

	// Set proxy to do the sorting automatically.
	proxy_ = new QSortFilterProxyModel(this);	
	proxy_->setSourceModel(&model_);
	view_.setModel(proxy_);

    // Set case-insensitive sorting.
    proxy_->setSortCaseSensitivity(Qt::CaseInsensitive);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Power domains"), this);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	PowerDomainsEditor::refresh();
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsEditor::refresh()
//-----------------------------------------------------------------------------
void PowerDomainsEditor::refresh()
{
	view_.update();
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsEditor::showEvent()
//-----------------------------------------------------------------------------
void PowerDomainsEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/powerdomains.html");
}
