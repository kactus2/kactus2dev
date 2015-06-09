/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: registereditor.cpp
 *		Project: Kactus 2
 */

#include "registereditor.h"

#include "ExpressionProxyModel.h"
#include "registertablemodel.h"
#include "registerdelegate.h"
#include "RegisterColumns.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: registereditor::RegisterEditor()
//-----------------------------------------------------------------------------
RegisterEditor::RegisterEditor(QSharedPointer<Register> reg, 
							   QSharedPointer<Component> component,
							   LibraryInterface* handler,
                               QSharedPointer<ParameterFinder> parameterFinder,
                               QSharedPointer<ExpressionFormatter> expressionFormatter,
							   QWidget* parent /*= 0*/ ):
QGroupBox(tr("Fields summary"), parent),
view_(new EditableTableView(this)),
model_(0)
{
    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));

    model_ = new RegisterTableModel(reg, component->getChoices(), expressionParser, parameterFinder,
        expressionFormatter, this);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(componentParametersModel);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(view_);

    ExpressionProxyModel* proxy = new ExpressionProxyModel(expressionParser, this);
    proxy->setColumnToAcceptExpressions(RegisterColumns::OFFSET_COLUMN);
    proxy->setColumnToAcceptExpressions(RegisterColumns::WIDTH_COLUMN);
    proxy->setColumnToAcceptExpressions(RegisterColumns::IS_PRESENT_COLUMN);

	proxy->setSourceModel(model_);
	view_->setModel(proxy);

	//! \brief Enable import/export csv file
    const QString compPath = handler->getDirectoryPath(*component->getVlnv());
	QString defPath = QString("%1/fieldListing.csv").arg(compPath);
    view_->setDefaultImportExportPath(defPath);
	
    view_->setAllowImportExport(true);
	view_->setItemsDraggable(false);
	view_->setSortingEnabled(true);

    view_->setItemDelegate(new RegisterDelegate(parameterCompleter, parameterFinder, this));

    view_->sortByColumn(RegisterColumns::OFFSET_COLUMN, Qt::AscendingOrder);

    connect(model_, SIGNAL(graphicsChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(fieldAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(model_, SIGNAL(fieldRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),
		model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    connect(model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: registereditor::~RegisterEditor()
//-----------------------------------------------------------------------------
RegisterEditor::~RegisterEditor()
{

}

//-----------------------------------------------------------------------------
// Function: registereditor::isValid()
//-----------------------------------------------------------------------------
bool RegisterEditor::isValid() const
{
	return model_->isValid();
}

//-----------------------------------------------------------------------------
// Function: registereditor::refresh()
//-----------------------------------------------------------------------------
void RegisterEditor::refresh()
{
	view_->update();
}
