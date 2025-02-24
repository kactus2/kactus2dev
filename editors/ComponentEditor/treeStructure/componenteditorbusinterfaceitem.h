//-----------------------------------------------------------------------------
// File: componenteditorbusinterfaceitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 14.05.2012
//
// Description:
// The item for single bus interface in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORBUSINTERFACEITEM_H
#define COMPONENTEDITORBUSINTERFACEITEM_H

#include <editors/common/ExpressionSet.h>
#include <editors/ComponentEditor/treeStructure/ParameterizableItem.h>

#include <QSharedPointer>

#include <IPXACTmodels/Component/BusInterface.h>


class BusInterface;
class BusInterfaceValidator;
class ExpressionParser;
class PortMapInterface;
class BusInterfaceInterface;
class PortMapInterface;



//-----------------------------------------------------------------------------
//! The item for single bus interface in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorBusInterfaceItem : public ParameterizableItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] busif                   The bus interface being edited.
	 *    @param [in] model                   The model that owns the items.
	 *    @param [in] libHandler              The instance that manages the library.
	 *    @param [in] component               The component being edited.
	 *    @param [in] referenceCounter        The reference counter.
	 *    @param [in] parameterFinder         The parameter finder.
	 *    @param [in] expressionFormatter     The expression formatter.
	 *    @param [in] expressionParser        The expression parser.
	 *    @param [in] validator               The validator for bus interfaces.
	 *    @param [in] busInterface            Interface for accessing bus interfaces.
	 *    @param [in] portMapInterface        Interface for accessing port maps.
	 *    @param [in] parent                  The owner of this item.
	 *    @param [in] parentWnd               The parent window.
	 */
	ComponentEditorBusInterfaceItem(QSharedPointer<BusInterface> busif,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		QSharedPointer<ReferenceCounter> referenceCounter,
		ExpressionSet expressions,
		QSharedPointer<BusInterfaceValidator> validator,
		BusInterfaceInterface* busInterface,
		PortMapInterface* portMapInterface,
		ComponentEditorItem* parent,
		QWidget* parentWnd);

	//! The destructor
	virtual ~ComponentEditorBusInterfaceItem();

	/*! Get the tool tip for the item.
	 *
	 *    @return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! Get the text to be displayed to user in the tree for this item.
	 *
	 *    @return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! Check the validity of this item and sub items.
	 *
	 *    @return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! Get The editor of this item.
	 *
	 *    @return The editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*!
	 *  Returns the possible actions for opening the item.
	 *
	 *    @return The actions to open the item.
	 */
	virtual QList<QAction*> actions();

public slots:

	/*! Opens the abstraction definition; can be called from the drop down menu
	 * and from openItem(), where it can open either the only abstraction deffinition in editor,
	 * or open the dialog with a combo box of bus definition and the list of all abstraction definitions.
	 * 
	 * @return bool True if there are no need to open Bus Deffinition, overwise bool False
	 */
	virtual bool openAbsDefSlot();

	/*! 
	 *  Opens and the Bus Definition by emmitting the signal openBus.	
	 */
	virtual void openBusSlot();


private:
	//! No copying
	ComponentEditorBusInterfaceItem(const ComponentEditorBusInterfaceItem& other);

	//! No assignment
	ComponentEditorBusInterfaceItem& operator=(const ComponentEditorBusInterfaceItem& other);

	/*! 
	 *  Sets the actions to Abstract Definitions in context menu.
	 * 
	 *		@param [in] actionList               The list of actions to be filled
	 */
	void setAbstractionDefinitionActions(QList<QAction*>& actionList);

	//! The bus interface being edited.
	QSharedPointer<BusInterface> busIf_;

	//! The parent window.
	QWidget* parentWnd_;

	//! Action to open the bus interface for editing.
	QAction* editBusDefAction_;

	//! Expression handling tools.
	ExpressionSet expressions_;

	//! Validator for bus interfaces.
	QSharedPointer<BusInterfaceValidator> validator_;

	//! Interface for accessing bus interfaces.
	BusInterfaceInterface* busInterface_;

	//! Interface for accessing port maps.
	PortMapInterface* portMapInterface_;
};

#endif // COMPONENTEDITORBUSINTERFACEITEM_H
