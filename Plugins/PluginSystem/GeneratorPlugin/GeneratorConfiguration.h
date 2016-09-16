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
#include <Plugins/common/HDLParser/HDLComponentParser.h>
#include <Plugins/common/HDLParser/HDLDesignParser.h>

//-----------------------------------------------------------------------------
//! Container class for generator configuration.
//-----------------------------------------------------------------------------
class GeneratorConfiguration : public QObject
{
    Q_OBJECT

public:

	//! The constructor.
    GeneratorConfiguration(QSharedPointer<ViewSelection> viewSelection,
        QSharedPointer<HDLComponentParser> componentParser,
        QSharedPointer<HDLDesignParser> designParser);

	//! The destructor.
	~GeneratorConfiguration();
    
    /*!
     *  Parses the documents so that we know what will be generated.
     */
    void parseDocuments();
    
    /*!
     *  Gets reference to the output file names.
     */
    QSharedPointer<QStringList> getDocumentNames();
	
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

signals:
	
    /*!
     *  Emitted when output files have changed.
     */
	void outputFilesChanged() const;

private:

	// Disable copying.
	GeneratorConfiguration(GeneratorConfiguration const& rhs);
	GeneratorConfiguration& operator=(GeneratorConfiguration const& rhs);

    //! The view selection configuration.
    QSharedPointer<ViewSelection> viewSelection_;

    QSharedPointer<HDLComponentParser> componentParser_;
    QSharedPointer<HDLDesignParser> designParser_;

    //! The base directory for output paths.
    QString outputPath_;
    //! The paths of individual output files.
    QSharedPointer<QStringList> outputPaths_;

    //! Flag for indicating if the output file should be saved to top component file sets.
    bool generateInterface_;
};

#endif // GENERATORCONFIGURATION_H
