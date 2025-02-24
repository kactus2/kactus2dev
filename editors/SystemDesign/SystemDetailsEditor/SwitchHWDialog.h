//-----------------------------------------------------------------------------
// File: SwitchHWDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 31.7.2012
//
// Description:
// Dialog for configuring settings how to switch HW for a system design.
//-----------------------------------------------------------------------------

#ifndef SWITCHHWDIALOG_H
#define SWITCHHWDIALOG_H

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/assistedLineEdit/BasicLineContentMatcher.h>

#include <QComboBox>
#include <QRadioButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QDialog>
#include <QButtonGroup>
#include <QDialogButtonBox>

class Component;
class LineEditEx;
class LibrarySelectorWidget;
//-----------------------------------------------------------------------------
// Dialog for configuring settings how to switch HW for a system design.
//-----------------------------------------------------------------------------
class SwitchHWDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *    @param [in] component  The new HW component to which to switch.
     *    @param [in] viewName   The initial name suggestion for the system view.
     *    @param [in] lh         The library interface.
     *    @param [in] parent     The parent widget.
     */
    SwitchHWDialog(QSharedPointer<Component> component, QString const& viewName, LibraryInterface* lh,
                   QWidget* parent);

    /*!
     *  Destructor.
     */
    ~SwitchHWDialog();

    /*!
     *  Sets the system view name suggestions.
     *
     *    @param [in] suggestions The list of suggestions.
     */
    void setViewNameSuggestions(QStringList const& suggestions);

    /*!
     *  Shows a combo box selection for choosing a HW view reference.
     */
    void showHWViewSelector();

    /*!
     *  Returns the selected HW view reference.
     */
    QString getHWViewRef() const;

    /*!
     *  Returns the system view name.
     */
    QString getSystemViewName() const;

    /*!
     *  Returns the base VLNV for sysdesign and sysdesigncfg.
     */
    VLNV getVLNV() const;

    /*!
     *  Returns the selected path for new IP-XACT objects.
     */
    QString getPath() const;

    /*!
     *  If true, the design should be copied instead of moving.
     */
    bool isCopyActionSelected() const;

private slots:
    /*!
     *  Called when the user presses OK button.
     */
    virtual void accept();

    /*!
     *  Called when the action selection has changed.
     *
     *    @param [in] button The action button.
     */
    void actionChanged(QAbstractButton* button);

    /*!
     *  Updates the directory based on the current VLNV.
     */
    void updateDirectory();

    /*!
     *  Validates the contents of the dialog and enables/disables the OK button based on the validation result.
     */
    void validate();

private:
    // Disable copying.
    SwitchHWDialog(SwitchHWDialog const& rhs);
    SwitchHWDialog& operator=(SwitchHWDialog const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* lh_;

    //! The new HW component.
    QSharedPointer<Component> component_;

    //! Label for information text.
    QLabel* infoLabel_;

    //! Label for HW view reference.
    QLabel* hwViewRefLabel_;

    //! Combo box for HW view reference.
    QComboBox* hwViewRefCombo_;

    //! Label for system view name.
    QLabel* viewNameLabel_;

    //! Edit box for system view name.
    LineEditEx* viewNameEdit_;

    //! View name content matcher.
    BasicLineContentMatcher viewNameMatcher_;

    //! Group box for radio buttons.
    QGroupBox* actionGroupBox_;

    //! Radio button group.
    QButtonGroup* actionGroup_;

    //! Radio button for move action.
    QRadioButton* moveRadioButton_;

    //! Description label for move action.
    QLabel* moveDescLabel_;

    //! Radio button for copy action.
    QRadioButton* copyRadioButton_;

    //! Description label for copy action.
    QLabel* copyDescLabel_;

    //! VLNV editor for design/designcfg VLNV (shown in case of copy action).
    VLNVEditor* vlnvEdit_;

	//! The editor to select the directory to save to. 
    LibrarySelectorWidget* directoryEditor_;

    //! Button box for OK and Cancel buttons.
    QDialogButtonBox* buttonBox_;

    //! The layout for the UI widgets.
    QVBoxLayout* layout_;
};

//-----------------------------------------------------------------------------

#endif // SWITCHHWDIALOG_H
