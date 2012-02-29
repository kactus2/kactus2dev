/* 
 *  	Created on: 29.2.2012
 *      Author: Antti Kamppi
 * 		filename: numberlineedit.h
 *		Project: Kactus 2
 */

#ifndef NUMBERLINEEDIT_H
#define NUMBERLINEEDIT_H

#include <QLineEdit>

/*! \brief Line editor meant to edit a number that may contain letter-multiples.
 * 
 * This editor accepts following multiples:
 * k/K = 2^10
 * M   = 2^20
 * G   = 2^30
 * T   = 2^40
 * P   = 2^50
 */
class NumberLineEdit : public QLineEdit {
	Q_OBJECT

public:

	/*! \brief The constructor 
	 *
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	NumberLineEdit(QWidget *parent);
	
	//! \brief The destructor
	virtual ~NumberLineEdit();

private:
	//! \brief No copying
	NumberLineEdit(const NumberLineEdit& other);

	//! \brief No assignment
	NumberLineEdit& operator=(const NumberLineEdit& other);
};

#endif // NUMBERLINEEDIT_H
