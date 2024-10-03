//-----------------------------------------------------------------------------
// File: componenteditorportsitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Ports-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORPORTSITEM_H
#define COMPONENTEDITORPORTSITEM_H

#include "componenteditoritem.h"
#include <editors/common/ExpressionSet.h>

#include <QList>

class Port;
class ExpressionParser;
class PortValidator;
class PortsValidator;
class PortsInterface;
class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! The Ports-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorPortsItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] model                   The model that owns the items.
	 *      @param [in] libHandler              The instance that manages the library.
	 *      @param [in] component               The component being edited.
	 *      @param [in] refCounter              The instance for counting references.
	 *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] expressionParser        The expression parser.
     *      @param [in] busInterface            Interface for accessing bus interfaces.
	 *      @param [in] parent                  The parent item.
	 */
	ComponentEditorPortsItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> refCounter, ExpressionSet expressions,
        BusInterfaceInterface* busInterface,
		ComponentEditorItem* parent);

	//! The destructor
	~ComponentEditorPortsItem() = default;

    //! No copying
    ComponentEditorPortsItem(const ComponentEditorPortsItem& other) = delete;

    //! No assignment
    ComponentEditorPortsItem& operator=(const ComponentEditorPortsItem& other) = delete;

	/*!
     *  Get the font to be used for text of this item.
	 *
	 *  The font is bold, if ports exist, otherwise not bold.
	 *
	 *      @return QFont instance that defines the font to be used.
	 */
	virtual QFont getFont() const;

	/*!
     *  Get the tool tip for the item.
	 * 
	 *      @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const;

	/*!
     *  Get the text to be displayed to user in the tree for this item.
	 *
	 *      @return QString Contains the text to display.
	 */
	virtual QString text() const;

	/*!
     *  Check the validity of this item and sub items.
	 *
	 *      @return bool True if item is in valid state.
	 */
	virtual bool isValid() const;

	/*!
     *  Get pointer to the editor of this item.
	 *
	 *      @return The editor to use for this item.
	 */
	virtual ItemEditor* editor();

signals:

    //! Emitted when a new interface should be created in the component editor tree.
    void createInterface();

private:

	ExpressionSet expressions_;

    //! The used port validator.
    QSharedPointer<PortValidator> portValidator_;

	//! The validator used to validate ports together.
	QSharedPointer<PortsValidator> portsValidator_;

	//! Interface for accessing ports.
	QSharedPointer<PortsInterface> portsInterface_;

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_;
};

#endif // COMPONENTEDITORPORTSITEM_H
