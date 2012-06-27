//-----------------------------------------------------------------------------
// File: PluginListDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.6.2012
//
// Description:
// Plugin list dialog.
//-----------------------------------------------------------------------------

#ifndef PLUGINLISTDIALOG_H
#define PLUGINLISTDIALOG_H

#include <QListWidget>
#include <QDialog>
#include <QPlainTextEdit>

class PluginManager;

//-----------------------------------------------------------------------------
//! Plugin list dialog.
//-----------------------------------------------------------------------------
class PluginListDialog : public QDialog
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] pluginMgr  The plugin manager.
     *      @param [in] parent     The parent widget.
     */
    PluginListDialog(PluginManager* pluginMgr, QWidget* parent);

    /*!
     *  Destructor.
     */
    ~PluginListDialog();

private:
    // Disable copying.
    PluginListDialog(PluginListDialog const& rhs);
    PluginListDialog& operator=(PluginListDialog const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Plugins list widget.
    QListWidget pluginsList_;

    //! Read-only text edit for plugin information.
    QPlainTextEdit detailBox_;
};

//-----------------------------------------------------------------------------

#endif // PLUGINLISTDIALOG_H
