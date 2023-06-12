//-----------------------------------------------------------------------------
// File: FileType.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.09.2015
//
// Description:
// NameSpace FileTypes is used to store functions related to file types.
//-----------------------------------------------------------------------------

#ifndef FILETYPE_H
#define FILETYPE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include "Document.h"

#include <QString>
#include <QSettings>
#include <QStringList>
#include <QFileInfo>

struct FileType
{
	QString type_;
	QString libext_;

	FileType() : type_(), libext_() {};

	explicit FileType(QString const& fileType, QString const& libext = QString()) :
		type_(fileType), libext_(libext) {};

	bool operator==(FileType const& rhs) const { return type_ == rhs.type_ && libext_ == rhs.libext_; };
};

//-----------------------------------------------------------------------------
//! NameSpace FileTypes is used to store functions related to file types.
//-----------------------------------------------------------------------------
namespace FileTypes
{
	//! The file types specified in the IP-Xact
	const QStringList FILE_TYPES = {
		QStringLiteral("asmSource"),
		QStringLiteral("cSource"),
		QStringLiteral("cppSource"),
		QStringLiteral("eSource"),
		QStringLiteral("OVASource"),
		QStringLiteral("perlSource"),
		QStringLiteral("pslSource"),
		QStringLiteral("SVASource"),
		QStringLiteral("tclSource"),
		QStringLiteral("veraSource"),
		QStringLiteral("systemCSource"),
		QStringLiteral("systemCSource-2.0"),
		QStringLiteral("systemCSource-2.0.1"),
		QStringLiteral("systemCSource-2.1"),
		QStringLiteral("systemCSource-2.2"),
		QStringLiteral("systemVerilogSource"),
		QStringLiteral("systemVerilogSource-3.0"),
		QStringLiteral("systemVerilogSource-3.1"),
		QStringLiteral("systemVerilogSource-3.1a"),
		QStringLiteral("verilogSource"),
		QStringLiteral("verilogSource-95"),
		QStringLiteral("verilogSource-2001"),
		QStringLiteral("verilogSource-2005"),
		QStringLiteral("vhdlSource"),
		QStringLiteral("vhdlSource-87"),
		QStringLiteral("vhdlSource-93"),
		QStringLiteral("swObject"),
		QStringLiteral("swObjectLibrary"),
		QStringLiteral("vhdlBinaryLibrary"),
		QStringLiteral("verilogBinaryLibrary"),
		QStringLiteral("executableHdl"),
		QStringLiteral("unelaboratedHdl"),
		QStringLiteral("systemVerilogSource"),
		QStringLiteral("systemVerilogSource-3.0"),
		QStringLiteral("systemVerilogSource-3.1"),
		QStringLiteral("systemVerilogSource-3.1a"),
		QStringLiteral("SDC"),
		QStringLiteral("unknown")
	};

	const QStringList FILE_TYPES_2022
	{
		QStringLiteral("vhdlSource2002"),
		QStringLiteral("vhdlSource2008"),
		QStringLiteral("systemVerilogSource-2009"),
		QStringLiteral("systemVerilogSource-2012"),
		QStringLiteral("systemVerilogSource-2017"),
		QStringLiteral("vhdlAmsSource"),
		QStringLiteral("verilogAmsSource"),
		QStringLiteral("systemCAmsSource"),
		QStringLiteral("libertySource"),
		QStringLiteral("spiceSource"),
		QStringLiteral("systemRDL"),
		QStringLiteral("systemRDL-1.0"),
		QStringLiteral("systemRDL-2.0"),
		QStringLiteral("systemCSource-2.3"),
		QStringLiteral("systemCSourceBinaryLibrary")
	};

	/*!
	 *  Checks if the file type belongs to the specified file types.
	 *
	 *      @param [in] fileType    The file type to check.
     *
     *      @return True if the file type is one of the specified ones, false otherwise.
	 */
	IPXACTMODELS_EXPORT bool isIpXactFileType(QString const& fileType, Document::Revision docRevision);

	/*!
	 *  Get the file types which are associated with the given file suffix.
	 *
	 *      @param [in] settings    The settings instance used to manage Kactus2 settings.
	 *      @param [in] fileSuffix  The file suffix to identify the file type.
     *
     *      @return QStringlist containing the file types.
	 */
	IPXACTMODELS_EXPORT QStringList getFileTypes(QSettings& settings, QString const& fileSuffix);

    /*!
	 *  Get the file types which are associated with the given file suffix.
	 *
	 *      @param [in] settings    The settings instance used to manage the Kactus2 settings.
	 *      @param [in] file        Represents the file which file types are searched.
     *
     *      @return The file types.
	 */
	IPXACTMODELS_EXPORT QStringList getFileTypes(QSettings& settings, QFileInfo const& file);

}

#endif // FILETYPE_H
