//-----------------------------------------------------------------------------
// File: MCAPISettingsWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.02.2013
//
// Description:
// MCAPI settings widget.
//-----------------------------------------------------------------------------

#ifndef MCAPISETTINGSWIDGET_H
#define MCAPISETTINGSWIDGET_H

#include <Plugins/PluginSystem/PluginSettingsWidget.h>

#include <QLineEdit>

//-----------------------------------------------------------------------------
//! MCAPI settings widget.
//-----------------------------------------------------------------------------
class MCAPISettingsWidget : public PluginSettingsWidget
{
public:
    /*!
     *  Constructor.
     */
    MCAPISettingsWidget();

    /*!
     *  Destructor.
     */
    ~MCAPISettingsWidget();

private:
    // Disable copying.
    MCAPISettingsWidget(MCAPISettingsWidget const& rhs);
    MCAPISettingsWidget& operator=(MCAPISettingsWidget const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
};


#endif // MCAPISETTINGSWIDGET_H
