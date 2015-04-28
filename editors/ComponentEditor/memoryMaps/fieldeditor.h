/* 
 *  	Created on: 28.8.2012
 *      Author: Antti Kamppi
 * 		filename: fieldeditor.h
 *		Project: Kactus 2
 */

#ifndef FIELDEDITOR_H
#define FIELDEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>
#include <IPXACTmodels/field.h>
#include <IPXACTmodels/component.h>

#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QGroupBox>

class EditableTableView;
class EnumeratedValueModel;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Editor for editing the enumerated values of a field.
//-----------------------------------------------------------------------------
class FieldEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] field       Pointer to the field being edited.
	 *      @param [in] component   Pointer to the component being edited.
	 *      @param [in] handler     Pointer to the instance managing the library.
	 *      @param [in] parent      Pointer to the parent of this editor.
	 */
	FieldEditor(QSharedPointer<Field> field, QSharedPointer<Component> component, LibraryInterface* handler,
        QWidget* parent = 0);

	/*!
	 *  The destructor.
	 */
	virtual ~FieldEditor();

	/*!
	 *  Check for the validity of the editor.
	 *
     *      @return True, if the editor is in a valid state, false otherwise.
	 */
	virtual bool isValid() const;

	/*!
	 *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

signals:

    /*!
     *  Informs of changes to the component editor tree.
     */
    void contentChanged();

private:
	
	//! No copying
	FieldEditor(const FieldEditor& other);

	//! No assignment
	FieldEditor& operator=(const FieldEditor& other);

	//! The view to display the enumerated values.
	EditableTableView* enumView_;

	//! The proxy that does the sorting of enumerated values.
	QSortFilterProxyModel* enumProxy_;

	//! The model that manages the enumerated values.
	EnumeratedValueModel* enumModel_;
};

#endif // FIELDEDITOR_H
