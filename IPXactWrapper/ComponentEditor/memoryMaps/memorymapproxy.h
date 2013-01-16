/* 
 *  	Created on: 15.1.2013
 *      Author: Antti Kamppi
 * 		filename: memorymapproxy.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPPROXY_H
#define MEMORYMAPPROXY_H

#include <QSortFilterProxyModel>

/*! \brief The proxy sorter which is used to sort the address blocks within memory map editor.
 *
 */
class MemoryMapProxy : public QSortFilterProxyModel {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner of the proxy
	 *
	*/
	MemoryMapProxy(QObject *parent);

	//! \brief The destructor
	virtual ~MemoryMapProxy();

protected:

	/*! \brief Returns true if indexed left < indexed right.
	 *
	 * \param left Index identifying the left item.
	 * \param right Index identifying the right item.
	 *
	 * \return True if left < right.
	*/
	virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const;

private:
	
	//! \brief No copying
	MemoryMapProxy(const MemoryMapProxy& other);

	//! \brief No assignment
	MemoryMapProxy& operator=(const MemoryMapProxy& other);
};

#endif // MEMORYMAPPROXY_H
