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
	 *
	*/
	DirListManager(const QString title, 
		const QFileInfo& baseLocation, QWidget *parent = 0);

	//! \brief The destructor.
	virtual ~DirListManager();

	/*! \brief Initialize the directory list manager.
	 *
	 * This function must be called after creating the directory list manager and before
	 * using it.
	 * 
	 * \param items QStringList that contains the items to add to the widget.
	 *
	*/
	virtual void initialize(const QStringList& items = QStringList());

private:
	
	//! No copying
	DirListManager(const DirListManager& other);

	//! No assignment
	DirListManager& operator=(const DirListManager& other);

	//! \brief Refers to the base directory that for relative paths.
	QFileInfo baseDir_;
};

#endif // DIRLISTMANAGER_H
