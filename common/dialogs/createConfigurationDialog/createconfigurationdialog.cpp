//-----------------------------------------------------------------------------
// File: createconfigurationdialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 30.08.2011
//
// Description:
// Dialog for user to input the info needed to create a new configuration for a component.
//-----------------------------------------------------------------------------

#include "createconfigurationdialog.h"

#include <KactusAPI/include/LibraryInterface.h>
#include <IPXACTmodels/common/VLNV.h>

#include <common/widgets/LineEditEx/LineEditEx.h>

#include <IPXACTmodels/Component/Component.h>

#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QCoreApplication>
#include <QSettings>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::CreateConfigurationDialog()
//-----------------------------------------------------------------------------
CreateConfigurationDialog::CreateConfigurationDialog(LibraryInterface* handler,
    QSharedPointer<Component> component,
    KactusAttribute::Implementation configType,
    QWidget *parent):
QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    library_(handler),
    component_(component),
    configNameEdit_(new LineEditEx(this)),
    configNameMatcher_(),
    useExistingButton_(new QRadioButton(tr("Use current design"), this)),
    createNewButton_(new QRadioButton(tr("Create new design"), this)),
    createCopyButton_(new QRadioButton(tr("Copy current design"), this)),
    vlnvEdit_(new VLNVEditor(VLNV::DESIGNCONFIGURATION, handler, this, this)),
    okButton_(new QPushButton(tr("OK"), this)),
    cancelButton_(new QPushButton(tr("Cancel"), this))
{
    Q_ASSERT(component);
    Q_ASSERT(handler);

    setStyleSheet(QString("*[mandatoryField=\"true\"] { background-color: LemonChiffon; }"));

    QSettings settings;
    QStringList suggestedNames;
    QStringList reservedNames;

    if (configType == KactusAttribute::HW)
    {
        suggestedNames = settings.value("Policies/HWViewNames").toStringList(); 
        reservedNames = component->getHierViews();
    }
    else if (configType == KactusAttribute::SW)
    {
        suggestedNames = settings.value("Policies/SWViewNames").toStringList();
        reservedNames = component->getHierViews();
    }
    else if (configType == KactusAttribute::SYSTEM)
    {
        suggestedNames = settings.value("Policies/SysViewNames").toStringList();
        reservedNames = component->getSystemViewNames();
    }
    else
    {
        Q_ASSERT(false);
    }

    configNameMatcher_.setItems(suggestedNames);
    if (!suggestedNames.isEmpty())
    {
        QString initialName = suggestedNames.first();
        configNameEdit_->setText(initialName);
        onConfNameChanged(initialName);
    }

    configNameEdit_->setContentMatcher(&configNameMatcher_);    
    configNameEdit_->setDisallowedInputs(reservedNames);
    configNameEdit_->setMessageIcon(QPixmap(":/icons/common/graphics/exclamation.png"));
    configNameEdit_->setMessageTemplate(tr("View '%1' already exists!"));
    configNameEdit_->setProperty("mandatoryField", true);

    VLNV topVLNV = component_->getVlnv();
	vlnvEdit_->setTitle(tr("Configuration VLNV"));	
	vlnvEdit_->setVendor(topVLNV.getVendor());
	vlnvEdit_->setLibrary(topVLNV.getLibrary());
	vlnvEdit_->setVersion(topVLNV.getVersion());
    vlnvEdit_->setMandatory(true);

    setupLayout();
    setupConnections();

	setWindowTitle(tr("Create New Configuration"));
    setFixedSize(470, heightForWidth(470));
    
    prevalidate();
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::~CreateConfigurationDialog()
//-----------------------------------------------------------------------------
CreateConfigurationDialog::~CreateConfigurationDialog()
{

}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::getConfigurationVLNV()
//-----------------------------------------------------------------------------
VLNV CreateConfigurationDialog::getConfigurationVLNV() const
{
	VLNV configVLNV = vlnvEdit_->getVLNV();
	configVLNV.setType(VLNV::DESIGNCONFIGURATION);

	return configVLNV;
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::getConfigurationName()
//-----------------------------------------------------------------------------
QString CreateConfigurationDialog::getConfigurationName() const
{
	return configNameEdit_->text();
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::getDesignVLNV()
//-----------------------------------------------------------------------------
VLNV CreateConfigurationDialog::getDesignVLNV() const
{
	// first get the inputted values
	VLNV designVLNV = getConfigurationVLNV();
	designVLNV.setType(VLNV::DESIGN);

	// remove the possible ".designcfg" from the end
	QString name = designVLNV.getName().remove(QStringLiteral(".designcfg"));
    name.append(QStringLiteral(".design"));

	designVLNV.setName(name);
	return designVLNV;
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::designSelection()
//-----------------------------------------------------------------------------
CreateConfigurationDialog::DesignSelection CreateConfigurationDialog::designSelection() const
{
    if (useExistingButton_->isChecked())
    {
        return CreateConfigurationDialog::USE_EXISTING;
    }
    else if (createNewButton_->isChecked())
    {
        return CreateConfigurationDialog::CREATE_EMPTY;
    }
    else
    {
        return CreateConfigurationDialog::CREATE_COPY;
    }
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::onOk()
//-----------------------------------------------------------------------------
void CreateConfigurationDialog::onOk()
{
    // Make sure that the VLNVs are not already in use.
    if (library_->contains(getConfigurationVLNV()))
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            tr("VLNV %1 already exists in the library.").arg(getConfigurationVLNV().toString()),
            QMessageBox::Ok, this);
        msgBox.exec();
        return;
    }

    if (!useExistingButton_->isChecked() && library_->contains(getDesignVLNV()))
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            tr("VLNV %1 already exists in the library.").arg(getDesignVLNV().toString()),
            QMessageBox::Ok, this);
        msgBox.exec();
        return;
    }

    accept();
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::onConfNameChanged()
//-----------------------------------------------------------------------------
void CreateConfigurationDialog::onConfNameChanged(QString const& newName)
{
	vlnvEdit_->setName(component_->getVlnv().getName() + "." + newName + ".designcfg");
}

//-----------------------------------------------------------------------------
// Function: CreateConfigurationDialog::prevalidate()
//-----------------------------------------------------------------------------
void CreateConfigurationDialog::prevalidate()
{
    okButton_->setEnabled(!configNameEdit_->text().isEmpty() && configNameEdit_->isInputValid() &&
        vlnvEdit_->isValid());
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::setupLayout()
//-----------------------------------------------------------------------------
void CreateConfigurationDialog::setupLayout()
{
    QLabel* introLabel = new QLabel(
        tr("<b>Create a new design configuration.</b><br>"
        "A new view will be added for the design configuration in the component. The given VLNV "
        "will be used for identifying the design configuration within the library."), this);
    introLabel->setWordWrap(true);

    QLabel* configurationIcon = new QLabel(this);
    configurationIcon->setPixmap(QPixmap(":icons/common/graphics/configuration.png"));
    
    QWidget* introWidget = new QWidget(this);
    QHBoxLayout* introLayout = new QHBoxLayout(introWidget);
    introLayout->addWidget(introLabel, 9);
    introLayout->addWidget(configurationIcon, 1);    

    QPalette introPalette = introWidget->palette();
    introPalette.setColor(QPalette::Background, Qt::white);
    introWidget->setPalette(introPalette);
    introWidget->setAutoFillBackground(true);

    QGroupBox* nameGroup = new QGroupBox(tr("Configuration settings"), this);
    QHBoxLayout* nameLayout = new QHBoxLayout(nameGroup);
    nameLayout->addWidget(new QLabel(tr("Name:"), this));
    nameLayout->addWidget(configNameEdit_);

    QGroupBox* radioGroup = new QGroupBox(tr("Select target design"), this);

    QFont buttonFont = useExistingButton_->font();
    buttonFont.setBold(true);
    useExistingButton_->setFont(buttonFont);
    createNewButton_->setFont(buttonFont);
    createCopyButton_->setFont(buttonFont);

    QString buttonStyle = "QRadioButton::indicator { width: 15px; height: 15px; }";
    useExistingButton_->setStyleSheet(buttonStyle);
    createNewButton_->setStyleSheet(buttonStyle);
    createCopyButton_->setStyleSheet(buttonStyle);

    // by default an existing design is used.
    useExistingButton_->toggle();

    QString labelStyle = "QLabel { padding-left: 19px; }";
    QLabel* useExistingLabel(new QLabel(tr("Creates a new configuration for the current design."), this));
    useExistingLabel->setStyleSheet(labelStyle);
    useExistingLabel->setWordWrap(true);

    QLabel* createNewLabel(new QLabel(tr("Creates a new empty design and a new configuration."), this));
    createNewLabel->setStyleSheet(labelStyle);
    createNewLabel->setWordWrap(true);

    QLabel* createCopyLabel(new QLabel(tr("Duplicates the current design with a new VLNV for "
        "the new configuration."), this));
    createCopyLabel->setStyleSheet(labelStyle);
    createCopyLabel->setWordWrap(true);

    QVBoxLayout* radioLayout = new QVBoxLayout(radioGroup);
    radioLayout->addWidget(useExistingButton_);
    radioLayout->addWidget(useExistingLabel);
    radioLayout->addWidget(createNewButton_);
    radioLayout->addWidget(createNewLabel);
    radioLayout->addWidget(createCopyButton_);
    radioLayout->addWidget(createCopyLabel);

    QGroupBox* separator = new QGroupBox(this);
    separator->setFlat(true);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton_, 0, Qt::AlignRight);
    buttonLayout->addWidget(cancelButton_, 0, Qt::AlignRight);

    QVBoxLayout* topLayout = new QVBoxLayout(this);   
    topLayout->addWidget(introWidget);
    topLayout->addWidget(nameGroup);
    topLayout->addWidget(vlnvEdit_);
    topLayout->addWidget(radioGroup);
    topLayout->addWidget(separator);
    topLayout->addLayout(buttonLayout);
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::setupConnections()
//-----------------------------------------------------------------------------
void CreateConfigurationDialog::setupConnections()
{
    connect(okButton_, SIGNAL(clicked(bool)), this, SLOT(onOk()), Qt::UniqueConnection);
    connect(cancelButton_, SIGNAL(clicked(bool)), this, SLOT(reject()), Qt::UniqueConnection);
    connect(configNameEdit_, SIGNAL(textEdited(const QString&)),
        this, SLOT(onConfNameChanged(const QString&)), Qt::UniqueConnection);

    connect(configNameEdit_, SIGNAL(textChanged(QString const&)), this, SLOT(prevalidate()), Qt::UniqueConnection);
    connect(vlnvEdit_, SIGNAL(contentChanged()), this, SLOT(prevalidate()), Qt::UniqueConnection);
}
