//-----------------------------------------------------------------------------
// File: BusDefinitionWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2015
//
// Description:
// Writer class for IP-XACT BusDefinition element.
//-----------------------------------------------------------------------------

#ifndef BUSDEFINITIONWRITER_H
#define BUSDEFINITIONWRITER_H

#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>

#include <IPXACTmodels/common/ParameterWriter.h>
#include <IPXACTmodels/common/DocumentWriter.h>

#include <IPXACTmodels/ipxactmodels_global.h>

class BusDefinition;
class VLNV;

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT BusDefinition element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT BusDefinitionWriter : public DocumentWriter
{
    Q_OBJECT
public:

	//! The constructor.
	BusDefinitionWriter(QObject* parent = 0);

	//! The destructor.
	~BusDefinitionWriter();

    void writeBusDefinition(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition) const;

private:

	// Disable copying.
	BusDefinitionWriter(BusDefinitionWriter const& rhs);
	BusDefinitionWriter& operator=(BusDefinitionWriter const& rhs);

    void writeBroadcast(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition) const;

    void writeExtends(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition) const;
  
    void writeMaximumMasters(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition) const;

    void writeMaximumSlaves(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition) const;

    void writeSystemGroupNames(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition) const;

    QString bool2Str(bool value) const;

};    

#endif // BUSDEFINITIONWRITER_H