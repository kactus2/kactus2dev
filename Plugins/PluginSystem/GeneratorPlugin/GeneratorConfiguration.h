//-----------------------------------------------------------------------------
// File: GeneratorConfiguration.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Container class for generator configuration.
//-----------------------------------------------------------------------------

#ifndef GENERATORCONFIGURATION_H
#define GENERATORCONFIGURATION_H

#include <QString>
#include <QMap>
#include <QSharedPointer>

#include "ViewSelection.h"
#include "FileOutput.h"
#include <Plugins/PluginSystem/GeneratorPlugin/IWriterFactory.h>
#include <library/LibraryManager/libraryinterface.h>

struct GenerationSettings
{
    //! If true, interfaces should be utilized in generation, else it is false.
    bool generateInterfaces_;
    //! The last values used by the generation.
    QString lastViewName_;
    QString lastFileSetName_;
};

//-----------------------------------------------------------------------------
//! Container class for generator configuration.
//-----------------------------------------------------------------------------
class GeneratorConfiguration
{
public:

	//! The constructors.
    GeneratorConfiguration(LibraryInterface* library, IWriterFactory* factory, GenerationTuple input,
        GenerationSettings* settings);

	//! The destructor.
    ~GeneratorConfiguration();

    void writeDocuments();
   
    /*!
     *  Checks if the generation configuration is valid.
     *
     *      @param [out] warning   The stated reason for not accepting.
     */
    bool validSelections(QString &warning);
    
    /*!
     *  Parses the documents so that we know what will be generated.
     */
    void parseDocuments();
	
    /*!
     *  Gets the view selection data.
     */
	QSharedPointer<ViewSelection> getViewSelection() const;
	
    /*!
     *  Gets the file output data.
     */
	QSharedPointer<FileOuput> getFileOuput() const;

    /*!
     *  Returns settings.
     */
    GenerationSettings* getSettings() const;

protected:

    /*!
     *  Finds the possible views for generation.
     *
     *      @param [in,out]	    targetComponent	The component for which the generator is run.
     *
     *      @return List of possible view names for which to run the generation.
     */
    QSharedPointer<QList<QSharedPointer<View> > > findPossibleViews(QSharedPointer<const Component> targetComponent) const;

    /*!
     *  Finds the possible views for generation.
     *
     *      @param [in] input			        The relevant IP-XACT documents.
     *
     *      @return List of possible view names for which to run the generation.
     */
    QSharedPointer<QList<QSharedPointer<View> > > findPossibleViews(GenerationTuple input) const;

private:

	// Disable copying.
	GeneratorConfiguration(GeneratorConfiguration const& rhs);
	GeneratorConfiguration& operator=(GeneratorConfiguration const& rhs);
    
    /*!
     *  Gets the default output path.     
     *
     *      @return The default output path.
     */
    QString defaultOutputPath() const;
    
    /*!
     *  Gets the relative path from the top component xml file to the given absolute path.
     *
     *      @param [in] filePath   The absolute path to the target file.
     *
     *      @return Relative path from the top component xml file to the target file.
     */
    QString relativePathFromXmlToFile(QString const& filePath) const;

    /*!
     *  Inserts description to a generated file.
     *
	 *      @param [in] file		The file which needs to be described.
     */
    void insertFileDescription(QSharedPointer<File> file);

    //! Saves the changes made to the top component.
    void saveChanges();

    /*!
     *  Finds all the views in containing component referencing the given design or design configuration VLNV.
     *
     *      @param [in] containingComponent     The component whose views to search through.
     *      @param [in] targetReference         The reference to find in views.
     *
     *      @return The the views referencing the given VLNV.
     */
    QSharedPointer<QList<QSharedPointer<View> > > findReferencingViews(QSharedPointer<Component> containingComponent,
		VLNV targetReference) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The plugin utility to use.
    LibraryInterface* library_;
    IWriterFactory* factory_;
    GenerationTuple input_;
    GenerationSettings* settings_;
    //! The view selection configuration.
    QSharedPointer<ViewSelection> viewSelection_;
    bool isDesign_;
    //! The file output configuration.
    QSharedPointer<FileOuput> fileOutput_;
};

#endif // GENERATORCONFIGURATION_H
