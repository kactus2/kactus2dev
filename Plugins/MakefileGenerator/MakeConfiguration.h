//-----------------------------------------------------------------------------
// File: MakeConfiguration.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 17.11.2016
//
// Description:
// Container class for make configuration.
//-----------------------------------------------------------------------------

#ifndef MAKECONFIGURATION_H
#define MAKECONFIGURATION_H

#include <QString>
#include <QMap>
#include <QSharedPointer>

#include <Plugins/PluginSystem/GeneratorPlugin/FileOutput.h>

#include "SWStackParser.h"

//-----------------------------------------------------------------------------
//! Container class for make configuration.
//-----------------------------------------------------------------------------
class MakeConfiguration : public QObject
{
    Q_OBJECT

public:

	//! The constructor.
    MakeConfiguration(SWStackParser* parser);

	//! The destructor.
    ~MakeConfiguration();
    
    /*!
     *  Checks if the generation configuration is valid.
     *
     *      @param [out] warning   The stated reason for not accepting.
     */
    bool validSelections(QString &warning);
	
    /*!
     *  Gets the file output data.
     */
	QSharedPointer<FileOuput> getFileOuput() const;

signals:
	
    /*!
     *  Emitted when output files have changed.
     */
	void outputFilesChanged(QStringList) const;

private:

	// Disable copying.
	MakeConfiguration(MakeConfiguration const& rhs);
	MakeConfiguration& operator=(MakeConfiguration const& rhs);

    //! The file output configuration.
    QSharedPointer<FileOuput> fileOutput_;
};

#endif // MAKECONFIGURATION_H
