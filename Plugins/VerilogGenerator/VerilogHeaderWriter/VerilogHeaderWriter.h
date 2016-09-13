//-----------------------------------------------------------------------------
// File: VerilogHeaderWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.08.2014
//
// Description:
// Class for generating a header for a Verilog file.
//-----------------------------------------------------------------------------

#ifndef VERILOGHEADERWRITER_H
#define VERILOGHEADERWRITER_H

#include "../veriloggeneratorplugin_global.h"

#include <QDateTime>
#include <QString>
#include <QTextStream>

#include <IPXACTmodels/common/VLNV.h>

//-----------------------------------------------------------------------------
// Class for generating a header for a Verilog file.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT VerilogHeaderWriter 
{
public:

    /*!
     *  The constructor.
	 *
	 *      @param [in] vlnv            The VLNV of the top level component.
     *      @param [in] xmlPath         The path to the top level component XML file.
     *      @param [in] author          The creator of the file.
     *      @param [in] description		The description of the top level component.
	 */
	VerilogHeaderWriter(VLNV const& vlnv, QString const& xmlPath, QString const& author, QString const& description);

	//! The destructor.
	~VerilogHeaderWriter();

    /*!
     *  Writes a header to the given output.
     *
	 *      @param [in] outputStream		The output to write to.
	 *      @param [in] kactusVersion		The version of the current Kactus build.
	 *      @param [in] generatorVersion	The current version of the generator.
     *      @param [in] generationTime		Time of the generation.
     */
    void write(QTextStream& outputStream, QString const& fileName, QString const& generatorVersion,
		QString const& kactusVersion, QDateTime const& generationTime) const;

private:

	// Disable copying.
	VerilogHeaderWriter(VerilogHeaderWriter const& rhs);
	VerilogHeaderWriter& operator=(VerilogHeaderWriter const& rhs);

    //! The VLNV of the top level component.
    VLNV vlnv_;

    //! The path to the top level component XML file.
    QString xmlPath_;

    //! The user who created the file.
    QString author_;

    //! The description of the top level component.
    QString description_;
};

#endif // VERILOGHEADERWRITER_H
