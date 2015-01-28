/* 
 *  	Created on: 23.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrblockitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORADDRBLOCKITEM_H
#define COMPONENTEDITORADDRBLOCKITEM_H

#include <editors/ComponentEditor/treeStructure/componenteditoritem.h>
#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/registermodel.h>

#include <QFont>
#include <QSharedPointer>

class AddressBlockEditor;
class MemoryMapsVisualizer;
class AddressBlockGraphItem;

/*! \brief The item for a single address block in component editor's navigation tree.
 *
 */
class ComponentEditorAddrBlockItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] addrBlock               Pointer to the address block being edited.
	 *      @param [in] model                   Pointer to the model that owns the items.
	 *      @param [in] libHandler              Pointer to the instance that manages the library.
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] parent                  Pointer to the parent item.
	 */
	ComponentEditorAddrBlockItem(QSharedPointer<AddressBlock> addrBlock,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorAddrBlockItem();

	/*! \brief Get the tool tip for the item.
	 * 
	 * \return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! \brief Get the text to be displayed to user in the tree for this item.
	 *
	 * \return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! \brief Check the validity of this item and sub items.
	 *
	 * \return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! \brief Add a new child to the item.
	 * 
	 * \param index The index to add the child into.
	 *
	*/
	virtual void createChild(int index);

	/*! \brief Get pointer to the visualizer of this item.
	 * 
	 * 
	 * \return Pointer to the visualizer to use for this item.
	*/
	virtual ItemVisualizer* visualizer();

	/*! \brief Set the visualizer for this item.
	 *
	 * \param visualizer Pointer to the visualizer.
	 *
	*/
	virtual void setVisualizer(MemoryMapsVisualizer* visualizer);

	/*! \brief Get the visualizer graphics item for the address block.
	 *
	 * \return QGraphicsItem* Pointer to the graphics item.
	*/
	virtual QGraphicsItem* getGraphicsItem();

	/*! \brief Update the graphics item of the address block.
	 *
	*/
	virtual void updateGraphics();

	/*! \brief Remove the graphics item of the address block.
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

	/*! \brief Handler for editor's contentChanged signal.
	 *
	*/
	virtual void onEditorChanged();

signals:
    void changeInAddressUnitBits(int newAddressUnitBits);

private:
	
	//! \brief No copying
	ComponentEditorAddrBlockItem(const ComponentEditorAddrBlockItem& other);

	//! \brief No assignment
	ComponentEditorAddrBlockItem& operator=(const ComponentEditorAddrBlockItem& other);

	//! \brief Pointer to the address block being edited.
	QSharedPointer<AddressBlock> addrBlock_;

	//! \brief Contains the registers that belong to this address block
	QList<QSharedPointer<RegisterModel> >& regItems_;

	//! \brief The visualizer for memory maps.
	MemoryMapsVisualizer* visualizer_;

	//! \brief Pointer to the graph item which visualizes the address block.
	AddressBlockGraphItem* graphItem_;

    unsigned int addressUnitBits_;
};

#endif // COMPONENTEDITORADDRBLOCKITEM_H
