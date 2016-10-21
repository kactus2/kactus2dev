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
        HDLComponentParser* componentParser,
        HDLDesignParser* designParser);

	//! The destructor.
	~GeneratorConfiguration();
    
    /*!
     *  Parses the documents so that we know what will be generated.
     */
    void parseDocuments();
    
    /*!
     *  Gets reference to the output file paths.
     */
    QSharedPointer<QList<QString*> > getFileNames();
	
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
     *  Sets true for generating, false for not generating.
     */
    void setMemoryGeneration(bool shouldGenerate);

    /*!
     *  Returns true for generating, false for not generating.
     */
    bool getMemoryGeneration() const;

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
    
    /*!
     *  Sets the file name in the given index of file names.
     *
     *      @param [in] path    The new name.
     *      @param [in] index   The index of the file name.
     */
    void setOutputFileName(QString newName, int index);

signals:
	
    /*!
     *  Emitted when output files have changed.
     */
	void outputFilesChanged(QStringList) const;

private:

	// Disable copying.
	GeneratorConfiguration(GeneratorConfiguration const& rhs);
	GeneratorConfiguration& operator=(GeneratorConfiguration const& rhs);

    //! The view selection configuration.
    QSharedPointer<ViewSelection> viewSelection_;

    //! The parsers used to parse IP-XACT for data usable in generation.
    HDLComponentParser* componentParser_;
    HDLDesignParser* designParser_;

    //! The base directory for output paths.
    QString outputPath_;
    //! The names of the potential new files.
    QSharedPointer<QList<QString*> > fileNames_;

    //! If true, interfaces should be utilized in generation, else it is false.
    bool generateInterface_;
    //! If true, definitions for registers are generated as well, else it is false.
    bool generateMemory_;
};

#endif // GENERATORCONFIGURATION_H
