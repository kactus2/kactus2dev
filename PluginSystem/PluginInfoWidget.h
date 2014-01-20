//-----------------------------------------------------------------------------
// File: PluginInfoWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.1.2014
//
// Description:
// Widget for displaying plugin information.
//-----------------------------------------------------------------------------

#ifndef PLUGININFOWIDGET_H
#define PLUGININFOWIDGET_H

#include "IPlugin.h"

#include <QWidget>

//-----------------------------------------------------------------------------
// class PluginInfoWidget.
//-----------------------------------------------------------------------------
class PluginInfoWidget : public QWidget
{
public:
    
    /*!
     *  <Description>.
     *
     *      @param [in] plugin   The plugin whose information to display.
     *      @param [in] parent   The parent widget.
     *
     *      @return 
     */
    PluginInfoWidget(IPlugin* plugin = 0, QWidget *parent = 0);

    //! The destructor.
    ~PluginInfoWidget();

private:
    
    //! Sets the widget layout.
    void setupLayout();

    //! The plugins whose information is displayed in this widget.
    IPlugin* plugin_;
};

#endif // PLUGININFOWIDGET_H
