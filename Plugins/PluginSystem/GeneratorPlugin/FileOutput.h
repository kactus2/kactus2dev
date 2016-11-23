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
#include <Plugins/common/HDLParser/HDLComponentParser.h>
#include <Plugins/common/HDLParser/HDLDesignParser.h>

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
     *  Gets reference to the output file paths.
     */
    QSharedPointer<QList<QString*> > getFileNames();
    
    /*!
     *  Gets reference to the VLNVs of the components corresponding the files.
     */
    QSharedPointer<QStringList> getVLNVs();

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

private:

	// Disable copying.
	FileOuput(FileOuput const& rhs);
	FileOuput& operator=(FileOuput const& rhs);

    //! The base directory for output paths.
    QString outputPath_;
    //! The names of the potential new files.
    QSharedPointer<QList<QString*> > fileNames_;
    //! The VLNVs corresponding the files.
    QSharedPointer<QStringList> vlnvs_;
};

#endif // FILEOUTPUT_H
