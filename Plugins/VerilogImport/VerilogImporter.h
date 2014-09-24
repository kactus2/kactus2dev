//-----------------------------------------------------------------------------
// File: VerilogImporter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.09.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef VERILOGIMPORTER_H
#define VERILOGIMPORTER_H

#include <IPXACTmodels/generaldeclarations.h>

#include <QSharedPointer>
#include <QString>

class Component;

class VerilogImporter
{
public:

	//! The constructor.
	VerilogImporter();

	//! The destructor.
	~VerilogImporter();

    virtual void runParser(QString const& input, QSharedPointer<Component> targetComponent);


private:

	// Disable copying.
	VerilogImporter(VerilogImporter const& rhs);
	VerilogImporter& operator=(VerilogImporter const& rhs);

};

#endif // VERILOGIMPORTER_H
