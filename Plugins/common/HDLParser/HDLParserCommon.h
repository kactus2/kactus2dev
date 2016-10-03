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

#ifndef VERILOGGENERATORCOMMON_H
#define VERILOGGENERATORCOMMON_H

#include <QSharedPointer>
#include <QString>

#include <IPXACTmodels/common/DirectionTypes.h>

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

struct GenerationField
{
	//! Describes the offset where this bit field starts.
	QString bitOffset_;
	//! The width of the field in bits.
    QString bitWidth_;
    //! The name of the field.
    QString name;
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
    QList<QSharedPointer<GenerationField> > fields;
    //! The name of the register.
    QString name;
};

struct GenerationAddressBlock
{
    //! The registers within the address block.
    QList<QSharedPointer<GenerationRegister> > registers;
    //! The base address of the address block.
    QString baseAddress_;
    //! The name of the address block.
    QString name;
};

struct GenerationRemap
{
    //! Addresses within the remap.
    QList<QSharedPointer<GenerationAddressBlock> > blocks;
    //! The name of the remap.
    QString name;
};

struct GenerationRemapState
{
    //! Ports that are to be used as condition for using the remap.
    QList<QSharedPointer<QPair<QSharedPointer<Port>, QString> > > ports;
    //! The name of the remap state.
    QString stateName;
};

struct GenerationInterface
{
    //! The matching IP-XACT interface.
    QSharedPointer<BusInterface> interface;
    //! The used abstraction type.
    QSharedPointer<AbstractionType> absType;
    //! The abstractino definition of the abstraction type.
    QSharedPointer<AbstractionDefinition> absDef;
    //! The used interface mode.
    QString mode;
    //! The description of the interface.
    QString description;
    //! The name of the interface.
    QString name;
};

struct GenerationPort
{
    //! The matching IP-XACT port.
    QSharedPointer<Port> port;
    //! The interfaces detected using the port.
    //QList<QSharedPointer<GenerationInterface> > interfaces;
    //! The bounds for the port.
    QPair<QString,QString> vectorBounds;
    //! The array bounds, if the port is arranged as an array.
    QPair<QString,QString> arrayBounds;
};

struct GenerationComponent
{
    //! The matching IP-XACT component.
    QSharedPointer<Component> component;
    //! The original, non-formatted parameters of the component.
    QList<QSharedPointer<Parameter> > originalParameters;
    //! The formatted paramaters of the component, ready for writing.
    QList<QSharedPointer<Parameter> > formattedParameters;
    //! The parsed interfaces of the component, keyed with its name.
    QMap<QString,QSharedPointer<GenerationInterface> > interfaces;
    //! The parsed ports of the component keyed with its physical name.
    QMap<QString,QSharedPointer<GenerationPort> > ports;

    //! The filename for the document.
    QString fileName_;
    //! The module name for HDL.
    QString moduleName_;

    //! The parsed remap states.
    QList<QSharedPointer<GenerationRemapState> > remapStates;
    //! The parsed remaps, including the default memory map.
    QList<QSharedPointer<GenerationRemap> > remaps;
    //! The addressable unit bits to be used within the component.
    QString aub;
    //! Total memory within the component counted in AUBs.
    QString totalRange;
};

struct GenerationWire
{
    //! The bounds of the wire.
    QPair<QString,QString> bounds;
    //! The name of the wire, to tell it apart from other wires of the same interconnection.
	QString name;
};

struct GenerationAdHoc
{
    //! The ports of the ad-hoc interconnection.
	QList<QPair<QString,QString> > ports;
    //! The wire of the ad-hoc connection.
	QSharedPointer<GenerationWire> wire;
    //! If this is actually tied value assignment, this is non-empty. 
	QString tieOff;
};

struct GenerationInterconnection
{
    //! The interfaces assigned to the interconnection.
	QList<QSharedPointer<GenerationInterface> > interfaces;
    //! The wires parsed for the interconnection.
	QMap<QString,QSharedPointer<GenerationWire> > wires_;
    //! An interface of the top component assigned to the interconnection. Is null if there is none.
	QSharedPointer<GenerationInterface> topInterface_;
    //! The name of the interconnection, to tell it apart from other interconnections of the design.
    QString name;
    //! The defined type name of the interconnection.
    QString typeName;
};

struct GenerationInterfaceAssignment
{
    //! The interface which is assigned.
    QSharedPointer<GenerationInterface> interface_;
    //! The interconnection where the interface is assigned to.
    QSharedPointer<GenerationInterconnection> interConnection_;
};

struct GenerationPortAssignMent
{
    //! The port which is assigned.
	QSharedPointer<GenerationPort> port;
    //! The bounds of the assignment.
	QPair<QString,QString> bounds;
    //! The wire of interconnection where port is assigned. Is null if there is none.
	QSharedPointer<GenerationWire> wire;
    //! The name of the physical port of the top component where the port is assigned. Is empty if there is none.
	QString topPort;
    //! The assigned tie-off value. Is empty if there is none.
    QString tieOff;
    //! True, if the port is assigned to ad-hoc interconnection, else false.
    bool adhoc;
};

struct GenerationInstance
{
    //! The component referenced by the instance.
	QSharedPointer<GenerationComponent> component;

	//! The matching IP-XACT component instance.
	QSharedPointer<ComponentInstance> componentInstance_;

	//! The design and design configuration corresponding the active view.
    QSharedPointer<Design> design_;
    QSharedPointer<DesignConfiguration> designConfiguration_;

	//! The assigned port connections.
    QMap<QString,QSharedPointer<GenerationPortAssignMent> > portAssignments_;

    //! The assigned interface connections.
    QList<QSharedPointer<GenerationInterfaceAssignment> > interfaceAssignments_;

    //! The active view of the instance.
	QSharedPointer<View> activeView_;

    //! The parsed parameters that are to be used with the instance. CEVs used where applicable.
	QList<QSharedPointer<Parameter> > parameters;
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

#endif // VERILOGGENERATORCOMMON_H
