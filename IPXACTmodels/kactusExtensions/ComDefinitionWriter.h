//-----------------------------------------------------------------------------
// File: ComDefinitionWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 11.12.2015
//
// Description:
// Writer class for IP-XACT ComDefinition element.
//-----------------------------------------------------------------------------

#ifndef ComDefinitionWriter_H
#define ComDefinitionWriter_H

#include "ComDefinition.h"

#include <IPXACTmodels/common/DocumentWriter.h>

#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT ComDefinition element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComDefinitionWriter : public DocumentWriter
{
    Q_OBJECT
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    ComDefinitionWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~ComDefinitionWriter();

    /*!
     *  Write a view to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] ComDefinition		The ComDefinition to be written.
     */
    void writeComDefinition(QXmlStreamWriter& writer, QSharedPointer<ComDefinition> ComDefinition) const;

private:

    //! No copying allowed.
    ComDefinitionWriter(ComDefinitionWriter const& rhs);
    ComDefinitionWriter& operator=(ComDefinitionWriter const& rhs);
};

#endif // ComDefinitionWriter_H
