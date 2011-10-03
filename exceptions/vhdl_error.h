/* 
 *
 *  Created on: 3.11.2010
 *      Author: Antti Kamppi
 */

#ifndef VHDL_ERROR_H_
#define VHDL_ERROR_H_

#include <stdexcept>
#include <QString>

class Vhdl_error: public std::runtime_error {

public:

	/*! \brief The constructor
	 *
	 * Can't throw exception
	 */
	Vhdl_error(const QString errorMsg) throw();

	//! \brief The destructor
	virtual ~Vhdl_error() throw();

	/*! \brief The copy constructor
	 *
	 */
	Vhdl_error(Vhdl_error const& other) throw();

	/*! \brief The assignment operator
	 *
	 */
	virtual Vhdl_error& operator=(Vhdl_error const& other) throw();

	/*! \brief Get the error message
	 *
	 * \return A reference to QString containing the error message.
	 */
	virtual QString& errorMsg() throw();

private:

	/*!
	 * Contains the error-specific message to identify the reason of the error.
	 */
	QString errorMsg_;

};

#endif /* VHDL_ERROR_H_ */
