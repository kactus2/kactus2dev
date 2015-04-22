/* 
 *  	Created on: 22.5.2012
 *      Author: Antti Kamppi
 * 		filename: itemvisualizer.h
 *		Project: Kactus 2
 */

#ifndef ITEMVISUALIZER_H
#define ITEMVISUALIZER_H

#include <IPXACTmodels/component.h>

#include <QWidget>
#include <QSharedPointer>

/*! \brief The base class for all IP-Xact visualizer classes.
 *
 */
class ItemVisualizer : public QWidget
{
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the parent of the visualizer. 
	*/
	ItemVisualizer(QWidget* parent = 0);
	
	//! \brief The destructor
	virtual ~ItemVisualizer();

private:
	//! \brief No copying
	ItemVisualizer(const ItemVisualizer& other);

	//! \brief No assignment
	ItemVisualizer& operator=(const ItemVisualizer& other);
};

#endif // ITEMVISUALIZER_H
