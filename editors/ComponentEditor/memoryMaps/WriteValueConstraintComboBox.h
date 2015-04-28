//-----------------------------------------------------------------------------
// File: WriteValueConstraintComboBox.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.04.2015
//
// Description:
// Combo box for the write value constraint of a field.
//-----------------------------------------------------------------------------

#ifndef WRITEVALUECONSTRAINTCOMBOBOX_H
#define WRITEVALUECONSTRAINTCOMBOBOX_H

#include <IPXACTmodels/writevalueconstraint.h>

#include <QSharedPointer>
#include <QComboBox>

//-----------------------------------------------------------------------------
//! Combo box for the write value constraint of a field.
//-----------------------------------------------------------------------------
class WriteValueConstraintComboBox : public QComboBox
{
    Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] writeConstraint     Pointer to the write constraint being edited.
	 *      @param [in] parent              Pointer to the owner of the group box.
	 */
	WriteValueConstraintComboBox(QSharedPointer<WriteValueConstraint> writeConstraint, QWidget* parent);

    /*!
     *  The destructor.
     */
    virtual ~WriteValueConstraintComboBox();

private slots:

    /*!
     *  Sets the type for the write constraint.
     *
     *      @param [in] newIndex    The index of the selected write constraint type.
     */
    void onIndexChanged(int newIndex);

private:
	
	//! No copying
    WriteValueConstraintComboBox(const WriteValueConstraintComboBox& other);

	//! No assignment
    WriteValueConstraintComboBox& operator=(const WriteValueConstraintComboBox& other);

	//! Pointer to the write constraint being edited.
	QSharedPointer<WriteValueConstraint> writeConstraint_;
};

#endif // WRITEVALUECONSTRAINTCOMBOBOX_H
