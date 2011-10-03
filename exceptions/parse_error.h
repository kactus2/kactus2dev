/* 
 *
 *  Created on: 13.8.2010
 *      Author: Antti Kamppi
 */

#ifndef PARSE_ERROR_H_
#define PARSE_ERROR_H_

#include <stdexcept>
#include <QString>

/*! \brief The Parse_error exception is thrown during parsing an IP-Xact document.
 *
 * Parse_error is thrown when an invalid value or invalid element is detected
 * while parsing document.
 */
class Parse_error: public std::runtime_error {

public:

	/*! \brief The constructor
	 *
	 * The constructor can not throw expection
	 */
	Parse_error(QString errorMsg) throw();

	/*! \brief The destructor
	 *
	 */
	virtual ~Parse_error() throw();

	/*! \brief The copy constructor
	 *
	 */
	Parse_error(Parse_error const& other) throw();

	/*! \brief The assignment operator
	 *
	 */
	virtual Parse_error& operator=(Parse_error const& other) throw();

	/*! \brief Add file path of the file where error occurred
	 *
	 * \param path QString containing the file path
	 */
	virtual void addFilePath(QString path) throw();

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

#endif /* PARSE_ERROR_H_ */
