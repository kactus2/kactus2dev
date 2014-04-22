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
#include <QDialogButtonBox>

class PluginManager;
class IPlugin;

//-----------------------------------------------------------------------------
//! Plugin list dialog.
//-----------------------------------------------------------------------------
class PluginListDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] pluginMgr  The plugin manager.
     *      @param [in] parent     The parent widget.
     */
    PluginListDialog(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~PluginListDialog();

    /*!
     *  Adds a plugin to the plugin list.
     *
     *      @param [in] plugin The plugin to add.
     */
    void addPlugin(IPlugin* plugin);

    /*!
     *  Returns the selected plugin.
     */
    IPlugin* getSelectedPlugin() const;

private slots:
    //! Called when the plugin selection has changed.
    void selectionChanged();

private:
    // Disable copying.
    PluginListDialog(PluginListDialog const& rhs);
    PluginListDialog& operator=(PluginListDialog const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The plugin manager.
    PluginManager* pluginMgr_;

    //! Plugins list widget.
    QListWidget pluginsList_;

    //! Read-only text edit for plugin information.
    QPlainTextEdit detailBox_;

    //! Button box.
    QDialogButtonBox buttonBox_;
};

//-----------------------------------------------------------------------------

#endif // PLUGINLISTDIALOG_H
