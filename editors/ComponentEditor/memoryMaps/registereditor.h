/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: registereditor.h
 *		Project: Kactus 2
 */

#ifndef REGISTEREDITOR_H
#define REGISTEREDITOR_H

#include <editors/ComponentEditor/itemeditor.h>
#include <IPXACTmodels/register.h>

#include <QSharedPointer>
#include <QSortFilterProxyModel>

class EditableTableView;
class RegisterTableModel;
class LibraryInterface;

/*! \brief The editor to edit the details of a register within component editor.
 *
 */
class RegisterEditor : public ItemEditor {
	Q_OBJECT

public:

	//! \brief The default height and width of the editor.
	enum Sizes {
		HEIGHT = 300,
		WIDTH = 700
	};

	/*! \brief The constructor
	 *
	 * \param reg Pointer to the register being edited.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent of this editor.
	 *
	*/
	RegisterEditor(QSharedPointer<Register> reg,
		QSharedPointer<Component> component,
		LibraryInterface* handler, 
		QWidget* parent = 0);

	//! \brief The destructor
	virtual ~RegisterEditor();

	/*! \brief Check for the validity of the editor
	* 
	* \return True if the editor is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

	/*! \brief The size hint for the editor.
	 *
	 * \return QSize contains the size hint.
	*/
	virtual QSize sizeHint() const;

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:

	//! \brief No copying
	RegisterEditor(const RegisterEditor& other);

	//! \brief No assignment
	RegisterEditor& operator=(const RegisterEditor& other);

	//! \brief Pointer to the view that displays the items.
	EditableTableView* view_;

	//! \brief Pointer to the proxy that does the sorting of items.
	QSortFilterProxyModel* proxy_;

	//! \brief Pointer to the model that manages the details of items.
	RegisterTableModel* model_;
};

#endif // REGISTEREDITOR_H
