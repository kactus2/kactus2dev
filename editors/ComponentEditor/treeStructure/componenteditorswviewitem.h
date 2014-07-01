/* 
 *  	Created on: 24.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorswviewitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORSWVIEWITEM_H
#define COMPONENTEDITORSWVIEWITEM_H

#include "componenteditoritem.h"
#include <IPXACTmodels/SWView.h>

/*! \brief The item for a single software view in the component editor's navigation tree.
 *
 */
class ComponentEditorSWViewItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 * 
	 * \param swView Pointer to the software view to edit.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorSWViewItem(QSharedPointer<SWView> swView,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorSWViewItem();

	/*! \brief Get the font to be used for text of this item.
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

	/*! \brief Tells if the item can be opened or not.
	 * 
	 * If the hasn't changed then it can be opened.
	*/
	virtual bool canBeOpened() const;
    
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
	ComponentEditorSWViewItem(const ComponentEditorSWViewItem& other);

	//! \brief No assignment
	ComponentEditorSWViewItem& operator=(const ComponentEditorSWViewItem& other);
   
	//! \brief Pointer to the view being edited.
	QSharedPointer<SWView> swView_;

    //! Action to open the SW view for editing.
    QAction* editAction_;
};

#endif // COMPONENTEDITORSWVIEWITEM_H
