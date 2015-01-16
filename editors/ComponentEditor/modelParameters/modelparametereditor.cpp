/* 
 *
 *  Created on: 29.3.2011
 *      Author: Antti Kamppi
 * 		filename: modelparametereditor.cpp
 */

#include "modelparametereditor.h"

#include "ModelParameterDelegate.h"
#include "ModelParameterColumns.h"
#include "ModelParameterEditorHeaderView.h"

#include <IPXACTmodels/component.h>

#include <common/widgets/summaryLabel/summarylabel.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ParameterResolver.h>
#include <editors/ComponentEditor/common/ParameterResolver.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>


#include <library/LibraryManager/libraryinterface.h>

#include <QHeaderView>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ModelParameterEditor()
//-----------------------------------------------------------------------------
ModelParameterEditor::ModelParameterEditor(QSharedPointer<Component> component,
	LibraryInterface* handler, 
	QWidget *parent): 
ItemEditor(component, handler, parent), 
view_(this),
model_(0),
proxy_(this)
{
    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(component));

    model_ = new ModelParameterModel(component->getModel(), component->getChoices(), 
        expressionParser, this);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(component, this);
    componentParametersModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(componentParametersModel);

	connect(model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SLOT(modelDataChanged(const QModelIndex&)), Qt::UniqueConnection);
	connect(model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/modelParamList.csv").arg(compPath);
	
    ModelParameterEditorHeaderView* modelParameterHorizontalHeader = 
        new ModelParameterEditorHeaderView(Qt::Horizontal, this);
    view_.setHorizontalHeader(modelParameterHorizontalHeader);
    view_.horizontalHeader()->setSectionsClickable(true);
    view_.horizontalHeader()->setStretchLastSection(true);

    view_.setDefaultImportExportPath(defPath);
    view_.setAllowImportExport(true);

	// set view to be sortable
	view_.setSortingEnabled(true);

	// set the delegate to edit the usage column
	view_.setItemDelegate(new ModelParameterDelegate(component->getChoices(), parameterCompleter,
        QSharedPointer<ParameterResolver>(new ParameterResolver(component)), this));

	// items can not be dragged
	view_.setItemsDraggable(false);

	// set source model for proxy
	proxy_.setSourceModel(model_);
	// set proxy to be the source for the view
	view_.setModel(&proxy_);

    // Set case-insensitive sorting.
    proxy_.setSortCaseSensitivity(Qt::CaseInsensitive);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);;

    view_.setColumnHidden(ModelParameterColumns::VALUEID, true);

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Model parameters"), this);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	refresh();
}

//-----------------------------------------------------------------------------
// Function: ~ModelParameterEditor()
//-----------------------------------------------------------------------------
ModelParameterEditor::~ModelParameterEditor() 
{

}

//-----------------------------------------------------------------------------
// Function: isValid()
//-----------------------------------------------------------------------------
bool ModelParameterEditor::isValid() const
{
	return model_->isValid();
}

//-----------------------------------------------------------------------------
// Function: refresh()
//-----------------------------------------------------------------------------
void ModelParameterEditor::refresh() 
{
	view_.update();
}

//-----------------------------------------------------------------------------
// Function: ModelParameterEditor::setComponent()
//-----------------------------------------------------------------------------
void ModelParameterEditor::setComponent(QSharedPointer<Component> component)
{
    model_->setModelAndLockCurrentModelParameters(component->getModel());
}

//-----------------------------------------------------------------------------
// Function: showEvent()
//-----------------------------------------------------------------------------
void ModelParameterEditor::showEvent( QShowEvent* event )
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/modelparams.html");
}

//-----------------------------------------------------------------------------
// Function: setAllowImportExport()
//-----------------------------------------------------------------------------
void ModelParameterEditor::setAllowImportExport( bool allow )
{
	view_.setAllowImportExport(allow);
}

//-----------------------------------------------------------------------------
// Function: modelDataChanged()
//-----------------------------------------------------------------------------
void ModelParameterEditor::modelDataChanged(QModelIndex const& index)
{
    // Only changes in the default value emits parameterChanged.
    if ( index.column() == ModelParameterColumns::VALUE )
    {
        QSharedPointer<ModelParameter> changedParameter = model_->getParameter(index);
        emit parameterChanged(changedParameter);
    }
    emit contentChanged();
}
