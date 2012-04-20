/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapsvisualizer.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPSVISUALIZER_H
#define MEMORYMAPSVISUALIZER_H

#include <models/memorymap.h>
#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapview.h>
#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapscene.h>

#include <QWidget>
#include <QList>
#include <QSharedPointer>

/*! \brief The widget to visualize a set of memory maps.
 *
 */
class MemoryMapsVisualizer : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of the visualizer widget.
	 *
	*/
	MemoryMapsVisualizer(QWidget *parent);
	
	//! \brief The destructor
	virtual ~MemoryMapsVisualizer();

	/*! \brief Set the memory maps to be displayed.
	 * 
	 * Any previous memory maps being displayed are removed.
	 * 
	 * \param memoryMaps QList containing pointers to the memory maps to display.
	 *
	*/
	void setMemoryMaps(QList<QSharedPointer<MemoryMap> >& memoryMaps);

private:
	
	//! \brief No copying
	MemoryMapsVisualizer(const MemoryMapsVisualizer& other);

	//! \brief No assignment
	MemoryMapsVisualizer& operator=(const MemoryMapsVisualizer& other);

	//! \brief The view to display the items.
	MemoryMapView view_;

	//! \brief The scene that contains the memory map items.
	MemoryMapScene scene_;
};

#endif // MEMORYMAPSVISUALIZER_H
