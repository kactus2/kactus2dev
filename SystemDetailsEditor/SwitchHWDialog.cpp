//-----------------------------------------------------------------------------
// File: SwitchHWDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 31.7.2012
//
// Description:
// Dialog for configuring settings how to switch HW for a system design.
//-----------------------------------------------------------------------------

#include "SwitchHWDialog.h"

#include <models/component.h>

#include <LibraryManager/libraryinterface.h>

#include <common/widgets/LineEditEx/LineEditEx.h>

#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: SwitchHWDialog::SwitchHWDialog()
//-----------------------------------------------------------------------------
SwitchHWDialog::SwitchHWDialog(QSharedPointer<Component> component, QString const& viewName,
                               LibraryInterface* lh, QWidget* parent)
    : QDialog(parent),
      lh_(lh),
      component_(component),
      infoLabel_(new QLabel(tr("Choose how the SW architecture specified in this "
                               "system design is mapped to new HW."), this)),
      hwViewRefLabel_(new QLabel(tr("HW design configuration to map:"), this)),
      hwViewRefCombo_(new QComboBox(this)),
      viewNameLabel_(new QLabel(tr("Name of the system view to be created for the mapped HW component:"), this)),
      viewNameEdit_(new LineEditEx(this)),
      viewNameMatcher_(),
      actionGroupBox_(new QGroupBox(tr("Action"), this)),
      actionGroup_(new QButtonGroup(this)),
      moveRadioButton_(new QRadioButton(tr("Move system design"), this)),
      moveDescLabel_(new QLabel(tr("Removes the system view from the "
                                   "previously mapped HW and moves it to the new one."), this)),
      copyRadioButton_(new QRadioButton(tr("Copy as a new system design"), this)),
      copyDescLabel_(new QLabel(tr("Creates an identical copy of the system design with a new VLNV "
                                   "and adds a new system view to the HW component."), this)),
      vlnvEdit_(new VLNVEditor(VLNV::DESIGN, lh, this, this)),
      directoryLabel_(new QLabel(tr("Directory:"), this)),
      directoryEdit_(new LibraryPathSelector(this)),
      buttonBox_(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this)),
      layout_(new QVBoxLayout(this))
{
    setWindowTitle(tr("Switch HW"));

    // Set widget settings.
    hwViewRefLabel_->setVisible(false);
    hwViewRefCombo_->setVisible(false);
    hwViewRefCombo_->setEnabled(false);

    viewNameEdit_->setDisallowedInputs(component->getSystemViewNames());
    viewNameEdit_->setMessageIcon(QPixmap(":/icons/graphics/exclamation.png"));
    viewNameEdit_->setMessageTemplate("System view name '%1' is already in use!");
    viewNameEdit_->setContentMatcher(&viewNameMatcher_);
    viewNameEdit_->setText(viewName);

    moveRadioButton_->setChecked(true);
    
    QFont font = moveRadioButton_->font();
    font.setBold(true);
    moveRadioButton_->setFont(font);
    moveRadioButton_->setStyleSheet("QRadioButton::indicator { width: 15px; height: 15px; }");
    copyRadioButton_->setFont(font);
    copyRadioButton_->setStyleSheet("QRadioButton::indicator { width: 15px; height: 15px; }");

    moveDescLabel_->setStyleSheet("QLabel { padding-left: 19px; }");
    moveDescLabel_->setWordWrap(true);
    copyDescLabel_->setStyleSheet("QLabel { padding-left: 19px; }");
    copyDescLabel_->setWordWrap(true);

    vlnvEdit_->setTitle(tr("VLNV for the new system design and design configuration"));
    vlnvEdit_->setVisible(false);
    vlnvEdit_->setNameExtension(".sysdesign/.sysdesigncfg");

    directoryLabel_->setVisible(false);
    directoryEdit_->setVisible(false);

    actionGroup_->addButton(moveRadioButton_);
    actionGroup_->addButton(copyRadioButton_);

    // Create layouts.
    QVBoxLayout* groupLayout = new QVBoxLayout(actionGroupBox_);
    groupLayout->addWidget(moveRadioButton_);
    groupLayout->addWidget(moveDescLabel_);
    groupLayout->addWidget(copyRadioButton_);
    groupLayout->addWidget(copyDescLabel_);

    QHBoxLayout* dirLayout = new QHBoxLayout();
    dirLayout->addWidget(directoryLabel_);
    dirLayout->addWidget(directoryEdit_, 1);

    layout_->addWidget(infoLabel_);
    layout_->addSpacing(12);
    layout_->addWidget(hwViewRefLabel_);
    layout_->addWidget(hwViewRefCombo_);
    layout_->addWidget(viewNameLabel_);
    layout_->addWidget(viewNameEdit_);
    layout_->addWidget(actionGroupBox_);
    layout_->addWidget(vlnvEdit_);
    layout_->addLayout(dirLayout);
    layout_->addStretch(1);
    layout_->addWidget(buttonBox_);

    // Setup connections.
    connect(actionGroup_, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(actionChanged(QAbstractButton*)), Qt::UniqueConnection);
    connect(buttonBox_->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(accept()), Qt::UniqueConnection);
    connect(buttonBox_->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(reject()), Qt::UniqueConnection);

    connect(vlnvEdit_, SIGNAL(contentChanged()), this, SLOT(updateDirectory()), Qt::UniqueConnection);
    connect(vlnvEdit_, SIGNAL(contentChanged()), this, SLOT(validate()), Qt::UniqueConnection);
    connect(viewNameEdit_, SIGNAL(textChanged(QString const&)), this, SLOT(validate()), Qt::UniqueConnection);

    setFixedHeight(sizeHint().height());
    validate();
}

