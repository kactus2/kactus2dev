//-----------------------------------------------------------------------------
// File: addressspacevisualizer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 19.12.2012
//
// Description:
// The visualizer to display the contents of an address space.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACEVISUALIZER_H
#define ADDRESSSPACEVISUALIZER_H

#include <editors/ComponentEditor/itemvisualizer.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/addressspace.h>
#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacescene.h>

#include <QSharedPointer>
#include <QGraphicsView>
#include <QSize>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The visualizer to display the contents of an address space.
//-----------------------------------------------------------------------------
class AddressSpaceVisualizer : public ItemVisualizer
{
	Q_OBJECT

public:

	/*! The constructor.
	 *
     *       @param [in] addrSpace          The address space being visualized.
     *       @param [in] expressionParser   The expression parser to use.
	 *       @param [in] parent             The owner of the visualizer.
	 *
	*/
	AddressSpaceVisualizer(QSharedPointer<AddressSpace> addrSpace,
        QSharedPointer<ExpressionParser> expressionParser,
		QWidget *parent = 0);
	
	//! The destructor
	virtual ~AddressSpaceVisualizer();

	//! Update the contents of the visualizer
	virtual void refresh();

	/*! The minimum size hint for the visualizer.
	 *
	 *      @return The minimum size hint.
	*/
	virtual QSize minimumSizeHint() const;

	/*! The size hint for the visualizer.
	 *
	 *      @return The size hint.
	*/
	virtual QSize sizeHint() const;

private:
	
	//! No copying
	AddressSpaceVisualizer(const AddressSpaceVisualizer& other);

	//! No assignment
	AddressSpaceVisualizer& operator=(const AddressSpaceVisualizer& other);

	//! The address space being visualized.
	QSharedPointer<AddressSpace> addrSpace_;

	//! The view displaying the visualization items.
	QGraphicsView* view_;

	//! The scene that manages the visualization items.
	AddressSpaceScene* scene_;
};

#endif // ADDRESSSPACEVISUALIZER_H
