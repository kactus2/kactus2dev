//-----------------------------------------------------------------------------
// File: ModelSimParser.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 10.06.2016
//
// Description:
// Parses design for sample data.
//-----------------------------------------------------------------------------

#ifndef MODELSIMPARSER_H
#define MODELSIMPARSER_H

#include <QString>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/View.h>

#include <library/LibraryManager/libraryinterface.h>
#include <Plugins/PluginSystem/IPluginUtility.h>

class Component;
class LibraryInterface;
class DesignConfiguration;

class ModelSimParser : public QObject
{
	Q_OBJECT
public:
	//! \brief Contains the info to compile a file
	struct FileOptions
	{
		//! \brief Absolute file path to the file.
		QString absFilePath_;

		//! \brief The name of the library to compile the file into
		QString libraryName_;

		//! \brief Contains the command to use to compile the file.
		QString buildCommand_;

		//! \brief Contains the flags for the command.
		QString flags_;

		//! \brief The component that included this file
		QString sourceComponent_;

		//! \brief The name of the file set this file belongs to
		QString sourceFileSet_;

		/*! \brief The default constructor
		 *
		*/
		FileOptions();

		/*! \brief The constructor
		 *
		 * \param filePath The absolute file path.
		 *
		*/
		FileOptions(const QString& filePath);

		/*! \brief The copy constructor
		 *
		 * \param other Reference to the FileOptions to be copied.
		 *
		*/
		FileOptions(const FileOptions& other);

		/*! \brief The == operator.
		 *
		 * \param other Reference to the other FileOptions-instance to compare.
		 *
		 * \return bool True if they have the same options.
		*/
		bool operator==(const FileOptions& other);

		/*! \brief The assignment operator
		 *
		 * \param other Reference to the other FileOptions-instance to assign.
		 *
		*/
		FileOptions& operator=(const FileOptions& other);
	};

    ModelSimParser( IPluginUtility* utility, LibraryInterface* handler );
	~ModelSimParser();

	/*!
	 *  Returns the parsed data.
	*/
	QSharedPointer<QList<FileOptions> > getParsedData();

	/*! \brief Parse the files associated with component's given view.
	 *
	 * \param component Pointer to the component
	 * \param view The  view the script is generated for.
	 *
	*/
	void parseFiles(QSharedPointer<Component> component, const QSharedPointer<View> view);

	QString getSourceXml() const { return sourceXml_; }
	QSharedPointer<View> getView() const { return view_; }
signals:

	//! \brief Print a notice message.
	void noticeMessage(const QString& message);

	//! \brief Print an error message.
	void errorMessage(const QString& message);

private:

	/*! \brief Parse the given file sets and add their files to source files.
	 *
	 * \param component Pointer to the component that's files are wanted.
	 * \param fileSetNames Names of the file sets to use.
	 *
	*/
	void parseFileSets(QSharedPointer<Component> component, QSharedPointer<QStringList> fileSetNames);

	/*! \brief Read the design and possibly design configuration and parse them.
	 * 
	 * Design is read and component instances are parsed to get their files also.
	 * 
	 * \param design Pointer to the design to parse.
	 * \param desConf Pointer to the design configuration to use. (OPTIONAL)
	 *
	*/
	void readDesign(const QSharedPointer<Design> design,
		const QSharedPointer<DesignConfiguration> desConf);

	/*! \brief Parse the component's file sets and add all vhdl- and verilog-files.
	 *
	 * \param component Pointer to the component to search.
	 *
	*/
	void parseBlindFileSet(QSharedPointer<Component> component);

	//! \brief List of the source files to be written to make file
	QSharedPointer<QList<FileOptions> > sourceFiles_;

	//! \brief The file path to the xml file used as the base.
	QString sourceXml_;

	QSharedPointer<View> view_;

	//! The utility used to print messages and access library etc.
	IPluginUtility* utility_;

	//! \brief Pointer to the LibraryHandler instance that controls the library
	LibraryInterface* handler_;
};

#endif // MODELSIMPARSER_H
