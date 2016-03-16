//-----------------------------------------------------------------------------
// File: portmapsdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 13.04.2011
//
// Description:
//  Delegate to provide editors to edit port maps.
//-----------------------------------------------------------------------------

#ifndef PORTMAPSDELEGATE_H
#define PORTMAPSDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! Delegate to provide editors to edit port maps.
//-----------------------------------------------------------------------------
class PortMapsDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent The owner of this delegate.
	*/
	PortMapsDelegate(QObject *parent);
	
	//! The destructor
	virtual ~PortMapsDelegate();

	/*!
     *  Returns the size hint for the given model index.
     */
    virtual QSize sizeHint(QStyleOptionViewItem const& option, QModelIndex const& index) const;

private:

	//! No copying
	PortMapsDelegate(const PortMapsDelegate& other);
	PortMapsDelegate& operator=(const PortMapsDelegate& other);
};

#endif // PORTMAPSDELEGATE_H
