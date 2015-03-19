/* 
 *
 *  Created on: 4.4.2011
 *      Author: Antti Kamppi
 * 		filename: parameterseditor.cpp
 */

#include "parameterseditor.h"

#include "ParameterColumns.h"
#include "ParameterDelegate.h"
#include "ParameterEditorHeaderView.h"

#include <common/widgets/summaryLabel/summarylabel.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <IPXACTmodels/component.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ParametersEditor::ParametersEditor()
//-----------------------------------------------------------------------------
ParametersEditor::ParametersEditor(QSharedPointer<Component> component, LibraryInterface* handler,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QWidget* parent): 
ItemEditor(component, handler, parent),
view_(1, QSharedPointer<EditableTableView> (new EditableTableView(this)), this),
model_(0), 
proxy_(0)
{
    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));

    model_ = new ParametersModel(component->getParameters(), component->getChoices(), expressionParser,
        parameterFinder, expressionFormatter, this);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(componentParametersModel);

	connect(model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
        model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/parameterList.csv").arg(compPath);

    ParameterEditorHeaderView* parameterHorizontalHeader = new ParameterEditorHeaderView(Qt::Horizontal, this);
    view_.setHorizontalHeader(parameterHorizontalHeader);

    view_.horizontalHeader()->setSectionsClickable(true);
    view_.horizontalHeader()->setStretchLastSection(true);

    view_.setDefaultImportExportPath(defPath);
    view_.setAllowImportExport(true);

    view_.setSortingEnabled(true);
    view_.setItemsDraggable(false);

    view_.setDelegate(new ParameterDelegate(component->getChoices(), parameterCompleter, parameterFinder,
        expressionFormatter, this));

    connect(view_.itemDelegate(), SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_.itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(view_.itemDelegate(), SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

	// set proxy to do the sorting automatically
	proxy_ = new QSortFilterProxyModel(this);

	// set source model for proxy
	proxy_->setSourceModel(model_);
	// set proxy to be the source for the view
    view_.setModel(proxy_);

    proxy_->setSortCaseSensitivity(Qt::CaseInsensitive);

    view_.sortByColumn(0, Qt::AscendingOrder);
    view_.setColumnHidden(ParameterColumns::VALUEID, true);

    // display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Parameters"), this);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
    layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	refresh();
}

//-----------------------------------------------------------------------------
// Function: ParametersEditor::~ParametersEditor()
//-----------------------------------------------------------------------------
ParametersEditor::~ParametersEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ParametersEditor::isValid()
//-----------------------------------------------------------------------------
bool ParametersEditor::isValid() const
{
	return model_->isValid();
}

//-----------------------------------------------------------------------------
// Function: ParametersEditor::refresh()
//-----------------------------------------------------------------------------
void ParametersEditor::refresh()
{
    view_.update();
}

//-----------------------------------------------------------------------------
// Function: ParametersEditor::showEvent()
//-----------------------------------------------------------------------------
void ParametersEditor::showEvent( QShowEvent* event )
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/params.html");
}
