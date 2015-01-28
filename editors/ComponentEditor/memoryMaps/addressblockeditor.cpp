/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockeditor.cpp
 *		Project: Kactus 2
 */

#include "addressblockeditor.h"

#include "addressblockdelegate.h"
#include "addressblockmodel.h"
#include "addressblockproxy.h"
#include "AddressBlockColumns.h"

#include <common/views/EditableTableView/editabletableview.h>
#include <common/widgets/summaryLabel/summarylabel.h>

#include <editors/ComponentEditor/common/ParameterCompleter.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: AddressBlockEditor::AddressBlockEditor()
//-----------------------------------------------------------------------------
AddressBlockEditor::AddressBlockEditor(QSharedPointer<AddressBlock> addressBlock,
									   QSharedPointer<Component> component,
									   LibraryInterface* handler,
                                       QSharedPointer<ParameterFinder> parameterFinder,
                                       QSharedPointer<ExpressionFormatter> expressionFormatter,
									   QWidget* parent /*= 0*/):
ItemEditor(component, handler, parent),
view_(new EditableTableView(this)),
proxy_(new AddressBlockProxy(this)),
model_(0)
{
    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(component));

    model_ = new AddressBlockModel(addressBlock, component->getChoices(), 
        expressionParser, expressionFormatter, this);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(this, parameterFinder);
    componentParametersModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(componentParametersModel);

	proxy_->setSourceModel(model_);
	view_->setModel(proxy_);

	//! Enable import/export csv file
	const QString compPath = handler->getDirectoryPath(*component->getVlnv());
	QString defPath = QString("%1/registerList.csv").arg(compPath);
	view_->setDefaultImportExportPath(defPath);
	view_->setAllowImportExport(true);

	view_->setItemsDraggable(false);

	view_->setSortingEnabled(true);

	view_->setItemDelegate(new AddressBlockDelegate(parameterCompleter, parameterFinder, this));

	view_->sortByColumn(AddressBlockColumns::REGISTER_OFFSET, Qt::AscendingOrder);

    setupLayout();

    connect(this, SIGNAL(addressUnitBitsChanged(int)), 
        model_, SLOT(addressUnitBitsChanged(int)), Qt::UniqueConnection);

	connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),	
        model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)), 
        model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)), 
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(decreaseReferences(QString)), 
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockEditor::AddressBlockEditor()
//-----------------------------------------------------------------------------
AddressBlockEditor::~AddressBlockEditor()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockEditor::isValid()
//-----------------------------------------------------------------------------
bool AddressBlockEditor::isValid() const
{
	return model_->isValid();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockEditor::refresh()
//-----------------------------------------------------------------------------
void AddressBlockEditor::refresh()
{
	view_->update();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockEditor::showEvent()
//-----------------------------------------------------------------------------
void AddressBlockEditor::showEvent( QShowEvent* event )
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/addressblock.html");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockEditor::sizeHint()
//-----------------------------------------------------------------------------
QSize AddressBlockEditor::sizeHint() const
{
	return QSize(AddressBlockEditor::WIDTH, AddressBlockEditor::HEIGHT);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockEditor::setupLayout()
//-----------------------------------------------------------------------------
void AddressBlockEditor::setupLayout()
{
    // display a label on top the table
    SummaryLabel* summaryLabel = new SummaryLabel(tr("Registers summary"), this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
    layout->addWidget(view_);
    layout->setContentsMargins(0, 0, 0, 0);
}