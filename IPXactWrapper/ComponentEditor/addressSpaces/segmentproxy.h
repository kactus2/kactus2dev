/* 
 *  	Created on: 15.3.2012
 *      Author: Antti Kamppi
 * 		filename: segmentproxy.h
 *		Project: Kactus 2
 */

#ifndef SEGMENTPROXY_H
#define SEGMENTPROXY_H

#include <QSortFilterProxyModel>

/*! \brief The proxy that is used when sorting objects in segment editor.
 *
 */
class SegmentProxy : public QSortFilterProxyModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this proxy
	 *
	*/
	SegmentProxy(QObject *parent);
	
	//! \brief The destructor
	virtual ~SegmentProxy();

protected:

	/*! \brief Returns true if left < right
	 *
	 * \param left References the item on the left side.
	 * \param right References the item on the right side.
	 *
	 * \return bool True if left < right
	*/
	virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const;

private:
	//! \brief No copying
	SegmentProxy(const SegmentProxy& other);

	//! \brief No assignment
	SegmentProxy& operator=(const SegmentProxy& other);
};

#endif // SEGMENTPROXY_H
