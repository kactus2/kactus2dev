/* 
 *  	Created on: 28.8.2012
 *      Author: Antti Kamppi
 * 		filename: fieldeditor.h
 *		Project: Kactus 2
 */

#ifndef FIELDEDITOR_H
#define FIELDEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <models/field.h>
#include <models/component.h>

#include <QSharedPointer>
#include <QSortFilterProxyModel>

class EditableTableView;
class EnumeratedValueModel;
class WriteValueConstraintEditor;

/*! \brief The editor to edit the enumerated values of a field.
 *
 */
class FieldEditor : public ItemEditor {
	Q_OBJECT

public:

	//! \brief The default height and width of the editor.
	enum Sizes {
		HEIGHT = 300,
		WIDTH = 700
	};

	/*! \brief The constructor
	 * 
	 * \param field Pointer to the field being edited.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent of this editor.
	 *
	 */
	FieldEditor(QSharedPointer<Field> field,
		QSharedPointer<Component> component,
		QWidget* parent = 0);

	//! \brief The destructor
	virtual ~FieldEditor();

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
	FieldEditor(const FieldEditor& other);

	//! \brief No assignment
	FieldEditor& operator=(const FieldEditor& other);

	//! \brief The view to display the enumerated values.
	EditableTableView* enumView_;

	//! \brief The proxy that does the sorting of enumerated values.
	QSortFilterProxyModel* enumProxy_;

	//! \brief The model that manages the enumerated values.
	EnumeratedValueModel* enumModel_;

	//! \brief The editor to set the write constraints of field.
	WriteValueConstraintEditor* writeConstr_;
};

#endif // FIELDEDITOR_H
