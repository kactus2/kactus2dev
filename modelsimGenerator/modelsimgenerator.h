/* 
 *
 *  Created on: 7.12.2010
 *      Author: Antti Kamppi
 */

#ifndef MODELSIMGENERATOR_H_
#define MODELSIMGENERATOR_H_

#include <models/generaldeclarations.h>

#include <QWidget>
#include <QObject>
#include <QString>
#include <QMap>
#include <QTextStream>
#include <QStringList>

class Component;
class Design;
class LibraryInterface;
class DesignConfiguration;

/*! \brief ModelsimGenerator creates a modelsim makefile.
 *
 * Makefile can be used to compile all needed vhdl-files into right libraries
 * so the top component can be simulated.
 */
class ModelsimGenerator: public QObject {

	Q_OBJECT

public:

	//! \brief Contains the info to compile a file
	struct FileOptions {
		
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

	/*! \brief The constructor
	 *
	 * \param handler Pointer to the instance that provides library-services.
	 * \param parent Pointer to the owner of this generator.
	 *
	*/
	ModelsimGenerator(LibraryInterface* handler, QWidget* parent);

	//! \brief The destructor
	virtual ~ModelsimGenerator();

	/*! \brief Parse the files associated with component's given view.
	 *
	 * \param component Pointer to the component
	 * \param viewName The name of the view the script is generated for.
	 *
	*/
	void parseFiles(QSharedPointer<Component> component, const QString& viewName);

	/*! \brief Create a makefile that compiles all files into libraries.
	 *
	 * The created file is not automatically added to the IP-Xact documentation
	 * even if the file is created to the library structure. If reference to the
	 * file is wanted to be added to the library then addMakefile2IPXact() must
	 * be called.
	 *
	 * \param outputPath file path where the new file is created to. 
	 * This must contain the output file name.
	 */
	void generateMakefile(const QString outputPath);

	/*! \brief Add a reference to a makefile in the metadata of top component.
	 *
	 * This function adds the makefile into top components metadata.
	 *
	 * \param component Pointer to the component to which's metadata the makefile
	 * is added to.
	 * \param makefilePath File path to the makefile that is added.
	 * \param basePath Path to the xml-file to create the relative file path.
	 * 
	 * \return True if the file was successfully added.
	 */
	bool addMakefile2IPXact(QSharedPointer<Component> component,
		const QString& makefilePath, 
		const QString& basePath);

signals:

	//! \brief Print a notice message.
	void noticeMessage(const QString& message);

	//! \brief Print an error message.
	void errorMessage(const QString& message);

private:

	//! No copying
	ModelsimGenerator(const ModelsimGenerator& other);

	//! No assignment
	ModelsimGenerator& operator=(const ModelsimGenerator& other);

	/*! \brief Parse the given file sets and add their files to source files.
	 *
	 * \param component Pointer to the component that's files are wanted.
	 * \param fileSetNames Names of the file sets to use.
	 *
	*/
	void parseFileSets(QSharedPointer<Component> component, const QStringList& fileSetNames);

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

	/*! \brief Write the makefile into stream.
	 *
	 * \param stream A reference to a QTextStream to write into.
	 */
	void writeMakefile(QTextStream& stream);

	//! \brief Pointer to the owner of this generator
	QWidget* parent_;

	//! \brief Pointer to the LibraryHandler instance that controls the library
	LibraryInterface* handler_;

	//! \brief List of the source files to be written to make file
	QList<FileOptions> sourceFiles_;

	//! \brief The name of the view the modelsim generator is run for.
	QString viewName_;
};

#endif /* MODELSIMGENERATOR_H_ */
