//-----------------------------------------------------------------------------
// File: parameterseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 04.04.2011
//
// Description:
// The editor to add/remove/edit parameters.
//-----------------------------------------------------------------------------

#include "parameterseditor.h"

#include "ParameterColumns.h"
#include "ParameterDelegate.h"
#include "ParameterEditorHeaderView.h"

#include <common/widgets/summaryLabel/summarylabel.h>

#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/parameters/ParametersView.h>
#include <KactusAPI/include/ParametersInterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <QCompleter>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ParametersEditor::ParametersEditor()
//-----------------------------------------------------------------------------
ParametersEditor::ParametersEditor(QSharedPointer<Component> component, LibraryInterface* handler,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter,
    ParametersInterface* parameterInterface, QWidget *parent):
ParameterItemEditor(component, handler, parent),
view_(new ParametersView(this)),
model_(0),
parameterInterface_(parameterInterface)
{
    view_->verticalHeader()->show();
    view_->verticalHeader()->setMaximumWidth(300);
    view_->verticalHeader()->setMinimumWidth(view_->horizontalHeader()->fontMetrics().horizontalAdvance(tr("Name"))*2);
    view_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    model_ = new ParametersModel(parameterInterface, expressionParser, parameterFinder, this);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser);


	connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    connect(view_, SIGNAL(addItem(const QModelIndex&)),
        model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(view_, SIGNAL(removeItem(const QModelIndex&)),
        model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	const QString compPath = ItemEditor::handler()->getDirectoryPath(ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/parameterList.csv").arg(compPath);

    ParameterEditorHeaderView* parameterHorizontalHeader = new ParameterEditorHeaderView(Qt::Horizontal, this);
    view_->setHorizontalHeader(parameterHorizontalHeader);

    view_->horizontalHeader()->setSectionsClickable(true);
    view_->horizontalHeader()->setStretchLastSection(true);

    view_->setDefaultImportExportPath(defPath);
    view_->setAllowImportExport(true);

    view_->setSortingEnabled(true);
    view_->setItemsDraggable(false);

    view_->setItemDelegate(new ParameterDelegate(component->getChoices(), componentParametersModel, parameterFinder,
        expressionFormatter, this));

    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(view_->itemDelegate(), SIGNAL(openReferenceTree(QString const&, QString const&)),
        this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

	// set proxy to do the sorting automatically
	QSortFilterProxyModel* sortingProxy = new QSortFilterProxyModel(this);

	// set source model for proxy
	sortingProxy->setSourceModel(model_);
	// set proxy to be the source for the view
    view_->setModel(sortingProxy);

    view_->resizeColumnsToContents();

    sortingProxy->setSortCaseSensitivity(Qt::CaseInsensitive);

    view_->sortByColumn(0, Qt::AscendingOrder);

    // display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Parameters"), this);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
    layout->addWidget(view_);
	layout->setContentsMargins(0, 0, 0, 0);

    connect(view_, SIGNAL(recalculateReferenceToIndexes(QModelIndexList)),
        model_, SLOT(onGetParametersMachingIndexes(QModelIndexList)), Qt::UniqueConnection);
    connect(model_,
        SIGNAL(recalculateReferencesToParameters(QVector<QString> const&, AbstractParameterInterface*)),
        this,
        SIGNAL(recalculateReferencesToParameters(QVector<QString> const&, AbstractParameterInterface*)),
        Qt::UniqueConnection);

    parameterInterface_->setParameters(component->getParameters());

	refresh();
}

//-----------------------------------------------------------------------------
// Function: ParametersEditor::refresh()
//-----------------------------------------------------------------------------
void ParametersEditor::refresh()
{
    view_->update();

    if (!view_->isColumnHidden(ParameterColumns::ID))
    {
        view_->setColumnHidden(ParameterColumns::ID, true);
    }
}

//-----------------------------------------------------------------------------
// Function: ParametersEditor::showEvent()
//-----------------------------------------------------------------------------
void ParametersEditor::showEvent( QShowEvent* event )
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/params.html");
}
