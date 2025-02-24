//-----------------------------------------------------------------------------
// File: dirlistmanager.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 04.02.2011
//
// Description:
// DirListManager can be used to manage a list of relative directory paths.
//-----------------------------------------------------------------------------

#ifndef DIRLISTMANAGER_H
#define DIRLISTMANAGER_H

#include "listmanager.h"

#include <QString>
#include <QWidget>
#include <QStringList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! DirListManager can be used to manage a list of relative directory paths.
//-----------------------------------------------------------------------------
class DirListManager : public ListManager
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *    @param [in] title       Title to be set for the QGroupBox
	 *    @param [in] basePath    Path to start relative paths from.
	 *    @param [in] parent      The owner of this widget.
	 */
	DirListManager(QString const& title, QString const& basePath, QWidget *parent = 0);

	//! The destructor.
	virtual ~DirListManager();

	/*!
     *  Initialize the directory list manager.
	 *
	 *  This function must be called after creating the directory list manager and before using it.
	 * 
	 *    @param [in] items   QStringList that contains the items to add to the widget.
	 */
	virtual void initialize(const QStringList& items = QStringList());

private:
	
	//! No copying. No assignment.
	DirListManager(const DirListManager& other);
	DirListManager& operator=(const DirListManager& other);

    //! Path to start search for relative paths.
    QString basePath_;
};

#endif // DIRLISTMANAGER_H
