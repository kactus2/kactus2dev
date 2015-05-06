//-----------------------------------------------------------------------------
// File: componenteditoraddrblockitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 23.08.2012
//
// Description:
// The item for a single address block in component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORADDRBLOCKITEM_H
#define COMPONENTEDITORADDRBLOCKITEM_H

#include <editors/ComponentEditor/treeStructure/componenteditoritem.h>

#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/registermodel.h>

#include <QFont>
#include <QSharedPointer>

class MemoryMapsVisualizer;
class AddressBlockGraphItem;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! The item for a single address block in component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorAddrBlockItem : public ComponentEditorItem {
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
		ComponentEditorItem* parent);

	//! The destructor
	virtual ~ComponentEditorAddrBlockItem();

	/*! Get the tool tip for the item.
	 * 
	 *      @return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! Get the text to be displayed to user in the tree for this item.
	 *
	 *      @return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! Check the validity of this item and sub items.
	 *
	 *      @return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! Get pointer to the editor of this item.
	 *
	 *      @return The editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! Add a new child to the item.
	 * 
	 *      @param [in] index The index to add the child into.
	 *
	*/
	virtual void createChild(int index);

	/*! Get pointer to the visualizer of this item.
	 * 
	 * 
	 *      @return The visualizer to use for this item.
	*/
	virtual ItemVisualizer* visualizer();

	/*! Set the visualizer for this item.
	 *
	 *      @param [in] visualizer The visualizer.
	*/
	virtual void setVisualizer(MemoryMapsVisualizer* visualizer);

	/*! Get the visualizer graphics item for the address block.
	 *
	 *      @return QGraphicsItem* The graphics item.
	*/
	virtual QGraphicsItem* getGraphicsItem();

	/*! Update the graphics item of the address block.
	 *
	*/
	virtual void updateGraphics();

	/*! Remove the graphics item of the address block.
	 *
	*/
	virtual void removeGraphicsItem();

    /*!
     *  Change the address unit bits in component editor.
     *
     *      @param [in] newAddressUnitBits   The new value for address unit bits.
     */
    void addressUnitBitsChanged(int newAddressUnitBits);

protected slots:

	/*! Handler for editor's contentChanged signal.
	 *
	*/
	virtual void onEditorChanged();
    
    /*!
     *  Handles the redrawing of the visualization of the item.
     */
    virtual void onGraphicsChanged();

signals:
    void changeInAddressUnitBits(int newAddressUnitBits);

private:
	
	//! No copying
	ComponentEditorAddrBlockItem(const ComponentEditorAddrBlockItem& other);
	ComponentEditorAddrBlockItem& operator=(const ComponentEditorAddrBlockItem& other);

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
};

#endif // COMPONENTEDITORADDRBLOCKITEM_H
