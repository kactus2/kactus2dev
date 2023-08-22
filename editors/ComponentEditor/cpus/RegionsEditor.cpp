//-----------------------------------------------------------------------------
// File: RegionsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.6.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#include "RegionsEditor.h"

#include "RegionColumns.h"
#include "RegionsDelegate.h"

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/memoryMaps/ExpressionProxyModel.h>
#include <editors/common/ExpressionSet.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Region.h>

#include <QCompleter>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: RegionsEditor::RegionsEditor()
//-----------------------------------------------------------------------------
RegionsEditor::RegionsEditor(QSharedPointer<QList<QSharedPointer<Region> > > regions,
    QSharedPointer<RegionValidator> regionValidator,
    QString const& componentPath, ExpressionSet expressions, QWidget* parent) :
QGroupBox(tr("Regions"), parent),
view_(this),
model_(regions, regionValidator, expressions, this)
{
	setFlat(true);

    model_.setExpressionParser(expressions.parser);

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&model_, SIGNAL(regionAdded(QSharedPointer<Region>)),
		this, SIGNAL(regionAdded(QSharedPointer<Region>)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(regionRemoved(const QString&)),
		this, SIGNAL(regionRemoved(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(regionRenamed(const QString&, const QString&)),
		this, SIGNAL(regionRenamed(const QString&, const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(regionChanged(QSharedPointer<Region>)),
		this, SIGNAL(regionChanged(QSharedPointer<Region>)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	QString defaultPath = QString("%1/regionsList.csv").arg(componentPath);
	view_.setDefaultImportExportPath(defaultPath);
	view_.setAllowImportExport(true);

	view_.setSortingEnabled(true);
	view_.setItemsDraggable(false);
    view_.setAllowElementCopying(true);

    ComponentParameterModel* completionModel = new ComponentParameterModel(expressions.finder, this);
    completionModel->setExpressionParser(expressions.parser);

	auto delegate = new RegionsDelegate(completionModel, expressions.finder, this);
	view_.setItemDelegate(delegate);

    connect(delegate, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(delegate, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(&model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    ExpressionProxyModel* proxy = new ExpressionProxyModel(expressions.parser, this);
    proxy->setColumnToAcceptExpressions(RegionColumns::OFFSET);
    proxy->setColumnToAcceptExpressions(RegionColumns::RANGE);

	proxy->setSourceModel(&model_);
	view_.setModel(proxy);

	view_.sortByColumn(1, Qt::AscendingOrder);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);

    connect(view_.itemDelegate(), SIGNAL(increaseReferences(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(view_.itemDelegate(), SIGNAL(decreaseReferences(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);

    connect(&model_, SIGNAL(decreaseReferences(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(&model_, SIGNAL(increaseReferences(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(copyRows(QModelIndexList)),
        &model_, SLOT(onCopyRows(QModelIndexList)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(pasteRows()), &model_, SLOT(onPasteRows()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: RegionsEditor::refresh()
//-----------------------------------------------------------------------------
void RegionsEditor::refresh()
{
	view_.update();
}
