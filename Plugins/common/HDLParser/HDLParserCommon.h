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
#include <IPXACTmodels\common\DirectionTypes.h>

class Component;
class ComponentInstance;
class ComponentInstantiation;
class View;
class BusInterface;
class Port;
class Parameter;

struct GenerationField
{
	//! Describes the offset where this bit field starts.
	QString bitOffset_;

	//! Width of the field.
    QString bitWidth_;
    QString name;
};

struct GenerationRegister
{
	//! Width of the field.
    QString dimension_;

    QString offset_;
    QString size_;

    QList<QSharedPointer<GenerationField> > fields;
    QString name;
};

struct GenerationAddressBlock
{
    QList<QSharedPointer<GenerationRegister> > registers;
    QString baseAddress_;
    QString name;
};

struct GenerationRemap
{
    QList<QSharedPointer<GenerationAddressBlock> > blocks;
    QString name;
};

struct GenerationRemapState
{
    QList<QSharedPointer<QPair<QSharedPointer<Port>, QString> > > ports;
    QString stateName;
};

struct GenerationPort
{
    QString name;
    QString typeName;
    QString description;
    DirectionTypes::Direction direction;
    QPair<QString,QString> vectorBounds;
    QPair<QString,QString> arrayBounds;
};

struct GenerationComponent
{
    QSharedPointer<Component> component;
    QList<QSharedPointer<GenerationRemap> > remaps;
    QList<QSharedPointer<Parameter> > parameters;
    QList<QSharedPointer<GenerationPort> > ports;
    QString aub;
    QString totalRange;
    QList<QSharedPointer<GenerationRemapState> > remapStates;
};

struct GenerationWire
{
    QList<QSharedPointer<Port> > ports;
    QPair<QString,QString> bounds;
	QString name;
};

struct GenerationAdHoc
{
	QList<QPair<QString,QString> > ports;
	QSharedPointer<GenerationWire> wire;
	QString name;
	QString tieOff;
};

struct GenerationInterconnection
{
	QList<QPair<QString,QString> > interfaces;
	QMap<QString,QSharedPointer<GenerationWire> > wires_;
	QSharedPointer<BusInterface> topInterface_;
	QString name;
};

struct GenerationPortAssignMent
{
	QSharedPointer<Port> port;
	QPair<QString,QString> bounds;
	QSharedPointer<GenerationWire> wire;
	QString topPortName;
    QString tieOff;
};

struct GenerationInstance
{
	QSharedPointer<Component> component;

	//! The component instance to write to Verilog.
	QSharedPointer<ComponentInstance> componentInstance_;

	//! The component instantiation corresponding the active view.
	QSharedPointer<const ComponentInstantiation> componentInstantiation_;

	//! The component referenced by the instance.
	QSharedPointer<Component> referencedComponent_;

	//! The assigned port connections.
	QMap<QString,QSharedPointer<GenerationPortAssignMent> > portAssignments_;

	QMap<QSharedPointer<BusInterface>,QSharedPointer<GenerationInterconnection> > interfaces_;

	QSharedPointer<View> activeView_;

	QList<QSharedPointer<Parameter> > parameters;
};

#endif // VERILOGGENERATORCOMMON_H