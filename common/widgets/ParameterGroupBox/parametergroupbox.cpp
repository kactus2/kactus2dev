//-----------------------------------------------------------------------------
// File: parametergroupbox.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 23.2.2012
//
// Description:
// Group box that can be used to edit list of parameters.
//-----------------------------------------------------------------------------

#include "parametergroupbox.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/parameters/ParameterColumns.h>
#include <editors/ComponentEditor/parameters/ParameterDelegate.h>
#include <editors/ComponentEditor/parameters/ParameterEditorHeaderView.h>
#include <editors/ComponentEditor/parameters/parametersmodel.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/ListParameterFinder.h>

#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <QVBoxLayout>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: ParameterGroupBox::ParameterGroupBox()
//-----------------------------------------------------------------------------
ParameterGroupBox::ParameterGroupBox(QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
                                     QSharedPointer<QList<QSharedPointer<Choice> > > choices,
                                     QSharedPointer<ParameterFinder> parameterFinder,
                                     QSharedPointer<ExpressionFormatter> expressionFormatter,
									 QWidget *parent):
QGroupBox(tr("Parameters"), parent),
view_(new EditableTableView(this)),
proxy_(new QSortFilterProxyModel(this)),
model_(0),
parameterFinder_(parameterFinder)
{
    ParameterEditorHeaderView* parameterHorizontalHeader = new ParameterEditorHeaderView(Qt::Horizontal, this);
    view_->setHorizontalHeader(parameterHorizontalHeader);

    view_->verticalHeader()->show();
    view_->verticalHeader()->setMaximumWidth(300);
    view_->verticalHeader()->setMinimumWidth(view_->horizontalHeader()->fontMetrics().width(tr("Name"))*2);
    view_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));

    QSharedPointer<ParameterValidator2014> validator(new ParameterValidator2014(expressionParser, 
        choices));

    model_ = new ParametersModel(parameters, choices, validator, expressionParser, parameterFinder, 
        expressionFormatter, this);

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

	// set view to be sortable
	view_->setSortingEnabled(true);

	// items can not be dragged
	view_->setItemsDraggable(false);

    ComponentParameterModel* parameterModel = new ComponentParameterModel(parameterFinder, this);
    parameterModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(parameterModel);

    view_->setItemDelegate(new ParameterDelegate(choices, parameterCompleter, parameterFinder,
        expressionFormatter, this));

    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)), 
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(view_->itemDelegate(), SIGNAL(openReferenceTree(QString const&, QString const&)),
        this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

	// set source model for proxy
	proxy_->setSourceModel(model_);
	// set proxy to be the source for the view
	view_->setModel(proxy_);

	// sort the view
	view_->sortByColumn(0, Qt::AscendingOrder);

    view_->setColumnHidden(ParameterColumns::ID, true);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(view_);
}

//-----------------------------------------------------------------------------
// Function: ParameterGroupBox::~ParameterGroupBox()
//-----------------------------------------------------------------------------
ParameterGroupBox::~ParameterGroupBox()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterGroupBox::refresh()
//-----------------------------------------------------------------------------
void ParameterGroupBox::refresh() 
{
    proxy_->invalidate();
	view_->update();
}

//-----------------------------------------------------------------------------
// Function: parametergroupbox::setNewParameters()
//-----------------------------------------------------------------------------
void ParameterGroupBox::setNewParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters)
{
    model_->setNewParameters(newParameters);
    
    QSharedPointer<ListParameterFinder> listFinder = parameterFinder_.dynamicCast<ListParameterFinder>();
    if (listFinder)
    {
        listFinder->setParameterList(newParameters);
    }
}
