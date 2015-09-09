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

#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------
class ModuleParameterWriter : public ParameterWriter
{
    Q_OBJECT
public:

	//! The constructor.
	ModuleParameterWriter(QObject* parent = 0);

	//! The destructor.
	~ModuleParameterWriter();

    /*!
     *  Writes the given module parameter into xml.
     *
     *      @param [in] writer      The xml writer to use.
     *      @param [in] parameter   The module parameter to write.
     */
    void writeModuleParameter(QXmlStreamWriter& writer, QSharedPointer<ModuleParameter> moduleParameter) const;

private:

    // Disable copying.
    ModuleParameterWriter(ModuleParameterWriter const& rhs);
    ModuleParameterWriter& operator=(ModuleParameterWriter const& rhs);

    /*!
     *  Writes the presence of the module parameter.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] moduleParameter     The selected module parameter.
     */
    void writeIsPresent(QXmlStreamWriter& writer, QSharedPointer<ModuleParameter> moduleParameter) const;
};

#endif // MODULEPARAMETERWRITER_H
