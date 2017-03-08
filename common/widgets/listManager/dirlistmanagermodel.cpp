//-----------------------------------------------------------------------------
// File: dirlistmanagermodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 26.04.2013
//
// Description:
// The model which re-implements the validity check of dir paths.
//-----------------------------------------------------------------------------

#include "dirlistmanagermodel.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <common/KactusColors.h>

#include <QFileInfo>

#include <QIcon>
#include <QPixmap>

//-----------------------------------------------------------------------------
// Function: dirlistmanagermodel::DirListManagerModel()
//-----------------------------------------------------------------------------
DirListManagerModel::DirListManagerModel(QString const& basePath, QStringList const& items, QObject* parent):
ListManagerModel(parent, items),
    basePath_(basePath)
{
    if (!basePath_.endsWith(QLatin1Char('/')))
    {
        basePath_.append(QLatin1Char('/'));
    }
}

//-----------------------------------------------------------------------------
// Function: dirlistmanagermodel::~DirListManagerModel()
//-----------------------------------------------------------------------------
DirListManagerModel::~DirListManagerModel()
{

}

//-----------------------------------------------------------------------------
// Function: dirlistmanagermodel::data()
//-----------------------------------------------------------------------------
QVariant DirListManagerModel::data(QModelIndex const& index, int role /*= Qt::DisplayRole*/) const
{
	if (!index.isValid())
    {
		return QVariant();
	}

	// if there are items to display then check the validity of them
	else if (role == Qt::ForegroundRole && !items_.isEmpty())
    {
        if (directoryExistsForPath(items_.at(index.row())))
        {
            return KactusColors::REGULAR_TEXT;
        }
        else
        {
            return KactusColors::ERROR;
        }
	}

    else if (role == Qt::DecorationRole && !items_.isEmpty())
    {
        if (!directoryExistsForPath(items_.at(index.row())))
        {
            return QIcon(QPixmap(":/icons/common/graphics/exclamation.png"));
        }
        else
        {
            return QVariant();
        }
    }

	// other than fore ground role use the base class implementation
	else
    {
		return ListManagerModel::data(index, role);
	}
}

//-----------------------------------------------------------------------------
// Function: dirlistmanagermodel::directoryExistsForItem()
//-----------------------------------------------------------------------------
bool DirListManagerModel::directoryExistsForPath(QString const& relativePath) const
{
    QString absDirPath = General::getAbsolutePath(basePath_, relativePath);
    
    QFileInfo dirInfo(absDirPath);
    return dirInfo.exists();
}
