//-----------------------------------------------------------------------------
// File: NewPluginsDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.1.2014
//
// Description:
// Dialog for displaying new plugins.
//-----------------------------------------------------------------------------

#ifndef NEWPLUGINSDIALOG_H
#define NEWPLUGINSDIALOG_H

#include <QListWidget>
#include <QDialog>
#include <QStackedWidget>
#include <QDialogButtonBox>

class PluginManager;
class IPlugin;

//-----------------------------------------------------------------------------
//! Plugin list dialog.
//-----------------------------------------------------------------------------
class NewPluginsDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent     The parent widget.
     */
    NewPluginsDialog(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~NewPluginsDialog();

    /*!
     *  Adds a plugin to the plugin list.
     *
     *      @param [in] plugin The plugin to add.
     */
    void addPlugin(IPlugin* plugin);

    /*!
     *  Adds plugins to the plugin list.
     *
     *      @param [in] plugins The plugins to add.
     */
    void addPlugins(QList<IPlugin*> plugins);

private slots:

    //! Called when the plugin selection has changed.
    void selectionChanged();

private:
    //! Disable copying.
    NewPluginsDialog(NewPluginsDialog const& rhs);
    NewPluginsDialog& operator=(NewPluginsDialog const& rhs);

    //! Sets the widget layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Plugins list widget.
    QListWidget pluginsList_;

    //! Stack for displaying information of the selected plugin.
    QStackedWidget detailsStack_;

    //! Button box.
    QDialogButtonBox buttonBox_;
};

//-----------------------------------------------------------------------------

#endif // NEWPLUGINSDIALOG_H
