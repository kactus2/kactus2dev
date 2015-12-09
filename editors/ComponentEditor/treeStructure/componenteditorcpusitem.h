//-----------------------------------------------------------------------------
// File: componenteditorcpusitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author:Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Cpus-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORCPUSITEM_H
#define COMPONENTEDITORCPUSITEM_H

#include "componenteditoritem.h"

#include <QList>

class Cpu;
class CPUValidator;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! The Cpus-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorCpusItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] model               The model that owns the items.
	 *      @param [in] libHandler          The instance that manages the library.
	 *      @param [in] component           The component being edited.
     *      @param [in] expressionParser    Parser for expressions in cpus.
     *      @param [in] parent              The parent item.
     */
    ComponentEditorCpusItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
        QSharedPointer<Component> component, 
        QSharedPointer<ExpressionParser> expressionParser,
        ComponentEditorItem* parent);

	//! The destructor
	virtual ~ComponentEditorCpusItem();

	/*! Get the font to be used for text of this item.
	*
	* The font is bold, if cpus exist, otherwise not bold.
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
	 *      @return The text to display.
	*/
	virtual QString text() const;

	/*! Get The editor of this item.
	 *
	 *      @return The editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! Check the validity of this item and sub items.
	 *
	 *      @return True if item is in valid state.
	*/
	virtual bool isValid() const;

private:
	//! No copying
	ComponentEditorCpusItem(const ComponentEditorCpusItem& other);
	ComponentEditorCpusItem& operator=(const ComponentEditorCpusItem& other);

	//! The cpus being edited
	QSharedPointer<QList<QSharedPointer<Cpu> > > cpus_;

    //! Validator for cpu elements.
    QSharedPointer<CPUValidator> validator_;
};

#endif // COMPONENTEDITORCPUSITEM_H
