//-----------------------------------------------------------------------------
// File: componenteditorbusinterfaceitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.05.2012
//
// Description:
// The item for single bus interface in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORBUSINTERFACEITEM_H
#define COMPONENTEDITORBUSINTERFACEITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>

class BusInterface;
class BusInterfaceValidator;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! The item for single bus interface in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorBusInterfaceItem : public ComponentEditorItem
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] busif                   The bus interface being edited.
	 *      @param [in] model                   The model that owns the items.
	 *      @param [in] libHandler              The instance that manages the library.
	 *      @param [in] component               The component being edited.
	 *      @param [in] referenceCounter        The reference counter.
	 *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
	 *      @param [in] expressionParser        The expression parser.
     *      @param [in] validator               The validator for bus interfaces.
	 *      @param [in] parent                  The owner of this item.
	 *      @param [in] parentWnd               The parent window.
	 */
	ComponentEditorBusInterfaceItem(QSharedPointer<BusInterface> busif,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<BusInterfaceValidator> validator,
		ComponentEditorItem* parent,
        QWidget* parentWnd);

	//! The destructor
	virtual ~ComponentEditorBusInterfaceItem();

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

	/*! Get The editor of this item.
	 *
	 *      @return The editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! Tells if the item can be opened or not.
	 * 
	 * If the bus interface contains valid bus definition reference is can be opened.
	*/
	virtual bool canBeOpened() const;

    /*!
     *  Returns the possible actions for opening the item.
     *
     *      @return The actions to open the item.
     */
    virtual QList<QAction*> actions() const;

public slots:
	
	/*! Open the bus definition/abstraction definition in a bus editor.
	 * 
	*/
	virtual void openItem();

private:
	//! No copying
	ComponentEditorBusInterfaceItem(const ComponentEditorBusInterfaceItem& other);

	//! No assignment
	ComponentEditorBusInterfaceItem& operator=(const ComponentEditorBusInterfaceItem& other);

	//! The bus interface being edited.
	QSharedPointer<BusInterface> busif_;

	//! The parent window.
	QWidget* parentWnd_;

    //! Action to open the bus interface for editing.
    QAction* editAction_;

    //! The expression parse used to form the results of the expressions.
    QSharedPointer<ExpressionParser> expressionParser_;

    QSharedPointer<BusInterfaceValidator> validator_;
};

#endif // COMPONENTEDITORBUSINTERFACEITEM_H
