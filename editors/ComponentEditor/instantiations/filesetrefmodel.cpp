//-----------------------------------------------------------------------------
// File: filesetrefmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 07.02.2012
//
// Description:
// Model class to manage a list of file set references.
//-----------------------------------------------------------------------------

#include "filesetrefmodel.h"

#include <IPXACTmodels/Component/Component.h>

#include <common/KactusColors.h>

#include <editors/ComponentEditor/fileSet/interfaces/FileSetInterface.h>

//-----------------------------------------------------------------------------
// Function: filesetrefmodel::FileSetRefModel()
//-----------------------------------------------------------------------------
FileSetRefModel::FileSetRefModel(QObject *parent, FileSetInterface* fileSetInterface, const QStringList& items):
ListManagerModel(items, parent),
fileSetInterface_(fileSetInterface)
{

}

//-----------------------------------------------------------------------------
// Function: filesetrefmodel::~FileSetRefModel()
//-----------------------------------------------------------------------------
FileSetRefModel::~FileSetRefModel()
{

}

//-----------------------------------------------------------------------------
// Function: filesetrefmodel::data()
//-----------------------------------------------------------------------------
QVariant FileSetRefModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const
{
    // nothing for invalid indexes
    if (!index.isValid())
    {
        return QVariant();
    }

    // if there are no real items then display the help text
    else if (index.row() == 0 && items_.isEmpty())
    {
        if (role == Qt::DisplayRole)
        {
            return tr("Double click to add new item.");
        }
        else if (role == Qt::ForegroundRole)
        {
            return KactusColors::DISABLED_TEXT;
        }
        else
        {
            return QVariant();
        }
    }

    // if index.row() is invalid
    else if (index.row() < 0 || index.row() >= items_.size())
    {
        return QVariant();
    }

    // return data for display role
    if (role == Qt::DisplayRole)
    {
        return items_.at(index.row());
    }
    else if (role == Qt::ForegroundRole)
    {
        // if component contains the named file set.
        if (fileSetInterface_->fileSetExists(items_.at(index.row()).toStdString()))
        {
            return KactusColors::REGULAR_TEXT;
        }

        // if the file set does not exist.
        else
        {
            return KactusColors::ERROR;
        }
    }
    // if unsupported role
    else
    {
        return QVariant();
    }
}
