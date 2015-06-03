/* 
 *
 *  Created on: 24.11.2010
 *      Author: Antti Kamppi
 */

#ifndef QUARTUSGENERATOR_H_
#define QUARTUSGENERATOR_H_

#include <QObject>
#include <QWidget>
#include <QString>
#include <QTextStream>
#include <QStringList>
#include <QSharedPointer>
#include <QFile>

class Component;
class File;
class LibraryInterface;
class Design;
class DesignConfiguration;
class View;
/*! \brief QuartusGenerator generates a Quartus project for the component.
 *
 * Generator creates two files:
 * - .qpf file which is the quartus project file
 * - .qsf file which contains list of needed files and possibly pin assignments
 *	for an FPGA board.
 *
 */
class QuartusGenerator: public QObject
{
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param handler Pointer to the instance that manages the library
	 * \param parent Owner of this generator
	 *
	*/
	QuartusGenerator(LibraryInterface* handler, QWidget* parent);

	/*! \brief Set the name of the FPGA board the project is created for.
	 *
	 * \param boardName The name of the FPGA board to be used.
	 */
	void readExistingPinMap(QSharedPointer<Component> component);

	//! \brief The destructor
	virtual ~QuartusGenerator();

	/*!
	 *  Parse the files associated with the given view in the selected component.
	 *
	 *      @param [in] component   Pointer to the selected component.
	 *      @param [in] viewName    Name of the view to parse file set references from.
	 */
	void parseFiles(QSharedPointer<Component> component, const QString& viewName);

	/*! \brief Generate the Quartus project files (.qpf .qsf)
	 *
	 * This function takes the output directory path and creates two files
	 * there:
	 * - Quartus settings file
	 * - Quartus project file
	 *
	 * The files are named after the top component's name.
	 *
	 */
	void generateProject(QString outputPath, const QString& top_entity, QString const& generatorInformation);

    /*!
     *  Replace the printed time with the given text.
     *
     *      @param [in] newTime     The replacement for the printed time.
     */
    void replaceTime(QString const& newTime);

signals:

	//! \brief Send error message to user.
	void errorMessage(const QString& errorMessage);

	//! \brief Send notification to user.
	void noticeMessage(const QString& noticeMessage);

private:

	//! No copying
	QuartusGenerator(const QuartusGenerator& other);

	//! No assignment
	QuartusGenerator& operator=(const QuartusGenerator& other);

    /*!
     *  Get the file containing a quartus pin map.
     *
     *      @param [in] component   Pointer to the component where to search the file from.
     *
     *      @return Pointer to the file that contains a quartus pin map. If no quartus pin map is found,
     *              returns a null pointer.
     */
    QSharedPointer<File> getQuartusPinMap(QSharedPointer<Component> component);

	/*! \brief Parse the given file sets and add their files to source files.
	 *
	 * \param component Pointer to the component that's files are wanted.
	 * \param fileSetNames Names of the file sets to use.
	 *
	*/
	void parseFileSets(QSharedPointer<Component> component, const QStringList& fileSetNames);

    /*!
     *  Parse a single file.
     *
     *      @param [in] currentFile     The selected file to parse.
     *      @param [in] basePath        The path of the file.
     *      @param [in] componentVLNV   The vlnv of the component in qstring format.
     */
    void parseSingleFile(QSharedPointer<File> currentFile, QString const& basePath, QString const& componentVLNV);

    /*! \brief Parse the component's file sets and add all vhdl- and verilog-files.
	 *
	 * \param component Pointer to the component to search.
	 *
	*/
	void parseBlindFileSet(QSharedPointer<Component> component);

    /*!
     *  Parse the design / design configuration and the file sets of a hierarchical view of the selected component.
     *
     *      @param [in] view        The hierarchical view.
     *      @param [in] component   The component of the hierarhical view.
     */
    void parseFilesFromHierarchicalView(View* view, QSharedPointer<Component> component);

	/*! \brief Read the design and possibly design configuration and parse them.
	 * 
	 * Design is read and component instances are parsed to get their files also.
	 * 
	 * \param design Pointer to the design to parse.
	 * \param desConf Pointer to the design configuration to use. (OPTIONAL)
	 *
	*/
	void readDesign(const QSharedPointer<Design> design, const QSharedPointer<DesignConfiguration> desConf);

	//! \brief Get list of all vhdl files that are needed in the project.
	//void getVhdlFiles();

	/*! \brief Writes the Quartus Project File into specified directory.
	 *
	 * \param outPutDir Path to the directory where the .qpf file is written to
	 *
	 * Exception guarantee: basic
	 * \exception std::runtime_error Occurs if output file can't be opened for
	 * writing.
	 */
	void writeQuartusProjectFile(const QString& outPutDir, const QString& top_entity,
        QString const& generatorInformation);

	/*! \brief Writes the Quartus Settings File into specified directory.
	 *
	 * \param outPutDir Path to the directory where the .qsf file is written to
	 *
	 */
	void writeQuartusSettingsFile(const QString& outputDir, const QString& top_entity,
        QString const& generatorInformation);

    /*!
     *  Create a quartus file.
     *
     *      @param [in] outputDirectory     Path to the directory where the file is written to.
     *      @param [in] topEntity           The file name of the created file.
     *      @param [in] fileSuffix          The suffix for the file name.
     *
     *      @return Either a quartus project file or a quartus settings file, depending on the given suffix.
     */
    QSharedPointer<QFile> createQuartusProjectFile(QString const& outputDirectory, QString const& topEntity,
        QString const& fileSuffix);

	/*! \brief Writes the header info for the Quartus Project File.
	 *
	 * \param stream Refernce to QTextStream to write into.
	 */
	void writeHeader(QTextStream& stream, QString const& generatorInformation);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	/*! \brief Contains the file paths of the needed files.
	 *
	 */
	QStringList files_;

	//! \brief Contains the pin assignments retrieved from the IP-Xact metadata.
	QStringList assignments_;

	//! \brief Pointer to the LibraryHandler instance managing the IP library.
	LibraryInterface* handler_;

	//! \brief Pointer to the owner of this generator.
	QWidget* parent_;

    //! Time replacement for the creation time. Used in tests.
    QString timeReplacement_;
};

#endif /* QUARTUSGENERATOR_H_ */
