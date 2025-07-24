//-----------------------------------------------------------------------------
// File: MemoryRemapItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.04.2015
//
// Description:
// The item for a single memory remap in component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef MEMORYREMAPITEM_H
#define MEMORYREMAPITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>
#include <QList>

class MemoryMapEditor;
class MemoryMapsVisualizer;
class MemoryMapGraphItem;
class ExpressionParser;
class MemoryMap;
class MemoryMapBase;
class MemoryBlockBase;
class MemoryMapValidator;
class MemoryMapInterface;

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
     *    @param [in] memoryRemap             The memory remap being edited.
     *    @param [in] parentMemoryMap         The parent memory map of the memory remap.
     *    @param [in] model                   The model that owns the items.
     *    @param [in] libHandler              The instance that manages the library.
     *    @param [in] component               The component being edited.
     *    @param [in] referenceCounter        The instance for counting references to parameters.
     *    @param [in] parameterFinder         The parameter finder.
     *    @param [in] expressionFormatter     The expression formatter.
     *    @param [in] expressionParser        The expression parser to use.
     *    @param [in] memoryMapValidator      Validator for memory maps.
     *    @param [in] mapInterface            Interface for accessing memory maps.
     *    @param [in] parent                  The parent item.
     */
    MemoryRemapItem(QSharedPointer<MemoryMapBase> memoryRemap,
        QSharedPointer<MemoryMap> parentMemoryMap,
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

    /*!
     *  The destructor.
     */
    virtual ~MemoryRemapItem();

    //! No copying
    MemoryRemapItem(const MemoryRemapItem& other) = delete;

    //! No assignment
    MemoryRemapItem& operator=(const MemoryRemapItem& other) = delete;

	/*!
	 *  Get the tool tip for this item.
	 *
     *    @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const;

	/*!
	 *  Get the text to be displayed to user in the tree for this item.
	 *
     *    @return The text to display.
	 */
	virtual QString text() const;

	/*!
	 *  Check the validity of this item and sub items.
	 *
     *    @return True, if the item is in a valid state, false otherwise.
	 */
	virtual bool isValid() const;

	/*!
	 *  Get the The editor of this item.
	 *
     *    @return The editor to use for this item.
	 */
	virtual ItemEditor* editor();

	/*!
	 *  Add a new child to the item.
	 *
	 *    @param [in] index   The index of the new child.
	 */
	virtual void createChild(int index) override final;

    virtual void removeChild(int index) override final;

	/*!
	 *  Get The visualizer of this item.
	 *
     *    @return The visualizer to user for this item.
	 */
	virtual ItemVisualizer* visualizer();

	/*!
	 *  Set the visualizer for this item.
	 *
	 *    @param [in] visualizer  The visualizer.
	 */
	virtual void setVisualizer(MemoryMapsVisualizer* visualizer);

	/*!
	 *  Get the visualizer graphics item for the memory remap.
	 *
     *    @return The graphics item.
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

    void onChildGraphicsChanged(int index);

    //! Handle the change in item's addressing data.
    void onAddressingChanged();

    //! Handle the change in child item's addressing data. Slot called when child is edited from table editor.
    void onChildAddressingChangedLocally(int index);

    //! Handle the change in child item's addressing data. Slot called when child is edited in its editor.
    void onChildAddressingChanged();
    
    //! Handle request to redo layout of children.
    void onLayoutRefreshRequested();

    /*!
     *  Change the address unit bits for the address blocks.
     */
    void changeAdressUnitBitsOnAddressBlocks();

signals:

    /*!
     *  Informs of a change in address unit bits.
     */
    void addressUnitBitsChanged();

    /*!
     *  Change the used address unit bits in the editor.
     *
     *    @param [in] newAddressUnitBits  The new address unit bits.
     */
    void assignNewAddressUnitBits(QString const& newAddressUnitBits);


    //! Signals a change in the item's address data.
    void addressingChanged();

    /*
     *  Informs of address block name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void addressBlockNameChanged(QString const& oldName, QString const& newName);
    
    /*
     *  Informs of subspace map name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void subspaceMapNameChanged(QString const& oldName, QString const& newName);

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
     *    @param [in] parentName  Name of the containing memory map.
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void memoryRemapNameChanged(QString const& parentName, QString const& oldName, QString const& newName);

private:

    void createGraphicsItemsForChild(ComponentEditorItem* childEditor);

	//! The memory remap being edited.
    QSharedPointer<MemoryMapBase> memoryRemap_;

    //! The parent memory map of this memory remap.
    QSharedPointer<MemoryMap> parentMemoryMap_;

	//! Contains the address blocks that are children of this tree item.
    QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > memoryBlocks_;

	//! The visualizer to display the memory maps
	MemoryMapsVisualizer* visualizer_ = nullptr;

	//! The graph item which visualizes this memory map.
	MemoryMapGraphItem* graphItem_ = nullptr;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The used memory map validator.
    QSharedPointer<MemoryMapValidator> memoryMapValidator_;

    //! Interface for memory maps.
    MemoryMapInterface* mapInterface_;
};

#endif // MEMORYREMAPITEM_H
