//-----------------------------------------------------------------------------
// File: componenteditorchannelsitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The channels-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORCHANNELSITEM_H
#define COMPONENTEDITORCHANNELSITEM_H

#include "componenteditoritem.h"

class Channel;
class ChannelValidator;
class ExpressionParser;
//-----------------------------------------------------------------------------
//! The channels-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorChannelsItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] model               The model that owns the items.
	 *      @param [in] libHandler          The instance that manages the library.
	 *      @param [in] component           The component being edited.
     *      @param [in] expressionParser    The expression parser to use.
	 *      @param [in] parent              The parent item.
	*/
	ComponentEditorChannelsItem(ComponentEditorTreeModel* model, 
        LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ExpressionParser> expressionParser,
        ComponentEditorItem* parent);

	//! The destructor
	virtual ~ComponentEditorChannelsItem();

	/*! Get the font to be used for text of this item.
	*
	* The font is bold, if channels exist, otherwise not bold.
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

	/*! Get The editor of this item.
	 *
	 *      @return The editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! Check the validity of this item.
	 *
	 *      @return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

private:
	//! No copying
	ComponentEditorChannelsItem(const ComponentEditorChannelsItem& other);
	ComponentEditorChannelsItem& operator=(const ComponentEditorChannelsItem& other);

	//! The channels being edited.
	QSharedPointer<QList<QSharedPointer<Channel> > > channels_;

    //! Validator for channels.
    QSharedPointer<ChannelValidator> validator_;
};

#endif // COMPONENTEDITORCHANNELSITEM_H
