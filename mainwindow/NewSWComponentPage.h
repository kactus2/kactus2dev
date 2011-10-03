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

#include <QLineEdit>
#include <QGroupBox>
#include <QRadioButton>

class LibraryInterface;
class VLNVEditor;
class KactusAttributeEditor;
class VLNV;

//-----------------------------------------------------------------------------
//! SWCreateType enumeration.
//-----------------------------------------------------------------------------
enum SWCreateType
{
    SW_CREATE_APPLICATION = 0,
    SW_CREATE_ENDPOINTS,
    SW_CREATE_PLATFORM_COMPONENT,
    SW_CREATE_PLATFORM_STACK,
    SW_CREATE_COUNT
};

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
     *  Asks the user to select a directory.
     */
    void selectDirectory();

    /*!
     *  Updates the directory based on the VLNV.
     */
    void updateDirectory();

signals:
    //! Signaled when a SW component should be created.
    void createSWComponent(SWCreateType createType, VLNV const& vlnv,
                           QString const& directory);

private:
    // Disable copying.
    NewSWComponentPage(NewSWComponentPage const& rhs);
    NewSWComponentPage& operator=(NewSWComponentPage const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* libInterface_;

    //! SW component type group.
    QGroupBox* typeGroup_;

    //! Component type selection radio buttons.
    QRadioButton* typeButtons_[SW_CREATE_COUNT];

    //! VLNV editor.
    VLNVEditor* vlnvEditor_;

    //! Directory line edit.
    QLineEdit* directoryEdit_;
};

//-----------------------------------------------------------------------------

#endif // NEWSWCOMPONENTPAGE_H
