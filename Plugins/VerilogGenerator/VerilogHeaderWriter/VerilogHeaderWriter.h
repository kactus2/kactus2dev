//-----------------------------------------------------------------------------
// File: VerilogHeaderWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.08.2014
//
// Description:
// Class for generating a header for a Verilog file.
//-----------------------------------------------------------------------------

#ifndef VERILOGHEADERWRITER_H
#define VERILOGHEADERWRITER_H

#include <QDateTime>
#include <QString>
#include <QTextStream>

#include <IPXACTmodels/vlnv.h>

//-----------------------------------------------------------------------------
// Class for generating a header for a Verilog file.
//-----------------------------------------------------------------------------
class VerilogHeaderWriter 
{
public:

	/*!
	 *  <Method description>.
	 *
	 *      @param [in] vlnv            The VLNV of the top level component.
     *      @param [in] xmlPath         The path to the top level component XML file.
	 *      @param [in] author          The creator of the file.
	 */
	VerilogHeaderWriter(VLNV const& vlnv, QString const& xmlPath, QString const& author);

	//! The destructor.
	~VerilogHeaderWriter();

    /*!
     *  Writes a header to the given output.
     *
     *      @param [in] outputStream    The output to write to.
     *      @param [in] description     The file description.
     *      @param [in] generationTime  Time of the generation.
     */
    void write(QTextStream& outputStream, QString const& fileName, QString const& description, 
        QDateTime const& generationTime) const;

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
};

#endif // VERILOGHEADERWRITER_H
