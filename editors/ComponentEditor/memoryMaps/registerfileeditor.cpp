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
#include "RegisterFileColumns.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/RegisterBase.h>

#include <QCompleter>
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
        view_->horizontalHeader()->fontMetrics().horizontalAdvance(tr("Name")) * 2);
    view_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    QSharedPointer<IPXactSystemVerilogParser> expressionParser(
        new IPXactSystemVerilogParser(parameterFinder));

    model_ = new RegisterFileModel(registerFile, expressionParser, parameterFinder,
            expressionFormatter, registerFileValidator, this);

    ComponentParameterModel *componentParametersModel =
        new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser);


    ExpressionProxyModel *proxy =
        new ExpressionProxyModel(expressionParser, this);
    proxy->setColumnToAcceptExpressions(RegisterFileColumns::ADDRESS_OFFSET);
    proxy->setColumnToAcceptExpressions(RegisterFileColumns::RANGE);
    proxy->setColumnToAcceptExpressions(RegisterFileColumns::DIMENSION);
    proxy->setColumnToAcceptExpressions(RegisterFileColumns::IS_PRESENT);

    proxy->setSourceModel(model_);
    view_->setModel(proxy);

    if (component->getRevision() == Document::Revision::Std22)
    {
        view_->hideColumn(RegisterFileColumns::IS_PRESENT);
    }

    //! Enable import/export csv file
    const QString compPath = handler->getDirectoryPath(component->getVlnv());
    QString defPath = QString("%1/registerList.csv").arg(compPath);
    view_->setDefaultImportExportPath(defPath);
    view_->setAllowImportExport(true);
    view_->setAllowElementCopying(true);
    view_->setItemsDraggable(false);
    view_->setSortingEnabled(true);

    view_->setItemDelegate(
        new RegisterFileDelegate(componentParametersModel, parameterFinder, this));

    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)), this,
        SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)), this,
        SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);

    view_->sortByColumn(RegisterFileColumns::ADDRESS_OFFSET, Qt::AscendingOrder);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(view_);

    connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()),
        Qt::UniqueConnection);
    connect(model_, SIGNAL(graphicsChanged(int)), this, SIGNAL(graphicsChanged(int)),
        Qt::UniqueConnection);
    connect(model_, SIGNAL(childAddressingChanged(int)), this, SIGNAL(childAddressingChanged(int)),
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
