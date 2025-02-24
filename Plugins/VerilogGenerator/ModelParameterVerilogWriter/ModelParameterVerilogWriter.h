//-----------------------------------------------------------------------------
// File: ModuleParameterVerilogWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Class for writing model parameter declarations in Verilog.
//-----------------------------------------------------------------------------

#ifndef ModuleParameterVerilogWriter_H
#define ModuleParameterVerilogWriter_H

#include "../veriloggeneratorplugin_global.h"
#include "../common/Writer.h"

#include <QString>
#include <QSharedPointer>
#include <QTextStream>

class Parameter;

//-----------------------------------------------------------------------------
//! Class for writing model parameter declarations in Verilog.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT ModuleParameterVerilogWriter : public Writer
{

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] ModuleParameter  The model parameter represented by this object.
	 */
	ModuleParameterVerilogWriter(QSharedPointer<Parameter> parameter);
	
	//! The destructor
	~ModuleParameterVerilogWriter();

	/*! Write the contents of the model parameter into text stream.
	 *
	 *    @param [in] output   The text stream to write the model parameter into.
	*/
	virtual void write(QTextStream& output) const;

private:
	
	//! No copying.
	ModuleParameterVerilogWriter(const ModuleParameterVerilogWriter& other);

	//! No assignment.
	ModuleParameterVerilogWriter& operator=(const ModuleParameterVerilogWriter& other);

    /*!
     *  Checks if the writer should write nothing.
     *
     *    @return True, if the writer has nothing to write, otherwise false.
     */
    bool nothingToWrite() const;

    /*!
     *  Creates a Verilog parameter declaration from the model parameter.
     *
     *    @return A Verilog parameter declaration.
     */
    QString createDeclaration() const;
    
    /*!
     *  Returns the array bounds for the model parameter.
     *     
     *    @return The array bounds.
     */
    QString arrayBounds() const;

    /*!
     *  Returns the formatted model parameter value.
     *     
     *    @return The formatted model parameter value.
     */
    QString formattedValue() const;
   
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The model parameter to write to Verilog.
    QSharedPointer<Parameter> parameter_;
};

#endif // ModuleParameterVerilogWriter_H
