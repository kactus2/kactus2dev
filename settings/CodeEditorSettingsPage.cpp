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

#include <editors/CSourceEditor/CSourceWidget.h>
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
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QGridLayout>
#include <QColorDialog>
#include <QMetaType>
#include <QFormLayout>

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
        "Language Specific"
    };
}

//-----------------------------------------------------------------------------
// Function: CodeEditorSettingsPage()
//-----------------------------------------------------------------------------
CodeEditorSettingsPage::CodeEditorSettingsPage(QSettings& settings) : 
SettingsPage(settings),
    indentWidthEdit_(0),
    fontCombo_(new QFontComboBox()),
    fontSizeCombo_(new QComboBox()),
    highlightTypeList_(new QListWidget()),
    colorBox_(new ColorBox(QSize(32, 32))),
    boldCheckBox_(new QCheckBox()),
    italicCheckBox_(new QCheckBox()),
    previewBox_(new ColorBox(QSize(200, 64))),
    styles_()
{
    fontCombo_->setWritingSystem(QFontDatabase::Latin);

    fontSizeCombo_->setMaxVisibleItems(MAX_FONT_SIZE - MIN_FONT_SIZE + 1);

    for (int size = MIN_FONT_SIZE; size <= MAX_FONT_SIZE; ++size)
    {
        fontSizeCombo_->addItem(QString::number(size));
    }

    for (int i = 0; i < CSourceHighlighter::STYLE_COUNT; ++i)
    {
        highlightTypeList_->addItem(STYLE_NAMES[i]);
    }

    previewBox_->setFixedSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));    
    previewBox_->setColor(Qt::white);
    previewBox_->setText("AaBbCcXxYyZz");

    setupLayout();

    connect(fontCombo_, SIGNAL(currentFontChanged(QFont const&)), this, SLOT(updateSample()));
    connect(fontSizeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSample()));
    connect(boldCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(updateSample()));
    connect(italicCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(updateSample()));

    connect(highlightTypeList_, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
        this, SLOT(onSelectStyle(QListWidgetItem*, QListWidgetItem*)));

    connect(colorBox_, SIGNAL(clicked()), this, SLOT(selectColor()));

    qRegisterMetaTypeStreamOperators<HighlightStyleDesc>("HighlightStyleDesc");
    loadSettings();

    highlightTypeList_->setCurrentRow(0);
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
    settings().setValue("Editor/IndentWidth", indentWidthEdit_->text().toInt());
    
    if (indentStyleRadioButtons_[INDENT_STYLE_SPACES]->isChecked())
    {
        settings().setValue("Editor/IndentStyle", INDENT_STYLE_SPACES);
    }
    else
    {
        settings().setValue("Editor/IndentStyle", INDENT_STYLE_TAB);
    }

    QFont font = fontCombo_->currentFont();
    font.setPointSize(fontSizeCombo_->currentText().toInt());
    settings().setValue("Editor/Font", QVariant::fromValue(font));

    for (int i = 0; i < CSourceHighlighter::STYLE_COUNT; ++i)
    {
        settings().setValue("Editor/Highlight/" + 
            CSourceHighlighter::getStyleName(static_cast<CSourceHighlighter::StyleType>(i)),
            QVariant::fromValue(styles_[i]));
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
    previewBox_->setTextColor(colorBox_->getColor());

    QFont font = fontCombo_->currentFont();
    font.setPointSize(fontSizeCombo_->currentText().toInt());
    font.setBold(boldCheckBox_->isChecked());
    font.setItalic(italicCheckBox_->isChecked());

    previewBox_->setTextFont(font);
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

//-----------------------------------------------------------------------------
// Function: CodeEditorSettingsPage::setupLayout()
//-----------------------------------------------------------------------------
void CodeEditorSettingsPage::setupLayout()
{
    // Create the indentation group and all of its widgets.
    QGroupBox* indentGroup = new QGroupBox(tr("Indentation"), this);

    QLabel* indentWidthLabel = new QLabel(tr("Indent width:"), indentGroup);
    indentWidthEdit_ = new QLineEdit(indentGroup);
    indentWidthEdit_->setFixedWidth(40);
    indentWidthEdit_->setValidator(new QRegularExpressionValidator(QRegularExpression("^\\d+$"), this));

    QHBoxLayout* widthLayout = new QHBoxLayout();
    widthLayout->addWidget(indentWidthLabel);
    widthLayout->addWidget(indentWidthEdit_);
    widthLayout->addStretch(1);

    indentStyleRadioButtons_[INDENT_STYLE_TAB] = new QRadioButton(tr("Use tabs"), indentGroup);
    indentStyleRadioButtons_[INDENT_STYLE_SPACES] = new QRadioButton(tr("Use spaces"), indentGroup);

    QVBoxLayout* indentLayout = new QVBoxLayout(indentGroup);    
    indentLayout->addWidget(indentStyleRadioButtons_[INDENT_STYLE_TAB]);
    indentLayout->addWidget(indentStyleRadioButtons_[INDENT_STYLE_SPACES]);
    indentLayout->addLayout(widthLayout);

    QGroupBox* fontGroup = new QGroupBox(tr("Font"), this);

    QFormLayout* fontAndSizeLayout = new QFormLayout(fontGroup);
    fontAndSizeLayout->addRow(tr("Font:"), fontCombo_);
    fontAndSizeLayout->addRow(tr("Size:"), fontSizeCombo_);

    highlightTypeList_->setFixedWidth(208);

    QGroupBox* highlightGroup = new QGroupBox(tr("Syntax highlighting"), this);

    QFormLayout* syntaxOptionsLayout = new QFormLayout();    
    syntaxOptionsLayout->addRow(tr("Bold:"), boldCheckBox_);
    syntaxOptionsLayout->addRow(tr("Italic:"), italicCheckBox_);
    syntaxOptionsLayout->addRow(tr("Color:"), colorBox_);
    syntaxOptionsLayout->addRow(tr("Preview:"), previewBox_);

    QGridLayout* highlightLayout = new QGridLayout(highlightGroup);
    highlightLayout->addWidget(highlightTypeList_, 0, 0, 1, 1);
    highlightLayout->addLayout(syntaxOptionsLayout, 0, 1, 1, 1);

    // Setup the layout.
    QGridLayout* topLayout = new QGridLayout(this);
    topLayout->addWidget(fontGroup, 0, 0, 1, 1);
    topLayout->addWidget(indentGroup, 0, 1, 1, 1);
    topLayout->addWidget(highlightGroup, 1, 0, 1, 2);
    topLayout->setColumnStretch(0, 1);
    topLayout->setColumnStretch(1, 1);
}

//-----------------------------------------------------------------------------
// Function: loadSettings()
//-----------------------------------------------------------------------------
void CodeEditorSettingsPage::loadSettings()
{
    // Load indentation settings.
    unsigned int width = settings().value("Editor/IndentWidth", 4).toInt();
    indentWidthEdit_->setText(QString::number(width));

    IndentStyle style = static_cast<IndentStyle>(settings().value("Editor/IndentStyle",
                                                                 INDENT_STYLE_SPACES).toInt());

    indentStyleRadioButtons_[style]->setChecked(true);
    indentStyleRadioButtons_[1 - style]->setChecked(false);

    // Load font and highlight settings.
    QFont font = settings().value("Editor/Font", QVariant::fromValue(QFont("Courier New", 10))).value<QFont>();
    fontCombo_->setCurrentFont(font);
    fontSizeCombo_->setCurrentIndex(font.pointSize() - MIN_FONT_SIZE);

    for (int i = 0; i < CSourceHighlighter::STYLE_COUNT; ++i)
    {
        styles_[i] = settings().value("Editor/Highlight/" + 
            CSourceHighlighter::getStyleName(static_cast<CSourceHighlighter::StyleType>(i)),
            QVariant::fromValue(CSourceHighlighter::DEFAULT_STYLES[i])).value<HighlightStyleDesc>();
    }
}
