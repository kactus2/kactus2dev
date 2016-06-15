// Copyright Tampere University of Technology
//-----------------------------------------------------------------------------
// File: ModelSimGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 10.06.2016
//
// Description:
// ModelSim generator.
//-----------------------------------------------------------------------------

#ifndef MODELSIMGENERATOR_H
#define MODELSIMGENERATOR_H

#include "ModelSimParser.h"

#include <Plugins/PluginSystem/IPluginUtility.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/generaldeclarations.h>

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

class ModelSimGenerator : public QObject
{
	Q_OBJECT

public:
    /*!
     *  The constructor
     *
     *      @param [in] parsedData	The data parsed by sample parser.
     *      @param [in] utility		The utility interface provided for the plugin.
     */
    ModelSimGenerator( ModelSimParser& parsedData, IPluginUtility* utility, LibraryInterface* handler );

	//! The destructor.
	~ModelSimGenerator();

    /*!
	*  Generates a file with the names of the file sets within the parsed data.
	*
	*      @param [in] topComponent	The top component of the design, where the generation is based to.
	*/
    void generate( QSharedPointer<Component> topComponent );

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
	ModelSimGenerator(const ModelSimGenerator& other);

	//! No assignment
	ModelSimGenerator& operator=(const ModelSimGenerator& other);

	/*! \brief Write the makefile into stream.
	 *
	 * \param stream A reference to a QTextStream to write into.
	 */
	void writeMakefile(QTextStream& stream);

	//! The parser, to get access to parsed data.
	ModelSimParser& parsedData_;

	//! The utility used to print messages and access library etc.
    IPluginUtility* utility_;

	//! \brief Pointer to the LibraryHandler instance that controls the library
	LibraryInterface* handler_;
};

#endif // MODELSIMGENERATOR_H
