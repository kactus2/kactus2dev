//-----------------------------------------------------------------------------
// File: ChoicesModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.11.2014
//
// Description:
// Model for editing choices in a component.
//-----------------------------------------------------------------------------

#include "ChoicesModel.h"

#include <IPXACTmodels/common/Choice.h>
#include <IPXACTmodels/common/validators/ChoiceValidator.h>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: ChoicesModel::ChoicesModel()
//-----------------------------------------------------------------------------
ChoicesModel::ChoicesModel(QSharedPointer<QList<QSharedPointer<Choice>>> choices, QSharedPointer<ChoiceValidator> validator, QObject* parent /* = 0 */) :
QAbstractListModel(parent),
choices_(choices),
validator_(validator)
{
    connect(this, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::validate()
//-----------------------------------------------------------------------------
bool ChoicesModel::validate() const
{
    for (auto choice : *choices_)
    {
        if (!validator_->validate(choice))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::rowCount()
//-----------------------------------------------------------------------------
int ChoicesModel::rowCount(const QModelIndex&) const
{
    if (choices_)
    {
        return choices_->size();
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::data()
//-----------------------------------------------------------------------------
QVariant ChoicesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= choices_->size())
    {
        return QVariant();
    }
    
    QSharedPointer<Choice> indexedChoice = choices_->at(index.row());
    
    if (role == Qt::DisplayRole)
    {
        return indexedChoice->name();
    }
    else if (role == Qt::ForegroundRole)
    {
        return validator_->validate(indexedChoice);
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::headerData()
//-----------------------------------------------------------------------------
QVariant ChoicesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section != 0 || orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
        return QVariant();
    }
    else
    {
        return tr("Choices");
    }
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::setData()
//-----------------------------------------------------------------------------
bool ChoicesModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || index.column() != 0 || index.row() < 0 || index.row() >= choices_->size())
    {
        return false;
    }
    
    if (role == Qt::EditRole)
    {
        choices_->at(index.row())->setName(value.toString());
        emit dataChanged(index, index);
        emit contentChanged();
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ChoicesModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }
    
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::addItem()
//-----------------------------------------------------------------------------
void ChoicesModel::addItem(const QModelIndex& index)
{
    int row = choices_->size();

    if (row == 0)
    {
        emit choicesEmpty(false);
    }

    if (index.isValid())
    {
        row = index.row();
    }
    
    beginInsertRows(QModelIndex(), row, row);
    
    QSharedPointer<Choice> newChoice(new Choice());
    newChoice->setName("newChoice");

    choices_->insert(row, newChoice);
    
    endInsertRows();
    
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::remove()
//-----------------------------------------------------------------------------
void ChoicesModel::remove(const QModelIndex& index)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= choices_->size())
    {
        return;
    }
    
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    
    choices_->removeAt(index.row());
    
    endRemoveRows();
    
    emit contentChanged();

    if (choices_->isEmpty())
    {
        emit choicesEmpty(true);
    }
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::moveItem()
//-----------------------------------------------------------------------------
void ChoicesModel::moveItem(const QModelIndex& originalPos, const QModelIndex& newPos)
{
    if (!originalPos.isValid() || originalPos == newPos || originalPos.row() < 0 || originalPos.row() >= choices_->size())
    {
        return;
    }
    
    if (!newPos.isValid() || newPos.row() < 0 || newPos.row() >= choices_->size())
    {
        beginResetModel();
        
        QSharedPointer<Choice> selectedChoice = choices_->takeAt(originalPos.row());
        choices_->append(selectedChoice);
        
        endResetModel();
    }
    else
    {
        beginResetModel();
        
        choices_->swapItemsAt(originalPos.row(), newPos.row());
        
        endResetModel();
    }
    
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::replace()
//-----------------------------------------------------------------------------
void ChoicesModel::replace(QModelIndex& index, const QString newText)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= choices_->size())
    {
        return;
    }

    choices_->at(index.row())->setName(newText);
    
    emit dataChanged(index, index);
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::getChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> ChoicesModel::getChoice(QModelIndex const& index) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= choices_->size())
    {
        return QSharedPointer<Choice>();
    }
    
    return choices_->at(index.row());
}
