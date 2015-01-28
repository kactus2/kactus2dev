/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormemmapitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORMEMMAPITEM_H
#define COMPONENTEDITORMEMMAPITEM_H

#include "componenteditoritem.h"
#include <IPXACTmodels/memorymap.h>

#include <QSharedPointer>
#include <QList>

class MemoryMapEditor;
class MemoryMapsVisualizer;
class MemoryMapGraphItem;

/*! \brief The item for a single memory map in component editor's navigation tree.
 *
 */
class ComponentEditorMemMapItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] memoryMap               Pointer to the memory map being edited.
	 *      @param [in] model                   Pointer to the model that owns the items.
	 *      @param [in] libHandler              Pointer to the instance that manages the library.
	 *      @param [in] component               Pointer to the component being edited.
     *      @param [in] referenceCounter        Pointer to the instance for counting references to parameters.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] parent                  Pointer to the parent item.
	 */
	ComponentEditorMemMapItem(QSharedPointer<MemoryMap> memoryMap,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorMemMapItem();

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

	/*! \brief Get the visualizer graphics item for the memory map.
	 *
	 * \return QGraphicsItem* Pointer to the graphics item.
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

signals:

    /*!
     *  Emitted when address unit bits have been changed.
     *
     *      @param [in] newAddressUnitBits  The new address unit bit value.
     */
    void addressUnitBitsChanged(int newAddressUnitBits);

private:
	//! \brief No copying
	ComponentEditorMemMapItem(const ComponentEditorMemMapItem& other);

	//! \brief No assignment
	ComponentEditorMemMapItem& operator=(const ComponentEditorMemMapItem& other);

	//! \brief Pointer to the memory map being edited.
	QSharedPointer<MemoryMap> memoryMap_;

	//! \brief Contains the address blocks that are children of this tree item.
	QList<QSharedPointer<MemoryMapItem> >& items_;

	//! \brief The visualizer to display the memory maps
	MemoryMapsVisualizer* visualizer_;

	//! \brief Pointer to the graph item which visualizes this memory map.
	MemoryMapGraphItem* graphItem_;
};

#endif // COMPONENTEDITORMEMMAPITEM_H
