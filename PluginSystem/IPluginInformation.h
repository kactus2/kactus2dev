//-----------------------------------------------------------------------------
// File: IPluginInformation.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.6.2012
//
// Description:
// Plugin information interface.
//-----------------------------------------------------------------------------

#ifndef IPLUGININFORMATION_H
#define IPLUGININFORMATION_H

#include <QString>
#include <QtPlugin>

//-----------------------------------------------------------------------------
//! Plugin information interface.
//-----------------------------------------------------------------------------
class IPluginInformation
{
public:
    /*!
     *  Destructor.
     */
    virtual ~IPluginInformation() {}

    /*!
     *  Returns the name of the plugin.
     */
    virtual QString const& getName() const = 0;

    /*!
     *  Returns the version of the plugin.
     */
    virtual QString const& getVersion() const = 0;

    /*!
     *  Returns the description of the plugin.
     */
    virtual QString const& getDescription() const = 0;
};

Q_DECLARE_INTERFACE(IPluginInformation, "com.tut.Kactus2.IPluginInformation/1.0")

//-----------------------------------------------------------------------------

#endif // IPLUGININFORMATION_H
