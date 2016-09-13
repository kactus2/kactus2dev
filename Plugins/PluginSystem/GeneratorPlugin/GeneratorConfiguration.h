//-----------------------------------------------------------------------------
// File: GeneratorConfiguration.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 23.02.2015
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


//-----------------------------------------------------------------------------
//! Container class for generator configuration.
//-----------------------------------------------------------------------------
class GeneratorConfiguration 
{
public:

	//! The constructor.
	GeneratorConfiguration(QSharedPointer<ViewSelection> viewSelection);

	//! The destructor.
	~GeneratorConfiguration();
	
    /*!
     *  Gets the view selection data.
     */
	QSharedPointer<ViewSelection> getViewSelection() const;

    /*!
     *  Sets true for generating, false for not generating.
     */
    void setInterfaceGeneration(bool shouldGenerate);

    /*!
     *  Returns true for generating, false for not generating.
     */
    bool getInterfaceGeneration() const;

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

    //! The view selection configuration.
    QSharedPointer<ViewSelection> viewSelection_;

    //! The path for output file.
    QString outputPath_;

    //! Flag for indicating if the output file should be saved to top component file sets.
    bool generateInterface_;
};

#endif // GENERATORCONFIGURATION_H
