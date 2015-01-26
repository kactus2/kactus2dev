/* 
 *  	Created on: 14.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorviewitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORVIEWITEM_H
#define COMPONENTEDITORVIEWITEM_H

#include "componenteditoritem.h"

#include <IPXACTmodels/view.h>

#include <QSharedPointer>

/*! \brief The item for a single view in the component editor's navigation tree.
 *
 */
class ComponentEditorViewItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param view Pointer to the view being edited.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param widget Pointer to the QWidget that displays the editor.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorViewItem(QSharedPointer<View> view, 
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorViewItem();

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
	 * If the view is hierarchical and reference hasn't changed then it can be opened.
	*/
	virtual bool canBeOpened() const;

	/*! \brief Tells if the view is hierarchical or not.
	 * 
	 * If the view is hierarchical, returns true, otherwise false.
	*/
    virtual bool isHierarchical() const;

	/*! \brief Tells if the view has a decoration icon or not.
	 * 
	 * If the view has an icon, returns true, otherwise false.
	*/
    virtual bool hasIcon() const;

	/*! \brief Gets the decoration icon for the view.
	 * 
	 * If the view is hierarchical, hierarchy icon is returned, otherwise a null icon.
	*/
    virtual QIcon getIcon() const;

    /*!
     *  Returns the possible actions for opening the item.
     *
     *      @return The actions to open the item.
     */
    virtual QList<QAction*> actions() const;

public slots:
	
	/*! \brief Open the view in a design editor.
	 * 
	*/
	virtual void openItem();

private:
	//! \brief No copying
	ComponentEditorViewItem(const ComponentEditorViewItem& other);

	//! \brief No assignment
	ComponentEditorViewItem& operator=(const ComponentEditorViewItem& other);

	//! \brief Pointer to the view being edited.
	QSharedPointer<View> view_;

    //! Action to open the view for editing.
    QAction* editAction_;
};

#endif // COMPONENTEDITORVIEWITEM_H
