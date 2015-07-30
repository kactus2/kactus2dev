/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorbusinterfacesitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORBUSINTERFACESITEM_H
#define COMPONENTEDITORBUSINTERFACESITEM_H

#include "componenteditoritem.h"

class ExpressionParser;

/*! \brief The Bus interfaces-item in the component editor's navigation tree.
 *
 */
class ComponentEditorBusInterfacesItem : public ComponentEditorItem {
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] model                   Pointer to the model that owns the items.
	 *      @param [in] libHandler              Pointer to the instance that manages the library.
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] referenceCounter        Pointer to the reference counter.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] expressionParser        Pointer to the expression parser.
	 *      @param [in] parent                  Pointer to the owner of this item.
	 *      @param [in] parentWnd               Pointer to the parent window.
	 */
	ComponentEditorBusInterfacesItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
		ComponentEditorItem* parent,
        QWidget* parentWnd);

	//! \brief The destructor
	virtual ~ComponentEditorBusInterfacesItem();

	/*! \brief Get the font to be used for text of this item.
	*
	* The font is bold, if bus interfaces exist, otherwise not bold.
	*
	* \return QFont instance that defines the font to be used.
	*/
	virtual QFont getFont() const;

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

	/*! \brief Get the tree item that holds the editor for given bus interface.
	 *
	 * \param interfaceName The name of the bus interface to get the item for.
	 *
	 * \return Pointer to the tree item.
	*/
	QSharedPointer<ComponentEditorItem> getBusInterfaceItem(const QString& interfaceName) const;

private:
	//! \brief No copying
	ComponentEditorBusInterfacesItem(const ComponentEditorBusInterfacesItem& other);

	//! \brief No assignment
	ComponentEditorBusInterfacesItem& operator=(const ComponentEditorBusInterfacesItem& other);

	//! \brief Contains the bus interfaces to edit.
	QList<QSharedPointer<BusInterface> >& busifs_;

    //! The parent window.
    QWidget* parentWnd_;

    //! The expression parser used to define the results of expressions.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // COMPONENTEDITORBUSINTERFACESITEM_H
