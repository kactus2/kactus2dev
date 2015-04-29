//-----------------------------------------------------------------------------
// File: componenteditormemmapitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 16.05.2012
//
// Description:
// The item for a single memory map in component editor's navigation tree.
//-----------------------------------------------------------------------------
#ifndef COMPONENTEDITORMEMMAPITEM_H
#define COMPONENTEDITORMEMMAPITEM_H

#include "componenteditoritem.h"
#include <IPXACTmodels/memorymap.h>

#include <QSharedPointer>
#include <QList>

class MemoryMapEditor;
class MemoryMapsVisualizer;
class MemoryMapGraphItem;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! The item for a single memory map in component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorMemMapItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] memoryMap               The memory map being edited.
	 *      @param [in] model                   The model that owns the items.
	 *      @param [in] libHandler              The instance that manages the library.
	 *      @param [in] component               The component being edited.
     *      @param [in] referenceCounter        The instance for counting references to parameters.
	 *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] expressionParser        The expression parser to use.
	 *      @param [in] parent                  The parent item.
	 */
	ComponentEditorMemMapItem(QSharedPointer<MemoryMap> memoryMap,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorMemMapItem();

	/*! \brief Get the tool tip for the item.
	 * 
	 *      @return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! \brief Get the text to be displayed to user in the tree for this item.
	 *
	 *      @return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! \brief Check the validity of this item and sub items.
	 *
	 *      @return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Get pointer to the editor of this item.
	 *
	 *      @return The editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! \brief Add a new child to the item.
	 * 
	 *      @param [in] index The index to add the child into.
	 *
	*/
	virtual void createChild(int index);

	/*! \brief Get pointer to the visualizer of this item.
	 * 
	 *      @return The visualizer to use for this item.
	*/
	virtual ItemVisualizer* visualizer();

	/*! \brief Set the visualizer for this item.
	 *
	 *      @param [in] visualizer The visualizer.
	 *
	*/
	virtual void setVisualizer(MemoryMapsVisualizer* visualizer);

	/*! \brief Get the visualizer graphics item for the memory map.
	 *
	 *      @return QGraphicsItem* The graphics item.
	*/
	virtual QGraphicsItem* getGraphicsItem();

	/*! \brief Update the graphics item of the memory map.
	 *
	*/
	virtual void updateGraphics();

	/*! \brief Remove the graphics item of the memory map.
	 *
	*/
	virtual void removeGraphicsItem();

    /*!
     *  Change the address unit bits for the address blocks.
     */
    void changeAdressUnitBitsOnAddressBlocks();

public slots:

    /*!
     *  Add a memory remap item to the tree.
     *
     *      @param [in] memoryRemapIndex    The index of the new memory remap item.
     *      @param [in] parentMemoryMap     The parent of the new memory remap item.
     */
    void onMemoryRemapAdded(int memoryRemapIndex, QSharedPointer<MemoryMap> parentMemoryMap);

    /*!
     *  Remove a memory remap item from the tree.
     *
     *      @param [in] memoryRemapIndex    The index of the removed memory remap item.
     *      @param [in] parentMemoryMap     The parent of the removed memory remap item.
     */
    void onMemoryRemapRemoved(int memoryRemapIndex, QSharedPointer<MemoryMap> parentMemoryMap);

private:
	//! No copying
	ComponentEditorMemMapItem(const ComponentEditorMemMapItem& other);
	ComponentEditorMemMapItem& operator=(const ComponentEditorMemMapItem& other);

	//! The memory map being edited.
	QSharedPointer<MemoryMap> memoryMap_;

	//! The visualizer to display the memory maps
	MemoryMapsVisualizer* visualizer_;

	//! The graph item which visualizes this memory map.
	MemoryMapGraphItem* graphItem_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // COMPONENTEDITORMEMMAPITEM_H
