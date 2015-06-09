/* 
 *  	Created on: 22.2.2012
 *      Author: Antti Kamppi
 * 		filename: segmenteditor.cpp
 *		Project: Kactus 2
 */

#include "segmenteditor.h"

#include "SegmentDelegate.h"

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/memoryMaps/ExpressionProxyModel.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: SegmentEditor::SegmentEditor()
//-----------------------------------------------------------------------------
SegmentEditor::SegmentEditor(QSharedPointer<AddressSpace> addrSpace, 
	QSharedPointer<Component> component,
    QString const& componentPath,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
							 QWidget *parent ):
QGroupBox(tr("Segments"), parent),
view_(this),
model_(addrSpace, parameterFinder, expressionFormatter, this),
component_(component)
{
    model_.setExpressionParser(expressionParser);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&model_, SIGNAL(segmentAdded(QSharedPointer<Segment>)),
		this, SIGNAL(segmentAdded(QSharedPointer<Segment>)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(segmentRemoved(const QString&)),
		this, SIGNAL(segmentRemoved(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(segmentRenamed(const QString&, const QString&)),
		this, SIGNAL(segmentRenamed(const QString&, const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(segmentChanged(QSharedPointer<Segment>)),
		this, SIGNAL(segmentChanged(QSharedPointer<Segment>)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	QString defPath = QString("%1/segmentList.csv").arg(componentPath);
	view_.setDefaultImportExportPath(defPath);
	view_.setAllowImportExport(true);

	view_.setSortingEnabled(true);
	view_.setItemsDraggable(false);

    ComponentParameterModel* completionModel = new ComponentParameterModel(parameterFinder, this);
    completionModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(completionModel);

	view_.setItemDelegate(new SegmentDelegate(parameterCompleter, parameterFinder, this));

    ExpressionProxyModel* proxy = new ExpressionProxyModel(expressionParser, this);
    proxy->setColumnToAcceptExpressions(1);
    proxy->setColumnToAcceptExpressions(2);

	proxy->setSourceModel(&model_);
	view_.setModel(proxy);

	view_.sortByColumn(1, Qt::AscendingOrder);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);
}

//-----------------------------------------------------------------------------
// Function: SegmentEditor::~SegmentEditor()
//-----------------------------------------------------------------------------
SegmentEditor::~SegmentEditor()
{
}

//-----------------------------------------------------------------------------
// Function: SegmentEditor::isValid()
//-----------------------------------------------------------------------------
bool SegmentEditor::isValid() const
{
	return model_.isValid();
}

//-----------------------------------------------------------------------------
// Function: SegmentEditor::refresh()
//-----------------------------------------------------------------------------
void SegmentEditor::refresh()
{
	view_.update();
	view_.sortByColumn(1, Qt::AscendingOrder);
}
