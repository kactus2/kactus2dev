/* 
 *
 *  Created on: 2.2.2011
 *      Author: Antti Kamppi
 * 		filename: itemeditor.h
 */

#ifndef ITEMEDITOR_H
#define ITEMEDITOR_H

#include <models/component.h>

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSize>

/*! \brief ItemEditor is a base class for editors in Component Editor module.
*
* This class should be used as a base class when implementing an element-specific
* editor. This base class provides means to communicate with 
* ComponentTreeNavigator.
*/
class ItemEditor : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor.
	*
	* \param component Pointer to the component instance that is being edited.
	* \param parent Pointer to the owner of this widget.
	*/
	ItemEditor(QSharedPointer<Component> component, 
		QWidget *parent = 0);

	//! \brief The destructor.
	virtual ~ItemEditor();

	/*! \brief Check for the validity of the edited item.
	* 
	* \return True if item is valid.
	*/
	virtual bool isValid() const = 0;

    /*!
     *  Allows/disallows the editor change.
     *
     *      @return True, if this editor can be hidden without consequences. Otherwise false.
     */
    virtual bool confirmEditorChange();

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh() = 0;

signals:

	/*! \brief Emitted when the contents of the widget change
	*
	* \param change This is set just for convenience so this signal can be
	* connected directly to a widget to set it enabled. This should always be
	* set to true.
	*/
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! \brief Prints a notification to the user.
	void noticeMessage(const QString& msg) const;

	/*! \brief Emitted when the editor adds a new item that should be shown in navigation tree.
	 *
	 * \param index The index of the added item.
	 *
	*/
	void childAdded(int index);

	/*! \brief Emitted when the editor removes a child that should also be removed from navigation tree.
	 *
	 * \param index The index of the removed item.
	 *
	*/
	void childRemoved(int index);

	/*! \brief Emitted when the editor moves child from one position to another.
	 *
	 * \param source The index of the moved item.
	 * \param target The index to move the child to.
	 *
	*/
	void childMoved(int source, int target);

protected:

	/*! \brief Get the the component pointer from the base-class.
	 *
	 *
	 * \return Pointer to the component model that is being edited
	*/
	QSharedPointer<Component> component() const;

private:

	//! No copying
	ItemEditor(const ItemEditor& other);

	//! No assignment
	ItemEditor& operator=(const ItemEditor& other);

	//! \brief A pointer to the component that's element is being edited.
	QSharedPointer<Component> component_;
	
};

#endif // ITEMEDITOR_H
