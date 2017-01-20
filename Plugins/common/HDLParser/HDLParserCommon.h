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

struct MetaPort;

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
    //! The physical bounds of the assignment.
    QPair<QString,QString> physicalBounds_;
    //! The logical bounds of the assignment.
    QPair<QString,QString> logicalBounds_;
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

#endif // HDLPARSERCOMMON_H
