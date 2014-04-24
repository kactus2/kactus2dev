/* 
 *  	Created on: 27.8.2012
 *      Author: Antti Kamppi
 * 		filename: testconstraintcombobox.h
 *		Project: Kactus 2
 */

#ifndef TESTCONSTRAINTCOMBOBOX_H
#define TESTCONSTRAINTCOMBOBOX_H

#include <IPXACTmodels/generaldeclarations.h>

#include <QComboBox>

/*! \brief The combo box to select one of the defined test constraints.
 *
 */
class TestConstraintComboBox : public QComboBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of the combo box.
	 *
	*/
	TestConstraintComboBox(QWidget *parent);
	
	//! \brief The destructor
	virtual ~TestConstraintComboBox();

	/*! \brief Get the currently selected test constraint.
	 *
	 * \return The currently selected test constraint.
	*/
	General::TestConstraint getCurrentValue() const;

	/*! \brief Set the currently selected value.
	 *
	 * \param testConstraint The test constraint to select.
	 *
	*/
	void setCurrentValue(General::TestConstraint testConstraint);

private:

	//! \brief No copying
	TestConstraintComboBox(const TestConstraintComboBox& other);

	//! \brief No assignment
	TestConstraintComboBox& operator=(const TestConstraintComboBox& other);
};

#endif // TESTCONSTRAINTCOMBOBOX_H
