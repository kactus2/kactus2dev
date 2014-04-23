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

class Component;
class LibraryInterface;
class Design;
class DesignConfiguration;

/*! \brief QuartusGenerator generates a Quartus project for the component.
 *
 * Generator creates two files:
 * - .qpf file which is the quartus project file
 * - .qsf file which contains list of needed files and possibly pin assignments
 *	for an FPGA board.
 *
 */
class QuartusGenerator: public QObject {

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
	void setBoardName(QSharedPointer<Component> component);

	//! \brief The destructor
	virtual ~QuartusGenerator();

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
	void generateProject(QString outputPath, const QString& top_entity);

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

	/*! \brief Reads the given stream for pin mappings and saves them.
	 *
	 * The found assignments are stored to assignments_
	 *
	 * \param stream The text stream to read the assignments from.
	 */
	void readAssignments(QTextStream& stream);

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
	void writeQPF(const QString& outPutDir, const QString& top_entity);

	/*! \brief Writes the Quartus Settings File into specified directory.
	 *
	 * \param outPutDir Path to the directory where the .qsf file is written to
	 *
	 */
	void writeQSF(const QString& outputDir, const QString& top_entity);

	/*! \brief Writes the header info for the Quartus Project File.
	 *
	 * \param stream Refernce to QTextStream to write into.
	 */
	void writeHeader(QTextStream& stream);

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
};

#endif /* QUARTUSGENERATOR_H_ */
