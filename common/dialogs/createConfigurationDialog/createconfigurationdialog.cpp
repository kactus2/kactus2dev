/* 
 *  	Created on: 30.8.2011
 *      Author: Antti Kamppi
 * 		filename: createconfigurationdialog.cpp
 *		Project: Kactus 2
 */

#include "createconfigurationdialog.h"

#include <LibraryManager/libraryinterface.h>
#include <LibraryManager/vlnv.h>

#include <common/widgets/LineEditEx/LineEditEx.h>

#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QCoreApplication>

#include <QDebug>

CreateConfigurationDialog::CreateConfigurationDialog(LibraryInterface* handler,
													 QSharedPointer<Component> component,
                                                     KactusAttribute::Implementation configType,
													 QWidget *parent,
													 Qt::WindowFlags f)
    : QDialog(parent, f),
      handler_(handler),
      component_(component),
      configType_(configType),
      useExistingRadio_(new QRadioButton(tr("Use current design"), this)),
      createNewRadio_(new QRadioButton(tr("Create new design"), this)),
      createCopyRadio_(new QRadioButton(tr("Copy old design to new configuration"), this)),
      configNameEdit_(new LineEditEx(this)),
      vlnvEdit_(new VLNVEditor(VLNV::DESIGNCONFIGURATION, handler, this, this, true)),
      implementationCombo_(new QComboBox(this)),
      okButton_(new QPushButton(tr("OK"), this)),
      cancelButton_(new QPushButton(tr("Cancel"), this))
{
	Q_ASSERT(component);
	Q_ASSERT(handler);

	setupLayout();
	setupConnections();

    configNameEdit_->setMessageIcon(QPixmap(":/icons/graphics/exclamation.png"));

    switch (configType)
    {
    case KactusAttribute::KTS_HW:
        {
            configNameEdit_->setDisallowedInputs(component->getHierViews());
            configNameEdit_->setMessageTemplate("View '%1' already exists!");
            break;
        }

    case KactusAttribute::KTS_SW:
        {
            configNameEdit_->setDisallowedInputs(component->getSWViewNames());
            configNameEdit_->setMessageTemplate("SW view '%1' already exists!");
            break;
        }

    case KactusAttribute::KTS_SYS:
        {
            configNameEdit_->setDisallowedInputs(component->getSystemViewNames());
            configNameEdit_->setMessageTemplate("System view '%1' already exists!");
            break;
        }

    default:
        {
            Q_ASSERT(false);
        }
    }

	// by default an existing design is used.
	useExistingRadio_->toggle();

	// set the views to the combo box
	implementationCombo_->addItem(QString(""));
	implementationCombo_->addItems(component->getViewNames());

	// set the title for vlnv editor
	vlnvEdit_->setTitle(tr("Configuration VLNV"));
	VLNV compVLNV = *component_->getVlnv();
	vlnvEdit_->setVendor(compVLNV.getVendor());
	vlnvEdit_->setLibrary(compVLNV.getLibrary());
	vlnvEdit_->setVersion(compVLNV.getVersion());

	setWindowTitle(tr("Create New Configuration"));
}

CreateConfigurationDialog::~CreateConfigurationDialog() {
}

void CreateConfigurationDialog::setupLayout() {

	QHBoxLayout* radioLayout = new QHBoxLayout();
	radioLayout->addWidget(useExistingRadio_);
	radioLayout->addWidget(createNewRadio_);
	radioLayout->addWidget(createCopyRadio_);
	radioLayout->addStretch();

	QFormLayout* nameLayout = new QFormLayout();
	nameLayout->addRow(tr("Configuration name:"), configNameEdit_);

	QFormLayout* comboLayout = new QFormLayout();
	comboLayout->addRow(tr("OPTIONAL reference to this component's\n"
		"top-level implementation view"), implementationCombo_);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget(okButton_, 0, Qt::AlignRight);
	buttonLayout->addWidget(cancelButton_, 0, Qt::AlignRight);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addLayout(radioLayout);
	topLayout->addLayout(nameLayout);
	topLayout->addWidget(vlnvEdit_);
	topLayout->addLayout(comboLayout);
	topLayout->addLayout(buttonLayout);
}

void CreateConfigurationDialog::setupConnections() {

	connect(okButton_, SIGNAL(clicked(bool)),
		this, SLOT(onOk()), Qt::UniqueConnection);
	connect(cancelButton_, SIGNAL(clicked(bool)),
		this, SLOT(reject()), Qt::UniqueConnection);
	connect(configNameEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onConfNameChanged(const QString&)), Qt::UniqueConnection);

    connect(configNameEdit_, SIGNAL(textChanged(QString const&)),
            this, SLOT(prevalidate()), Qt::UniqueConnection);
    connect(vlnvEdit_, SIGNAL(contentChanged()),
            this, SLOT(prevalidate()), Qt::UniqueConnection);
}

void CreateConfigurationDialog::onOk()
{
    // Make sure that the VLNVs are not already in use.
    if (handler_->contains(getConfigurationVLNV()))
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                           tr("VLNV %1 already exists in the library.").arg(getConfigurationVLNV().toString()),
                           QMessageBox::Ok, this);
        msgBox.exec();
        return;
    }

    if (!useExistingRadio_->isChecked() && handler_->contains(getDesignVLNV()))
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                           tr("VLNV %1 already exists in the library.").arg(getDesignVLNV().toString()),
                           QMessageBox::Ok, this);
        msgBox.exec();
        return;
    }

    accept();
}

VLNV CreateConfigurationDialog::getConfigurationVLNV() const {
	VLNV configVLNV = vlnvEdit_->getVLNV();
	configVLNV.setType(VLNV::DESIGNCONFIGURATION);
	return configVLNV;
}

QString CreateConfigurationDialog::getConfigurationName() const {
	return configNameEdit_->text();
}

QString CreateConfigurationDialog::getImplementationViewName() const {
	return implementationCombo_->currentText();
}

VLNV CreateConfigurationDialog::getDesignVLNV() const {
	// first get the inputted values
	VLNV designVLNV = getConfigurationVLNV();
	designVLNV.setType(VLNV::DESIGN);

	// remove the possible ".designcfg" from the end
	QString name = designVLNV.getName().remove(QString(".designcfg"));

	// add .design to the end
	designVLNV.setName(name + ".design");
	return designVLNV;
}

void CreateConfigurationDialog::onConfNameChanged( const QString& newName ) {
	vlnvEdit_->setName(component_->getVlnv()->getName() + "." + newName + ".designcfg");
}

CreateConfigurationDialog::DesignSelection CreateConfigurationDialog::designSelection() const {
	if (useExistingRadio_->isChecked())
		return CreateConfigurationDialog::USE_EXISTING;
	else if (createNewRadio_->isChecked())
		return CreateConfigurationDialog::CREATE_EMPTY;
	else
		return CreateConfigurationDialog::CREATE_COPY;
}

//-----------------------------------------------------------------------------
// Function: CreateConfigurationDialog::prevalidate()
//-----------------------------------------------------------------------------
void CreateConfigurationDialog::prevalidate()
{
    okButton_->setEnabled(!configNameEdit_->text().isEmpty() && configNameEdit_->isInputValid() &&
                          vlnvEdit_->isValid());
}
