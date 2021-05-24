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

#include "SettingsPage.h"
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

#include <editors/CSourceEditor/CSourceHighlighter.h>

//-----------------------------------------------------------------------------
//! CodeEditorSettingsPage class.
//-----------------------------------------------------------------------------
class CodeEditorSettingsPage : public SettingsPage
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
    virtual ~CodeEditorSettingsPage() = default;

    // Disable copying.
    CodeEditorSettingsPage(CodeEditorSettingsPage const& rhs) = delete;

    CodeEditorSettingsPage& operator=(CodeEditorSettingsPage const& rhs) = delete;

    /*!
     *  Validates the contents of the page thoroughly.
     *
     *      @return True, if the contents are valid. False, if they are invalid.
     *
     *      @remarks Showing message boxes for errors is allowed.
     */
    virtual bool validate()  override final;

    /*!
     *  Applies the changes that were done in the page.
     */
    virtual void apply()  override final;

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
    
    //! Sets the page layout.
    void setupLayout();

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
    ColorBox* previewBox_;

    //! The current (non-saved) highlight styles.
    HighlightStyleDesc styles_[CSourceHighlighter::STYLE_COUNT];
};

//-----------------------------------------------------------------------------

#endif // CODEEDITORSETTINGSPAGE_H
