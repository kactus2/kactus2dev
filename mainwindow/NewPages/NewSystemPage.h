//-----------------------------------------------------------------------------
// File: NewSystemPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 9.6.2011
//
// Description:
// New system property page.
//-----------------------------------------------------------------------------

#ifndef NEWSYSTEMPAGE_H
#define NEWSYSTEMPAGE_H

#include "NewPage.h"

#include <QTreeWidget>
#include <QComboBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>

class LibraryInterface;
class LibraryItem;
class VLNV;

//-----------------------------------------------------------------------------
//! NewSystemPage class.
//-----------------------------------------------------------------------------
class NewSystemPage : public NewPage
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *    @param [in] libInterface    The library interface.
     *    @param [in] parent          The parent widget.
     */
    NewSystemPage(LibraryInterface* libInterface, QWidget* parent);

    /*!
     *  Destructor.
     */
    virtual ~NewSystemPage() = default;

    // Disable copying.
    NewSystemPage(NewSystemPage const& rhs) = delete;
    NewSystemPage& operator=(NewSystemPage const& rhs) = delete;

    /*!
     *  Pre-validates the contents of the page. This is used for enabling/disabling the OK button.
     *
     *    @return True, if the contents are valid and OK button should be enabled.
     *              False, if the contents are invalid and OK button should be disabled.
     *
     *      @remarks Must not show any message boxes.
     */
    bool prevalidate() const final;

    /*!
     *  Validates the contents of the page thoroughly.
     *
     *    @return True, if the contents are valid. False, if they are invalid.
     *
     *      @remarks Showing message boxes for errors is allowed.
     */
    bool validate() final;

    /*!
     *  Applies the changes that were done in the page.
     */
    void apply() final;

    /*!
     *  Called when the page is to be changed and this page would be hidden.
     *
     *    @return False, if the page change should be rejected. Otherwise true.
     */
    bool onPageChange() final;

public slots:

    /*!
     *  Called when the item in the tree widget has changed.
     */
    void onTreeItemChanged(QTreeWidgetItem* cur, QTreeWidgetItem*);

signals:
    //! Signaled when a system design should be created.
    void createSystem(VLNV const& compVLNV, QString const& viewName,
                      VLNV const& sysVLNV, Document::Revision revision, QString const& directory);

private slots:
    //! Called when radio button for mapping options selection is clicked.
    void actionChanged(QAbstractButton* button);

private:

    /*!
     *  Initializes the top component selection listing for HW mapping.
     *
     *    @param [in] libInterface   The library interface.
     */
    void initializeHWComponentSelectionWidget(LibraryInterface* libInterface);

    /*!
     *  Creates an item for selection widget for the given library item finding child items recursively.
     *
     *    @param [in] libraryItem   The library item to create the item for.
     *
     *    @return The item for selection widget.
     */
    QTreeWidgetItem* createChildRecursively(LibraryItem const* libraryItem) const;

    //! Sets the page layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Group box for radio buttons.
    QGroupBox* optionGroupBox_;

    //! Radio button for empty SW architecture action.
    QRadioButton* unmapRadioButton_;

    //! Radio button for map HW action.
    QRadioButton* mapRadioButton_;

    //! Component selection tree widget.
    QTreeWidget* compTreeWidget_;

    //! View selection combo box.
    QComboBox* viewComboBox_;
};

//-----------------------------------------------------------------------------

#endif // NEWSYSTEMPAGE_H