//-----------------------------------------------------------------------------
// Function: SwitchHWDialog::~SwitchHWDialog()
//-----------------------------------------------------------------------------
SwitchHWDialog::~SwitchHWDialog()
{

}

//-----------------------------------------------------------------------------
// Function: SwitchHWDialog::showHWViewSelector()
//-----------------------------------------------------------------------------
void SwitchHWDialog::showHWViewSelector()
{
    hwViewRefCombo_->addItems(component_->getHierViews());
    hwViewRefLabel_->setVisible(true);
    hwViewRefCombo_->setVisible(true);
    hwViewRefCombo_->setEnabled(true);

    layout_->activate();
    setFixedHeight(sizeHint().height());
    validate();
}

//-----------------------------------------------------------------------------
// Function: SwitchHWDialog::accept()
//-----------------------------------------------------------------------------
void SwitchHWDialog::accept()
{
    // If copy action has been chosen, check if the sysdesign or sysdesigncfg VLNV is already in use.
    if (copyRadioButton_->isChecked())
    {
        VLNV vlnv = vlnvEdit_->getVLNV();

        VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
                        vlnv.getName() + ".sysdesign", vlnv.getVersion());
        VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
                         vlnv.getName() + ".sysdesigncfg", vlnv.getVersion());

        if (lh_->contains(designVLNV))
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                               tr("VLNV %1 already exists in the library.").arg(designVLNV.toString()),
                               QMessageBox::Ok, (QWidget*)parent());
            msgBox.exec();
            return;
        }

        if (lh_->contains(desConfVLNV))
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                               tr("VLNV %1 already exists in the library.").arg(desConfVLNV.toString()),
                               QMessageBox::Ok, (QWidget*)parent());
            msgBox.exec();
            return;
        }
    }

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: SwitchHWDialog::actionChanged()
//-----------------------------------------------------------------------------
void SwitchHWDialog::actionChanged(QAbstractButton* button)
{
    vlnvEdit_->setVisible(button == copyRadioButton_);
    directoryLabel_->setVisible(button == copyRadioButton_);
    directoryEdit_->setVisible(button == copyRadioButton_);
    layout_->activate();
    setFixedHeight(sizeHint().height());

    validate();
}

//-----------------------------------------------------------------------------
// Function: SwitchHWDialog::getHWViewRef()
//-----------------------------------------------------------------------------
QString SwitchHWDialog::getHWViewRef() const
{
    return hwViewRefCombo_->currentText();
}

//-----------------------------------------------------------------------------
// Function: SwitchHWDialog::getSystemViewName()
//-----------------------------------------------------------------------------
QString SwitchHWDialog::getSystemViewName() const
{
    return viewNameEdit_->text();
}

//-----------------------------------------------------------------------------
// Function: SwitchHWDialog::getVLNV()
//-----------------------------------------------------------------------------
VLNV SwitchHWDialog::getVLNV() const
{
    return vlnvEdit_->getVLNV();
}

//-----------------------------------------------------------------------------
// Function: SwitchHWDialog::isCopyActionSelected()
//-----------------------------------------------------------------------------
bool SwitchHWDialog::isCopyActionSelected() const
{
    return copyRadioButton_->isChecked();
}

//-----------------------------------------------------------------------------
// Function: SwitchHWDialog::getPath()
//-----------------------------------------------------------------------------
QString SwitchHWDialog::getPath() const
{
    return directoryEdit_->currentText();
}

//-----------------------------------------------------------------------------
// Function: updateDirectory()
//-----------------------------------------------------------------------------
void SwitchHWDialog::updateDirectory()
{
    QString dir = directoryEdit_->currentLocation();

    VLNV vlnv = vlnvEdit_->getVLNV();

    if (!vlnv.getVendor().isEmpty())
    {
        dir += "/" + vlnv.getVendor();

        if (!vlnv.getLibrary().isEmpty())
        {
            dir += "/" + vlnv.getLibrary();

            if (!vlnv.getName().isEmpty())
            {
                dir += "/" + vlnv.getName();

                if (!vlnv.getVersion().isEmpty())
                {
                    dir += "/" + vlnv.getVersion();
                }
            }
        }
    }

    directoryEdit_->setEditText(dir);
}

//-----------------------------------------------------------------------------
// Function: SwitchHWDialog::validate()
//-----------------------------------------------------------------------------
void SwitchHWDialog::validate()
{
    QAbstractButton* btnOK = buttonBox_->button(QDialogButtonBox::Ok);
    btnOK->setEnabled(!viewNameEdit_->text().isEmpty() && viewNameEdit_->isInputValid() &&
                      (moveRadioButton_->isChecked() || vlnvEdit_->isValid()) &&
                      (!hwViewRefCombo_->isEnabled() || hwViewRefCombo_->count() > 0) &&
                      !directoryEdit_->currentText().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: SwitchHWDialog::setViewNameSuggestions()
//-----------------------------------------------------------------------------
void SwitchHWDialog::setViewNameSuggestions(QStringList const& suggestions)
{
    viewNameMatcher_.setItems(suggestions);
}
