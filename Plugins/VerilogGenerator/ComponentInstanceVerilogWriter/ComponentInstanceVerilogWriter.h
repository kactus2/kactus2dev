//-----------------------------------------------------------------------------
// File: ComponentInstanceVerilogWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.08.2014
//
// Description:
// Class for writing a component instance as a Verilog instance.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCEVERILOGWRITER_H
#define COMPONENTINSTANCEVERILOGWRITER_H

#include "../veriloggeneratorplugin_global.h"

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/ComponentInstance.h>

#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <Plugins/VerilogGenerator/PortSorter/PortSorter.h>
#include <Plugins/VerilogGenerator/common/Writer.h>

#include <QTextStream>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Class for writing a component instance as a Verilog instance.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT ComponentInstanceVerilogWriter : public Writer
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] instance                The component instance to write to Verilog.
	 *      @param [in] referencedComponent     The component instantiated by the instance.
	 *      @param [in] sorter                  Sorter for the ports in the component.
     *      @param [in] expressionFormatter     The expression formatter.
	 */
	ComponentInstanceVerilogWriter(ComponentInstance const& instance,
        QSharedPointer<const Component> referencedComponent,
        QSharedPointer<const PortSorter> sorter,
        QSharedPointer<ExpressionFormatter> expressionFormatter);

	//! The destructor.
	~ComponentInstanceVerilogWriter();

   /*!
	*  Writes the Verilog instance into a text stream.
    *
    *      @param [in] outputStream   The output to write to.    
    */
    virtual void write(QTextStream& outputStream) const;

    /*!
     *  Adds a port connection assignment for the instance.
     *
     *      @param [in] instancePortName    The name of the port in the instance to assign.
     *      @param [in] assignedConnection  The name of the connection to assign.
     */
    void assignPortForFullWidth(QString const& instancePortName, QString const& assignedConnection);

    /*!
     *  Adds a port connection assignment for the instance for a range of bits.
     *
     *      @param [in] instancePortName    The name of the port in the instance to assign.
     *      @param [in] assignedConnection  The name of the connection to assign.
     *      @param [in] leftBound           The left bound of the connection to assign.
     *      @param [in] rightBound          The right bound of the connection to assign.
     */
    void assignPortForRange(QString const& instancePortName, QString const& assignedConnection, 
        int leftBound, int rightBound);

private:

	// Disable copying.
	ComponentInstanceVerilogWriter(ComponentInstanceVerilogWriter const& rhs);
	ComponentInstanceVerilogWriter& operator=(ComponentInstanceVerilogWriter const& rhs);

    /*!
     *  Checks if the writer should write nothing.
     *
     *      @return True, if the writer has nothing to write, otherwise false.
     */
    bool nothingToWrite() const;

    /*!
     *  Gets the component instance name with or without indentation as required.
     *
     *      @return The formatted instance name.
     */
    QString formattedInstanceName() const;

    /*!
     *  Gets the indentation to use.
     *
     *      @return The indentation to use.
     */
    QString indentation() const;

    /*!
     *  Gets the instance-specific parameter assignments.
     *
     *      @return The Verilog description of the instance parameters and their values.
     */
    QString parameterAssignments() const;

    /*!
    *  Gets the instance ports and their connections.
    *
    *      @return The Verilog description of the instance ports and their connections.
    */
    QString portConnections() const;

    /*!
     *  Creates an interface separator for port connections.
     *
     *      @param [in] interfaceName       The name of the interface for the current port.
     *      @param [in] previousInteface    The name of the interface for the previous port.
     *
     *      @return Separator for port interface.
     */
    QString createInterfaceSeparator(QString const& interfaceName, QString const& previousInteface) const;

    /*!
    *  Gets the connection assignment for a port.
    *
    *      @param [in] portName     The name of the port whose connection assignment to get.
    *
    *      @return The Verilog description of the instance port connection assignment.
    */
    QString connectionForPort(QString portName) const;

    /*!
    *  Checks if a given port is not connected.
    *
    *      @param [in] portName     The name of the port to check.
    *
    *      @return True, if the port is not connected, otherwise false.
    */
    bool notConnected(QString portName) const;

    /*!
     *  Gets the default value for a port in port connection.
     *
     *      @param [in] portName   The name of the port whose default value to get.
     *
     *      @return The default value for the port.
     */
    QString portDefaultValue(QString const& portName) const;

    /*!
    *  Checks if all bits in a given connection are assigned to a port.
    *
    *      @param [in] signalBounds     The connection bounds.
    *
    *      @return True, if all the bits are connected, otherwise false.
    */
    bool assignAllBitsInConnection(General::PortBounds const& signalBounds) const;

    /*!
    *  Checks if only one bit in a given connection is assigned to a port.
    *
    *      @param [in] signalBounds     The connection bounds.
    *
    *      @return True, if one bit is connected, otherwise false.
    */
    bool assignSingleBitInConnection(General::PortBounds const& signalBounds) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component instance to write to Verilog.
    const ComponentInstance componentInstance_;

    //! The component referenced by the instance.
    QSharedPointer<const Component> referencedComponent_;

    //! Sorter for the ports of the component.
    QSharedPointer<const PortSorter> sorter_;

    //! The assigned port connections.
    QMap<QString, General::PortBounds> portAssignments_;

    //! The assigned expression formatter.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;
};

#endif // COMPONENTINSTANCEVERILOGWRITER_H
