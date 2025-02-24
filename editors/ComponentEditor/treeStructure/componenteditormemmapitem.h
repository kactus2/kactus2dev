//-----------------------------------------------------------------------------
// File: componenteditormemmapitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 16.05.2012
//
// Description:
// The item for a single memory map in component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORMEMMAPITEM_H
#define COMPONENTEDITORMEMMAPITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>
#include <QList>

class MemoryMapEditor;
class MemoryMapsVisualizer;
class MemoryMapGraphItem;
class ExpressionParser;
class MemoryMap;
class MemoryMapValidator;
class MemoryMapInterface;

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
	 *    @param [in] memoryMap               The memory map being edited.
	 *    @param [in] model                   The model that owns the items.
	 *    @param [in] libHandler              The instance that manages the library.
	 *    @param [in] component               The component being edited.
     *    @param [in] referenceCounter        The instance for counting references to parameters.
	 *    @param [in] parameterFinder         The parameter finder.
	 *    @param [in] expressionFormatter     The expression formatter.
     *    @param [in] expressionParser        The expression parser to use.
     *    @param [in] memoryMapValidator      Validator used for memory maps.
     *    @param [in] mapInterface            Interface for accessing memory maps.
	 *    @param [in] parent                  The parent item.
	 */
	ComponentEditorMemMapItem(QSharedPointer<MemoryMap> memoryMap,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<MemoryMapValidator> memoryMapValidator,
        MemoryMapInterface* mapInterface,
		ComponentEditorItem* parent);

	//! The destructor.
	virtual ~ComponentEditorMemMapItem() = default;

    //! No copying. No assignment.
    ComponentEditorMemMapItem(const ComponentEditorMemMapItem& other) = delete;
    ComponentEditorMemMapItem& operator=(const ComponentEditorMemMapItem& other) = delete;

	/*!
     *  Get the tool tip for the item.
	 * 
	 *    @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const;

	/*!
     *  Get the text to be displayed to user in the tree for this item.
	 *
	 *    @return QString Contains the text to display.
	 */
	virtual QString text() const;

	/*!
     *  Check the validity of this item and sub items.
	 *
	 *    @return bool True if item is in valid state.
	 */
	virtual bool isValid() const;

	/*!
     *  Get pointer to the editor of this item.
	 *
	 *    @return The editor to use for this item.
	 */
	virtual ItemEditor* editor();

	/*!
     *  Add a new child to the item.
	 * 
	 *    @param [in] index The index to add the child into.
	 */
	virtual void createChild(int index);

	/*!
     *  Get pointer to the visualizer of this item.
	 * 
	 *    @return The visualizer to use for this item.
	 */
	virtual ItemVisualizer* visualizer();

	/*!
     *  Set the visualizer for this item.
	 *
	 *    @param [in] visualizer The visualizer.
	 */
	virtual void setVisualizer(MemoryMapsVisualizer* visualizer);

	/*!
     *  Get the visualizer graphics item for the memory map.
	 *
	 *    @return QGraphicsItem* The graphics item.
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

    /*!
     *  Add a memory remap item to the tree.
     *
     *    @param [in] memoryRemapIndex    The index of the new memory remap item.
     *    @param [in] mapName             Name of the parent of the new memory remap item.
     */
    void onMemoryRemapAdded(int memoryRemapIndex, QString const& mapName);

    /*!
     *  Remove a memory remap item from the tree.
     *
     *    @param [in] memoryRemapIndex    The index of the removed memory remap item.
     *    @param [in] mapName             Name of the parent of the removed memory remap item.
     */
    void onMemoryRemapRemoved(int memoryRemapIndex, QString const& mapName);

signals:
    
    /*
     *  Informs of memory map name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void memoryMapNameChanged(QString const& oldName, QString const& newName);

    /*
     *  Informs of memory remap name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void memoryRemapNameChanged(QString const& parentName, QString const& oldName, QString const& newName);

private:

	//! The memory map being edited.
	QSharedPointer<MemoryMap> memoryMap_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The validator used for memory maps.
    QSharedPointer<MemoryMapValidator> memoryMapValidator_;

    //! Interface for accessing memory maps.
    MemoryMapInterface* mapInterface_;
};

#endif // COMPONENTEDITORMEMMAPITEM_H
