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

struct GenerationInterface
{
    QSharedPointer<BusInterface> interface;
    QString name;
    QString typeName;
    QString mode;
    QString description;
    QString fileName;
};

struct GenerationPort
{
    QList<QSharedPointer<GenerationInterface> > interfaces;
    QPair<QString,QString> vectorBounds;
    QPair<QString,QString> arrayBounds;
    QSharedPointer<Port> port;
};

struct GenerationComponent
{
    QSharedPointer<Component> component;
    QList<QSharedPointer<GenerationRemap> > remaps;
    QList<QSharedPointer<Parameter> > originalParameters;
    QList<QSharedPointer<Parameter> > formattedParameters;
    QMap<QString,QSharedPointer<GenerationPort> > ports;
    QMap<QString,QSharedPointer<GenerationInterface> > interfaces;
    QString aub;
    QString totalRange;
    QList<QSharedPointer<GenerationRemapState> > remapStates;

    //! The filename for the document.
    QString fileName_;
    //! The module name for HDL.
    QString moduleName_;
};

struct GenerationWire
{
    QList<QSharedPointer<GenerationPort> > ports;
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
	QSharedPointer<GenerationInterface> topInterface_;
    QString name;
    QString typeName;
};

struct GenerationInterfaceAssignment
{
    QSharedPointer<GenerationInterconnection> interConnection_;
    QString name;
};

struct GenerationPortAssignMent
{
	QSharedPointer<GenerationPort> port;
	QPair<QString,QString> bounds;
	QSharedPointer<GenerationWire> wire;
	QString topPort;
    QString tieOff;
    bool adhoc;
};

struct GenerationInstance
{
    //! The component referenced by the instance.
	QSharedPointer<GenerationComponent> component;

	//! The component instance to write to Verilog.
	QSharedPointer<ComponentInstance> componentInstance_;

	//! The instantiations corresponding the active view.
    QSharedPointer<Design> design_;
    QSharedPointer<DesignConfiguration> designConfiguration_;

	//! The assigned port connections.
    QMap<QString,QSharedPointer<GenerationPortAssignMent> > portAssignments_;
    QList<QSharedPointer<GenerationInterfaceAssignment> > interfaceAssignments_;

	QMap<QSharedPointer<BusInterface>,QSharedPointer<GenerationInterconnection> > interfaces_;

	QSharedPointer<View> activeView_;

	QList<QSharedPointer<Parameter> > parameters;
};

struct GenerationDesign
{
    //! The top level component.
    QSharedPointer<GenerationComponent> topComponent_;
    //! The corresponding instance at upper level.
    QSharedPointer<GenerationInstance> topInstance_;

    QList<QSharedPointer<GenerationInstance> > instances_;
    QList<QSharedPointer<GenerationInterconnection> > interConnections_;
    QList<QSharedPointer<GenerationAdHoc> > adHocs_;
    QMap<QString, QString> portTiedValues_;
};

#endif // VERILOGGENERATORCOMMON_H
