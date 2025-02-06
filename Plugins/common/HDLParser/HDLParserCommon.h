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

class AbstractionType;
class AbstractionDefinition;
class BusInterface;
class Component;
class ComponentInstance;
class ComponentInstantiation;
class Design;
class DesignConfiguration;
class MessageMediator;
class Port;
class RemapState;
class View;

struct GenerationTuple
{
    // The component for which the generator is run. If design is not null, component
    // will refer to design or designConfiguration.
    QSharedPointer<Component> component;
    // The design, if the generator is ran for a design.
    QSharedPointer<Design> design;
    // The design configuration for design, if it is not null.
    QSharedPointer<DesignConfiguration> designConfiguration;
    // The messages.
    MessageMediator* messages;
};

struct MetaPort;

struct MetaWire
{
    //! The bounds of the wire.
    QPair<QString,QString> bounds_;
    //! The array bounds of the wire.
    QPair<QString, QString> arrayBounds_;
    //! The name of the wire, to tell it apart from other wires of the same interconnection.
    QString name_;
    //! The hierarchical ports that are assigned to the wire.
    QList<QSharedPointer<MetaPort> > hierPorts_;
    //! How many times to the wire is referred by a port assignment.
    int refCount;
};

struct MetaTransactional
{
    //! Name of the transactional.
    QString name_;

    //! Hierarchical ports that are assigned to the transactional.
    QList<QSharedPointer<MetaPort> > hierPorts_;

    //! Number of times this transactional has been assigned to a port.
    int refCount_;
};

struct MetaInterface;

struct MetaPortAssignment
{
    //! The interface in which this mapping is made.
    QSharedPointer<MetaInterface> mappedInterface_;
    //! The physical bounds of the assignment.
    QPair<QString,QString> physicalBounds_;
    //! The logical bounds of the assignment.
    QPair<QString,QString> logicalBounds_;
    //! The wire of interconnection where port is assigned. Is null if there is none.
    QSharedPointer<MetaWire> wire_;
    //! Interconnection transactional where the port is assigned. Null if not assigned.
    QSharedPointer<MetaTransactional> transactional_;
    //! The assigned and parsed default value: Either a tie-off or the abstraction definition default.
    QString defaultValue_;
    //! Flag for inverting the signal.
    bool invert_;
};

struct MetaPort
{
    //! The matching IP-XACT port.
    QSharedPointer<Port> port_;
    //! The parsed vector bounds for the port. For wires.
    QPair<QString,QString> vectorBounds_;
    //! Port width. For transactional.
    QString width_;
    //! The parsed array bounds for the port.
    QPair<QString,QString> arrayBounds_;
    //! The parsed default value for port.
    QString defaultValue_;
    //! The parsed assignments for port that go upper level in hierarchy.
    QMultiMap<QString, QSharedPointer<MetaPortAssignment> > upAssignments_;
    //! The parsed assignments for port that go lower level in hierarchy.
    QMultiMap<QString, QSharedPointer<MetaPortAssignment> > downAssignments_;
    //! Flag for wire ports.
    bool isWire_;
    //! Flag for transactional ports.
    bool isTransactional_;
};

struct MetaInterconnection
{
    //! The name of the interconnection, to tell it apart from other interconnections of the design.
    QString name_;
    //! The wires for the logical wires in interconnections.
    QMap<QString, QSharedPointer<MetaWire> > wires_;
    //! The transactionals.
    QMap<QString, QSharedPointer<MetaTransactional> > transactionals_;
    //! The hierarchical interfaces that are assigned to the interconection.
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
    //! The parsed ports of the component keyed with its physical name.
    QMap<QString, QSharedPointer<MetaPort> > ports_;
    //! The interconnection attached to the interface go upper level in hierarchy.
    QSharedPointer<MetaInterconnection> upInterconnection_;
    //! The interconnection attached to the interface go lower level in hierarchy.
    QSharedPointer<MetaInterconnection> downInterconnection_;
};

struct FormattedRemapState
{
    //! Ports and their formatted values that are to be used as the condition for using the remap.
    QList<QSharedPointer<QPair<QSharedPointer<Port>, QString> > > ports_;
    //! The matching IP-XACT remap state.
    QSharedPointer<RemapState> state_;
};

#endif // HDLPARSERCOMMON_H
