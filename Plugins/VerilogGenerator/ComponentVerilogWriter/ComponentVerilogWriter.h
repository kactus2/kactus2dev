//-----------------------------------------------------------------------------
// File: ComponentVerilogWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.08.2014
//
// Description:
// Class for writing a component as a Verilog module.
//-----------------------------------------------------------------------------

#ifndef COMPONENTVERILOGWRITER_H
#define COMPONENTVERILOGWRITER_H

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/port.h>

#include <Plugins/VerilogGenerator/PortSorter/PortSorter.h>
#include <Plugins/VerilogGenerator/ModelParameterVerilogWriter/ModelParameterVerilogWriter.h>
#include <Plugins/VerilogGenerator/common/Writer.h>

#include <QSharedPointer>
#include <QTextStream>
#include <QList>

//-----------------------------------------------------------------------------
//! Class for writing a component as a Verilog module.
//-----------------------------------------------------------------------------
class ComponentVerilogWriter 
{
public:


	/*!
	 *  The constructor.
	 *
	 *      @param [in] component   The component to write to Verilog.
	 *      @param [in] sorter      Sorter for the ports in the component.
	 */
	ComponentVerilogWriter(QSharedPointer<const Component> component, QSharedPointer<const PortSorter> sorter);

	//! The destructor.
	~ComponentVerilogWriter();

	/*! Writes the Verilog module into a text stream.
	 *
	 *      @param [in] output   The text stream to write the module into.
	 */
    void write(QTextStream& outputStream) const;

    void add(Writer* writer);

private:
	// Disable copying.
	ComponentVerilogWriter(ComponentVerilogWriter const& rhs);
	ComponentVerilogWriter& operator=(ComponentVerilogWriter const& rhs);
    
     /*!
      *  Checks if the writer should write nothing.
      *
      *      @return True, if the writer has nothing to write, otherwise false.
      */
    bool nothingToWrite() const;

    /*!
     *  Writes the module declaration.
     *
     *      @param [in] outputStream   The output to write to.
     */
    void writeModuleDeclaration( QTextStream& outputStream ) const;

     /*!
      *  Gets the module port names.     
      *
      *      @return The module port names.
      */
    QString portNames() const;

     /*!
      *  Writes the module parameter declaration.
      *
      *      @param [in] outputStream   The output to write to.
      */
    void writeParameterDeclarations(QTextStream& outputStream) const;

    /*!
     *  Writes the module port declaration.
     *
     *      @param [in] outputStream   The output to write to.
     */
    void writePortDeclarations(QTextStream& outputStream) const;

    /*!
     *  Writes a bus interface introduction.
     *
     *      @param [in] interfaceName           The name of the bus interface to introduce.
     *      @param [in] previousInterfaceName   The name of the previously introduced bus interface.
     *      @param [in] outputStream            The output to write to.
     */
    void writeInterfaceIntroduction(QString const& interfaceName, QString& previousInterfaceName,
        QTextStream& outputStream ) const;

        void writeComponentInstances(QTextStream& outputStream) const;

    /*!
     *  Writes the ending clause for the module.
     *
     *      @param [in] outputStream   The output to write to.
     */
    void writeModuleEnd(QTextStream& outputStream) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component to write to Verilog module.
    QSharedPointer<const Component> component_;

    //! Sorter for the ports of the component.
    QSharedPointer<const PortSorter> sorter_;

    //! Writers for the submodule instances.
    QList<Writer*> instanceWriters_;
};

#endif // COMPONENTVERILOGWRITER_H
