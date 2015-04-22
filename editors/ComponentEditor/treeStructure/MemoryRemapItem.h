//-----------------------------------------------------------------------------
// File: MemoryRemapItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.04.2015
//
// Description:
// The item for a single memory remap in component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef MEMORYREMAPITEM_H
#define MEMORYREMAPITEM_H

#include "componenteditoritem.h"

#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/AbstractMemoryMap.h>

#include <QSharedPointer>
#include <QList>

class MemoryMapEditor;
class MemoryMapsVisualizer;
class MemoryMapGraphItem;

//-----------------------------------------------------------------------------
//! The item for a single memory remap in component editor's navigation tree.
//-----------------------------------------------------------------------------
class MemoryRemapItem : public ComponentEditorItem
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] memoryRemap             Pointer to the memory remap being edited.
     *      @param [in] parentMemoryMap         Pointer to the parent memory map of the memory remap.
     *      @param [in] model                   Pointer to the model that owns the items.
     *      @param [in] libHandler              Pointer to the instance that manages the library.
     *      @param [in] component               Pointer to the component being edited.
     *      @param [in] referenceCounter        Pointer to the instance for counting references to parameters.
     *      @param [in] parameterFinder         Pointer to the parameter finder.
     *      @param [in] expressionFormatter     Pointer to the expression formatter.
     *      @param [in] parent                  Pointer to the parent item.
     */
    MemoryRemapItem(QSharedPointer<AbstractMemoryMap> memoryRemap,
        QSharedPointer<MemoryMap> parentMemoryMap,
        ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        ComponentEditorItem* parent);

    /*!
     *  The destructor.
     */
    virtual ~MemoryRemapItem();

	/*!
	 *  Get the tool tip for this item.
	 *
     *      @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const;

	/*!
	 *  Get the text to be displayed to user in the tree for this item.
	 *
     *      @return The text to display.
	 */
	virtual QString text() const;

	/*!
	 *  Check the validity of this item and sub items.
	 *
     *      @return True, if the item is in a valid state, false otherwise.
	 */
	virtual bool isValid() const;

	/*!
	 *  Get the pointer to the editor of this item.
	 *
     *      @return Pointer to the editor to use for this item.
	 */
	virtual ItemEditor* editor();

	/*!
	 *  Add a new child to the item.
	 *
	 *      @param [in] index   The index of the new child.
	 */
	virtual void createChild(int index);

	/*!
	 *  Get pointer to the visualizer of this item.
	 *
     *      @return Pointer to the visualizer to user for this item.
	 */
	virtual ItemVisualizer* visualizer();

	/*!
	 *  Set the visualizer for this item.
	 *
	 *      @param [in] visualizer  Pointer to the visualizer.
	 */
	virtual void setVisualizer(MemoryMapsVisualizer* visualizer);

	/*!
	 *  Get the visualizer graphics item for the memory remap.
	 *
     *      @return Pointer to the graphics item.
	 */
	virtual QGraphicsItem* getGraphicsItem();

	/*!
	 *  Update the graphics item of the memory map.
	 */
	virtual void updateGraphics();

	/*!
	 *  Remove the graphics item of the memory map.
	 */
	virtual void removeGraphicsItem();

public slots:

    /*!
     *  Change the address unit bits for the address blocks.
     */
    void changeAdressUnitBitsOnAddressBlocks();

private:
	//! No copying
    MemoryRemapItem(const MemoryRemapItem& other);

	//! No assignment
    MemoryRemapItem& operator=(const MemoryRemapItem& other);

	//! Pointer to the memory remap being edited.
    QSharedPointer<AbstractMemoryMap> memoryRemap_;

    //! Pointer to the parent memory map of this memory remap.
    QSharedPointer<MemoryMap> parentMemoryMap_;

	//! Contains the address blocks that are children of this tree item.
	QList<QSharedPointer<MemoryMapItem> >& items_;

	//! The visualizer to display the memory maps
	MemoryMapsVisualizer* visualizer_;

	//! Pointer to the graph item which visualizes this memory map.
	MemoryMapGraphItem* graphItem_;
};

#endif // MEMORYREMAPITEM_H
