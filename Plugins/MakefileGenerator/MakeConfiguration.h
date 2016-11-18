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
#include <Plugins/common/HDLParser/HDLComponentParser.h>
#include <Plugins/common/HDLParser/HDLDesignParser.h>

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

    /*!
     *  Sets true for generating, false for not generating.
     */
    void setAddLauncher(bool value);

    /*!
     *  Returns true for generating, false for not generating.
     */
    bool getAddLauncher() const;

signals:
	
    /*!
     *  Emitted when output files have changed.
     */
	void outputFilesChanged(QStringList) const;

private:

	// Disable copying.
	MakeConfiguration(MakeConfiguration const& rhs);
	MakeConfiguration& operator=(MakeConfiguration const& rhs);

    //! The parsers are executed interactively with the configuration
    SWStackParser* parser_;

    //! The file output configuration.
    QSharedPointer<FileOuput> fileOutput_;

    //! If true, a launcher script is generated in conjunction with other files.
    bool addLauncher_;
};

#endif // MAKECONFIGURATION_H
