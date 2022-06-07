//-----------------------------------------------------------------------------
// File: IncludeImportPlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 09.04.2015
//
// Description:
// Marker interface for include import plugins.
//-----------------------------------------------------------------------------

#ifndef INCLUDEIMPORTPLUGIN_H
#define INCLUDEIMPORTPLUGIN_H

#include "ImportPlugin.h"

//-----------------------------------------------------------------------------
//! Marker interface for include import plugins.
//-----------------------------------------------------------------------------
class IncludeImportPlugin : public ImportPlugin
{
public:
    
    //! The destructor.
    virtual ~IncludeImportPlugin() {};

};

Q_DECLARE_INTERFACE(IncludeImportPlugin, "com.tut.Kactus2.IncludeImportPlugin/1.0")

#endif // INCLUDEIMPORTPLUGIN_H
