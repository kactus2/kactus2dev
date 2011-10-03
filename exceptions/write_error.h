/* 
 *
 *  Created on: 15.10.2010
 *      Author: Antti Kamppi
 */

#ifndef WRITE_ERROR_H_
#define WRITE_ERROR_H_

#include <stdexcept>
#include <QString>

class Write_error: public std::runtime_error {

public:

	/*! \brief The constructor
	 *
	 * The constructor can not throw expection
	 */
	Write_error(QString errorMsg) throw();

	/*! \brief The destructor
	 *
	 */
	virtual ~Write_error() throw();

	/*! \brief The copy constructor
	 *
	 */
	Write_error(Write_error const& other) throw();

	/*! \brief The assignment operator
	 *
	 */
	virtual Write_error& operator=(Write_error const& other) throw();

	/*! \brief Add file path of the file where error occurred
	 *
	 * \param path QString containing the file path
	 */
	virtual void addFilePath(const QString path) throw();

	/*! \brief Get the file path stored in the exception
	 *
	 * \return A reference to the QString containing the file path.
	 */
	virtual QString& filePath() throw();

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

	/*!
	 * Contains the file path of the invalid file.
	 */
	QString filePath_;
};

#endif /* WRITE_ERROR_H_ */
