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

    /*!
     *  Writes the given bus definition into xml.
     *
     *      @param [in] writer          The xml writer to use.
     *      @param [in] busDefinition   The bus definition to write.
     */
    void writeBusDefinition(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition) const;

private:

	// Disable copying.
	BusDefinitionWriter(BusDefinitionWriter const& rhs);
	BusDefinitionWriter& operator=(BusDefinitionWriter const& rhs);

    /*!
     *  Writes the broadcast property of a given bus definition into xml.
     *
     *      @param [in] writer          The xml writer to use.
     *      @param [in] busDefinition   The bus definition whose broadcast to write.
     */
    void writeBroadcast(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition) const;
    
    /*!
     *  Writes the bus definition extended in a given bus definition into xml.
     *
     *      @param [in] writer          The xml writer to use.
     *      @param [in] busDefinition   The bus definition whose extends property to write.
     */
    void writeExtends(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition) const;
  
    /*!
     *  Writes the maximum number of masters allowed in a given bus definition into xml.
     *
     *      @param [in] writer          The xml writer to use.
     *      @param [in] busDefinition   The bus definition whose master count to write.
     */
    void writeMaximumMasters(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition) const;
      
    /*!
     *  Writes the maximum number of slaves allowed in a given bus definition into xml.
     *
     *      @param [in] writer          The xml writer to use.
     *      @param [in] busDefinition   The bus definition whose slave count to write.
     */
    void writeMaximumSlaves(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition) const;
      
    /*!
     *  Writes the system group names defined in a given bus definition into xml.
     *
     *      @param [in] writer          The xml writer to use.
     *      @param [in] busDefinition   The bus definition whose system group names to write.
     */
    void writeSystemGroupNames(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition) const;
      
    /*!
     *  Converts boolean value to string.
     *
     *      @param [in] value   The boolean value to convert.
     *
     *      @return     The string representation of the boolean value.
     */
    QString bool2Str(bool value) const;

};    

#endif // BUSDEFINITIONWRITER_H