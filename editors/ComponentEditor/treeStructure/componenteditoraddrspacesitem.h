//-----------------------------------------------------------------------------
// File: componenteditoraddrspacesitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Address spaces-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORADDRSPACESITEM_H
#define COMPONENTEDITORADDRSPACESITEM_H

#include "componenteditoritem.h"
#include <IPXACTmodels/addressspace.h>

#include <QList>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The Address spaces-item in the component editor navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorAddrSpacesItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] model                   The model that owns the items.
	 *      @param [in] libHandler              The instance that manages the library.
	 *      @param [in] component               The component being edited.
     *      @param [in] referenceCounter        The instance for counting references to parameters.
	 *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] expressionParser        The expression parser to use.
	 *      @param [in] parent                  The parent item.
	 */
	ComponentEditorAddrSpacesItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
		ComponentEditorItem* parent);

	//! The destructor
	virtual ~ComponentEditorAddrSpacesItem();

	/*! Get the font to be used for text of this item.
	*
	* The font is bold, if address spaces exist, otherwise not bold.
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
	 *      @return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! Get pointer to the editor of this item.
	 *
	 *      @return The editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! Add a new child to the item.
	 * 
	 *      @param [in] index The index to add the child into.
	*/
	virtual void createChild(int index);

private:
	//! No copying
	ComponentEditorAddrSpacesItem(const ComponentEditorAddrSpacesItem& other);
	ComponentEditorAddrSpacesItem& operator=(const ComponentEditorAddrSpacesItem& other);

	//! The address spaces being edited.
	QList<QSharedPointer<AddressSpace> >& addrSpaces_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // COMPONENTEDITORADDRSPACESITEM_H
