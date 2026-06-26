//-----------------------------------------------------------------------------
// File: MandatoryComboBox.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 26.06.2026
//
// Description:
// Combobox for mandatory items. Implements correct item colors for dark mode.
//-----------------------------------------------------------------------------

#pragma once

#include <QComboBox>

class MandatoryComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit MandatoryComboBox(QWidget* parent = nullptr);
    virtual ~MandatoryComboBox() = default;
    
    MandatoryComboBox(MandatoryComboBox const& other) = delete;
    MandatoryComboBox& operator=(MandatoryComboBox const& other) = delete;

    //! Overridden functions for changing color of combobox items correctly when using dark mode
	virtual void showPopup() override;
    virtual void hidePopup() override;

private:

    //! Flag indicating dark mode is in use
    bool isDarkMode_ = false;
};
