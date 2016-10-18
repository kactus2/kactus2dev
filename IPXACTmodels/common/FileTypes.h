//-----------------------------------------------------------------------------
// File: FileTypes.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.09.2015
//
// Description:
// NameSpace FileTypes is used to store functions related to file types.
//-----------------------------------------------------------------------------

#ifndef FILETYPES_H
#define FILETYPES_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QSettings>
#include <QStringList>
#include <QFileInfo>

//-----------------------------------------------------------------------------
//! NameSpace FileTypes is used to store functions related to file types.
//-----------------------------------------------------------------------------
namespace FileTypes
{
    //! The number of supported file type definitions
    const unsigned int FILE_TYPE_COUNT = 33;

	//! The file types specified in the IP-Xact
	const QString FILE_TYPES[FILE_TYPE_COUNT] = {
		QLatin1String("asmSource"),
		QLatin1String("cSource"),
		QLatin1String("cppSource"),
		QLatin1String("eSource"),
		QLatin1String("OVASource"),
		QLatin1String("perlSource"),
		QLatin1String("pslSource"),
		QLatin1String("SVASource"),
		QLatin1String("tclSource"),
		QLatin1String("veraSource"),
		QLatin1String("systemCSource"),
		QLatin1String("systemCSource-2.0"),
		QLatin1String("systemCSource-2.0.1"),
		QLatin1String("systemCSource-2.1"),
		QLatin1String("systemCSource-2.2"),
		QLatin1String("systemVerilogSource"),
		QLatin1String("systemVerilogSource-3.0"),
		QLatin1String("systemVerilogSource-3.1"),
		QLatin1String("systemVerilogSource-3.1a"),
		QLatin1String("verilogSource"),
		QLatin1String("verilogSource-95"),
		QLatin1String("verilogSource-2001"),
		QLatin1String("vhdlSource"),
		QLatin1String("vhdlSource-87"),
		QLatin1String("vhdlSource-93"),
		QLatin1String("swObject"),
		QLatin1String("swObjectLibrary"),
		QLatin1String("vhdlBinaryLibrary"),
		QLatin1String("verilogBinaryLibrary"),
		QLatin1String("executableHdl"),
		QLatin1String("unelaboratedHdl"),
		QLatin1String("SDC"),
		QLatin1String("unknown")
	};

	/*!
	 *  Checks if the file type belongs to the specified file types.
	 *
	 *      @param [in] fileType    The file type to check.
     *
     *      @return True if the file type is one of the specified ones, false otherwise.
	 */
	IPXACTMODELS_EXPORT bool isIpXactFileType(const QString& fileType);

	/*!
	 *  Get the file types which are associated with the given file suffix.
	 *
	 *      @param [in] settings    The settings instance used to manage Kactus2 settings.
	 *      @param [in] fileSuffix  The file suffix to identify the file type.
     *
     *      @return QStringlist containing the file types.
	 */
	IPXACTMODELS_EXPORT QStringList getFileTypes(QSettings& settings, const QString& fileSuffix);

    /*!
	 *  Get the file types which are associated with the given file suffix.
	 *
	 *      @param [in] settings    The settings instance used to manage the Kactus2 settings.
	 *      @param [in] file        Represents the file which file types are searched.
     *
     *      @return The file types.
	 */
	IPXACTMODELS_EXPORT QStringList getFileTypes(QSettings& settings, const QFileInfo& file);

}

#endif // FILETYPES_H
