/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#include "typebox.h"

#include <QVBoxLayout>

// struct constructor
TypeBox::TypeSearchSettings::TypeSearchSettings(
		bool abstractors,
		bool abstractionDefs,
		bool busDefs,
		bool components,
		bool designs,
		bool designConfs,
		bool generatorChains): abstractors_(abstractors),
		abstractionDefs_(abstractionDefs), busDefs_(busDefs),
		components_(components), designs_(designs), designConfs_(designConfs),
		generatorChains_(generatorChains) {

}
TypeBox::TypeSearchSettings::TypeSearchSettings(): abstractors_(true),
		abstractionDefs_(true), busDefs_(true),
		components_(true), designs_(true), designConfs_(true),
		generatorChains_(true) {
}

TypeBox::TypeBox(QWidget* parent): QGroupBox(tr("Search for: "), parent),
searchAbstractors_(this),
searchAbstractionDefs_(this), searchBusDefs_(this), searchComponents_(this),
searchDesigns_(this), searchDesignConfs_(this), searchGeneratorChains_(this),
settings_(){

	// set the texts for the check boxes
	searchAbstractors_.setText(tr("Abstractors"));
	searchAbstractionDefs_.setText(tr("Abstraction Definitions"));
	searchBusDefs_.setText(tr("Bus Definitions"));
	searchComponents_.setText(tr("Components"));
	searchDesigns_.setText(tr("Designs"));
	searchDesignConfs_.setText(tr("Design Configurations"));
	searchGeneratorChains_.setText(tr("Generator Chains"));

	// set all checkBoxes to checked
	searchAbstractors_.setChecked(true);
	searchAbstractionDefs_.setChecked(true);
	searchBusDefs_.setChecked(true);
	searchComponents_.setChecked(true);
	searchDesigns_.setChecked(true);
	searchDesignConfs_.setChecked(true);
	searchGeneratorChains_.setChecked(true);

	// connect all check boxes to inform if their state is changed
	connect(&searchAbstractors_, SIGNAL(stateChanged(int)),
			this, SLOT(onCheckStateChanged()), Qt::UniqueConnection);
	connect(&searchAbstractionDefs_, SIGNAL(stateChanged(int)),
			this, SLOT(onCheckStateChanged()), Qt::UniqueConnection);
	connect(&searchBusDefs_, SIGNAL(stateChanged(int)),
			this, SLOT(onCheckStateChanged()), Qt::UniqueConnection);
	connect(&searchComponents_, SIGNAL(stateChanged(int)),
			this, SLOT(onCheckStateChanged()), Qt::UniqueConnection);
	connect(&searchDesigns_, SIGNAL(stateChanged(int)),
			this, SLOT(onCheckStateChanged()), Qt::UniqueConnection);
	connect(&searchDesignConfs_, SIGNAL(stateChanged(int)),
			this, SLOT(onCheckStateChanged()), Qt::UniqueConnection);
	connect(&searchGeneratorChains_, SIGNAL(stateChanged(int)),
			this, SLOT(onCheckStateChanged()), Qt::UniqueConnection);

	// the layout to manage the layout of the group box
	QVBoxLayout* layout = new QVBoxLayout(this);

	// add widgets to layout and set stretch to the bottom so check boxes are
	// always aligned to the top of the group box
	layout->addWidget(&searchAbstractors_);
	layout->addWidget(&searchAbstractionDefs_);
	layout->addWidget(&searchBusDefs_);
	layout->addWidget(&searchComponents_);
	layout->addWidget(&searchDesigns_);
	layout->addWidget(&searchDesignConfs_);
	layout->addWidget(&searchGeneratorChains_);
	layout->addStretch();
}

TypeBox::~TypeBox() {
}

void TypeBox::onCheckStateChanged() {

	// update the settings
	settings_.abstractors_ = searchAbstractors_.isChecked();
	settings_.abstractionDefs_ = searchAbstractionDefs_.isChecked();
	settings_.busDefs_ = searchBusDefs_.isChecked();
	settings_.components_ = searchComponents_.isChecked();
	settings_.designs_ = searchDesigns_.isChecked();
	settings_.designConfs_ = searchDesignConfs_.isChecked();
	settings_.generatorChains_ = searchGeneratorChains_.isChecked();

	// inform the change outside
	emit stateChanged(&settings_);
}
