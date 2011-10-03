/* 
 *
 *  Created on: 20.8.2010
 *      Author: Antti Kamppi
 */

#ifndef INVALID_FILE_H_
#define INVALID_FILE_H_

#include <stdexcept>
#include <QString>

/*! \brief Invalid_file exception is used to inform the system that an
 * invalid file has been i.e given as a parameter to a function or a file
 * containing settings is invalid.
 */
class Invalid_file: public std::runtime_error {

public:

	/*! \brief The constructor
	 *
	 * \param errorMsg QString containing the error message
	 * \param filePath QString containing the path of the file
	 */
	Invalid_file(QString errorMsg, QString filePath = QString()) throw();

	/*! \brief The destructor
	 *
	 */
	virtual ~Invalid_file() throw();

	/*! \brief The copy constructor
	 *
	 */
	Invalid_file(const Invalid_file& other) throw();

	/*! \brief The assignment operator
	 *
	 */
	virtual Invalid_file& operator=(const Invalid_file& other) throw();

	/*! \brief Get the error message
	 *
	 * \return A reference to the QString containing the error message
	 */
	virtual QString& errorMsg() throw();

	/*! \brief Get the path of the file that caused the error
	 *
	 * \return A reference to the QString containing the file path
	 */
	virtual QString& filePath() throw();

	/*! \brief Set the path of the file that caused the error
	 *
	 * \param filePath QString containing the path to the file that caused
	 * the error.
	 */
	virtual void addFilePath(QString filePath) throw();

private:

	/*! \brief Contains the error specific message
	 *
	 */
	QString errorMsg_;

	/*! \brief Contains the path of the file
	 *
	 */
	QString filePath_;
};

#endif /* INVALID_FILE_H_ */
