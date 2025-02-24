//-----------------------------------------------------------------------------
// File: addressspacescene.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 22.12.2012
//
// Description:
// The graphics scene containing the segments and local memory map of an address space.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACESCENE_H
#define ADDRESSSPACESCENE_H

#include "addressspacegapitem.h"

#include <editors/ComponentEditor/common/MemoryAlignScene.h>

#include <QGraphicsScene>
#include <QSharedPointer>
#include <QMultiMap>

class AddressSpaceVisualizationItem;
class ExpressionParser;
class AddressSpace;

//-----------------------------------------------------------------------------
//! The graphics scene containing the segments and local memory map of an address space.
//-----------------------------------------------------------------------------
class AddressSpaceScene : public MemoryAlignScene
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *    @param [in] addrSpace         The address space being visualized.
     *    @param [in] expressionParser  The expression parser to use for local memory maps.
	 *    @param [in] parent            The owner of the scene.
	 */
	AddressSpaceScene(QSharedPointer<AddressSpace> addrSpace, 
        QSharedPointer<ExpressionParser> expressionParser,
		QObject *parent);
	
	//! The destructor.
	virtual ~AddressSpaceScene() = default;

    //! No copying.
    AddressSpaceScene(const AddressSpaceScene& other) = delete;

    //! No assignment.
    AddressSpaceScene& operator=(const AddressSpaceScene& other) = delete;

protected:

    /*!
     *  Get the width of the visualized memory.
     *
     *    @return The width of the memory.
     */
    QString getWidth() const final;

    /*!
     *  Get the range of the visualized memory.
     *
     *    @return The range of the memory.
     */
    QString getRange() const final;

    /*!
     *  Create the region/segment items placed on left.
     *
     */
    void createRegionItems() final;

    /*!
     *  Create the address block items placed on right.
     *
     */
    void createAddressBlockItems() final;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The address space being visualized.
	QSharedPointer<AddressSpace> addrSpace_;

};

#endif // ADDRESSSPACESCENE_H
