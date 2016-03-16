//-----------------------------------------------------------------------------
// File: componenteditorotherclocksitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Other clock drivers-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITOROTHERCLOCKSITEM_H
#define COMPONENTEDITOROTHERCLOCKSITEM_H

#include "componenteditoritem.h"

class Component;
class OtherClockDriver;

class ExpressionParser;
class OtherClockDriverValidator;
//-----------------------------------------------------------------------------
//! The Other clock drivers-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorOtherClocksItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *      @param [in] model               The model that owns the items.
	 *      @param [in] libHandler          The instance that manages the library.
	 *      @param [in] component           The component being edited.
     *      @param [in] expressionParser    The used expression parser.
	 *      @param [in] parent              The parent item.
	 */
	ComponentEditorOtherClocksItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
		QSharedPointer<Component> component, QSharedPointer<ExpressionParser> expressionParser,
        ComponentEditorItem* parent);

	//! The destructor.
	virtual ~ComponentEditorOtherClocksItem();

	/*!
     *  Get the font to be used for text of this item.
	 *
	 *  The font is bold, if other clock drivers exist, otherwise not bold.
	 *
	 *      @return The instance that defines the font to be used.
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
	 *      @return The text to display.
	 */
	virtual QString text() const;

	/*!
     *  Check the validity of this item and sub items.
	 *
	 *      @return True if item is in valid state.
	 */
	virtual bool isValid() const;

	/*!
     *Get The editor of this item.
	 *
	 *      @return The editor to use for this item.
	 */
	virtual ItemEditor* editor();

private:

    //! No copying.
	ComponentEditorOtherClocksItem(const ComponentEditorOtherClocksItem& other);
	ComponentEditorOtherClocksItem& operator=(const ComponentEditorOtherClocksItem& other);

	//! The other clock drivers to edit.
	QSharedPointer<QList<QSharedPointer<OtherClockDriver> > > otherClocks_;

    //! The validator used for other clock drivers.
    QSharedPointer<OtherClockDriverValidator> clockValidator_;
};

#endif // COMPONENTEDITOROTHERCLOCKSITEM_H
