//-----------------------------------------------------------------------------
// File: HDLParserCommon.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 06.07.2016
//
// Description:
// The structures utilized as intermediate storage for data for HDL generation.
//-----------------------------------------------------------------------------

#ifndef HDLPARSERCOMMON_H
#define HDLPARSERCOMMON_H

#include <QSharedPointer>
#include <QString>

#include <QMap>
#include <QMultiMap>

class Component;
class ComponentInstance;
class ComponentInstantiation;
class Design;
class DesignConfiguration;
class View;
class BusInterface;
class AbstractionType;
class AbstractionDefinition;
class Port;
class Parameter;

struct  MetaPort;

struct MetaWire
{
    //! The bounds of the wire.
    QPair<QString,QString> bounds_;
    //! The name of the wire, to tell it apart from other wires of the same interconnection.
    QString name_;

    QList<QSharedPointer<MetaPort> > hierPorts_;
};

struct MetaPortAssignMent
{
    //! The bounds of the assignment.
    QPair<QString,QString> bounds_;
    //! The wire of interconnection where port is assigned. Is null if there is none.
    QSharedPointer<MetaWire> wire_;
    //! The assigned and parsed default value: Either a tie-off or the abstraction definition default.
    QString defaultValue_;
};

struct MetaPort
{
    //! The matching IP-XACT port.
    QSharedPointer<Port> port_;
    //! The parsed vector bounds for the port.
    QPair<QString,QString> vectorBounds_;
    //! The parsed array bounds for the port.
    QPair<QString,QString> arrayBounds_;
    //! The parsed default value for port.
    QString defaultValue_;
    
    QMultiMap<QString, QSharedPointer<MetaPortAssignMent> > assignments_;
};

struct MetaInterface;

struct MetaInterconnection
{
    //! The name of the interconnection, to tell it apart from other interconnections of the design.
    QString name_;

    QMap<QString, QSharedPointer<MetaWire> > wires_;

    QList<QSharedPointer<MetaInterface> > hierIfs_;
};

struct MetaInterface
{
    //! The matching IP-XACT interface.
    QSharedPointer<BusInterface> interface_;
    //! The used abstraction type.
    QSharedPointer<AbstractionType> absType_;
    //! The abstraction definition of the abstraction type.
    QSharedPointer<AbstractionDefinition> absDef_;
    //! The used interface mode.
    QString mode_;
    //! The parsed ports of the component keyed with its physical name.
    QMap<QString, QSharedPointer<MetaPort> > ports_;
    //! The interconnection attached to the interface.
    QSharedPointer<MetaInterconnection> interconnection_;
};

struct GenerationField
{
	//! Describes the offset where this bit field starts.
	QString bitOffset_;
	//! The width of the field in bits.
    QString bitWidth_;
    //! The name of the field.
    QString name_;
};

struct GenerationRegister
{
	//! Number of instances of the register.
    QString dimension_;
    //! The offset of the register within the address block.
    QString offset_;
    //! The size of register in bits.
    QString size_;
    //! The fields within the register.
    QList<QSharedPointer<GenerationField> > fields_;
    //! The name of the register.
    QString name_;
};

struct GenerationAddressBlock
{
    //! The registers within the address block.
    QList<QSharedPointer<GenerationRegister> > registers_;
    //! The base address of the address block.
    QString baseAddress_;
    //! The name of the address block.
    QString name_;
};

struct GenerationRemap
{
    //! Addresses within the remap.
    QList<QSharedPointer<GenerationAddressBlock> > blocks_;
    //! The name of the remap.
    QString name_;
};

struct GenerationRemapState
{
    //! Ports that are to be used as condition for using the remap.
    QList<QSharedPointer<QPair<QSharedPointer<Port>, QString> > > ports_;
    //! The name of the remap state.
    QString stateName_;
};

struct GenerationInterface
{
    //! The matching IP-XACT interface.
    QSharedPointer<BusInterface> interface_;
    //! The used abstraction type.
    QSharedPointer<AbstractionType> absType_;
    //! The abstraction definition of the abstraction type.
    QSharedPointer<AbstractionDefinition> absDef_;
    //! The used interface mode.
    QString mode_;
    //! The description of the interface.
    QString description_;
};

struct GenerationPort
{
    //! The matching IP-XACT port.
    QSharedPointer<Port> port_;
    //! The bounds for the port.
    QPair<QString,QString> vectorBounds_;
    //! The array bounds, if the port is arranged as an array.
    QPair<QString,QString> arrayBounds_;
};

