//-----------------------------------------------------------------------------
// File: ComponentWizardDependencyPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 06.06.2013
//
// Description:
// Dependency analysis page for the component wizard.
//-----------------------------------------------------------------------------

#ifndef COMPONENTWIZARDDEPENDENCYPAGE_H
#define COMPONENTWIZARDDEPENDENCYPAGE_H

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/fileSet/filesetsmodel.h>
#include <editors/ComponentEditor/fileSet/dependencyAnalysis/FileDependencyEditor.h>

#include <QWizardPage>
#include <QSortFilterProxyModel>
#include <QSplitter>

class ParameterFinder;
class FileSetInterface;

//-----------------------------------------------------------------------------
//! Files page for the component wizard.
//-----------------------------------------------------------------------------
class ComponentWizardDependencyPage : public QWizardPage
{
public:

    /*!
     *  The constructor.
     *
     *    @param [in] component           The component.
     *    @param [in] parameterFinder     Finder used to identify parameters.
     *    @param [in] componentPath       Path to the component.
     *    @param [in] fileSetInterface    Interface for accessing file sets.
     *    @param [in] parent              The parent wizard.
     */
    ComponentWizardDependencyPage(QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QString const& componentPath,
        FileSetInterface* fileSetInterface,
        QWidget* parent);

    /*!
     *  Destructor.
     */
    ~ComponentWizardDependencyPage();

    /*!
     *  Returns the ID of the next page.
     */
    virtual int nextId() const;

    /*!
     *  Initializes the page.
     */
    virtual void initializePage();

    /*!
     *  Returns true if the scan has finished; otherwise false.
     */
    virtual bool isComplete() const;

private:
    // Disable copying.
    ComponentWizardDependencyPage(ComponentWizardDependencyPage const& rhs);
    ComponentWizardDependencyPage& operator=(ComponentWizardDependencyPage const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The target component.
    QSharedPointer<Component> component_;

    //! Splitter for the fileset table and dependency graph.
    QSplitter splitter_;

    //! \brief The view to display the file sets.
    EditableTableView view_;

    //! \brief The model that manages the file set objects for the view.
    FileSetsModel model_;

    //! \brief The proxy to do the sorting
    QSortFilterProxyModel proxy_;

    //! The source directories editor.
    FileDependencyEditor editor_;

    //! Interface for accessing file sets.
    FileSetInterface* fileSetInterface_;

    //! List of editable file sets.
    QSharedPointer<QList<QSharedPointer<FileSet> > > availableFileSets_;
};

#endif // COMPONENTWIZARDDEPENDENCYPAGE_H
