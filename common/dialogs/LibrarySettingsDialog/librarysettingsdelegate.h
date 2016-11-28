/* 
 *	Created on: 7.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	librarysettingsdelegate.h
 * 	Project:	Kactus 2
*/

#ifndef LIBRARYSETTINGSDELEGATE_H
#define LIBRARYSETTINGSDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! This delegate is used to draw the check boxes in the middle of default and active library columns.
//-----------------------------------------------------------------------------
class LibrarySettingsDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	//! The columns for the library path table.
	enum PathColumns
    {
		DEF_COLUMN = 0,
		ACTIVE_COL,
		PATH_COL
	};

	/*! The constructor. 
	 * 
	 *      @param [in] parent Pointer to the owner of the delegate.
	*/
	LibrarySettingsDelegate(QObject *parent);
	
	//! The destructor
	virtual ~LibrarySettingsDelegate();

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:

	virtual bool editorEvent(QEvent* event, QAbstractItemModel* model, QStyleOptionViewItem const& option, 
        QModelIndex const& index);

private:

	//! No copying
	LibrarySettingsDelegate(const LibrarySettingsDelegate& other);

	//! No assignment
	LibrarySettingsDelegate& operator=(const LibrarySettingsDelegate& other);
};

#endif // LIBRARYSETTINGSDELEGATE_H
