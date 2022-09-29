//-----------------------------------------------------------------------------
// File: ComponentInstanceVerilogWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.08.2014
//
// Description:
// Class for writing a component instance as a Verilog instance.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCEVERILOGWRITER_H
#define COMPONENTINSTANCEVERILOGWRITER_H

#include "../veriloggeneratorplugin_global.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <KactusAPI/include/ExpressionParser.h>
#include <KactusAPI/include/ExpressionFormatter.h>

#include <Plugins/common/PortSorter/PortSorter.h>
#include <Plugins/VerilogGenerator/common/Writer.h>

#include <QTextStream>
#include <QSharedPointer>
#include <Plugins/common/HDLParser/MetaDesign.h>

//-----------------------------------------------------------------------------
//! Class for writing a component instance as a Verilog instance.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT ComponentInstanceVerilogWriter : public Writer
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] instance                The component instance to write to Verilog
     *      @param [in] sorter                  Sorter for the ports in the component.
     *      @param [in] useInterfaces           True, if interfaces are used where applicable. False means always ports.
	 */
	ComponentInstanceVerilogWriter(QSharedPointer<MetaInstance> instance,
        QSharedPointer<const PortSorter> sorter, bool useInterfaces);

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
        QPair<QString, QString> wireBounds);

    /*!
     *  Assigns a port tie off value for the instance writer.
     *
     *      @param [in] portName        The name of the port containing the tie off value.
     *      @param [in] tieOffValue     The tie off value.
     */
    void assignPortTieOff(QString const& portName, QString const& tieOffValue);

private:

	// Disable copying.
	ComponentInstanceVerilogWriter(ComponentInstanceVerilogWriter const& rhs);
	ComponentInstanceVerilogWriter& operator=(ComponentInstanceVerilogWriter const& rhs);

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
    *  Gets the connection assignment for a port of a module instance.
    *
    *      @param [in] mPort            The meta port whose connection assignment to get.
    *
    *      @return The Verilog description of the instance port connection assignment.
    */
    QString assignmentForInstancePort(QSharedPointer<MetaPort> mPort) const;

    /*!
     *  Get the default value assignment for a port.
     *
     *      @param [in] mPort   The selected port.
     *
     *      @return The default value assignment of the selected port.
     */
    QString getDefaultValueAssignment(QSharedPointer<MetaPort> mPort) const;

    /*!
     *  Get the connection assignment for an INOUT port.
     *
     *      @param [in] mPort   The selected INOUT port.
     *
     *      @return Verilog description of the port assignment.
     */
    QString getInOutAssignment(QSharedPointer<MetaPort> mPort) const;

    /*!
     *  Get the hierarchical port connected to the selected port.
     *
     *      @param [in] metaPort        The selected port.
     *      @param [in] metaAssignment  Assignment for the selected port.
     *      @param [in] assignmentWire  Wire of the connection.
     *
     *      @return The hierarchical port connected to the selected port and its port assignment.
     */
    QPair<QSharedPointer<MetaPort>, QSharedPointer<MetaPortAssignment> > getConnectedHierarchicalPort(
        QSharedPointer<MetaPort> metaPort,
        QSharedPointer<MetaPortAssignment> metaAssignment,
        QSharedPointer<MetaWire> assignmentWire) const;

    /*!
     *  Get the port assignment for the selected port.
     *
     *      @param [in] hierarchicalPort    The selected port.
     *      @param [in] assignedWire        Wire of the connection.
     *
     *      @return Port assignment of the selected port.
     */
    QSharedPointer<MetaPortAssignment> getHierarchicalPortAssignmentConnectedToWire(
        QSharedPointer<MetaPort> hierarchicalPort,
        QSharedPointer<MetaWire> assignedWire) const;

    //! Data for the written default value.
    struct DefaultValueData
    {
        //! The vector width.
        int bitvalue;

        //! Base of the value.
        int base;

        //! Flag for signed value.
        bool isSigned;
    };

    /*!
     *  Get the data required for writing the default value of the selected port.
     *
     *      @param [in] portDefaultValue    The default value.
     *      @param [in] port                The selected port.
     *
     *      @return Data required for writing the default value.
     */
    DefaultValueData getDefaultValueBits(QString const& portDefaultValue, QSharedPointer<MetaPort> port) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	QSharedPointer<MetaInstance> instance_;

	//! Sorter for the ports of the component.
    QSharedPointer<const PortSorter> sorter_;

    //! True, if interfaces are utilized separately from physical ports, else false.
    bool useInterfaces_;
};

#endif // COMPONENTINSTANCEVERILOGWRITER_H
