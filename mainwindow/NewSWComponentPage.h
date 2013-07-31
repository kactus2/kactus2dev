//-----------------------------------------------------------------------------
// File: NewSWComponentPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.8.2011
//
// Description:
// New SW component page.
//-----------------------------------------------------------------------------

#ifndef NEWSWCOMPONENTPAGE_H
#define NEWSWCOMPONENTPAGE_H

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>
#include <common/KactusAttribute.h>
#include <common/widgets/LibraryPathSelector/librarypathselector.h>

#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>

class LibraryInterface;
class VLNVEditor;
class KactusAttributeEditor;
class VLNV;

//-----------------------------------------------------------------------------
//! NewSWComponentPage class.
//-----------------------------------------------------------------------------
class NewSWComponentPage : public PropertyPageView
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] libInterface The library interface.
     *      @param [in] parentDlg    The parent dialog.
     */
    NewSWComponentPage(LibraryInterface* libInterface, QWidget* parentDlg);

    /*!
     *  Destructor.
     */
    ~NewSWComponentPage();

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
     *  Updates the directory based on the VLNV.
     */
    void updateDirectory();

    //! Called when the user clicks browse button.
    void onBrowse();

signals:
    //! Signaled when a SW component should be created.
    void createSWComponent(VLNV const& vlnv, QString const& directory);

private:
    // Disable copying.
    NewSWComponentPage(NewSWComponentPage const& rhs);
    NewSWComponentPage& operator=(NewSWComponentPage const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* libInterface_;

    //! VLNV editor.
    VLNVEditor* vlnvEditor_;

	//! \brief The editor to select the directory to save to. 
	LibraryPathSelector* directoryEdit_;

    //! Pushbutton for browsing the save directory.
    QPushButton* browseButton_;

    //! Flag for indicating whether the directory has been chosen using browse.
    bool directorySet_;
};

//-----------------------------------------------------------------------------

#endif // NEWSWCOMPONENTPAGE_H
