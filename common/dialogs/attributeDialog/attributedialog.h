/* 
 *
 *  Created on: 15.1.2011
 *      Author: Antti Kamppi
 */

#ifndef ATTRIBUTEDIALOG_H_
#define ATTRIBUTEDIALOG_H_

#include <QDialog>
#include <QWidget>
#include <QLineEdit>
#include <QString>

/*! \brief AttributeDialog is a dialog to input a new name-value attribute pair.
 *
 * Dialog constructs a dialog with two input fields for the name and value of
 * the attribute. When this dialog is executed it returns a pointer to the new
 * attribute.
 */
class AttributeDialog: public QDialog {

public:

	//! \brief Attribute contains one name-value pair.
	struct Attribute {

		//! \brief Name of the attribute.
		QString name_;

		//! \brief Value of the attribute.
		QString value_;

		/*! \brief Attribute constructor
		 *
		 * \param name Name of the attribute.
		 * \param value Value of the attribute.
		 */
		Attribute(const QString name, const QString value);
	};

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this dialog.
	 */
	AttributeDialog(QWidget* parent = 0);

	//! \brief The destructor
	virtual ~AttributeDialog();

	/*! \brief Execute the dialog and create a new attribute.
	 *
	 * If user clicks "cancel" an empty pointer is returned. Dialog checks that
	 * both name and value field are not empty and must contain some text. If
	 * either field is empty then dialog is executed again until cancel is
	 * clicked or something is written to empty field.
	 *
	 * The dialog gives the ownership of the attribute to the caller of this
	 * function.
	 *
	 * \return Pointer to an attribute.
	 */
	Attribute* execDialog();

private:

	//! No copying
	AttributeDialog(const AttributeDialog& other);

	//! No assignment
	AttributeDialog& operator=(const AttributeDialog& other);

	//! \brief To input name for the attribute.
	QLineEdit* nameEdit_;

	//! \brief To input value for the attribute
	QLineEdit* valueEdit_;
};

#endif /* ATTRIBUTEDIALOG_H_ */
