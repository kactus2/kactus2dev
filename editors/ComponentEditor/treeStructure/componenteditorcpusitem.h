//-----------------------------------------------------------------------------
// File: componenteditorcpusitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author:Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Cpus-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORCPUSITEM_H
#define COMPONENTEDITORCPUSITEM_H

#include "componenteditoritem.h"

#include <editors/common/ExpressionSet.h>

#include <QList>

class Cpu;
class CPUValidator;
class CPUsValidator;
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
	 *    @param [in] model               The model that owns the items.
	 *    @param [in] libHandler          The instance that manages the library.
	 *    @param [in] component           The component being edited.
     *    @param [in] expressions			Parser for expressions in cpus.
     *    @param [in] parent              The parent item.
     */
    ComponentEditorCpusItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
        QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> referenceCounter,
		ExpressionSet expressions,
        ComponentEditorItem* parent);

	//! The destructor
	virtual ~ComponentEditorCpusItem() = default;

    //! No copying
    ComponentEditorCpusItem(const ComponentEditorCpusItem& other) = delete;
    ComponentEditorCpusItem& operator=(const ComponentEditorCpusItem& other) = delete;

	/*! Get the font to be used for text of this item.
	*
	* The font is bold, if cpus exist, otherwise not bold.
	*
	*    @return QFont instance that defines the font to be used.
	*/
	QFont getFont() const final;

	/*! Get the tool tip for the item.
	 * 
	 *    @return The text for the tool tip to print to user.
	*/
	QString getTooltip() const final;

	/*! Get the text to be displayed to user in the tree for this item.
	 *
	 *    @return The text to display.
	*/
	QString text() const final;

	/*! Get The editor of this item.
	 *
	 *    @return The editor to use for this item.
	*/
	ItemEditor* editor() final;

	/*! Check the validity of this item and sub items.
	 *
	 *    @return True if item is in valid state.
	*/
	bool isValid() const final;

	/*!
     *  Add a new child to the item.
	 *
	 *    @param [in] index   The index to add the child into.
	 */
	void createChild(int index) final;


private:

	//! The cpus being edited
	QSharedPointer<QList<QSharedPointer<Cpu> > > cpus_;

    //! Validator for cpu elements.
    QSharedPointer<CPUValidator> singleCpuValidator_;

	//! Validator for validating cpus together.
    QSharedPointer<CPUsValidator> cpusValidator_;

	ExpressionSet expressions_;
};

#endif // COMPONENTEDITORCPUSITEM_H
