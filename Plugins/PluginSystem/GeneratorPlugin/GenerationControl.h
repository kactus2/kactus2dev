//-----------------------------------------------------------------------------
// File: GenerationControl.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Class to control the over structure of generation.
//-----------------------------------------------------------------------------

#ifndef GENERATIONCONTROL_H
#define GENERATIONCONTROL_H

#include "ViewSelection.h"
#include "OutputControl.h"

#include <Plugins/PluginSystem/GeneratorPlugin/IWriterFactory.h>

class LibraryInterface;

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
class GenerationControl
{
public:

	//! The constructors.
    GenerationControl(LibraryInterface* library,
        IWriterFactory* factory,
        GenerationTuple input,
        GenerationSettings* settings);

	//! The destructor.
    ~GenerationControl();

    /*!
     *  Tries to write documents to file system. Returns true if succeeds, else false.
     */
    bool writeDocuments();
    
    /*!
     *  Tries to write changes of the top component to file system. Returns true if succeeds, else false.
     */
    bool saveChanges();
   
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
     *  Gets the output data.
     */
	QSharedPointer<OutputControl> getOutputControl() const;

    /*!
     *  Returns settings.
     */
    GenerationSettings* getSettings() const;

    /*!
     *  Returns isDesignGeneration_;
     */
    bool isDesignGeneration() const;
    
    /*!
     *  Returns true, if target path is a sub folder of the component path.
     */
    bool isUnder() const;

    /*!
     *  Finds the possible views for generation.
     *
     *      @param [in] input   The relevant IP-XACT documents.
     *
     *      @return List of possible view names for which to run the generation.
     */
    static QSharedPointer<QList<QSharedPointer<View> > > findPossibleViews(GenerationTuple input);

protected:

    /*!
     *  Finds the possible views for generation.
     *
     *      @param [in,out] targetComponent     The component for which the generator is run.
     *
     *      @return List of possible view names for which to run the generation.
     */
    QSharedPointer<QList<QSharedPointer<View> > > findPossibleViews(QSharedPointer<const Component> targetComponent) const;

private:

	// Disable copying.
	GenerationControl(GenerationControl const& rhs);
	GenerationControl& operator=(GenerationControl const& rhs);
    
    /*!
     *  Returns the default output path.     
     */
    QString defaultOutputPath() const;
    
    /*!
     *  Gets the relative path from the top component xml file to the given absolute path.
     *
     *      @param [in] filePath                The absolute path to the target file.
     *
     *      @return Relative path from the top component xml file to the target file.
     */
    QString relativePathFromXmlToFile(QString const& filePath) const;

    /*!
     *  Inserts description to a generated file.
     *
	 *      @param [in] file		            The file which needs to be described.
     */
    void insertFileDescription(QSharedPointer<File> file);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The plugin utility to use.
    LibraryInterface* library_;

    //! The language dependent factory for this generation session.
    IWriterFactory* factory_;
    //! Has the input objects for the generation.
    GenerationTuple input_;
    //! Points to the memorized settings.
    GenerationSettings* settings_;
    //! True, if the generation target is deemed to be a design. False, if it is a component.
    bool isDesignGeneration_;
    //! The file output configuration.
    QSharedPointer<OutputControl> outputControl_;
    //! The view selection configuration.
    QSharedPointer<ViewSelection> viewSelection_;
};

#endif // GENERATIONCONTROL_H
