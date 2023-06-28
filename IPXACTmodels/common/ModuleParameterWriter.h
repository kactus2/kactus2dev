//-----------------------------------------------------------------------------
// File: ModuleParameterWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.09.2015
//
// Description:
// Writer class for IP-XACT Module parameter element.
//-----------------------------------------------------------------------------

#ifndef MODULEPARAMETERWRITER_H
#define MODULEPARAMETERWRITER_H

#include "ParameterWriter.h"
#include "ModuleParameter.h"
#include "Document.h"

#include <QSharedPointer>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------
class ModuleParameterWriter : public ParameterWriter
{
public:

	//! The constructor.
	ModuleParameterWriter();

	//! The destructor.
	~ModuleParameterWriter() final;

    // Disable copying.
    ModuleParameterWriter(ModuleParameterWriter const& rhs) = delete;
    ModuleParameterWriter& operator=(ModuleParameterWriter const& rhs) = delete;

    /*!
     *  Writes the given module parameter into xml.
     *
     *      @param [in] writer      The xml writer to use.
     *      @param [in] parameter   The module parameter to write.
     *      @param [in] docRevision The document std revision.
     */
    void writeModuleParameter(QXmlStreamWriter& writer, QSharedPointer<ModuleParameter> moduleParameter, 
        Document::Revision docRevision = Document::Revision::Std14) const;

};

#endif // MODULEPARAMETERWRITER_H
