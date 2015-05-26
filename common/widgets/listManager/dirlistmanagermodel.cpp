/* 
 *	Created on:	26.4.2013
 *	Author:		Antti Kamppi
 *	File name:	dirlistmanagermodel.cpp
 *	Project:		Kactus 2
*/

#include "dirlistmanagermodel.h"
#include <library/LibraryManager/libraryinterface.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QFileInfo>
#include <QColor>
#include <QIcon>
#include <QPixmap>

DirListManagerModel::DirListManagerModel(LibraryInterface* handler, 
	QSharedPointer<Component> component,
	QObject *parent, 
	const QStringList& items):
ListManagerModel(parent, items),
handler_(handler),
component_(component) {
}

DirListManagerModel::~DirListManagerModel() {
}

QVariant DirListManagerModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const
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
            return QColor("black");
        }
        else
        {
            return QColor("red");
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
    QString xmlPath = handler_->getPath(*component_->getVlnv());
    QString absDirPath = General::getAbsolutePath(xmlPath, relativePath);

    QFileInfo dirInfo(absDirPath);

    return dirInfo.exists();
}