struct GenerationComponent
{
    //! The matching IP-XACT component.
    QSharedPointer<Component> component_;
    //! The original, non-formatted parameters of the component.
    QList<QSharedPointer<Parameter> > originalParameters_;
    //! The formatted parameters of the component, ready for writing.
    QList<QSharedPointer<Parameter> > formattedParameters_;
    //! The parsed interfaces of the component, keyed with its name.
    QMap<QString,QSharedPointer<GenerationInterface> > interfaces_;
    //! The parsed ports of the component keyed with its physical name.
    QMap<QString,QSharedPointer<GenerationPort> > ports_;

    //! The filename for the document.
    QString fileName_;
    //! The module name for HDL.
    QString moduleName_;

    //! The parsed remap states.
    QList<QSharedPointer<GenerationRemapState> > remapStates_;
    //! The parsed remaps, including the default memory map.
    QList<QSharedPointer<GenerationRemap> > remaps_;
    //! The addressable unit bits to be used within the component.
    QString aub_;
    //! Total memory within the component counted in AUBs.
    QString totalRange_;
};

struct GenerationWire
{
    //! The bounds of the wire.
    QPair<QString,QString> bounds_;
    //! The name of the wire, to tell it apart from other wires of the same interconnection.
	QString name_;
};

struct GenerationPortAssignMent
{
    //! The port which is assigned.
	QSharedPointer<GenerationPort> port_;
    //! The bounds of the assignment.
	QPair<QString,QString> bounds_;
    //! The wire of interconnection where port is assigned. Is null if there is none.
	QSharedPointer<GenerationWire> wire_;
    //! The name of the physical port of the top component where the port is assigned. Is empty if there is none.
	QString topPort_;
    //! The assigned tie-off value. Is empty if there is none.
    QString tieOff_;
    //! True, if the port is assigned to ad-hoc interconnection, else false.
    bool adhoc_;
    //! The width used in the corresponding abstraction port wire.
    QString abstractionWidth_;
};

struct GenerationInterconnection
{
    //! The interfaces assigned to the interconnection.
    QList<QSharedPointer<GenerationInterface> > interfaces_;
    //! The assignments of the ports that are mapped to the interconnection, keyed by logical port name.
    QMultiMap<QString,QSharedPointer<GenerationPortAssignMent> > ports_;
    //! The wires parsed for the interconnection.
    QMap<QString,QSharedPointer<GenerationWire> > wires_;
    //! An interface of the top component assigned to the interconnection. Is null if there is none.
    QSharedPointer<GenerationInterface> topInterface_;
    //! The name of the interconnection, to tell it apart from other interconnections of the design.
    QString name_;
    //! The defined type name of the interconnection.
    QString typeName_;
};

struct GenerationInterfaceAssignment
{
    //! The interface which is assigned.
    QSharedPointer<GenerationInterface> interface_;
    //! The interconnection where the interface is assigned to.
    QSharedPointer<GenerationInterconnection> interConnection_;
};

struct GenerationAdHoc
{
    //! The ports of the ad-hoc interconnection.
	QList<QSharedPointer<GenerationPortAssignMent> > ports_;
    //! The wire of the ad-hoc connection.
	QSharedPointer<GenerationWire> wire_;
};

struct GenerationInstance
{
    //! The component referenced by the instance.
    QSharedPointer<GenerationComponent> component_;
    //! The active view of the instance.
    QSharedPointer<View> activeView_;
    //! The matching IP-XACT component instance.
    QSharedPointer<ComponentInstance> componentInstance_;

    //! The parsed parameters that are to be used with the instance. CEVs used where applicable.
    QList<QSharedPointer<Parameter> > parameters_;
    //! The assigned port connections.
    QMap<QString,QSharedPointer<GenerationPortAssignMent> > portAssignments_;
    //! The assigned interface connections.
    QList<QSharedPointer<GenerationInterfaceAssignment> > interfaceAssignments_;

	//! The design and design configuration corresponding the active view, if hierarchical.
    QSharedPointer<Design> design_;
    QSharedPointer<DesignConfiguration> designConfiguration_;
};

struct GenerationDesign
{
    //! The top level component of the design.
    QSharedPointer<GenerationComponent> topComponent_;
    //! The corresponding instance at upper level.
    QSharedPointer<GenerationInstance> topInstance_;
    //! Parsed instances in the design, keyed with their names.
    QMap<QString, QSharedPointer<GenerationInstance> > instances_;

    //! The detected and parsed interconnections between the instances.
    QList<QSharedPointer<GenerationInterconnection> > interConnections_;
    //! The detected and parsed ad-hoc connections between the instances.
    QList<QSharedPointer<GenerationAdHoc> > adHocs_;
    //! The tied values assigned to the top component of the design. Key is the port name.
    QMap<QString, QString> portTiedValues_;
};

#endif // HDLPARSERCOMMON_H
