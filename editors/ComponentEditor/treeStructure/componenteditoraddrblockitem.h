//-----------------------------------------------------------------------------
// File: componenteditoraddrblockitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 23.08.2012
//
// Description:
// The item for a single address block in component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORADDRBLOCKITEM_H
#define COMPONENTEDITORADDRBLOCKITEM_H

#include <editors/ComponentEditor/treeStructure/componenteditoritem.h>

#include <IPXACTmodels/Component/AddressBlock.h>

#include <QFont>
#include <QSharedPointer>

class MemoryMapsVisualizer;
class AddressBlockGraphItem;
class ExpressionParser;
class AddressBlockValidator;
class RegisterInterface;
class AddressBlockInterface;
class MemoryMapBase;

//-----------------------------------------------------------------------------
//! The item for a single address block in component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorAddrBlockItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
     *    @param [in] containingMap           Memory map containing the address block.
	 *    @param [in] addrBlock               The address block being edited.
	 *    @param [in] model                   The model that owns the items.
	 *    @param [in] libHandler              The instance that manages the library.
	 *    @param [in] component               The component being edited.
     *    @param [in] referenceCounter        The instance for counting references to parameters.
	 *    @param [in] parameterFinder         The parameter finder.
	 *    @param [in] expressionFormatter     The expression formatter.
	 *    @param [in] expressionParser        The expression formatter.
     *    @param [in] addressBlockValidator   Validator used for address blocks.
     *    @param [in] blockInterface          Interface for accessing address blocks.
	 *    @param [in] parent                  The parent item.
	 */
    ComponentEditorAddrBlockItem(QSharedPointer<MemoryMapBase> containingMap,
        QSharedPointer<AddressBlock> addrBlock,
        ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<AddressBlockValidator> addressBlockValidator,
        AddressBlockInterface* blockInterface,
		ComponentEditorItem* parent);

	//! The destructor.
	virtual ~ComponentEditorAddrBlockItem() = default;


    //! No copying. No assignment.
    ComponentEditorAddrBlockItem(const ComponentEditorAddrBlockItem& other) = delete;
    ComponentEditorAddrBlockItem& operator=(const ComponentEditorAddrBlockItem& other) = delete;

	/*!
     *  Get the tool tip for the item.
	 * 
	 *    @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const final;

	/*!
     *  Get the text to be displayed to user in the tree for this item.
	 *
	 *    @return QString Contains the text to display.
	 */
	virtual QString text() const final;

	/*!
     *  Check the validity of this item and sub items.
	 *
	 *    @return bool True if item is in valid state.
	 */
	virtual bool isValid() const final;

	/*!
     *  Get pointer to the editor of this item.
	 *
	 *    @return The editor to use for this item.
	 */
	virtual ItemEditor* editor() final;

	/*!
     *  Add a new child to the item.
	 * 
	 *    @param [in] index The index to add the child into.
	 */
	virtual void createChild(int index) final;

    virtual void removeChild(int index) final;

    /*!
     *  Get pointer to the visualizer of this item.
	 * 
	 *    @return The visualizer to use for this item.
	 */
	virtual ItemVisualizer* visualizer() final;

	/*!
     *  Set the visualizer for this item.
	 *
	 *    @param [in] visualizer The visualizer.
	 */
	void setVisualizer(MemoryMapsVisualizer* visualizer);

    /*!
     *	Create graph item for this item or multiple items in case of dimensions.
     *
     *    @param [in] parentItem     The parent item for created item(s).
     */
    void createGraphicsItems(QGraphicsItem* parentItem);

    /*!
     *	Create graph items for all child items.
     */
    void createGraphicsItemsForChildren();

	/*!
     *  Update the graphics item of the address block.
	 */
	virtual void updateGraphics() final;

    /*!
     *	Remove the graph items of this item.
     */
    void removeGraphicsItems();

    /*!
     *  Change the address unit bits in component editor.
     *
     *    @param [in] newAddressUnitBits   The new value for address unit bits.
     */
    void addressUnitBitsChanged(int newAddressUnitBits);

public slots:

    //! Handle the change in item's addressing data.
    void onAddressingChanged();

    //! Handle the change in child item's addressing data. Slot called when child is edited from table editor.
    void onChildAddressingChangedLocally(int index);

    //! Handle the change in child item's addressing data. Slot called when child is edited in its editor.
    void onChildAddressingChanged();

    //! Handle request to redo layout of children and re-emit request to parent.
    void onLayoutRefreshRequested();

protected slots:
    
    /*!
     *  Handles the redrawing of the visualization of the item.
     */
    virtual void onGraphicsChanged();

    void onChildGraphicsChanged(int index);    

signals:
    
    /*!
     *  Signals a change in the address unit bits.
     *
     *    @param [in] newAddressUnitBits  The new address unit bits.
     */
    void changeInAddressUnitBits(int newAddressUnitBits);


    //! Signals a change in the item's address data.
    void addressingChanged();

    //! Signals that the parent should redo the layout of its children.
    void refreshLayout();

    /*
     *  Informs of register name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void registerNameChanged(QString const& oldName, QString const& newName);

    /*
     *  Informs of address block name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void addressBlockNameChanged(QString const& oldName, QString const& newName);

private:

    /*!
     *	Create the graph items for given child item. Creates child graph items for each child graph item replica.
     *
     *    @param [in] childEditor     The child to create items for.
     */
    void createGraphicsItemsForChild(ComponentEditorItem* childEditor);
	
    //! Memory map containing the edited address block.
    QSharedPointer<MemoryMapBase> containingMap_;

	//! The address block being edited.
	QSharedPointer<AddressBlock> addrBlock_;

	//! The visualizer for memory maps.
	MemoryMapsVisualizer* visualizer_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The number of addressable unit bits in the address block.
    unsigned int addressUnitBits_;

    //! The used address block validator.
    QSharedPointer<AddressBlockValidator> addressBlockValidator_;    
    

    //! Interface for address blocks.
    AddressBlockInterface* blockInterface_;
};

#endif // COMPONENTEDITORADDRBLOCKITEM_H
