/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorportsitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORPORTSITEM_H
#define COMPONENTEDITORPORTSITEM_H

#include "componenteditoritem.h"
#include <IPXACTmodels/port.h>

#include <QList>

/*! \brief The Ports-item in the component editor's navigation tree.
 *
 */
class ComponentEditorPortsItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] model                   Pointer to the model that owns the items.
	 *      @param [in] libHandler              Pointer to the instance that manages the library.
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] refCounter              Pointer to the instance for counting references.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] parent                  Pointer to the parent item.
	 */
	ComponentEditorPortsItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> refCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorPortsItem();

	/*! \brief Get the font to be used for text of this item.
	*
	* The font is bold, if ports exist, otherwise not bold.
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

signals:

    //! Emitted when a new interface should be created in the component editor tree.
    void createInterface();

private:
	//! \brief No copying
	ComponentEditorPortsItem(const ComponentEditorPortsItem& other);

	//! \brief No assignment
	ComponentEditorPortsItem& operator=(const ComponentEditorPortsItem& other);
};

#endif // COMPONENTEDITORPORTSITEM_H
