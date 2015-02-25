//-----------------------------------------------------------------------------
// File: GeneratorConfiguration.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.02.2015
//
// Description:
// Container class for generator configuration.
//-----------------------------------------------------------------------------

#ifndef GENERATORCONFIGURATION_H
#define GENERATORCONFIGURATION_H

#include <QString>

//-----------------------------------------------------------------------------
//! Container class for generator configuration.
//-----------------------------------------------------------------------------
class GeneratorConfiguration 
{
public:

	//! The constructor.
	GeneratorConfiguration();

	//! The destructor.
	~GeneratorConfiguration();
    
    /*!
     *  Sets the active view for the top component in generation.
     *
     *      @param [in] viewName   The name of the active view.
     */
    void setActiveView(QString const& viewName);

    /*!
     *  Gets the active view for the top component.
     *
     *      @return The active view name.
     */
    QString getActiveViewName() const;

    /*!
     *  Sets whether the output file should be added to top component file sets or not.
     *
     *      @param [in] shouldSave   True for saving, false for not saving.
     */
    void setSaveToFileset(bool shouldSave);

    /*!
     *  Checks if the output file should be added to top component file sets.
     *
     *      @return True, if file should be saved, false otherwise.
     */
    bool getSaveToFileset() const;

    /*!
     *  Sets the path for the output file for the generation.
     *
     *      @param [in] path   The path to set.
     */
    void setOutputPath(QString const& path);

    /*!
     *  Gets the output path for generation.
     *
     *      @return The path to output file.
     */
    QString getOutputPath() const;

private:

	// Disable copying.
	GeneratorConfiguration(GeneratorConfiguration const& rhs);
	GeneratorConfiguration& operator=(GeneratorConfiguration const& rhs);

    //! The path for output file.
    QString outputPath_;

    //! The name of the top component active view.
    QString activeViewName_;

    //! Flag for indicating if the output file should be saved to top component file sets.
    bool saveToFileset_;
};

#endif // GENERATORCONFIGURATION_H
