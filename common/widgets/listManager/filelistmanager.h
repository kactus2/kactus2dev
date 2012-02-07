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
	*/
	FileListManager(const QString title, 
		const QFileInfo& baseLocation, QWidget *parent = 0);

	//! \brief The destructor
	virtual ~FileListManager();

	/*! \brief Initialize the file list manager.
	 *
	 * This function must be called after creating the file list manager and before
	 * using it.
	 * 
	 * \param items QStringList that contains the items to add to the widget.
	 *
	*/
	virtual void initialize(const QStringList& items = QStringList());

private:

	//! No copying
	FileListManager(const FileListManager& other);

	//! No assignment
	FileListManager& operator=(const FileListManager& other);

	//! \brief Refers to the base location that is used to create relative paths
	QFileInfo baseLocation_;
	
};

#endif // FILELISTMANAGER_H
