/* 
 *
 *  Created on: 10.8.2010
 *      Author: Antti Kamppi
 */

#ifndef FUNCTION_H_
#define FUNCTION_H_

#include "generaldeclarations.h"

#include <QString>
#include <QList>
#include <QDomNode>
#include <QMap>
#include <QSharedPointer>
#include <QXmlStreamWriter>

/*! \brief Equals the spirit:function element in IP-Xact specification
 *
 * Specifies information about a software function.
 */
class Function {

public:

	/*! \brief Equals the spirit:argument element in IP-Xact specification.
	 *
	 * Holds argument-specific information.
	 */
	struct Argument {

		/*! \brief The name of the argument
		 * MANDATORY spirit:name
		 */
		QString name_;

		/*! \brief The value of the argument
		 * MANDATORY spirit:value
		 */
		QString value_;

		/*! \brief The dataType of the argument
		 * MANDATORY attribute spirit:dataType
		 */
		QString dataType_;

		/*! \brief The constructor
		 *
		 * \param argNode A reference to a QDomNode to parse the information
		 * from.
		 *
		 * Exception guarantee: basic
		 * \exception Parse_error Occurs when a mandatory element is missing in
		 * this struct.
		 */
		Argument(QDomNode &argNode);
	};

	/*! \brief Equals the spirit:sourceFile element in IP-Xact specification.
	 *
	 * Holds sourceFile-specific information.
	 */
	struct SourceFile {

		//! \brief Contains absolute or relative path to a file name or directory
		QString sourceName_;

		/*! \brief Specifies the file type known by IP-Xact.
		 *
		 * Either fileType or userFileType must be specified.
		 */
		QString fileType_;

		/*! \brief Specifies a user defined file type for the file.
		 *
		 * Either fileType or userFileType must be specified.
		 */
		QString userFileType_;

		/*! \brief The constructor
		 *
		 * Constructs the sourceFile element to match the data given in
		 * QDomNode.
		 *
		 * \param sourceFileNode A reference to a QDomNode to parse the
		 * information from.
		 *
		 * \exception Parse_error Occurs when a mandatory element is missing in
		 * this struct. This also occurs if both fileType_ and userFileType_
		 * are defined, only one of them can be defined at a time.
		 */
		SourceFile(QDomNode &sourceFileNode);

		//! \brief Default constructor
		SourceFile();
	};

	/*! \brief The constructor
	 *
	 * \param functionNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	Function(QDomNode &functionNode);

	//! \brief Copy constructor
	 Function(const Function &other);

	//! \brief Assignment operator
	 Function &operator=(const Function &other);

	/*! \brief The destructor
	 *
	 */
	~Function();

	/*! \brief Get list of the arguments for the function
	 *
	 * \return QList containing pointers to the arguments.
	 */
	const QList<QSharedPointer<Argument> >& getArguments();

	/*! \brief Get the disabled value
	 *
	 * \return boolean value.
	 */
	bool getDisabled() const;

	/*! \brief Get the attributes of the disabled-element.
	 *
	 * \return QList containing pointers to the attributes.
	 */
	const QMap<QString, QString>& getDisabledAttributes();

	/*! \brief Get entry point of the function
	 *
	 * \return QString containing the entry point.
	 */
	QString getEntryPoint() const;

	/*! \brief Get the file reference
	 *
	 * \return QString containing the file ref.
	 */
	QString getFileRef() const;

	/*! \brief Get the value of the replicate-attribute
	 *
	 * \return boolean value.
	 */
	bool getReplicate() const;

	/*! \brief Get the return type of the function
	 *
	 * \return QString containing the return type.
	 */
	QString getReturnType() const;

	/*! \brief Get list of the source files for the function
	 *
	 * \return QList containing the paths to the source files.
	 */
	const QList<QSharedPointer<SourceFile> >& getSourceFiles();

	/*! \brief Set arguments for the function
	 *
	 * Calling this function will delete old arguments.
	 *
	 * \param arguments QList containing pointers to the new arguments.
	 */
	void setArguments(QList<QSharedPointer<Argument> > &arguments);

	/*! \brief Set the disabled value
	 *
	 * \param disabled The new value.
	 */
	void setDisabled(bool disabled);

	/*! \brief Set the attributes for the disabled-element.
	 *
	 * Calling this function will delete the old attributes.
	 *
	 * \param disabledAttributes QMap containing pointers to the attributes.
	 */
	void setDisabledAttributes(
			QMap<QString, QString> &disabledAttributes);

	/*! \brief Set the entry point for the function
	 *
	 * \param entryPoint QString containing the new entry point.
	 */
	void setEntryPoint(QString &entryPoint);

	/*! \brief Set the file reference
	 *
	 * \param fileRef QString containing the reference to the new file.
	 */
	void setFileRef(QString &fileRef);

	/*! \brief Set the replicate value.
	 *
	 * \param replicate boolean value.
	 */
	void setReplicate(bool replicate);

	/*! \brief Set the return type of the function
	 *
	 * \param returnType QString containing the return type.
	 */
	void setReturnType(QString &returnType);

	/*! \brief Set the source files for the function.
	 *
	 * Calling this function will delete the old source files.
	 *
	 * \param sourceFiles QList containing the source files.
	 */
	void setSourceFiles(QList<QSharedPointer<SourceFile> > &sourceFiles);

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

private:

	 /*!
	  * OPTIONAL spirit:replicate
	  * When true the generator compiles a separate object module for each
	  * instance of the component.
	  * default = false
	  */
	 bool replicate_;

	 /*!
	  * OPTIONAL spirit:entryPoint
	  * The entry point name for the function or subroutine.
	  */
	 QString entryPoint_;

	 /*!
	  * MANDATORY spirit:fileRef
	  * Reference to the file that contains the entry point for the function.
	  */
	 QString fileRef_;

	 /*!
	  * OPTIONAL spirit:returnType
	  * Indicates the return type of the function.
	  */
	 QString returnType_;

	 /*!
	  * OPTIONAL spirit:argument
	  * Contains arguments passed when this function is called.
	  */
	 QList<QSharedPointer<Argument> > arguments_;

	 /*!
	  * OPTIONAL spirit:disabled
	  * Disabled the software function.
	  * default = false
	  */
	 bool disabled_;

	 /*!
	  * OPTIONAL spirit:disabled
	  * Contains attributes for the spirit:disabled element
	  */
	 QMap<QString, QString> disabledAttributes_;

	 /*!
	  * OPTIONAL spirit:sourceFile
	  * References any source files. The order of the source files must be
	  * maintained as this could indicate a compile order.
	  */
	 QList<QSharedPointer<SourceFile> > sourceFiles_;
};

#endif /* FUNCTION_H_ */
