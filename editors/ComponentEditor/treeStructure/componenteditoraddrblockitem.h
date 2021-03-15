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
	 *      @param [in] addrBlock               The address block being edited.
	 *      @param [in] model                   The model that owns the items.
	 *      @param [in] libHandler              The instance that manages the library.
	 *      @param [in] component               The component being edited.
     *      @param [in] referenceCounter        The instance for counting references to parameters.
	 *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
	 *      @param [in] expressionParser        The expression formatter.
     *      @param [in] addressBlockValidator   Validator used for address blocks.
	 *      @param [in] parent                  The parent item.
	 */
	ComponentEditorAddrBlockItem(QSharedPointer<AddressBlock> addrBlock,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<AddressBlockValidator> addressBlockValidator,
		ComponentEditorItem* parent);

	//! The destructor.
	virtual ~ComponentEditorAddrBlockItem() = default;

    //! No copying. No assignment.
    ComponentEditorAddrBlockItem(const ComponentEditorAddrBlockItem& other) = delete;
    ComponentEditorAddrBlockItem& operator=(const ComponentEditorAddrBlockItem& other) = delete;

	/*!
     *  Get the tool tip for the item.
	 * 
	 *      @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const override final;

	/*!
     *  Get the text to be displayed to user in the tree for this item.
	 *
	 *      @return QString Contains the text to display.
	 */
	virtual QString text() const override final;

	/*!
     *  Check the validity of this item and sub items.
	 *
	 *      @return bool True if item is in valid state.
	 */
	virtual bool isValid() const override final;

	/*!
     *  Get pointer to the editor of this item.
	 *
	 *      @return The editor to use for this item.
	 */
	virtual ItemEditor* editor() override final;

	/*!
     *  Add a new child to the item.
	 * 
	 *      @param [in] index The index to add the child into.
	 */
	virtual void createChild(int index) override final;

    virtual void removeChild(int index) override final;

    /*!
     *  Get pointer to the visualizer of this item.
	 * 
	 *      @return The visualizer to use for this item.
	 */
	virtual ItemVisualizer* visualizer() override final;

	/*!
     *  Set the visualizer for this item.
	 *
	 *      @param [in] visualizer The visualizer.
	 */
	void setVisualizer(MemoryMapsVisualizer* visualizer);

	/*!
     *  Get the visualizer graphics item for the address block.
	 *
	 *      @return QGraphicsItem* The graphics item.
	 */
	virtual QGraphicsItem* getGraphicsItem() override final;

	/*!
     *  Update the graphics item of the address block.
	 */
	virtual void updateGraphics() override final;

	/*!
     *  Remove the graphics item of the address block.
	 */
	virtual void removeGraphicsItem() override final;

    /*!
     *  Change the address unit bits in component editor.
     *
     *      @param [in] newAddressUnitBits   The new value for address unit bits.
     */
    void addressUnitBitsChanged(int newAddressUnitBits);

public slots:

    //! Handle the change in item's addressing data.
    void onAddressingChanged();

    //! Handle the change in child item's addressing data.
    void onChildAddressingChanged(int index);

protected slots:
    
    /*!
     *  Handles the redrawing of the visualization of the item.
     */
    virtual void onGraphicsChanged();

signals:
    
    /*!
     *  Signals a change in the address unit bits.
     *
     *      @param [in] newAddressUnitBits  The new address unit bits.
     */
    void changeInAddressUnitBits(int newAddressUnitBits);

    //! Signals a change in the item's address data.
    void addressingChanged();

private:	

	//! The address block being edited.
	QSharedPointer<AddressBlock> addrBlock_;

	//! The visualizer for memory maps.
	MemoryMapsVisualizer* visualizer_;

	//! The graph item which visualizes the address block.
	AddressBlockGraphItem* graphItem_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The number of addressable unit bits in the address block.
    unsigned int addressUnitBits_;

    //! The used address block validator.
    QSharedPointer<AddressBlockValidator> addressBlockValidator_;
};

#endif // COMPONENTEDITORADDRBLOCKITEM_H
