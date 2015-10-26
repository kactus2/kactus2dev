//-----------------------------------------------------------------------------
// File: componenteditorviewitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.05.2012
//
// Description:
// The item for a single view in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORVIEWITEM_H
#define COMPONENTEDITORVIEWITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>

class View;

class LibraryInterface;
class ComponentInstantiation;
class DesignInstantiation;
class DesignConfigurationInstantiation;

//-----------------------------------------------------------------------------
//! The item for a single view in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorViewItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] view                    Pointer to the view being edited.
	 *      @param [in] model                   Pointer to the model that owns the items.
	 *      @param [in] libHandler              Pointer to the instance that manages the library.
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
     *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] parent                  Pointer to the parent item.
	 */
	ComponentEditorViewItem(QSharedPointer<View> view,
        ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        ComponentEditorItem* parent);

	//! The destructor.
	virtual ~ComponentEditorViewItem();

	/*!
     *  Get the tool tip for the item.
	 * 
	 *      @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const;

	/*!
     *  Get the text to be displayed to user in the tree for this item.
	 *
	 *      @return QString Contains the text to display.
	 */
	virtual QString text() const;

	/*!
     *  Check the validity of this item and sub items.
	 *
	 *      @return bool True if item is in valid state.
	 */
	virtual bool isValid() const;

	/*!
     *  Get pointer to the editor of this item.
	 *
	 *      @return Pointer to the editor to use for this item.
	 */
	virtual ItemEditor* editor();

	/*!
     *  Tells if the item can be opened or not.
	 * 
	 *  If the view is hierarchical and reference hasn't changed then it can be opened.
	 */
	virtual bool canBeOpened() const;

	/*!
     *  Tells if the view is hierarchical or not.
	 * 
	 *  If the view is hierarchical, returns true, otherwise false.
	 */
    virtual bool isHierarchical() const;

	/*!
     *  Tells if the view has a decoration icon or not.
	 * 
	 *  If the view has an icon, returns true, otherwise false.
	 */
    virtual bool hasIcon() const;

	/*!
     *  Gets the decoration icon for the view.
	 * 
	 *  If the view is hierarchical, hierarchy icon is returned, otherwise a null icon.
	 */
    virtual QIcon getIcon() const;

    /*!
     *  Returns the possible actions for opening the item.
     *
     *      @return The actions to open the item.
     */
    virtual QList<QAction*> actions() const;

public slots:
	
	/*!
     *  Open the view in a design editor.
	 */
	virtual void openItem();

private:
	
    /*!
     *  Get the component instantiation referenced by the view.
     *
     *      @return The referenced component instantiation.
     */
    QSharedPointer<ComponentInstantiation> getComponentInstantiation() const;

    /*!
     *  Get the design instantiation referenced by the view.
     *
     *      @return The referenced design instantiation.
     */
    QSharedPointer<DesignInstantiation> getDesignInstantiation() const;

    /*!
     *  Get the design configuration instantiation referenced by the view.
     *
     *      @return The referenced design configuration instantiation.
     */
    QSharedPointer<DesignConfigurationInstantiation> getDesignConfigurationInstantiation() const;

    //! No copying.
	ComponentEditorViewItem(const ComponentEditorViewItem& other);

	//! No assignment.
	ComponentEditorViewItem& operator=(const ComponentEditorViewItem& other);

	//! Pointer to the view being edited.
	QSharedPointer<View> view_;

    //! Action to open the view for editing.
    QAction* editAction_;
};

#endif // COMPONENTEDITORVIEWITEM_H
