//-----------------------------------------------------------------------------
// File: ApiDefinitionWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 11.12.2015
//
// Description:
// Writer class for IP-XACT ApiDefinition element.
//-----------------------------------------------------------------------------

#ifndef ApiDefinitionWriter_H
#define ApiDefinitionWriter_H

#include "ApiDefinition.h"

#include <IPXACTmodels/common/DocumentWriter.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT ApiDefinition element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ApiDefinitionWriter : public DocumentWriter
{
    Q_OBJECT
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    ApiDefinitionWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~ApiDefinitionWriter();

    /*!
     *  Write a view to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] ApiDefinition		The ApiDefinition to be written.
     */
    void writeApiDefinition(QXmlStreamWriter& writer, QSharedPointer<ApiDefinition> apiDefinition) const;

private:

    //! No copying allowed.
    ApiDefinitionWriter(ApiDefinitionWriter const& rhs);
    ApiDefinitionWriter& operator=(ApiDefinitionWriter const& rhs);
};

#endif // ApiDefinitionWriter_H
