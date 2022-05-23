//-----------------------------------------------------------------------------
// File: NewDesignDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 2.8.2012
//
// Description:
// Dialog used in creation of new designs to existing components.
//-----------------------------------------------------------------------------

#include "NewDesignDialog.h"

#include <QCoreApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>

#include <IPXACTmodels/Component/Component.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <common/widgets/LibrarySelectorWidget/LibrarySelectorWidget.h>
#include <common/widgets/LineEditEx/LineEditEx.h>
#include <common/NameGenerationPolicy.h>

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::NewDesignDialog()
//-----------------------------------------------------------------------------
NewDesignDialog::NewDesignDialog(LibraryInterface* libInterface, QSharedPointer<Component> component,
    KactusAttribute::Implementation designType, QWidget *parent):
QDialog(parent), 
      library_(libInterface),
      component_(component),
      introLabel_(new QLabel(this)),
      qualifierLabel_(new QLabel(tr("Qualifier:"), this)),
      qualifierEdit_(new LineEditEx(this)),
      viewNameLabel_(new QLabel(tr("View name:"), this)),
      viewNameMatcher_(),
      viewNameEdit_(new LineEditEx(this)),
      designIcon_(new QLabel(this)),
      vlnvEditor_(new VLNVEditor(VLNV::DESIGN, libInterface, this, this)),
      directoryEditor_(new LibrarySelectorWidget(this)), 
      okButton_(new QPushButton(tr("&OK"))),
      designExt_(),
      designConfExt_()
{
    setWindowFlags(windowFlags() &= ~Qt::WindowContextHelpButtonHint);

    setStyleSheet(QString("*[mandatoryField=\"true\"] { background-color: LemonChiffon; }"));

    introLabel_->setWordWrap(true);
    introLabel_->setText(tr("<b>Create a new design for the component.</b><br>"
        "A new view will be added for the design in the component. An optional qualifier can be "
        "used for differentiating the designs and views e.g. structural_rtl vs. structural_tlm. "
        "The given VLNV will be used for identifying the new design within the library."));

    if (designType == KactusAttribute::HW)
    {
        setWindowTitle("New HW Design");
        viewNameLabel_->setText(tr("View name:"));
        viewNameEdit_->setDisallowedInputs(component->getViewNames());
        viewNameEdit_->setMessageTemplate("View name '%1' already exists!");
        vlnvEditor_->setTitle("VLNV for new HW design and design configuration");
        designExt_ = ".design";
        designConfExt_ = ".designcfg";
        designIcon_->setPixmap(QPixmap(":/icons/common/graphics/hw-design.png"));
    }
    else if (designType == KactusAttribute::SW)
    {
        setWindowTitle(tr("New SW Design"));
        viewNameLabel_->setText(tr("View name:"));
        viewNameEdit_->setDisallowedInputs(component->getViewNames());
        viewNameEdit_->setMessageTemplate("View name '%1' already exists!");
        vlnvEditor_->setTitle("VLNV for new SW design and design configuration");
        designExt_ = ".swdesign";
        designConfExt_ = ".swdesigncfg";
        designIcon_->setPixmap(QPixmap(":/icons/common/graphics/sw-design48x48.png"));
    }
    else if (designType == KactusAttribute::SYSTEM)
    {
        setWindowTitle(tr("New System Design"));
        viewNameLabel_->setText(tr("System view name:"));
        viewNameEdit_->setDisallowedInputs(component->getSystemViewNames());
        viewNameEdit_->setMessageTemplate("System view name '%1' already exists!");
        vlnvEditor_->setTitle("VLNV for new system design and design configuration");
        designExt_ = ".sysdesign";
        designConfExt_ = ".sysdesigncfg";
        designIcon_->setPixmap(QPixmap(":/icons/common/graphics/sw-design48x48.png"));
    }
    else
    {
        Q_ASSERT(false);
    }

    qualifierEdit_->setPlaceholderText(tr("optional category for the view and configuration"));

    viewNameEdit_->setMessageIcon(QPixmap(":/icons/common/graphics/exclamation.png"));
    viewNameEdit_->setContentMatcher(&viewNameMatcher_);
    viewNameEdit_->setProperty("mandatoryField", true);

    vlnvEditor_->addNameExtension(designExt_);
    vlnvEditor_->addNameExtension(designConfExt_);
    vlnvEditor_->addContentType(VLNV::DESIGNCONFIGURATION);
    vlnvEditor_->setMandatory(true);

    okButton_->setDefault(true);
    okButton_->setEnabled(false);

    connect(qualifierEdit_, SIGNAL(textChanged(QString const&)), this, SLOT(updateViewName()));
    connect(qualifierEdit_, SIGNAL(textChanged(QString const&)), this, SLOT(updateVlnvName()));
    connect(qualifierEdit_, SIGNAL(textChanged(QString const&)), this, SLOT(onContentChanged()));

    connect(viewNameEdit_, SIGNAL(textChanged(QString const&)), this, SLOT(onContentChanged()));

    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));
    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(updateDirectory()));

    connect(directoryEditor_, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::setViewName()
//-----------------------------------------------------------------------------
void NewDesignDialog::setViewName(QString const& viewName)
{
    viewNameEdit_->setText(viewName);
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::getQualifierName()
//-----------------------------------------------------------------------------
QString NewDesignDialog::getQualifierName() const
{
    return qualifierEdit_->text();
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::getViewName()
//-----------------------------------------------------------------------------
QString NewDesignDialog::getViewName() const
{
    return viewNameEdit_->text();
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::getDesignVLNV()
//-----------------------------------------------------------------------------
VLNV NewDesignDialog::getDesignVLNV() const
{
    VLNV vlnv = vlnvEditor_->getVLNV();
    vlnv.setType(VLNV::DESIGN);
    vlnv.setName(vlnv.getName().remove(".comp") + designExt_);
    return vlnv;
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::getDesignConfVLNV()
//-----------------------------------------------------------------------------
VLNV NewDesignDialog::getDesignConfVLNV() const
{
    VLNV vlnv = vlnvEditor_->getVLNV();
    vlnv.setType(VLNV::DESIGNCONFIGURATION);

    QString name = vlnv.getName().remove(".comp");
    if (!qualifierEdit_->text().isEmpty())
    {
        name.append("."  + qualifierEdit_->text());
    }
    name.append(designConfExt_);

    vlnv.setName(name);
    return vlnv;
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::getPath()
//-----------------------------------------------------------------------------
QString NewDesignDialog::getPath() const
{
    return directoryEditor_->getPath();
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::onContentChanged()
//-----------------------------------------------------------------------------
void NewDesignDialog::onContentChanged()
{
    okButton_->setEnabled(!viewNameEdit_->text().isEmpty() &&
        viewNameEdit_->isInputValid() && directoryEditor_->isValid() && vlnvEditor_->isValid());
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::accept()
//-----------------------------------------------------------------------------
void NewDesignDialog::accept()
{
    if (library_->contains(getDesignVLNV()))
    {
        QMessageBox::warning(this, QCoreApplication::applicationName(),
            tr("VLNV %1 already exists in the library.").arg(getDesignVLNV().toString()));

        return;
    }

    if (library_->contains(getDesignConfVLNV()))
    {
        QMessageBox::warning(this, QCoreApplication::applicationName(),
            tr("VLNV %1 already exists in the library.").arg(getDesignConfVLNV().toString()));

        return;
    }

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::setVLNV()
//-----------------------------------------------------------------------------
void NewDesignDialog::setVLNV( const VLNV& vlnv )
{
	vlnvEditor_->setVLNV(vlnv);
    updateDirectory();
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::updateDirectory()
//-----------------------------------------------------------------------------
void NewDesignDialog::updateDirectory()
{
    QString vlnvDir;

    VLNV vlnv = vlnvEditor_->getVLNV();

    if (!vlnv.getVendor().isEmpty())
    {
        vlnvDir += "/" + vlnv.getVendor();

        if (!vlnv.getLibrary().isEmpty())
        {
            vlnvDir += "/" + vlnv.getLibrary();

            if (!vlnv.getName().isEmpty())
            {
                vlnvDir += "/" + vlnv.getName();

                if (!vlnv.getVersion().isEmpty())
                {
                    vlnvDir += "/" + vlnv.getVersion();
                }
            }
        }
    }

    directoryEditor_->updatePath(vlnvDir);
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::updateViewName()
//-----------------------------------------------------------------------------
void NewDesignDialog::updateViewName()
{
    viewNameEdit_->setText(NameGenerationPolicy::hierarchicalViewName(qualifierEdit_->text()));
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::updateVlnvName()
//-----------------------------------------------------------------------------
void NewDesignDialog::updateVlnvName()
{
    QString name = component_->getVlnv().getName().remove(".comp");
    if (!qualifierEdit_->text().isEmpty())
    {
        name.append("." + qualifierEdit_->text());
    }

    vlnvEditor_->setName(name);
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::setViewNameSuggestions()
//-----------------------------------------------------------------------------
void NewDesignDialog::setViewNameSuggestions(QStringList const& suggestions)
{
    viewNameMatcher_.setItems(suggestions);
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::setupLayout()
//-----------------------------------------------------------------------------
void NewDesignDialog::setupLayout()
{
    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(okButton_, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(new QPushButton(tr("&Cancel")), QDialogButtonBox::RejectRole);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QWidget* introWidget = new QWidget(this);
    QHBoxLayout* introLayout = new QHBoxLayout(introWidget);
    introLayout->addWidget(introLabel_, 9);
    introLayout->addWidget(designIcon_, 1);    
    
    QPalette introPalette = introWidget->palette();
    introPalette.setColor(QPalette::Background, Qt::white);
    introWidget->setPalette(introPalette);
    introWidget->setAutoFillBackground(true);
    
    QGroupBox* viewSettingsBox = new QGroupBox(tr("View settings"), this);    
    QFormLayout* viewLayout = new QFormLayout(viewSettingsBox);
    viewLayout->addRow(qualifierLabel_, qualifierEdit_);
    viewLayout->addRow(viewNameLabel_, viewNameEdit_);

    directoryEditor_->layout()->setContentsMargins(0, 4, 0, 4);

    QGroupBox* separator = new QGroupBox(this);
    separator->setFlat(true);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(introWidget);
    mainLayout->addWidget(viewSettingsBox);
    mainLayout->addWidget(vlnvEditor_);
    mainLayout->addWidget(directoryEditor_);
    mainLayout->addWidget(separator);
    mainLayout->addWidget(buttonBox);    
    
    setFixedHeight(sizeHint().height());
    resize(400, sizeHint().height());
}
