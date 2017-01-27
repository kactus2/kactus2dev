//-----------------------------------------------------------------------------
// File: FileOutput.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 15.11.2016
//
// Description:
// Container class for file output of generation.
//-----------------------------------------------------------------------------

#ifndef FILEOUTPUT_H
#define FILEOUTPUT_H

#include <QString>
#include <QMap>
#include <QSharedPointer>

#include "ViewSelection.h"

struct GenerationFile
{
    //! The name of the file for the document
    QString fileName_;
    QString vlnv_;
    QString fileContent_;
    
    /*!
     *  Writes the content.
     */
	virtual void write(){}
};

//-----------------------------------------------------------------------------
//! Container class for generator configuration.
//-----------------------------------------------------------------------------
class FileOuput : public QObject
{
    Q_OBJECT

public:

	//! The constructor.
    FileOuput();

	//! The destructor.
	~FileOuput();
    
    /*!
     *  Checks if the file output configuration is valid.
     *
     *      @param [out] warning   The stated reason for not accepting.
     */
    bool validSelections(QString &warning);

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
     *  Gets reference to the output file paths.
     */
    QSharedPointer<QList<QSharedPointer<GenerationFile> > > getFiles();

private:

	// Disable copying.
	FileOuput(FileOuput const& rhs);
	FileOuput& operator=(FileOuput const& rhs);

    //! The base directory for output paths.
    QString outputPath_;
    //! The potential new files.
    QSharedPointer<QList<QSharedPointer<GenerationFile> > > files_;
};

#endif // FILEOUTPUT_H
