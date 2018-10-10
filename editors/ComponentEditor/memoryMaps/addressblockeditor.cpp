//-----------------------------------------------------------------------------
// File: addressblockeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 24.08.2012
//
// Description:
// Editor for editing the details of registers in an address block.
//-----------------------------------------------------------------------------

#include "addressblockeditor.h"

#include "addressblockdelegate.h"
#include "addressblockmodel.h"
#include "ExpressionProxyModel.h"
#include "AddressBlockColumns.h"
#include "RegisterDataTableView.h"

#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <library/LibraryInterface.h>

#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: AddressBlockEditor::AddressBlockEditor()
//-----------------------------------------------------------------------------
AddressBlockEditor::AddressBlockEditor(QSharedPointer<AddressBlock> addressBlock,
    QSharedPointer<Component> component, LibraryInterface* handler,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<RegisterFileValidator> registerFileValidator, QWidget* parent):
QGroupBox(tr("Registers summary"), parent),
    view_(new RegisterDataTableView(this)),
    model_(0)
{
    view_->verticalHeader()->show();
    view_->verticalHeader()->setMaximumWidth(300);
    view_->verticalHeader()->setMinimumWidth(view_->horizontalHeader()->fontMetrics().width(tr("Name"))*2);
    view_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));

    model_ = new AddressBlockModel(addressBlock, expressionParser, parameterFinder, expressionFormatter,
        registerFileValidator, this);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(componentParametersModel);

    ExpressionProxyModel* proxy = new ExpressionProxyModel(expressionParser, this);

    proxy->setColumnToAcceptExpressions(AddressBlockColumns::REGISTER_OFFSET);
    proxy->setColumnToAcceptExpressions(AddressBlockColumns::REGISTERFILE_RANGE);
    proxy->setColumnToAcceptExpressions(AddressBlockColumns::REGISTER_SIZE);
    proxy->setColumnToAcceptExpressions(AddressBlockColumns::REGISTER_DIMENSION);
    proxy->setColumnToAcceptExpressions(AddressBlockColumns::IS_PRESENT);

    proxy->setSourceModel(model_);
	view_->setModel(proxy);

	//! Enable import/export csv file
	const QString compPath = handler->getDirectoryPath(component->getVlnv());
	QString defPath = QString("%1/registerList.csv").arg(compPath);
	view_->setDefaultImportExportPath(defPath);
	view_->setAllowImportExport(true);
    view_->setAllowElementCopying(true);
	view_->setItemsDraggable(false);
	view_->setSortingEnabled(true);

    view_->setItemDelegate(new AddressBlockDelegate(parameterCompleter, parameterFinder, this));

    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);

	view_->sortByColumn(AddressBlockColumns::REGISTER_OFFSET, Qt::AscendingOrder);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(view_);

    connect(this, SIGNAL(addressUnitBitsChanged(int)),
        model_, SLOT(addressUnitBitsChanged(int)), Qt::UniqueConnection);

	connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(graphicsChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

    connect(view_, SIGNAL(addRegister(const QModelIndex&)),
        model_, SLOT(onAddRegister(const QModelIndex&)), Qt::UniqueConnection);
    connect(view_, SIGNAL(addRegisterFile(const QModelIndex&)),
        model_, SLOT(onAddRegisterFile(const QModelIndex&)), Qt::UniqueConnection);

	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
        model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);

    connect(view_, SIGNAL(copyRows(QModelIndexList)),
        model_, SLOT(onCopyRows(QModelIndexList)), Qt::UniqueConnection);
    connect(view_, SIGNAL(pasteRows()), model_, SLOT(onPasteRows()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockEditor::AddressBlockEditor()
//-----------------------------------------------------------------------------
AddressBlockEditor::~AddressBlockEditor()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockEditor::refresh()
//-----------------------------------------------------------------------------
void AddressBlockEditor::refresh()
{
	view_->update();
}
