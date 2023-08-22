//-----------------------------------------------------------------------------
// File: ResetsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 11.01.2019
//
// Description:
// Editor for editing the resets of a field.
//-----------------------------------------------------------------------------

#include "ResetsEditor.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/memoryMaps/ResetsModel.h>
#include <editors/ComponentEditor/memoryMaps/ResetsDelegate.h>
#include <KactusAPI/include/ResetInterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ResetsEditor::ResetsEditor()
//-----------------------------------------------------------------------------
ResetsEditor::ResetsEditor(ResetInterface* resetInterface,
    QSharedPointer<QList<QSharedPointer<ResetType>>> resetTypes, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<Field> containingField, QWidget* parent):
QGroupBox(tr("Resets"), parent),
resetsView_(new EditableTableView(this)),
resetsProxy_(new QSortFilterProxyModel(this)),
resetsModel_(new ResetsModel(resetInterface, expressionParser, parameterFinder, this)),
containingField_(containingField),
interface_(resetInterface)
{
    interface_->setResets(containingField_);

	QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(resetsView_, 0);

    resetsProxy_->setSourceModel(resetsModel_);
	resetsView_->setModel(resetsProxy_);

	resetsView_->setItemsDraggable(false);
	resetsView_->setSortingEnabled(true);

    ComponentParameterModel* parametersModel = new ComponentParameterModel(parameterFinder, this);
    parametersModel->setExpressionParser(expressionParser);

    resetsView_->setItemDelegate(new ResetsDelegate(parametersModel, parameterFinder, resetTypes, this));

	connect(resetsModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(resetsView_, SIGNAL(addItem(const QModelIndex&)),
		resetsModel_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(resetsView_, SIGNAL(removeItem(const QModelIndex&)),
        resetsModel_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    connect(resetsView_->itemDelegate(), SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(resetsView_->itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(resetsModel_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(resetsModel_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ResetsEditor::~ResetsEditor()
//-----------------------------------------------------------------------------
ResetsEditor::~ResetsEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ResetsEditor::refresh()
//-----------------------------------------------------------------------------
void ResetsEditor::refresh()
{
	resetsView_->update();

    interface_->setResets(containingField_);
}
