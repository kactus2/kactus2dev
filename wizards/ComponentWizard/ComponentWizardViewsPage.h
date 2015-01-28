//-----------------------------------------------------------------------------
// File: ComponentWizardViewsPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 09.10.2014
//
// Description:
// Component wizard page for setting up views.
//-----------------------------------------------------------------------------

#ifndef COMPONENTWIZARDVIEWSPAGE_H
#define COMPONENTWIZARDVIEWSPAGE_H

#include <QWizardPage>
#include <QSharedPointer>
#include <QTabWidget>
#include <QListView>
#include <QMap>

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

class Component;
class ComponentWizard;
class LibraryInterface;
class View;
class ViewEditor;
class ViewListModel;

//-----------------------------------------------------------------------------
//! Component wizard page for setting up views.
//-----------------------------------------------------------------------------
class ComponentWizardViewsPage : public QWizardPage
{
    Q_OBJECT
public:

    /*!
     *  The Constructor.
     *
     *      @param [in] lh                      The library interface.
     *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] parent                  The parent wizard.
     */
    ComponentWizardViewsPage(LibraryInterface* lh,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        ComponentWizard* parent);

    //! The destructor.     
    ~ComponentWizardViewsPage();

    /*!
    *  Returns the ID of the next page.
    */
    virtual int nextId() const;

    /*!
    *  Initializes the page.
    */
    virtual void initializePage();
    
    /*!
     *  Returns true if the page is complete and next can be selected.
     */
    virtual bool isComplete() const;

private slots:

    //! Called when a view has been edited.
    virtual void onViewEdited();

    //! Called when an new view should be added.
    void onViewAdded();

    //! Called when a view should be removed.
    void onViewRemoved();


    /*!
     *  Called when a view is selected from the list.
     *
     *      @param [in] index   The index of the selected view.
     */
    void onViewSelected(QModelIndex const& index);

private:
    // Disable copying.
    ComponentWizardViewsPage(ComponentWizardViewsPage const& rhs);
    ComponentWizardViewsPage& operator=(ComponentWizardViewsPage const& rhs);

    /*!
     *  Updates the icon for a given tab according to editor validity.
     *
     *      @param [in] tabIndex   The index of the tab to update.
     */
    void updateIconForTab(int tabIndex) const;

    /*!
     *  Creates an editor for a given view.
     *
     *      @param [in] component   The component whose view the editor is for.
     *      @param [in] view        The view for which to create an editor.
     */
    void createEditorForView(QSharedPointer<Component> component, QSharedPointer<View> view);

    /*!
     *  Removes all editors for a given view.
     *
     *      @param [in] viewName   The name of the view whose editors to remove.
     */
    void removeEditorsForView(QString const& viewName);

    //! Sets the page layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* library_;

    //! The parent wizard.
    ComponentWizard* parent_;

    //! The child widget for holding tabs for different views.
    QTabWidget* editorTabs_;

    //! The button for adding new views.
    QPushButton* addButton_;

    //! The button for removing the selected view.
    QPushButton* removeButton_;

    //! The widget displaying the list of views in a component.
    QListView* viewList_;

    //! The model for view list.
    ViewListModel* viewModel_;

    //! Parameter finder, finds the desired parameters.
    QSharedPointer<ParameterFinder> parameterFinder_;

    //! Expression formatter, formats referencing expressions to show names.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;
};

#endif // COMPONENTWIZARDVIEWSPAGE_H
