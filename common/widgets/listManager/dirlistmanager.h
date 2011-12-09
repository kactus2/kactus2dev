/* 
 *
 *  Created on: 4.2.2011
 *      Author: Antti Kamppi
 * 		filename: dirlistmanager.h
 */

#ifndef DIRLISTMANAGER_H
#define DIRLISTMANAGER_H

#include "listmanager.h"

#include <QString>
#include <QFileInfo>
#include <QWidget>
#include <QStringList>

/*! \brief DirListManager can be used to manage a list of relative directory paths.
 *
 */
class DirListManager : public ListManager {
	Q_OBJECT

public:

	
	/*! \brief The constructor
	 *
	 * \param title Title to be set for the QGroupBox
	 * \param baseLocation Refers to the location used as base for relative 
	 * directory paths.
	 * \param parent Pointer to the owner of this widget.
	 * \param items List of directory paths to be added to this widget.
	 *
	*/
	DirListManager(const QString title, 
		const QFileInfo& baseLocation, QWidget *parent = 0, 
		const QStringList& items = QStringList());

	//! \brief The destructor.
	virtual ~DirListManager();

private:
	
	//! No copying
	DirListManager(const DirListManager& other);

	//! No assignment
	DirListManager& operator=(const DirListManager& other);

	//! \brief Refers to the base directory that for relative paths.
	QFileInfo baseDir_;
};

#endif // DIRLISTMANAGER_H
