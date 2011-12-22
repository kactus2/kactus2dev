/* 
 *
 *  Created on: 10.8.2010
 *      Author: Antti Kamppi
 */

#ifndef FILEBUILDER_H_
#define FILEBUILDER_H_

#include "buildmodel.h"

#include <QString>
#include <QList>
#include <QDomNode>
#include <QStringList>
#include <QXmlStreamWriter>

/*! \brief Equals the spirit:fileBuilderType element in IP-Xact specification.
 *
 * Contains flags or commands for building the containing source file types.
 */
class FileBuilder: public BuildModel {

public:

	/*! \brief The constructor
	 *
	 * \param fileBuilderNode A reference to a QDomNode to parse the
	 * information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	FileBuilder(QDomNode &fileBuilderNode);

	/*! \brief The default constructor
	 * 
	 * Constructs an empty invalid file builder.
	*/
	FileBuilder();

	/*! \brief The constructor
	 *
	 * \param fileType Specifies the file type of the file build command.
	 *
	*/
	FileBuilder(const QString& fileType);

	//! \brief Copy constructor
	FileBuilder(const FileBuilder &other);

	//! \brief Assignment operator
	FileBuilder &operator=(const FileBuilder &other);

	/*! \brief The destructor
	 *
	 */
	virtual ~FileBuilder();

	/*! \brief Get the file types
	 *
	 * \return QList containing the file types.
	 */
	const QList<QString>& getFileTypes();

	/*! \brief Get the first file type specified.
	 *
	 * If no file type or user file type is specified then returns empty string.
	 * 
	 * \return QString containing the file type
	*/
	QString getFileType() const;

	/*! \brief Set a file type
	 *
	 * Sets the given file type as first specified file type. The type is added
	 * to file types or user file types depending on the type.
	 *
	 * \param fileType QString containing the file type.
	 *
	*/
	void setFileType(const QString& fileType);

	/*! \brief Get the user file types
	 *
	 * \return QList containing the user file types
	 */
	const QList<QString>& getUsetFileTypes();

	/*! \brief Set the user file types
	 *
	 * \param userFileTypes QList containing the file types
	 */
	void setUserFileTypes(QList<QString>& userFileTypes);

	/*! \brief Set the file types
	 *
	 * Calling this function will delete old file types.
	 *
	 * \param fileTypes QList containing the file types.
	 */
    void setFileTypes(QList<QString> &fileTypes);

    /*! \brief Write the contents of the class using the writer.
     *
     * Uses the specified writer to write the class contents into file as valid
     * IP-Xact.
     *
     * \param writer A reference to a QXmlStreamWriter instance that is used to
     * write the document into file.
     *
     * Exception guarantee: basic
     * \exception Write_error Occurs if class or one of it's member classes is
     * not valid IP-Xact in the moment of writing.
     */
    void write(QXmlStreamWriter& writer);

	/*! \brief Is the file builder in valid condition or not.
	 *
	 * \return True if file builder is valid.
	*/
	bool isValid() const;

	/*! \brief Check if the file builder is meant for given file type.
	 *
	 * \param fileType The file type to check.
	 *
	 * \return bool True if builder is meant for given file type.
	*/
	bool hasFileType(const QString& fileType) const;

	/*! \brief Check if the FileBuilder is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, const QString& parentIdentifier) const;

private:

	/*! \brief Defines the format of a referenced file.
	 * MANDATORY spirit:fileType
	 */
	QList<QString> fileTypes_;

	//! \brief MANDATORY spirit:userFileType
	//! Defines the format of the referenced file. Used for filetypes that are
	//! unknown to IP-Xact.
	QList<QString> userFileTypes_;
};
#endif /* FILEBUILDER_H_ */
