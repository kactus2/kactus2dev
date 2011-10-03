/* 
 *
 *  Created on: 4.2.2011
 *      Author: Antti Kamppi
 * 		filename: filelistmanager.h
 */

#ifndef FILELISTMANAGER_H
#define FILELISTMANAGER_H

#include "listmanager.h"

#include <QStringList>
#include <QFileInfo>

/*! \brief FileListManager can be used to manage a list of relative file paths.
 *
 */
class FileListManager : public ListManager {
	Q_OBJECT

public:

	
	/*! \brief The constructor
	 *
	 * \param title The title to be set for the QGroupBox
	 * \param baseLocation Refers to the location that is used as base when 
	 * creating relative file paths
	 * \param parent Pointer to the owner of this widget
	 * \param items List of file paths to be added to the list.
	 *
	 * \return 
	*/
	FileListManager(const QString title, 
		const QFileInfo& baseLocation, QWidget *parent = 0, 
		const QStringList& items = QStringList());

	//! \brief The destructor
	virtual ~FileListManager();

public slots:

	//! \brief Called when user clicks "Add" button.
	virtual void onAdd();

	//! \brief Called when user clicks "Edit" button.
	virtual void onEdit();

private:

	//! No copying
	FileListManager(const FileListManager& other);

	//! No assignment
	FileListManager& operator=(const FileListManager& other);

	//! \brief Refers to the base location that is used to create relative paths
	QFileInfo baseLocation_;
	
};

#endif // FILELISTMANAGER_H
