/* 
 *  Created on: 18.6.2010
 *  Author: Antti Kamppi
 */

#ifndef IPXACTITEM_H_
#define IPXACTITEM_H_

#include <QDomNode>
#include <QHash>
#include <QSharedPointer>

/*! \brief A wrapper to control single QDomNode-instance
 *
 * This class acts as a wrapper for QDomNode class and provides services
 * for accessing these the nodes of an XML QDomDocument
 */
class IPXactItem {
public:
	/*! \brief The constructor
	 *
	 * \param node A reference to the node that this instance wraps
	 * \param row To which row this instance belongs to in a QAbstractItemModel
	 * \param parent A pointer to the parent of this node
	 */
	IPXactItem(QDomNode &node, int row, IPXactItem *parent = 0);

	/*! \brief The destructor of the class IPXactItem
	 *
	 */
	~IPXactItem();

	/*! \brief Create wrapper for the child of a QDomNode
	 *
	 * This function creates a wrapper for a new node. IPXactModen class uses
	 * this function to help create new model indexes
	 * \param index The index of the child to be created
	 * \return A pointer to the new instance of a IPXactItem. If the QDomNode
	 * was previously wrapped then it is returned. If the child is invalid 0
	 * is returned. For valid children a new wrapper is constructed and
	 * stored.
	 */
	IPXactItem *child(int index);

	/*! \brief Get the pointer to this wrappers parent
	 *
	 * \return A pointer to the parent item
	 */
	IPXactItem *parent();

	/*! \brief Get the wrapped node
	 *
	 * \return Pointer to the wrapped QDomNode
	 */
	QDomNode *node();

	/*! \brief Get the row number
	 *
	 * \return The row number of the item
	 */
	int row();

private:

	/*!
	 * \brief No copying
	 */
	IPXactItem(const IPXactItem &other);

	/*!
	 * \brief No assignment
	 */
	IPXactItem &operator=(const IPXactItem &other);

	/*!
	 * \brief The node that the instance of this class wraps
	 */
	QDomNode domNode_;

	/*!
	 * \brief Hash of wrappers for the child items
	 */
	QHash<int, QSharedPointer<IPXactItem> > childItems_;

	/*!
	 * \brief A pointer to the parent item of this instance
	 */
	IPXactItem* parentItem_;

	/*!
	 * \brief what is the row number of this wrappers node
	 */
	int rowNumber_;

};

#endif /* IPXACTITEM_H_ */
