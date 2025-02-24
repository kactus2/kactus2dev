/* 
 *
 *  Created on: 24.11.2010
 *      Author: Antti Kamppi
 */

#ifndef QUARTUSGENERATOR_H
#define QUARTUSGENERATOR_H

#include <QTextStream>
#include <QFile>

class Component;
class File;
class LibraryInterface;
class Design;
class DesignConfiguration;
class View;
class IPluginUtility;

/*!  QuartusGenerator generates a Quartus project for the component.
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

	/*! The constructor
	 *	 
	 *    @param [in] utility The Plugin utility, where to get libraries etc.
	 *
	*/
	explicit QuartusGenerator(IPluginUtility* utility);

	//! The destructor
	virtual ~QuartusGenerator() = default;

    //! No copying
    QuartusGenerator(const QuartusGenerator& other) = delete;

    //! No assignment
    QuartusGenerator& operator=(const QuartusGenerator& other) = delete;

    /*! Read already saved pin mappings from file sets to store in the settings file later.
    *
    *    @param [in] component The component whose pin mappings to find.
    */
    void readExistingPinMap(QSharedPointer<Component> component);

	/*!
	 *  Parse the files associated with the given view in the selected component.
	 *
	 *    @param [in] component   The selected component.
	 *    @param [in] view		The view to parse file set references from.
	 */
	void parseFiles(QSharedPointer<Component> component, QSharedPointer<View> view);

  
	/*! Generate the Quartus project files (.qpf .qsf)
	 *
     *    @param [in] outputPath              The directory path, where the project files are generated.
     *    @param [in] topEntity              The name of the topmost entity.
     *    @param [in] generatorInformation    The generator information to write in the header.
	 *
	 */
	void generateProject(QString const& outputPath, QString const& topEntity,QString const& generatorInformation);

    /*!
     *  Replace the printed time with the given text.
     *
     *    @param [in] newTime     The replacement for the printed time.
     */
    void replaceTime(QString const& newTime);

signals:

	//! Send error message to user.
	void errorMessage(QString const& errorMessage);

	//! Send notification to user.
	void noticeMessage(QString const& noticeMessage);

private:

    /*!
     *  Get the file containing a quartus pin map.
     *
     *    @param [in] component   The component where to search the file from.
     *
     *    @return The file that contains a quartus pin map. If no quartus pin map is found, returns a null.
     */
    QSharedPointer<File> getQuartusPinMap(QSharedPointer<Component> component);

    /*! Writes the Quartus Project File into specified directory.
    *
    *    @param [in] outPutDir Path to the directory where the .qpf file is written to
    */
    void writeQuartusProjectFile(QString const& outputPath, QString const& topEntity, QString const& generatorInformation);
    
    /*!
    *  Create a quartus file.
    *
    *    @param [in] outputDirectory     Path to the directory where the file is written to.
    *    @param [in] topEntity           The name of the topmost entity for the file name.
    *    @param [in] fileSuffix          The suffix for the file name.
    *
    *    @return Either a quartus project file or a quartus settings file, depending on the given suffix.
    */
    QFile* createQuartusProjectFile(QString const& outputPath, QString const& topEntity, QString const& fileSuffix);

    /*! Writes the header info for the Quartus file.
    *
    *    @param [in] stream                   The QTextStream to write into.
    *    @param [in] generatorInformation     The generator information to write in the header.
    */
    void writeHeader(QTextStream& stream, QString const& generatorInformation);

    /*! Writes the Quartus Settings File into specified directory.
    *
    *    @param [in] outPutDir                Path to the directory where the .qsf file is written to.
    *    @param [in] topEntity                The name of the topmost entity for the file name.
    *    @param [in] generatorInformation     The generator information to write in the header.
    */
    void writeQuartusSettingsFile(QString const& outputPath, QString const& topEntity, QString const& generatorInformation);
   
    /*! Writes all the related files into the Quartus settings file.
     *
     *    @param [in] stream      The QTextStream to write into.
     */
    void writeIncludedFiles(QTextStream& stream);
   
    /*! Writes all the stored pin assignments into the Quartus settings file.
     *
     *    @param [in] stream      The QTextStream to write into.
     */
    void writePinAssignments(QTextStream& stream);

    /*! Parse the component's file sets and add all vhdl- and verilog-files.
    *
    *    @param [in] component The component to search.
    */
    void parseAllFileSets(QSharedPointer<Component> component);

    /*!
     *  Parse a single file.
     *
     *    @param [in] targetFile      The selected file to parse.
     *    @param [in] basePath        The path of the file.
     *    @param [in] componentVLNV   The vlnv of the component in qstring format.
     */
    void parseSingleFile(QSharedPointer<File> targetFile, QString const& basePath, QString const& componentVLNV);

    /*! Parse the given file sets and add their files to source files.
    *
    *    @param [in] component       The component that's files are wanted.
    *    @param [in] fileSetNames     Names of the file sets to use.
    */
    void parseFileSets(QSharedPointer<Component> component, QStringList const& fileSetNames);

    /*!
     *  Parse the design / design configuration and the file sets of a hierarchical view of the selected component.
     *
     *    @param [in] view        The hierarchical view.
     *    @param [in] component   The component of the hierarhical view.
     */
    void parseFilesFromHierarchicalView(QSharedPointer<View> view, QSharedPointer<Component> component);

	/*! Read the design and possibly design configuration and parse them.
	 * 
	 * Design is read and component instances are parsed to get their files also.
	 * 
	 *    @param [in] design  The design to parse.
	 *    @param [in] desConf The design configuration to use
	 *
	*/
	void readDesign(QSharedPointer<const Design> design, QSharedPointer<const DesignConfiguration> desConf);

    /*! Parse the file sets in a given view and add their files to source files.
    *
    *    @param [in] component        The component that's files are wanted.
    *    @param [in] view             The view whose filesets to search.
    */
    void parseFilesFromFlatView(QSharedPointer<Component> component, QSharedPointer<View> view);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The Plugin utility, where to get libraries etc.
	IPluginUtility* utility_;

	//! Contains the file paths of the needed files.
	QStringList files_;

	//! Contains the pin assignments retrieved from the IP-Xact metadata.
	QStringList assignments_;

	//! The instance managing the IP library.
	LibraryInterface* handler_;

    //! Time replacement for the creation time. Used in tests.
    QString timeReplacement_;
};

#endif /* QUARTUSGENERATOR_H */
