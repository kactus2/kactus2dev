//-----------------------------------------------------------------------------
// File: LibrarySettingsDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 07.02.2013
//
// Description:
// This delegate is used to draw the check marks in the middle of default and active library columns.
//-----------------------------------------------------------------------------

#ifndef LIBRARYSETTINGSDELEGATE_H
#define LIBRARYSETTINGSDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! This delegate is used to draw the check marks in the middle of default and active library columns.
//-----------------------------------------------------------------------------
class LibrarySettingsDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*!
     *  The constructor. 
	 * 
	 *      @param [in] parent  Pointer to the owner of the delegate.
     */
	LibrarySettingsDelegate(QObject *parent);
	
	/*!
     *  The destructor.
     */
	virtual ~LibrarySettingsDelegate();

	/*!
	 *  Paint the check marks in the middle of the active and default columns.
	 *
	 *      @param [in] painter     The selected painter.
	 *      @param [in] option      Options of the painter.
	 *      @param [in] index       Current table index.
	 */
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:

	//! No copying
	LibrarySettingsDelegate(const LibrarySettingsDelegate& other);

	//! No assignment
	LibrarySettingsDelegate& operator=(const LibrarySettingsDelegate& other);
};

#endif // LIBRARYSETTINGSDELEGATE_H
