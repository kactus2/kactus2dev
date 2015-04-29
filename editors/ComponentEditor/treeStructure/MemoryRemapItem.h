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
class ExpressionParser;
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
     *      @param [in] memoryRemap             The memory remap being edited.
     *      @param [in] parentMemoryMap         The parent memory map of the memory remap.
     *      @param [in] model                   The model that owns the items.
     *      @param [in] libHandler              The instance that manages the library.
     *      @param [in] component               The component being edited.
     *      @param [in] referenceCounter        The instance for counting references to parameters.
     *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] expressionParser        The expression parser to use.
     *      @param [in] parent                  The parent item.
     */
    MemoryRemapItem(QSharedPointer<AbstractMemoryMap> memoryRemap,
        QSharedPointer<MemoryMap> parentMemoryMap,
        ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
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
	 *  Get the The editor of this item.
	 *
     *      @return The editor to use for this item.
	 */
	virtual ItemEditor* editor();

	/*!
	 *  Add a new child to the item.
	 *
	 *      @param [in] index   The index of the new child.
	 */
	virtual void createChild(int index);

	/*!
	 *  Get The visualizer of this item.
	 *
     *      @return The visualizer to user for this item.
	 */
	virtual ItemVisualizer* visualizer();

	/*!
	 *  Set the visualizer for this item.
	 *
	 *      @param [in] visualizer  The visualizer.
	 */
	virtual void setVisualizer(MemoryMapsVisualizer* visualizer);

	/*!
	 *  Get the visualizer graphics item for the memory remap.
	 *
     *      @return The graphics item.
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

	//! The memory remap being edited.
    QSharedPointer<AbstractMemoryMap> memoryRemap_;

    //! The parent memory map of this memory remap.
    QSharedPointer<MemoryMap> parentMemoryMap_;

	//! Contains the address blocks that are children of this tree item.
	QList<QSharedPointer<MemoryMapItem> >& items_;

	//! The visualizer to display the memory maps
	MemoryMapsVisualizer* visualizer_;

	//! The graph item which visualizes this memory map.
	MemoryMapGraphItem* graphItem_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // MEMORYREMAPITEM_H
