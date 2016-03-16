//-----------------------------------------------------------------------------
// File: WriteValueConstraintComboBox.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 28.08.2012
//
// Description:
// Editor for editing the enumerated values of a field.
//-----------------------------------------------------------------------------

#ifndef FIELDEDITOR_H
#define FIELDEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>

#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QGroupBox>

class EditableTableView;
class EnumeratedValueModel;
class LibraryInterface;
class EnumeratedValue;
class EnumeratedValueValidator;
class Component;
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
	 *      @param [in] enumeratedValues            List of enumerated values being edited.
     *      @param [in] enumeratedValueValidator    The validator for enumerated values.
	 *      @param [in] component                   Pointer to the component being edited.
	 *      @param [in] handler                     Pointer to the instance managing the library.
	 *      @param [in] parent                      Pointer to the parent of this editor.
	 */
	FieldEditor(QSharedPointer<QList<QSharedPointer<EnumeratedValue> > > enumeratedValues,
        QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator, QSharedPointer<Component> component,
        LibraryInterface* handler, QWidget* parent = 0);

	/*!
	 *  The destructor.
	 */
	virtual ~FieldEditor();

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
