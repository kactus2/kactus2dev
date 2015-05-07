//-----------------------------------------------------------------------------
// File: componenteditoraddrspaceitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.05.2012
//
// Description:
// The item for a single address space in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORADDRSPACEITEM_H
#define COMPONENTEDITORADDRSPACEITEM_H

#include "componenteditoritem.h"

#include <IPXACTmodels/addressspace.h>
#include <IPXACTmodels/memorymap.h>

#include <editors/ComponentEditor/addressSpaces/addressspaceeditor.h>
#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizer.h>

class LocalMemoryMapGraphItem;
class MemoryMapsVisualizer;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! The item for a single address space in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorAddrSpaceItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] addrSpace               The address space being edited.
	 *      @param [in] model                   The model that owns the items.
	 *      @param [in] libHandler              The instance that manages the library.
	 *      @param [in] component               The component being edited.
     *      @param [in] referenceCounter        The instance for counting references to parameters.
	 *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
	 *      @param [in] expressionParser        The expression parser to use.
	 *      @param [in] parent                  The parent item.
	 */
	ComponentEditorAddrSpaceItem(QSharedPointer<AddressSpace> addrSpace,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
		ComponentEditorItem* parent);

	//! The destructor
	virtual ~ComponentEditorAddrSpaceItem();

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
	
	/*! Get the visualizer graphics item for the memory map.
	 *
	 *      @return QGraphicsItem* The graphics item.
	*/
	virtual QGraphicsItem* getGraphicsItem();

	/*! Update the graphics item of the memory map.
	 *
	*/
	virtual void updateGraphics();

	/*! Remove the graphics item of the memory map.
	 *
	*/
	virtual void removeGraphicsItem();

	/*! Get pointer to the visualizer of this item.
	 * 
	 * 
	 *      @return The visualizer to use for this item.
	*/
	virtual ItemVisualizer* visualizer();

protected slots:

	/*! Handler for editor's contentChanged signal.
	 *
	*/
	virtual void onGraphicsChanged();

private:
	//! No copying
	ComponentEditorAddrSpaceItem(const ComponentEditorAddrSpaceItem& other);

	//! No assignment
	ComponentEditorAddrSpaceItem& operator=(const ComponentEditorAddrSpaceItem& other);

	//! The address space being edited.
	QSharedPointer<AddressSpace> addrSpace_;

	//! The local memory map of the address space.
	QSharedPointer<MemoryMap> localMemMap_;

	//! Contains the address blocks that are children of this tree item.
	QList<QSharedPointer<MemoryMapItem> >& items_;

	//! The graph item of the local memory map.
	LocalMemoryMapGraphItem* graphItem_;

	//! The visualizer for the local memory maps.
	MemoryMapsVisualizer* localMemMapVisualizer_;

	//! The visualizer of this tree item, visualizes the relation of segments and local memory map. 
	AddressSpaceVisualizer* addrSpaceVisualizer_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // COMPONENTEDITORADDRSPACEITEM_H
