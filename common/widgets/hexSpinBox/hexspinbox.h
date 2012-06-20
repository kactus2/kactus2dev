/* 
 *
 *  Created on: 1.4.2011
 *      Author: Antti Kamppi
 * 		filename: hexspinbox.h
 */

#ifndef HEXSPINBOX_H
#define HEXSPINBOX_H

#include <QSpinBox>
#include <QValidator>
#include <QRegExpValidator>

/*! \brief Widget for user to set a hexadecimal value in a spin box.
 *
 */
class HexSpinBox : public QSpinBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 * 
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	HexSpinBox(QWidget *parent = 0);
	
	//! \brief The destructor
	virtual ~HexSpinBox();

protected:

	/*! \brief Validate the text to check if it is valid hexadecimal number
	 *
	 * \param text The text to be validated
	 * \param pos The cursor position in the text.
	 *
	 * \return QValidator::State The result of the validation.
	*/
	virtual QValidator::State validate(QString& text, int& pos) const;

	/*! \brief Get the value from the textual input.
	 *
	 * \param text The input 
	 *
	 * \return The number that was given as text.
	*/
	virtual int valueFromText(const QString &text) const;

	/*! \brief Convert the number into text
	 *
	 * \param value The value to be converted into text
	 *
	 * \return QString containing the number in a textual expression.
	*/
	virtual QString textFromValue(int value) const;

private:

	//! \brief No copying
	HexSpinBox(const HexSpinBox& other);

	//! No assignment
	HexSpinBox& operator=(const HexSpinBox& other);
	
	//! \brief The validator used to validate user inputs.
	QRegExpValidator* validator_;
};

#endif // HEXSPINBOX_H
