//-----------------------------------------------------------------------------
// File: filebuilderseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 31.05.2012
//
// Description:
// Editor to edit file builders.
//-----------------------------------------------------------------------------

#include "filebuilderseditor.h"
#include "filebuildersdelegate.h"

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <IPXACTmodels/common/FileBuilder.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: FileBuildersEditor::FileBuildersEditor()
//-----------------------------------------------------------------------------
FileBuildersEditor::FileBuildersEditor(QSharedPointer<QList<QSharedPointer<FileBuilder> > > fileBuilders,
                                       QSharedPointer<ParameterFinder> parameterFinder,
                                       QSharedPointer<ExpressionParser> expressionParser,
                                       QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget* parent):
QGroupBox(tr("Default file build commands"), parent),
view_(this), 
model_(0),
proxy_(this)
{
    model_ = new FileBuildersModel(fileBuilders, parameterFinder, expressionFormatter, expressionParser, this);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(componentParametersModel);

	// set view to be sortable
	view_.setSortingEnabled(true);

	// items can not be dragged
	view_.setItemsDraggable(false);

	// set the delegate to provide editors
    view_.setItemDelegate(new FileBuildersDelegate(parameterCompleter, parameterFinder, this));

	// set source model for proxy
    proxy_.setSourceModel(model_);
	// set proxy to be the source for the view
	view_.setModel(&proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);

    connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(model_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
        model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    connect(view_.itemDelegate(), SIGNAL(increaseReferences(QString)), 
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_.itemDelegate(), SIGNAL(decreaseReferences(QString)), 
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileBuildersEditor::~FileBuildersEditor()
//-----------------------------------------------------------------------------
FileBuildersEditor::~FileBuildersEditor()
{

}

//-----------------------------------------------------------------------------
// Function: FileBuildersEditor::isValid()
//-----------------------------------------------------------------------------
bool FileBuildersEditor::isValid() const
{
    return model_->isValid();
}

//-----------------------------------------------------------------------------
// Function: FileBuildersEditor::refresh()
//-----------------------------------------------------------------------------
void FileBuildersEditor::refresh()
{
	view_.update();
}
