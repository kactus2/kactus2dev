//-----------------------------------------------------------------------------
// File: HDLParserCommon.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 06.07.2016
//
// Description:
// Joku
//-----------------------------------------------------------------------------

#ifndef VERILOGGENERATORCOMMON_H
#define VERILOGGENERATORCOMMON_H

#include <QSharedPointer>
#include <QString>

class Component;
class ComponentInstance;
class ComponentInstantiation;
class View;
class BusInterface;

struct GenerationPort
{
	//QPair<QString,QString> bounds;
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
	QSharedPointer<BusInterface> topInterface_;
	QString name;
};

struct GenerationPortAssignMent
{
	QSharedPointer<GenerationPort> port;
	QPair<QString,QString> bounds;
	QSharedPointer<GenerationWire> wire;
	QString otherName;
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

	//! The assigned tie off values.
	QMap<QString,QString> tieOffAssignments_;

	QSharedPointer<View> activeView_;
};

#endif // VERILOGGENERATORCOMMON_H