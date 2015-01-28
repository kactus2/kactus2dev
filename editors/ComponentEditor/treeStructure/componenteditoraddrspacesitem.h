/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrspacesitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORADDRSPACESITEM_H
#define COMPONENTEDITORADDRSPACESITEM_H

#include "componenteditoritem.h"
#include <IPXACTmodels/addressspace.h>

#include <QList>

/*! \brief The Address spaces-item in the component editor navigation tree.
*
*/
class ComponentEditorAddrSpacesItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] model                   Pointer to the model that owns the items.
	 *      @param [in] libHandler              Pointer to the instance that manages the library.
	 *      @param [in] component               Pointer to the component being edited.
     *      @param [in] referenceCounter        Pointer to the instance for counting references to parameters.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] parent                  Pointer to the parent item.
	 */
	ComponentEditorAddrSpacesItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorAddrSpacesItem();

	/*! \brief Get the font to be used for text of this item.
	*
	* The font is bold, if address spaces exist, otherwise not bold.
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

private:
	//! \brief No copying
	ComponentEditorAddrSpacesItem(const ComponentEditorAddrSpacesItem& other);

	//! \brief No assignment
	ComponentEditorAddrSpacesItem& operator=(const ComponentEditorAddrSpacesItem& other);

	//! \brief The address spaces being edited.
	QList<QSharedPointer<AddressSpace> >& addrSpaces_;
};

#endif // COMPONENTEDITORADDRSPACESITEM_H
