//-----------------------------------------------------------------------------
// File: registerfileeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Dan Chianucci
// Date: 19.06.2018
//
// Description:
// Editor for register files.
//-----------------------------------------------------------------------------

#include "registerfileeditor.h"

#include "registerfiledelegate.h"
#include "registerfilemodel.h"
#include "ExpressionProxyModel.h"
#include "AddressBlockColumns.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/RegisterBase.h>

#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: RegisterFileEditor::RegisterFileEditor()
//-----------------------------------------------------------------------------
RegisterFileEditor::RegisterFileEditor(QSharedPointer<QList<QSharedPointer<RegisterBase> > >  registerFile,
    QSharedPointer<Component> component, LibraryInterface *handler,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<RegisterFileValidator> registerFileValidator,
    QWidget *parent) :
    QGroupBox(tr("Register files summary"), parent),
    view_(new EditableTableView(this)),
    model_(0)
{
    view_->verticalHeader()->show();
    view_->verticalHeader()->setMaximumWidth(300);
    view_->verticalHeader()->setMinimumWidth(
        view_->horizontalHeader()->fontMetrics().width(tr("Name")) * 2);
    view_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    QSharedPointer<IPXactSystemVerilogParser> expressionParser(
        new IPXactSystemVerilogParser(parameterFinder));

    model_ =
        new RegisterFileModel(registerFile, expressionParser, parameterFinder,
            expressionFormatter, registerFileValidator, this);

    ComponentParameterModel *componentParametersModel =
        new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser);

    ParameterCompleter *parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(componentParametersModel);

    ExpressionProxyModel *proxy =
        new ExpressionProxyModel(expressionParser, this);
    proxy->setColumnToAcceptExpressions(AddressBlockColumns::REGISTER_OFFSET);
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

    view_->setItemDelegate(
        new RegisterFileDelegate(parameterCompleter, parameterFinder, this));

    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)), this,
        SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)), this,
        SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);

    view_->sortByColumn(AddressBlockColumns::REGISTER_OFFSET, Qt::AscendingOrder);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(view_);

    connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()),
        Qt::UniqueConnection);
    connect(model_, SIGNAL(graphicsChanged()), this, SIGNAL(graphicsChanged()),
        Qt::UniqueConnection);
    connect(model_, SIGNAL(childAddressInfoChanged(int)), this, SIGNAL(childAddressInfoChanged(int)),
        Qt::UniqueConnection);
    connect(model_, SIGNAL(errorMessage(const QString &)), this,
        SIGNAL(errorMessage(const QString &)), Qt::UniqueConnection);
    connect(model_, SIGNAL(itemAdded(int)), this, SIGNAL(childAdded(int)),
        Qt::UniqueConnection);
    connect(model_, SIGNAL(itemRemoved(int)), this, SIGNAL(childRemoved(int)),
        Qt::UniqueConnection);

    connect(view_, SIGNAL(addItem(const QModelIndex &)), model_,
        SLOT(onAddItem(const QModelIndex &)), Qt::UniqueConnection);

    connect(view_, SIGNAL(removeItem(const QModelIndex &)), model_,
        SLOT(onRemoveItem(const QModelIndex &)), Qt::UniqueConnection);

    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)), this,
        SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(decreaseReferences(QString)), this,
        SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(decreaseReferences(QString)), this,
        SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(increaseReferences(QString)), this,
        SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);

    connect(view_, SIGNAL(copyRows(QModelIndexList)), model_,
        SLOT(onCopyRows(QModelIndexList)), Qt::UniqueConnection);
    connect(view_, SIGNAL(pasteRows()), model_, SLOT(onPasteRows()),
        Qt::UniqueConnection);

    connect(this, SIGNAL(addressUnitBitsChanged(int)),
        model_, SLOT(addressUnitBitsChanged(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: RegisterFileEditor::refresh()
//-----------------------------------------------------------------------------
void RegisterFileEditor::refresh()
{
    view_->update();
}
