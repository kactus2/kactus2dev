/* 
 *  	Created on: 16.1.2013
 *      Author: Antti Kamppi
 * 		filename: addressblockproxy.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSBLOCKPROXY_H
#define ADDRESSBLOCKPROXY_H

#include <QSortFilterProxyModel>

/*! \brief The proxy sorter which is used to sort the registers within address block editor.
 *
 */
class AddressBlockProxy : public QSortFilterProxyModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of the proxy
	 *
	*/
	AddressBlockProxy(QObject *parent);
	
	//! \brief The destructor
	virtual ~AddressBlockProxy();

protected:

	/*! \brief Returns true if indexed left < indexed right.
	 *
	 * \param left Index identifying the left item.
	 * \param right Index identifying the right item.
	 *
	 * \return True if left < right.
	*/
	virtual bool lessThan(QModelIndex const& left, QModelIndex const& right) const;

private:
	
	//! \brief No copying
	AddressBlockProxy(const AddressBlockProxy& other);

	//! \brief No assignment
	AddressBlockProxy& operator=(const AddressBlockProxy& other);
};

#endif // ADDRESSBLOCKPROXY_H
