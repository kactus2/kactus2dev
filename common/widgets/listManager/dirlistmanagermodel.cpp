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

QVariant DirListManagerModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}

	// if there are items to display then check the validity of them
	else if (role == Qt::ForegroundRole && !items_.isEmpty()) {
		
		// the relative path to the directory
		QString relPath = items_.at(index.row());

		// the path to the containing component
		QString xmlPath = handler_->getPath(*component_->getVlnv());

		// check if the directory exists.
		QString absDirPath = General::getAbsolutePath(xmlPath, relPath);
		QFileInfo dirInfo(absDirPath);

		if (dirInfo.exists()) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}

	// other than fore ground role use the base class implementation
	else {
		return ListManagerModel::data(index, role);
	}
}
