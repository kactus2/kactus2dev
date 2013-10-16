/* 
 *
 *  Created on: 13.4.2011
 *      Author: Antti Kamppi
 * 		filename: portmapsdelegate.h
 */

#ifndef PORTMAPSDELEGATE_H
#define PORTMAPSDELEGATE_H

#include <QStyledItemDelegate>

/*! \brief Delegate to provide editors to edit port maps.
 *
 */
class PortMapsDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this delegate.
	 *
	*/
	PortMapsDelegate(QObject *parent);
	
	//! \brief The destructor
	virtual ~PortMapsDelegate();

	/*!
     *  Returns the size hint for the given model index.
     */
    virtual QSize sizeHint(QStyleOptionViewItem const& option, QModelIndex const& index) const;

private:

	//! \brief No copying
	PortMapsDelegate(const PortMapsDelegate& other);

	//! No assignment
	PortMapsDelegate& operator=(const PortMapsDelegate& other);
};

#endif // PORTMAPSDELEGATE_H
