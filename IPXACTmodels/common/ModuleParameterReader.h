//-----------------------------------------------------------------------------
// File: ModuleParameterReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.09.2015
//
// Description:
// XML reader class for IP-XACT module parameter element.
//-----------------------------------------------------------------------------

#ifndef MODULEPARAMETERREADER_H
#define MODULEPARAMETERREADER_H

#include "ModuleParameter.h"
#include "ParameterReader.h"

#include <QObject>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! XML reader class for IP-XACT module parameter element.
//-----------------------------------------------------------------------------
class ModuleParameterReader : public ParameterReader
{
    Q_OBJECT

public:

	//! The constructor.
    ModuleParameterReader(QObject* parent = 0);

	//! The destructor.
    ~ModuleParameterReader();

    /*!
     *  Creates a module parameter from XML description.
     *
     *      @param [in] moduleParameterNode   The XML description of the module parameter.
     *
     *      @return The created module parameter.
     */
    QSharedPointer<ModuleParameter> createModuleParameterFrom(QDomNode const& moduleParameterNode) const;

private:

	// Disable copying.
    ModuleParameterReader(ModuleParameterReader const& rhs);
    ModuleParameterReader& operator=(ModuleParameterReader const& rhs);

    /*!
     *  Reads the presence of the module parameter.
     *
     *      @param [in] moduleParameterNode     XML description of the module parameter.
     *      @param [in] moduleParameter         The module parameter whose presence is being read.
     */
    void parseIsPresent(QDomNode const& moduleParameterNode, QSharedPointer<ModuleParameter> moduleParameter) const;
};

#endif // MODULEPARAMETERREADER_H