//-----------------------------------------------------------------------------
// File: GeneratorConfigurationDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.02.2015
//
// Description:
// Dialog for setting file generation options.
//-----------------------------------------------------------------------------

#include "GeneratorConfigurationDialog.h"
#include "GeneratorConfiguration.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::GeneratorConfigurationDialog()
//-----------------------------------------------------------------------------
GeneratorConfigurationDialog::GeneratorConfigurationDialog(QSharedPointer<GeneratorConfiguration> configuration, 
    QSharedPointer<QMap<QString,QSharedPointer<ComponentInstantiation> > > instantiations, QWidget *parent) : 
QDialog(parent), 
    configuration_(configuration),
    instantiations_(instantiations),
    viewSelection_(new QComboBox(this)),
    addToFileset_(new QGroupBox(tr("Add file to fileset"))),
    instantiationSelection_(new QComboBox(this)),
    fileSetSelection_(new QComboBox(this)),
    pathEditor_(new QLineEdit(this)),
    instantiationWarningLabel_(new QLabel),
    fileSetWarningLabel_(new QLabel),
    generalWarningLabel_(new QLabel)
{    
    setWindowTitle(tr("Configure file generation"));

	// Make view selection its own layout.
	QFormLayout* viewSelectionLayout = new QFormLayout();
	viewSelectionLayout->addRow(tr("Select view:"), viewSelection_);

	// Checkable group box used to include generated file in the IP-XACT component.
	addToFileset_->setCheckable(true);
	addToFileset_->setChecked(configuration->getSaveToFileset() != 0);

	// It will have its own sub layout.
	QFormLayout* filesetLayout = new QFormLayout();
	addToFileset_->setLayout(filesetLayout);

	// Widgets for choosing configuration instantiation and the file set.
	filesetLayout->addRow(tr("Select component instantiation:"), instantiationSelection_);
	filesetLayout->addRow(tr("Select file set:"), fileSetSelection_);
	// Both are editable, in case a custom entry is desired.
	instantiationSelection_->setEditable(true);
	fileSetSelection_->setEditable(true);

	// The instantiation selection shall be populated by all available instantiations.
	foreach (QSharedPointer<ComponentInstantiation> cimp, *instantiations_)
	{
		instantiationSelection_->addItem(cimp->name());
	}

	// Evaluate the instantiations.
	instantiationSelection_->setCurrentIndex(0);
	onInstantiationChanged(instantiationSelection_->currentText());

	// Layout for path selection widgets.
    QHBoxLayout* pathSelectionLayout = new QHBoxLayout();
    pathSelectionLayout->addWidget(new QLabel(tr("Select output file:")));
    pathSelectionLayout->addWidget(pathEditor_);

	// Get the default output path from the generation configuration.
	pathEditor_->setText(configuration_->getOutputPath());

	// Add button for choosing the path via dialog.
    QPushButton* browseButton = new QPushButton(tr("Browse"), this);
    pathSelectionLayout->addWidget(browseButton);

	// Layout for thing coming to the bottom part of the dialog.
	QHBoxLayout* bottomLayout = new QHBoxLayout();

	// Widgets for warning messages
	QVBoxLayout* warningLayout = new QVBoxLayout();
	bottomLayout->addLayout(warningLayout);
	warningLayout->addWidget(instantiationWarningLabel_);
	warningLayout->addWidget(fileSetWarningLabel_);

	// Add Ok and cancel give the dialog results.
    QDialogButtonBox* dialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
        Qt::Horizontal, this);

	bottomLayout->addWidget(dialogButtons);

	// Add everything it their proper position in the final layout.
	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addLayout(viewSelectionLayout);
    topLayout->addWidget(addToFileset_);
    topLayout->addLayout(pathSelectionLayout);
    topLayout->addStretch(1);
	topLayout->addLayout(bottomLayout);
	topLayout->addWidget(generalWarningLabel_);

	// Use red to make it more warny.
	generalWarningLabel_->setStyleSheet("QLabel { color : red; }");
	onPathEdited("");

	// Finally, connect the relevant events to their handler functions.
    connect(addToFileset_, SIGNAL(toggled(bool)), 
        this, SLOT(onFileSetStateChanged(bool)), Qt::UniqueConnection);
    connect(viewSelection_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onViewChanged(QString const&)), Qt::UniqueConnection);
	connect(instantiationSelection_, SIGNAL(currentTextChanged(QString const&)),
		this, SLOT(onInstantiationInserted(QString const&)), Qt::UniqueConnection);
	connect(instantiationSelection_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onInstantiationChanged(QString const&)), Qt::UniqueConnection);
	connect(fileSetSelection_, SIGNAL(currentIndexChanged(QString const&)),
		this, SLOT(onFileSetChanged(QString const&)), Qt::UniqueConnection);
	connect(fileSetSelection_, SIGNAL(currentTextChanged(QString const&)),
		this, SLOT(onFileSetChanged(QString const&)), Qt::UniqueConnection);
    connect(pathEditor_, SIGNAL(textChanged(const QString &)), this,
		SLOT(onPathEdited(const QString &)), Qt::UniqueConnection);
    connect(browseButton, SIGNAL(clicked(bool)), this, SLOT(onBrowse()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::~GeneratorConfigurationDialog()
//-----------------------------------------------------------------------------
GeneratorConfigurationDialog::~GeneratorConfigurationDialog()
{

}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::setViewNames()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::setViews(QSharedPointer<QList<QSharedPointer<View> > > const views)
{
    viewSelection_->clear();

	// Set names as the of views as items, but also keep track on which name corresponds which view.
	foreach (QSharedPointer<View> currentView, *views)
	{
		views_[currentView->name()] = currentView;
		viewSelection_->addItem(currentView->name());
	}

	viewSelection_->setCurrentIndex(0);
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::accept()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::accept()
{
	// Must have path for a file. 
    if (pathEditor_->text().isEmpty())
	{
		fileSetWarningLabel_->setText("File path must be defined!");
		return;
	}
	
	// If file is saved to file set, must have a file set and instantiation.
	if (configuration_->getSaveToFileset())
	{
		if (instantiationSelection_->currentText().isEmpty())
		{
			instantiationWarningLabel_->setText("Please define a component instantiation.");
			return;
		}

		if (fileSetSelection_->currentText().isEmpty())
		{
			fileSetWarningLabel_->setText("Please define a file set.");
			return;
		}
    }

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onFileSetStateChanged()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onFileSetStateChanged(bool on)
{
    configuration_->setSaveToFileset(on);
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onPathEdited()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onPathEdited(const QString &text)
{
	// At any rate, tell the path to the configuration.
    configuration_->setOutputPath(pathEditor_->text());

	QFile fileCandidate(pathEditor_->text());

	if (fileCandidate.exists())
	{
		// Warn user if it already exists
		generalWarningLabel_->setText("<b>The selected file already exists and will be overwritten.</b>");
	}
	else
	{
		// Else clear any warning.
		generalWarningLabel_->setText("");
	}
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onBrowse()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onBrowse()
{
	// Acquire the path to the file through a dialog.
    QString selectedPath = QFileDialog::getSaveFileName(this,
        tr("Select output file for generation"), pathEditor_->text(),
        tr("Verilog files (*.v)"));

	// If any path chosen, set it as the selected path.
    if (!selectedPath.isEmpty())
    {
        pathEditor_->setText(selectedPath);
        configuration_->setOutputPath(selectedPath);
    }
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onViewChanged()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onViewChanged(QString const& selectedViewName)
{
	// Get matching view from the list.
	QSharedPointer<View> selectedView = views_[selectedViewName];

	// Should exist.
	if (selectedView)
	{
		// Set to the configuration.
		configuration_->setActiveView(selectedView);

		// For convenience, set the matching component instantiation as the default choice.
		int index = instantiationSelection_->findText(selectedView->getComponentInstantiationRef());

		if (index != -1)
		{
			instantiationSelection_->setCurrentIndex(index);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onInstantiationInserted()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onInstantiationInserted(QString const& selectedInstantiationName)
{
	// Get matching instantiation from the list.
	QSharedPointer<ComponentInstantiation> selectedInstantiation =
		instantiations_->value(selectedInstantiationName);

	if (!selectedInstantiation && !selectedInstantiationName.isEmpty())
	{
		// Create a new one if does not exist.
		selectedInstantiation = QSharedPointer<ComponentInstantiation>( new ComponentInstantiation );
		selectedInstantiation->setName(selectedInstantiationName);
		selectedInstantiation->setLanguage("verilog");

		// Set it as the selected instantiation.
		configuration_->setInstantiation(selectedInstantiation);

		// Warn user that a new instantiation will be created.
		instantiationWarningLabel_->setText(tr("New component instantiation '%1' will be created.").arg(
            selectedInstantiationName));
	}
	else
	{
		instantiationWarningLabel_->setText("");
	}
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onInstantiationChanged()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onInstantiationChanged(QString const& selectedInstantiationName)
{
	// Get matching instantiation from the list.
	QSharedPointer<ComponentInstantiation> selectedInstantiation =
		instantiations_->value(selectedInstantiationName);

	if (selectedInstantiation)
	{
		// Set to the configuration.
		configuration_->setInstantiation(selectedInstantiation);

		// Affects the available file sets.
		fileSetSelection_->clear();
		fileSetSelection_->addItems(*(selectedInstantiation->getFileSetReferences()));
		fileSetSelection_->setCurrentIndex(0);
		onFileSetChanged(fileSetSelection_->currentText());
	}
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onFileSetChanged()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onFileSetChanged(QString const& fileSetName)
{
	// Set to the configuration.
	configuration_->setFileSetRef(fileSetName);

	if (!fileSetName.isEmpty() && ( !configuration_->getInstantiation() ||
		!configuration_->getInstantiation()->getFileSetReferences()->contains(fileSetName)))
	{
		// Warn user that a new file set will be created.
		fileSetWarningLabel_->setText(tr("New file set '%1' will be created.").arg(fileSetName));
	}
	else
	{
		fileSetWarningLabel_->setText("");
	}
}
