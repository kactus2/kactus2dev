/* 
 *  	Created on: 30.8.2012
 *      Author: Antti Kamppi
 * 		filename: writevalueconstrainteditor.h
 *		Project: Kactus 2
 */

#ifndef WRITEVALUECONSTRAINTEDITOR_H
#define WRITEVALUECONSTRAINTEDITOR_H

#include <IPXACTmodels/writevalueconstraint.h>

#include <QGroupBox>
#include <QSharedPointer>
#include <QRadioButton>
#include <QSpinBox>

/*! \brief The editor to edit the write value constraint of a field.
 *
 */
class WriteValueConstraintEditor : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param writeConstraint Pointer to the write constraint being edited.
	 * \param title The title for the group box.
	 * \param parent Pointer to the owner of the group box.
	 *
	*/
	WriteValueConstraintEditor(QSharedPointer<WriteValueConstraint> writeConstraint,
		const QString& title,
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~WriteValueConstraintEditor();

signals:

	//! \brief Emitted when contents of the editor change.
	void contentChanged();

private slots:

	//! \brief Handler for changes in no constraints button.
	void onNoConstr(bool checked);

	//! \brief Handler for changes in write as read button.
	void onWriteAsRead(bool checked);

	//! \brief Handler for changes in use enum button.
	void onUseEnum(bool checked);

	//! \brief Handler for changes in min max button.
	void onMinMax(bool checked);

	//! \brief Handler for changes in minimum limit.
	void onMinChanged(int newValue);

	//! \brief Handler for changes in maximum limit.
	void onMaxChanged(int newValue);

private:
	
	//! \brief No copying
	WriteValueConstraintEditor(const WriteValueConstraintEditor& other);

	//! \brief No assignment
	WriteValueConstraintEditor& operator=(const WriteValueConstraintEditor& other);

	//! \brief Pointer to the write constraint being edited.
	QSharedPointer<WriteValueConstraint> writeConstraint_;

	//! \brief Button to select no write constraints.
	QRadioButton* noConstrButton_;

	//! \brief Button to select write as read-constraint.
	QRadioButton* writeAsReadButton_;

	//! \brief Button to select enumerated values as constraint.
	QRadioButton* useEnumButton_;

	//! \brief Button to select min and max values as constraints.
	QRadioButton* minMaxButton_;

	//! \brief The spin box to set the minimum limit.
	QSpinBox* minBox_;

	//! \brief The spin box to set the maximum limit.
	QSpinBox* maxBox_;
};

#endif // WRITEVALUECONSTRAINTEDITOR_H
