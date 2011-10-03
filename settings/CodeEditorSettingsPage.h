//-----------------------------------------------------------------------------
// File: CodeEditorSettingsPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 14.6.2011
//
// Description:
// Code editor settings property page.
//-----------------------------------------------------------------------------

#ifndef CODEEDITORSETTINGSPAGE_H
#define CODEEDITORSETTINGSPAGE_H

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>
#include <common/widgets/colorBox/ColorBox.h>
#include <common/widgets/assistedTextEdit/HighlightStyleDesc.h>

#include <QLineEdit>
#include <QSettings>
#include <QRadioButton>
#include <QFontComboBox>
#include <QComboBox>
#include <QListWidget>
#include <QFrame>
#include <QCheckBox>

#include "../MCAPI/MCAPIHighlighter.h"

//-----------------------------------------------------------------------------
//! CodeEditorSettingsPage class.
//-----------------------------------------------------------------------------
class CodeEditorSettingsPage : public PropertyPageView
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in/out] settings The settings store.
     */
    CodeEditorSettingsPage(QSettings& settings);

    /*!
     *  Destructor.
     */
    ~CodeEditorSettingsPage();

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
    //! Requests the user to select a color.
    void selectColor();

    /*!
     *  Called when a different highlight style is selected from the list box.
     */
    void onSelectStyle(QListWidgetItem* cur, QListWidgetItem* prev);

    /*!
     *  Updates the highlight sample.
     */
    void updateSample();

private:
    // Disable copying.
    CodeEditorSettingsPage(CodeEditorSettingsPage const& rhs);
    CodeEditorSettingsPage& operator=(CodeEditorSettingsPage const& rhs);

    /*!
     *  Loads the current settings.
     */
    void loadSettings();

    enum
    {
        MIN_FONT_SIZE = 6,
        MAX_FONT_SIZE = 24
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The settings store.
    QSettings& settings_;

    //! Indent width edit.
    QLineEdit* indentWidthEdit_;

    //! Indentation style radio buttons.
    QRadioButton* indentStyleRadioButtons_[2];

    //! Font combobox.
    QFontComboBox* fontCombo_;

    //! Font size combobox.
    QComboBox* fontSizeCombo_;

    //! Highlight type listbox.
    QListWidget* highlightTypeList_;

    //! Color box for showing the currently selected color.
    ColorBox* colorBox_;

    //! Check box for bold style.
    QCheckBox* boldCheckBox_;

    //! Check box for italic style.
    QCheckBox* italicCheckBox_;

    //! Color box for showing a sample of the highlighted text.
    ColorBox* sampleBox_;

    //! The current (non-saved) highlight styles.
    HighlightStyleDesc styles_[MCAPIHighlighter::STYLE_COUNT];
};

//-----------------------------------------------------------------------------

#endif // CODEEDITORSETTINGSPAGE_H
