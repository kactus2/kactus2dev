//-----------------------------------------------------------------------------
// File: PluginSettingsPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.02.2013
//
// Description:
// Settings page for adjusting plugin settings.
//-----------------------------------------------------------------------------

#ifndef PLUGINSETTINGSPAGE_H
#define PLUGINSETTINGSPAGE_H

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>
#include <common/widgets/DirectoryListSelector/DirectoryListSelector.h>
#include <PluginSystem/PluginManager.h>

#include <QTreeWidget>
#include <QPlainTextEdit>
#include <QSettings>
#include <QStackedWidget>
//-----------------------------------------------------------------------------
//! PluginSettingsPage class.
//-----------------------------------------------------------------------------
class PluginSettingsPage : public PropertyPageView
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in/out] settings   The settings store.
     *      @param [in]     pluginMgr  The plugin manager.
     */
    PluginSettingsPage(QSettings& settings, PluginManager& pluginMgr);

    /*!
     *  Destructor.
     */
    ~PluginSettingsPage();

    /*!
     *  Validates the contents of the page thoroughly.
     *
     *      @return True, if the contents are valid. False, if they are invalid.
     *
     *      @remarks Showing message boxes for errors is allowed.
     */
    bool validate();

    /*!
     *  Applies the changes that were done in the page.
     */
    void apply();

    /*!
     *  Called when the page is to be changed and this page would be hidden.
     *
     *      @return False, if the page change should be rejected. Otherwise true.
     */
    bool onPageChange();

private slots:
    /*!
     *  Updates the correct settings in view when the tree item selection has changed.
     */
    void onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* prev);

    /*!
     *  Updates the plugins tree when plugins directories have changed.
     */
    void onDirectoriesChanged();

private:
    // Disable copying.
    PluginSettingsPage(PluginSettingsPage const& rhs);

    /*!
     *  Updates the plugins tree.
     */
    void refreshPluginsTree();

    PluginSettingsPage& operator=(PluginSettingsPage const& rhs);

    /*!
     *  Creates a new category item to the tree.
     *
     *      @param [in] text  Category title text.
     *      @param [in] icon  Icon for the category.
     */
    QTreeWidgetItem* createCategoryItem(QString const& text, QIcon const& icon);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The settings store.
    QSettings& settings_;

    //! The plugin manager.
    PluginManager& pluginMgr_;

    //! The plugins directories editor.
    DirectoryListSelector pluginDirSelector_;

    //! The plugins tree widget.
    QTreeWidget pluginsTree_;

    //! The settings stack.
    QStackedWidget settingsStack_;

    //! Flag for indicating changes in plugin directories.    
    bool directoriesChanged_;
};

//-----------------------------------------------------------------------------

#endif // PLUGINSETTINGSPAGE_H
