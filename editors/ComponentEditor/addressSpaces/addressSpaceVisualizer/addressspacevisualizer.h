/* 
 *  	Created on: 19.12.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacevisualizer.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACEVISUALIZER_H
#define ADDRESSSPACEVISUALIZER_H

#include <editors/ComponentEditor/itemvisualizer.h>
#include <models/component.h>
#include <models/addressspace.h>
#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacescene.h>

#include <QSharedPointer>
#include <QGraphicsView>
#include <QSize>

/*! \brief The visualizer to display the contents of an address space.
 *
 */
class AddressSpaceVisualizer : public ItemVisualizer {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param addrSpace Pointer to the address space being visualized.
	 * \param component Pointer to the component being visualized.
	 * \param parent Pointer to the owner of the visualizer.
	 *
	*/
	AddressSpaceVisualizer(QSharedPointer<AddressSpace> addrSpace,
		QSharedPointer<Component> component,
		QWidget *parent = 0);
	
	//! \brief The destructor
	virtual ~AddressSpaceVisualizer();

	//! \brief Update the contents of the visualizer
	virtual void refresh();

	/*! \brief The minimum size hint for the visualizer.
	 *
	 * \return QSize contains the minimum size hint.
	*/
	virtual QSize minimumSizeHint() const;

	/*! \brief The size hint for the visualizer.
	 *
	 * \return QSize contains the size hint.
	*/
	virtual QSize sizeHint() const;

private:
	
	//! \brief No copying
	AddressSpaceVisualizer(const AddressSpaceVisualizer& other);

	//! \brief No assignment
	AddressSpaceVisualizer& operator=(const AddressSpaceVisualizer& other);

	//! \brief Pointer to the address space being visualized.
	QSharedPointer<AddressSpace> addrSpace_;

	//! \brief The view displaying the visualization items.
	QGraphicsView* view_;

	//! \brief The scene that manages the visualization items.
	AddressSpaceScene* scene_;
};

#endif // ADDRESSSPACEVISUALIZER_H
