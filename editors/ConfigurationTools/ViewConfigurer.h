//-----------------------------------------------------------------------------
// File: ViewConfigurer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.05.2015
//
// Description:
// Configures the views in the component design of the selected component.
//-----------------------------------------------------------------------------

#ifndef VIEWCONFIGURER_H
#define VIEWCONFIGURER_H

#include <QDialog>
#include <QTreeWidget>
#include <QObject>
#include <QStringList>
#include <QPushButton>

class Component;
class Design;
class DesignConfiguration;
class LibraryHandler;
class Kactus2Group;
//-----------------------------------------------------------------------------
//! Configures the views in the component design of the selected component.
//-----------------------------------------------------------------------------
class ViewConfigurer : public QDialog
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] selectedComponent               The component used to configure the views.
     *      @param [in] selectedDesign                  The component design used to configure the views.
     *      @param [in] selectedDesignConfiguration     The design configuration used to configure the views.
     *      @param [in] libraryHandler                  The library manager.
     *      @param [in] openViewName                    The name of the currently open view.
     *      @param [in] parent                          The parent of this widget.
     */
    ViewConfigurer(QSharedPointer<Component> selectedComponent, QSharedPointer<Design> selectedDesign,
        QSharedPointer<DesignConfiguration> selectedDesignConfiguration, LibraryHandler* libraryHandler,
        QString const& openViewName, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~ViewConfigurer();

private slots:

    /*!
     *  Handles the changes in the instance views.
     *
     *      @param [in] changedItem     The changed tree widget item.
     *      @param [in] column          The column of the changed data.
     */
    void onInstanceViewChanged(QTreeWidgetItem* changedItem, int column);

    /*!
     *  Save the data created to the design configurations vendor extensions.
     */
    void saveAndCloseConfigurer();

private:
    // No copying allowed.
    ViewConfigurer(ViewConfigurer const& rhs);

    // No assignment allowed.
    ViewConfigurer& operator=(ViewConfigurer const& rhs);

    /*!
     *  Setup the layout.
     */
    void setupLayout();

    /*!
     *  Create child tree items.
     *
     *      @param [in] currentDesign               The design associated with the current tree item.
     *      @param [in] currentDesignConfiguration  The design configuration associated with the current tree item.
     *      @param [in] parentItem                  The parent tree item of the child items.
     */
    void createChildTreeWidgetItems(QSharedPointer<Design> currentDesign,
        QSharedPointer<DesignConfiguration> currentDesignConfiguration, QTreeWidgetItem* parentItem);
    
    /*!
     *  Check the possible design of the component instance.
     *
     *      @param [in] component           The component associated with the instance.
     *      @param [in] viewName            The name of the view used by the instance.
     *      @param [in] currentTreeItem     The tree item associated with the instance.
     */
    void checkInstanceDesign(QSharedPointer<Component> component, QString const& viewName,
        QTreeWidgetItem* currentTreeItem);

    /*!
     *  Modify the contents of the tree with the tree with the stored view configuration.
     */
    void modifyTreeWithExistingViewGroup();

    /*!
     *  Change the view data of the tree item, if found in the stored configuration.
     *
     *      @param [in] currentTreeItem     The current tree item.
     *      @param [in] viewOverrides       The stored view overrides.
     */
    void parseExistingInstanceView(QTreeWidgetItem* currentTreeItem, QMap<QString, QString> viewOverrides);

    /*!
     *  Check if the changed item is a top level tree item.
     *
     *      @param [in] changedTreeItem     The changed item.
     *
     *      @return True, if the changed item is a top item, otherwise false.
     */
    bool isChangedItemTopItem(QTreeWidgetItem* changedTreeItem);

    /*!
     *  A changed top item means that the used design configuration needs to be changed.
     *
     *      @param [in] component   The component associated with the top tree item.
     *      @param [in] viewName    The new view name.
     */
    void changedTopItemChangesDesignConfiguration(QSharedPointer<Component> component, QString const& viewName);

    /*!
     *  Set the enabled status for the commit button.
     */
    void setCommmitButtonEnabledStatus();

    /*!
     *  Check if the tree item contains empty or cyclical views.
     *
     *      @param [in] treeItem    The selected tree item.
     */
    bool treeItemContainsEmptyOrCyclicalViews(QTreeWidgetItem* treeItem);

    /*!
     *  Prepare the tree items for saving.
     *
     *      @param [in] treeItem        The current tree item.
     *      @param [in] viewOverrides   The view overrides.
     */
    void parseChildTreeItem(QTreeWidgetItem* treeItem, QMap<QString, QString>& viewOverrides);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The design configuration used to store the view configuration.
    QSharedPointer<DesignConfiguration> selectedDesignConfiguration_;

    //! The constructed tree.
    QTreeWidget* viewsTree_;

    //! The library manager
    LibraryHandler* libraryHandler_;

    //! A list of used hierarchical component VLNVs, used to check for cyclical designs.
    QStringList usedHierarchicalComponentVLNVS_;

    //! The commit button.
    QPushButton* commitButton_; 
};

//-----------------------------------------------------------------------------

#endif // VIEWCONFIGURER_H
