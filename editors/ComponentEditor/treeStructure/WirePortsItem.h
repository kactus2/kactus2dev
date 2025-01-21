//-----------------------------------------------------------------------------
// File: WirePortsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 22.11.2023
//
// Description:
// The Wire ports-item in the component editor's navigation tree. 
//-----------------------------------------------------------------------------

#ifndef WIRE_PORTS_ITEM_H
#define WIRE_PORTS_ITEM_H

#include "componenteditoritem.h"

#include <editors/common/ExpressionSet.h>

#include <QList>

class Port;
class ExpressionParser;
class PortValidator;
class PortsInterface;
class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! The Wire ports-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class WirePortsItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] model                   The model that owns the items.
	 *    @param [in] libHandler              The instance that manages the library.
	 *    @param [in] component               The component being edited.
	 *    @param [in] refCounter              The instance for counting references.
	 *    @param [in] parameterFinder         The parameter finder.
	 *    @param [in] expressionFormatter     The expression formatter.
     *    @param [in] expressionParser        The expression parser.
     *    @param [in] busInterface            Interface for accessing bus interfaces.
	 *    @param [in] parent                  The parent item.
	 */
	WirePortsItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> refCounter,
        ExpressionSet expressions,
        QSharedPointer<PortsInterface> portsInterface,
        BusInterfaceInterface* busInterface,
		ComponentEditorItem* parent);

	//! The destructor
	~WirePortsItem() = default;

	/*!
     *  Get the tool tip for the item.
	 * 
	 *    @return The text for the tool tip to print to user.
	 */
	QString getTooltip() const final;

	/*!
     *  Get the text to be displayed to user in the tree for this item.
	 *
	 *    @return QString Contains the text to display.
	 */
	QString text() const final;

	/*!
     *  Check the validity of this item and sub items.
	 *
	 *    @return bool True if item is in valid state.
	 */
	bool isValid() const final;

	/*!
     *  Get pointer to the editor of this item.
	 *
	 *    @return The editor to use for this item.
	 */
	ItemEditor* editor() final;

	/*! 
	 *  Get the font to be used for text of this item.
	 *	The font is bold, if wire ports exist, otherwise regular.
	 *
	 *    @return QFont instance that defines the font to be used.
	 */
	QFont getFont() const final;

signals:

    //! Emitted when a new interface should be created in the component editor tree.
    void createInterface();

private:


	ExpressionSet expressions_;

    //! The used port validator.
    QSharedPointer<PortValidator> portValidator_;

	//! Interface for accessing ports.
	QSharedPointer<PortsInterface> portsInterface_;

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_;
};

#endif // WIRE_PORTS_ITEM_H
