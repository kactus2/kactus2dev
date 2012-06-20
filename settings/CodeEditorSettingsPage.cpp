//-----------------------------------------------------------------------------
// File: CodeEditorSettingsPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 14.6.2011
//
// Description:
// Code editor settings property page.
//-----------------------------------------------------------------------------

#include "CodeEditorSettingsPage.h"

#include <MCAPI/CSourceWidget.h>
#include <common/widgets/assistedTextEdit/AssistedTextEdit.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDir>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QGroupBox>
#include <QCoreApplication>
#include <QMessageBox>
#include <QRegExp>
#include <QRegExpValidator>
#include <QGridLayout>
#include <QColorDialog>
#include <QMetaType>
#include <QDebug>

namespace
{
    //! Style names to show in the page.
    QString const STYLE_NAMES[] =
    {
        "Keywords",
        "Preprocessor",
        "Strings",
        "Single-line Comments",
        "Multi-line Comments",
        "API Data Types",
        "API Functions",
    };
}

//-----------------------------------------------------------------------------
// Function: CodeEditorSettingsPage()
//-----------------------------------------------------------------------------
CodeEditorSettingsPage::CodeEditorSettingsPage(QSettings& settings) : settings_(settings),
                                                                      indentWidthEdit_(0),
                                                                      fontCombo_(0),
                                                                      fontSizeCombo_(0),
                                                                      highlightTypeList_(0)
{
    // Create the indentation group and all of its widgets.
    QGroupBox* indentGroup = new QGroupBox(tr("Indentation"), this);

    QLabel* indentWidthLabel = new QLabel(tr("Indent width:"), indentGroup);
    indentWidthEdit_ = new QLineEdit(indentGroup);
    indentWidthEdit_->setFixedWidth(40);
    indentWidthEdit_->setValidator(new QRegExpValidator(QRegExp("^\\d+$"), this));

    QHBoxLayout* widthLayout = new QHBoxLayout();
    widthLayout->addWidget(indentWidthLabel);
    widthLayout->addWidget(indentWidthEdit_);
    widthLayout->addStretch(1);

    indentStyleRadioButtons_[AssistedTextEdit::INDENT_STYLE_TAB] =
        new QRadioButton(tr("Use tabs"), indentGroup);
    indentStyleRadioButtons_[AssistedTextEdit::INDENT_STYLE_SPACES] =
        new QRadioButton(tr("Use spaces"), indentGroup);

    QVBoxLayout* indentLayout = new QVBoxLayout(indentGroup);
    indentLayout->addLayout(widthLayout);
    indentLayout->addWidget(indentStyleRadioButtons_[AssistedTextEdit::INDENT_STYLE_TAB]);
    indentLayout->addWidget(indentStyleRadioButtons_[AssistedTextEdit::INDENT_STYLE_SPACES]);

    // Create the font & colors group and all of its widgets.
    QGroupBox* fontColorGroup = new QGroupBox(tr("Font and Colors"), this);

    QLabel* fontLabel = new QLabel(tr("Font:"), fontColorGroup);
    fontCombo_ = new QFontComboBox(fontColorGroup);
    fontCombo_->setWritingSystem(QFontDatabase::Latin);
    fontCombo_->setFixedWidth(320);
    connect(fontCombo_, SIGNAL(currentFontChanged(QFont const&)), this, SLOT(updateSample()));

    QLabel* fontSizeLabel = new QLabel(tr("Size:"), fontColorGroup);
    fontSizeCombo_ = new QComboBox(fontColorGroup);
    fontSizeCombo_->setFixedWidth(80);
    fontSizeCombo_->setMaxVisibleItems(MAX_FONT_SIZE - MIN_FONT_SIZE + 1);

    for (int size = MIN_FONT_SIZE; size <= MAX_FONT_SIZE; ++size)
    {
        fontSizeCombo_->addItem(QString::number(size));
    }

    connect(fontSizeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSample()));

    QGridLayout* fontLayout = new QGridLayout();
    fontLayout->addWidget(fontLabel, 0, 0, 1, 1, Qt::AlignLeft);
    fontLayout->addWidget(fontCombo_, 1, 0, 1, 1, Qt::AlignLeft);
    fontLayout->addWidget(fontSizeLabel, 0, 1, 1, 1, Qt::AlignLeft);
    fontLayout->addWidget(fontSizeCombo_, 1, 1, 1, 1, Qt::AlignLeft);
    fontLayout->setColumnStretch(1, 1);

    QLabel* highlightLabel = new QLabel(tr("Syntax highlight for:"), fontColorGroup);
    highlightTypeList_ = new QListWidget(fontColorGroup);
    highlightTypeList_->setFixedWidth(208);
    connect(highlightTypeList_, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(onSelectStyle(QListWidgetItem*, QListWidgetItem*)));

    for (int i = 0; i < CSourceHighlighter::STYLE_COUNT; ++i)
    {
        highlightTypeList_->addItem(STYLE_NAMES[i]);
    }

    QLabel* colorLabel = new QLabel(tr("Color:"), fontColorGroup);
    colorBox_ = new ColorBox(QSize(56, 56), fontColorGroup);
    connect(colorBox_, SIGNAL(clicked()), this, SLOT(selectColor()));

    boldCheckBox_ = new QCheckBox(tr("Bold"), fontColorGroup);
    connect(boldCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(updateSample()));

    italicCheckBox_ = new QCheckBox(tr("Italic"), fontColorGroup);
    connect(italicCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(updateSample()));

    QVBoxLayout* checkLayout = new QVBoxLayout();
    checkLayout->addWidget(boldCheckBox_);
    checkLayout->addWidget(italicCheckBox_);

    QLabel* sampleLabel = new QLabel(tr("Sample:"), fontColorGroup);
    sampleBox_ = new ColorBox(QSize(208, 44), fontColorGroup);
    sampleBox_->setColor(Qt::white);
    sampleBox_->setText("AaBbCcXxYyZz");

    QVBoxLayout* sampleLayout = new QVBoxLayout();
    sampleLayout->addWidget(sampleLabel);
    sampleLayout->addWidget(sampleBox_);
   
    QGridLayout* colorLayout = new QGridLayout();
    colorLayout->addWidget(highlightLabel, 0, 0, 1, 1, Qt::AlignLeft);
    colorLayout->addWidget(highlightTypeList_, 1, 0, 2, 1, Qt::AlignLeft);
    colorLayout->addWidget(colorLabel, 0, 1, 1, 1, Qt::AlignLeft);
    colorLayout->addWidget(colorBox_, 1, 1, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    colorLayout->addLayout(checkLayout, 1, 2, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    colorLayout->addLayout(sampleLayout, 2, 1, 1, 2, Qt::AlignLeft | Qt::AlignTop);
    colorLayout->setColumnStretch(2, 1);

    QVBoxLayout* fontColorLayout = new QVBoxLayout(fontColorGroup);
    fontColorLayout->addLayout(fontLayout);
    fontColorLayout->addSpacing(12);
    fontColorLayout->addLayout(colorLayout);

    // Setup the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(indentGroup);
    layout->addWidget(fontColorGroup);
    layout->addStretch(1);

    loadSettings();
    highlightTypeList_->setCurrentRow(0);
}

//-----------------------------------------------------------------------------
// Function: ~CodeEditorSettingsPage()
//-----------------------------------------------------------------------------
CodeEditorSettingsPage::~CodeEditorSettingsPage()
{
}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool CodeEditorSettingsPage::validate()
{
    Q_ASSERT(prevalidate());

    if (indentWidthEdit_->text().isEmpty() || indentWidthEdit_->text().toInt() < 1 ||
        indentWidthEdit_->text().toInt() > 60)
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
                           tr("Please enter an indent width between 1 and 60."), QMessageBox::Ok, this);
        msgBox.exec();
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void CodeEditorSettingsPage::apply()
{
    // Store the current style's settings to the data structure.
    int row = highlightTypeList_->currentRow();
    styles_[row].bold = boldCheckBox_->isChecked();
    styles_[row].italic = italicCheckBox_->isChecked();
    styles_[row].color = colorBox_->getColor();

    // Save the settings.
    settings_.setValue("editor/indentWidth", indentWidthEdit_->text().toInt());
    
    if (indentStyleRadioButtons_[AssistedTextEdit::INDENT_STYLE_SPACES]->isChecked())
    {
        settings_.setValue("editor/indentStyle", AssistedTextEdit::INDENT_STYLE_SPACES);
    }
    else
    {
        settings_.setValue("editor/indentStyle", AssistedTextEdit::INDENT_STYLE_TAB);
    }

    QFont font = fontCombo_->currentFont();
    font.setPointSize(fontSizeCombo_->currentText().toInt());
    settings_.setValue("editor/font", QVariant::fromValue(font));

    for (int i = 0; i < CSourceHighlighter::STYLE_COUNT; ++i)
    {
        settings_.setValue("editor/highlight/" + CSourceHighlighter::STYLE_IDS[i],
                           QVariant::fromValue(styles_[i]));
    }
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool CodeEditorSettingsPage::onPageChange()
{
    // Do not change the page if the settings are invalid.
    if (!validate())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: loadSettings()
//-----------------------------------------------------------------------------
void CodeEditorSettingsPage::loadSettings()
{
    // Load indentation settings.
    unsigned int width = settings_.value("editor/indentWidth", 4).toInt();
    indentWidthEdit_->setText(QString::number(width));

    AssistedTextEdit::IndentStyle style =
        static_cast<AssistedTextEdit::IndentStyle>(settings_.value("editor/indentStyle",
                                                   AssistedTextEdit::INDENT_STYLE_SPACES).toInt());

    indentStyleRadioButtons_[style]->setChecked(true);
    indentStyleRadioButtons_[1 - style]->setChecked(false);

    // Load font and highlight settings.
    QFont font = settings_.value("editor/font", QVariant::fromValue(QFont("Courier New", 10))).value<QFont>();
    fontCombo_->setCurrentFont(font);
    fontSizeCombo_->setCurrentIndex(font.pointSize() - MIN_FONT_SIZE);

    qRegisterMetaTypeStreamOperators<HighlightStyleDesc>("HighlightStyleDesc");

    for (int i = 0; i < CSourceHighlighter::STYLE_COUNT; ++i)
    {
        styles_[i] = settings_.value("editor/highlight/" + CSourceHighlighter::STYLE_IDS[i],
            QVariant::fromValue(CSourceHighlighter::DEFAULT_STYLES[i])).value<HighlightStyleDesc>();
    }
}

//-----------------------------------------------------------------------------
// Function: selectColor()
//-----------------------------------------------------------------------------
void CodeEditorSettingsPage::selectColor()
{
    QColorDialog dialog(this);
    dialog.setCurrentColor(colorBox_->getColor());
    
    if (dialog.exec() == QDialog::Accepted)
    {
        colorBox_->setColor(dialog.currentColor());
        updateSample();
    }
}

//-----------------------------------------------------------------------------
// Function: updateSample()
//-----------------------------------------------------------------------------
void CodeEditorSettingsPage::updateSample()
{
    sampleBox_->setTextColor(colorBox_->getColor());

    QFont font = fontCombo_->currentFont();
    font.setPointSize(fontSizeCombo_->currentText().toInt());
    font.setBold(boldCheckBox_->isChecked());
    font.setItalic(italicCheckBox_->isChecked());

    sampleBox_->setTextFont(font);
}

//-----------------------------------------------------------------------------
// Function: onSelectStyle()
//-----------------------------------------------------------------------------
void CodeEditorSettingsPage::onSelectStyle(QListWidgetItem* cur, QListWidgetItem* prev)
{
    int prevRow = highlightTypeList_->row(prev);
    int curRow = highlightTypeList_->row(cur);

    if (prevRow >= 0)
    {
        // Save the previous style.
        styles_[prevRow].bold = boldCheckBox_->isChecked();
        styles_[prevRow].italic = italicCheckBox_->isChecked();
        styles_[prevRow].color = colorBox_->getColor();
    }

    // Import the settings from the new style.
    colorBox_->setColor(styles_[curRow].color);
    boldCheckBox_->setChecked(styles_[curRow].bold);
    italicCheckBox_->setChecked(styles_[curRow].italic);

    updateSample();
}
