//-----------------------------------------------------------------------------
// File: CatalogFileDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.02.2017
//
// Description:
// Delegate to provide editors to edit catalog files.
//-----------------------------------------------------------------------------

#ifndef CATALOGFILEDELEGATE_H
#define CATALOGFILEDELEGATE_H

#include <QPainter>
#include <QSharedPointer>
#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
// ! Delegate to provide editors to edit catalog files.
//-----------------------------------------------------------------------------
class CatalogFileDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent              The parent of this delegate.
	 */
	CatalogFileDelegate(QObject *parent);
	
	/*!
     *  The destructor.
     */
    ~CatalogFileDelegate();
       
    /*!
     *  Returns the size hint for the given model index.
     */
    virtual QSize sizeHint(QStyleOptionViewItem const& option, QModelIndex const& index) const;

    /*!
     *  Paint the memory maps.
     *
     *      @param [in] painter     The used painter.
     *      @param [in] option      Available style options.
     *      @param [in] index       The current index.
     */
    virtual void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;
    
private:

	//! No copying.
	CatalogFileDelegate(const CatalogFileDelegate& other);
	CatalogFileDelegate& operator=(const CatalogFileDelegate& other);
  
};

#endif // CATALOGFILEDELEGATE_H
