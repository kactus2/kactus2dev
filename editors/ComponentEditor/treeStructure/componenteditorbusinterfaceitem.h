/* 
 *  	Created on: 14.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorbusinterfaceitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORBUSINTERFACEITEM_H
#define COMPONENTEDITORBUSINTERFACEITEM_H

#include "componenteditoritem.h"

#include <IPXACTmodels/businterface.h>

#include <QSharedPointer>

/*! \brief The item for single bus interface in the component editor's navigation tree.
 *
 */
class ComponentEditorBusInterfaceItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] busif                   Pointer to the bus interface being edited.
	 *      @param [in] model                   Pointer to the model that owns the items.
	 *      @param [in] libHandler              Pointer to the instance that manages the library.
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] parent                  Pointer to the parent item.
	 *      @param [in] parentWnd               Pointer to the parent window.
	 */
	ComponentEditorBusInterfaceItem(QSharedPointer<BusInterface> busif,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
		ComponentEditorItem* parent,
        QWidget* parentWnd);

	//! \brief The destructor
	virtual ~ComponentEditorBusInterfaceItem();

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

	/*! \brief Tells if the item can be opened or not.
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
	
	/*! \brief Open the bus definition/abstraction definition in a bus editor.
	 * 
	*/
	virtual void openItem();

private:
	//! \brief No copying
	ComponentEditorBusInterfaceItem(const ComponentEditorBusInterfaceItem& other);

	//! \brief No assignment
	ComponentEditorBusInterfaceItem& operator=(const ComponentEditorBusInterfaceItem& other);

	//! \brief Pointer to the bus interface being edited.
	QSharedPointer<BusInterface> busif_;

	//! \brief Pointer to the parent window.
	QWidget* parentWnd_;

    //! Action to open the bus interface for editing.
    QAction* editAction_;
};

#endif // COMPONENTEDITORBUSINTERFACEITEM_H
