//-----------------------------------------------------------------------------
// File: NewSystemPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.6.2011
//
// Description:
// New system property page.
//-----------------------------------------------------------------------------

#ifndef NEWSYSTEMPAGE_H
#define NEWSYSTEMPAGE_H

#include <mainwindow/NewPage.h>

#include <QTreeWidget>
#include <QComboBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class LibraryInterface;
class LibraryItem;
class VLNVEditor;
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
     *      @param [in] libInterface The library interface.
     *      @param [in] parentDlg    The parent dialog.
     */
    NewSystemPage(LibraryInterface* libInterface, QWidget* parentDlg);

    /*!
     *  Destructor.
     */
    ~NewSystemPage();

    /*!
     *  Pre-validates the contents of the page. This is used for enabling/disabling the OK button.
     *
     *      @return True, if the contents are valid and OK button should be enabled.
     *              False, if the contents are invalid and OK button should be disabled.
     *
     *      @remarks Must not show any message boxes.
     */
    bool prevalidate() const;

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

public slots:

    /*!
     *  Called when the item in the tree widget has changed.
     */
    void onTreeItemChanged(QTreeWidgetItem* cur, QTreeWidgetItem*);

signals:
    //! Signaled when a system design should be created.
    void createSystem(VLNV const& compVLNV, QString const& viewName,
                      VLNV const& sysVLNV, QString const& directory);

private slots:
    //! Called when radio button for mapping options selection is clicked.
    void actionChanged(QAbstractButton* button);

private:
    // Disable copying.
    NewSystemPage(NewSystemPage const& rhs);
    NewSystemPage& operator=(NewSystemPage const& rhs);

    /*!
     *  Adds child items to a tree item from a library item.
     *
     *      @param [in]      libItem   The library item ("source").
     *      @param [in/out]  treeItem  The tree item ("destination").
     */
    void addChildItems(LibraryItem const* libItem, QTreeWidgetItem* treeItem);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Group box for radio buttons.
    QGroupBox* actionGroupBox_;

    //! Radio button group.
    QButtonGroup* actionGroup_;

    //! Radio button for empty SW architecture action.
    QRadioButton* emptyRadioButton_;

    //! Description label for empty selection.
    QLabel* emptyDescLabel_;

    //! Radio button for map HW action.
    QRadioButton* mapRadioButton_;

    //! Description label fro mapping.
    QLabel* mapDescLabel_;

    //! Label for the component selection tree widget.
    QLabel* treeLabel_;

    //! Component selection tree widget.
    QTreeWidget* compTreeWidget_;

    //! Label for the view selection combo box.
    QLabel* viewLabel_;

    //! View selection combo box.
    QComboBox* viewComboBox_;

    //! The layout for UI widgets.
    QVBoxLayout* layout_;

};

//-----------------------------------------------------------------------------

#endif // NEWSYSTEMPAGE_H
