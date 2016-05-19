//-----------------------------------------------------------------------------
// File: SWStackParser.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 18.05.2016
//
// Description:
// Parses design and its configuration for makefile generation data.
//-----------------------------------------------------------------------------

#ifndef SWSTACKPARSER_H
#define SWSTACKPARSER_H

#include <QString>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BuildCommand.h>

#include <IPXACTmodels/kactusExtensions/SWView.h>

#include <library/LibraryManager/libraryinterface.h>

#include "MakeCommon.h"

class SWStackParser
{
public:
    /*!
     *  The constructor.
     *
	 *      @param [in] library   The library containing all components in the design.
	 */
    SWStackParser( LibraryInterface* library );

    ~SWStackParser();

    // Returns reference to all parsed MakeFileData.
    QSharedPointer<QList<QSharedPointer<MakeFileData> > > getParsedData();

    // Return the general file set.
    const QSharedPointer<FileSet>& getGeneralFileSet();

    // Returns list of all files to be replaced.
    const QStringList& getReplacedFiles();

    /*!
     *  Parses all software components from the design for the data needed in makefiles.
     *
     *      @param [in] topComponent   The top component associated with the design.
     *      @param [in] desgConf   The design configuration object associated with the design.
	 *      @param [in] design   The design object.
	 *      @param [in] sysViewName  Name of the system view pointing to used design configuration.
	 *      @param [in] targetPath  Path where the generated files will be placed.
     */
    void parse( QSharedPointer<Component> topComponent, QSharedPointer<DesignConfiguration const> desgConf,
		QSharedPointer<const Design> design, QString sysViewName, QString targetPath = "" );

private:

    /*!
     *  Checks if softInstance is the topmost instance in its stack.
     *
     *      @param [in] design   The design object.
     *      @param [in] softInstance   The software instance to be checked.
     *      @param [in] softComponent   The software component of the instance.
     *
     *      @return True, if the softInstance is the topmost instance in its stack. Otherwise false.
     */
     bool isTopOfStack(QSharedPointer<const Design> design, QSharedPointer<SWInstance> softInstance,
         QSharedPointer<Component> softComponent);

    /*!
     *  Recursively parses source files for the whole stack fow the given software instance.
	 *
	 *      @param [in] topComponent   The top component associated with the design.
     *      @param [in] softComponent   The component of softInstance.
     *      @param [in] softInstance   The software instance which active view is to be parsed.
     *      @param [in] design   The design object.
     *      @param [in] desgConf   The design configuration object associated with the design.
	 *      @param [in] makeData   The make data associated with the makefile as whole.
	 *      @param [in] sysViewName  Name of the system view pointing to used design configuration.
     */
     void parseStackObjects(QSharedPointer<Component> topComponent, QSharedPointer<Component> softComponent,
		 QSharedPointer<SWInstance> softInstance, QSharedPointer<const Design> design,
		 QSharedPointer<DesignConfiguration const> desgConf, QSharedPointer<MakeFileData> makeData,
		 QString& systemViewName);

    /*!
     *  Tries to find a software instance by name and the component associated with it.
     *
     *      @param [in] design   The design object.
     *      @param [in] instanceName   Name of the searched software instance.
     *      @param [out] targetInstance   The found software instance.
     *
     *      @return The component of the found instance.
     */
     QSharedPointer<Component> searchSWComponent(QSharedPointer<const Design> design, QString instanceName,
		 QSharedPointer<SWInstance>& targetInstance);

	 //! The library containing all components in the design.
	 LibraryInterface* library_;
     //! Collection of data sets, one for each makefile.
	 QSharedPointer<QList<QSharedPointer<MakeFileData> > > parsedData_;
     //! The fileSet for the main makefile and the launcher.
     QSharedPointer<FileSet> generalFileSet_;
     //! List of files that may be replaced
     QStringList replacedFiles_;
};

#endif // SWSTACKPARSER_H
