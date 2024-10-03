//-----------------------------------------------------------------------------
// File: componenteditorbusinterfacesitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Bus interfaces-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORBUSINTERFACESITEM_H
#define COMPONENTEDITORBUSINTERFACESITEM_H

#include "componenteditoritem.h"

#include <editors/common/ExpressionSet.h>

class BusInterface;
class BusInterfaceValidator;
class BusInterfacesValidator;
class ExpressionParser;
class PortMapInterface;
class BusInterfaceInterface;
class PortMapInterface;

//-----------------------------------------------------------------------------
//! The Bus interfaces-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorBusInterfacesItem : public ComponentEditorItem
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
     *      @param [in] busInterface            Interface for accessing bus interfaces.
     *      @param [in] portMapInterface        Interface for accessing port maps.
     *      @param [in] model                   The model that owns the items.
	 *      @param [in] libHandler              The instance that manages the library.
	 *      @param [in] component               The component being edited.
	 *      @param [in] referenceCounter        The reference counter.
	 *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
	 *      @param [in] expressionParser        The expression parser.
	 *      @param [in] parent                  The owner of this item.
	 *      @param [in] parentWnd               The parent window.
	 */
	ComponentEditorBusInterfacesItem(
        BusInterfaceInterface* busInterface,
        PortMapInterface* portMapInterface,
        ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
		ExpressionSet expressions,
        ComponentEditorItem* parent,
        QWidget* parentWnd);

	/*!
     *  The destructor
     */
	virtual ~ComponentEditorBusInterfacesItem() = default;

	/*! Get the font to be used for text of this item.
	*
	* The font is bold, if bus interfaces exist, otherwise not bold.
	*
	*      @return QFont instance that defines the font to be used.
	*/
	virtual QFont getFont() const;

	/*! Get the tool tip for the item.
	 * 
	 *      @return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! Get the text to be displayed to user in the tree for this item.
	 *
	 *      @return The text to display.
	*/
	virtual QString text() const;

	/*! Get The editor of this item.
	 *
	 *      @return The editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! Add a new child to the item.
	 *
	 *      @param [in] index The index to add the child into.
	*/
	virtual void createChild(int index);

	/*! Get the tree item that holds the editor for given bus interface.
	 *
	 *      @param [in] interfaceName The name of the bus interface to get the item for.
	 *
	 *      @return The tree item.
	*/
	QSharedPointer<ComponentEditorItem> getBusInterfaceItem(const QString& interfaceName) const;

	bool isValid() const override;

signals:
	void openAbsDef(VLNV const& absDefVLNV);

private:
	//! No copying
	ComponentEditorBusInterfacesItem(const ComponentEditorBusInterfacesItem& other);

	//! No assignment
	ComponentEditorBusInterfacesItem& operator=(const ComponentEditorBusInterfacesItem& other);

    /*!
     *  Create the validator for bus interfaces.
     */
    void createBusInterfaceValidator();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Contains the bus interfaces to edit.
	QSharedPointer<QList<QSharedPointer<BusInterface> > > busifs_;

    //! The parent window.
    QWidget* parentWnd_;

	//! Expression handling tools.
	ExpressionSet expressions_;

    //! Validator for bus interfaces.
    QSharedPointer<BusInterfaceValidator> busInterfaceValidator_;

	//! Validator for bus interfaces together.
    QSharedPointer<BusInterfacesValidator> busInterfacesValidator_;

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_;

    //! Interface for accessing port maps.
    PortMapInterface* portMapInterface_;
};

#endif // COMPONENTEDITORBUSINTERFACESITEM_H
