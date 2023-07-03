/* 
 *  	Created on: 28.5.2012
 *      Author: Antti Kamppi
 * 		filename: namegroupeditor.cpp
 *		Project: Kactus 2
 */

#include "namegroupeditor.h"
#include <IPXACTmodels/common/validators/namevalidator.h>

#include <QLabel>
#include <QGridLayout>

//-----------------------------------------------------------------------------
// Function: NameGroupEditor::NameGroupEditor()
//-----------------------------------------------------------------------------
NameGroupEditor::NameGroupEditor(QSharedPointer<NameGroup> nameGroup, Document::Revision docRevision,
    QWidget* parent, QString const& title) :
QGroupBox(title, parent),
    nameGroup_(nameGroup),
    nameEdit_(this),
    displayNameEdit_(this),
    shortDescriptionEdit_(this),
    descriptionEdit_(this)
{
    setFlat(true);

    descriptionEdit_.setTabChangesFocus(true);

    shortDescriptionEdit_.setVisible(docRevision == Document::Revision::Std22);
    setupLayout(docRevision);

    connect(&nameEdit_, SIGNAL(textEdited(QString const&)),
        this, SLOT(onNameChanged(QString const&)), Qt::UniqueConnection);
    connect(&nameEdit_, SIGNAL(editingFinished()), this, SIGNAL(nameChanged()), Qt::UniqueConnection);
    connect(&displayNameEdit_, SIGNAL(textEdited(QString const&)),
        this, SLOT(onDisplayNameChanged(QString const&)), Qt::UniqueConnection);
    connect(&shortDescriptionEdit_, SIGNAL(textEdited(QString const&)),
            this, SLOT(onShortDescriptionChanged(QString const&)), Qt::UniqueConnection);
    connect(&descriptionEdit_, SIGNAL(textChanged()),
        this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: NameGroupEditor::~NameGroupEditor()
//-----------------------------------------------------------------------------
NameGroupEditor::~NameGroupEditor()
{
}

//-----------------------------------------------------------------------------
// Function: NameGroupEditor::name()
//-----------------------------------------------------------------------------
QString NameGroupEditor::name() const
{
	return nameGroup_->name();
}

//-----------------------------------------------------------------------------
// Function: NameGroupEditor::displayName()
//-----------------------------------------------------------------------------
QString NameGroupEditor::displayName() const
{
	return nameGroup_->displayName();
}

//-----------------------------------------------------------------------------
// Function: NameGroupEditor::description()
//-----------------------------------------------------------------------------
QString NameGroupEditor::description() const
{
	return nameGroup_->description();
}

//-----------------------------------------------------------------------------
// Function: NameGroupEditor::onNameChanged()
//-----------------------------------------------------------------------------
void NameGroupEditor::onNameChanged(QString const& newName )
{
	nameGroup_->setName(newName);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: NameGroupEditor::onDisplayNameChanged()
//-----------------------------------------------------------------------------
void NameGroupEditor::onDisplayNameChanged(QString const& newDisplayName )
{
	nameGroup_->setDisplayName(newDisplayName);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: NameGroupEditor::onShortDescriptionChanged()
//-----------------------------------------------------------------------------
void NameGroupEditor::onShortDescriptionChanged(QString const& newShortDescription)
{
    nameGroup_->setShortDescription(newShortDescription);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: NameGroupEditor::onDescriptionChanged()
//-----------------------------------------------------------------------------
void NameGroupEditor::onDescriptionChanged()
{
	nameGroup_->setDescription(descriptionEdit_.toPlainText());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: NameGroupEditor::isValid()
//-----------------------------------------------------------------------------
bool NameGroupEditor::isValid() const
{
	return !nameGroup_->name().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: NameGroupEditor::refresh()
//-----------------------------------------------------------------------------
void NameGroupEditor::refresh()
{
	nameEdit_.setText(nameGroup_->name());
	displayNameEdit_.setText(nameGroup_->displayName());
    
    disconnect(&shortDescriptionEdit_, SIGNAL(textEdited(QString const&)), 
        this, SLOT(onShortDescriptionChanged(QString const&)));
    shortDescriptionEdit_.setText(nameGroup_->shortDescription());
    connect(&shortDescriptionEdit_, SIGNAL(textEdited(QString const&)),
        this, SLOT(onShortDescriptionChanged(QString const&)), Qt::UniqueConnection);
	
	disconnect(&descriptionEdit_, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()));
	descriptionEdit_.setPlainText(nameGroup_->description());
	connect(&descriptionEdit_, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: NameGroupEditor::setupLayout()
//-----------------------------------------------------------------------------
void NameGroupEditor::setupLayout(Document::Revision docRevision)
{
    // the layout manager for this widget
    QGridLayout* layout = new QGridLayout(this);

    QLabel* nameLabel = new QLabel(tr("Name:"), this);
    layout->addWidget(nameLabel, 0, 0, 1, 1);

    nameEdit_.setToolTip(tr("Set the name for the element"));
    nameEdit_.setProperty("mandatoryField", true);

    NameValidator* validator = new NameValidator(&nameEdit_);
    nameEdit_.setValidator(validator);
    layout->addWidget(&nameEdit_, 0, 1, 1, 1);

    QLabel* displayLabel = new QLabel(tr("Display Name:"), this);
    layout->addWidget(displayLabel, 1, 0, 1, 1);

    displayNameEdit_.setToolTip(tr("Set the display name for the element"));
    layout->addWidget(&displayNameEdit_, 1, 1, 1, 1);

    if (docRevision == Document::Revision::Std22)
    {
        QLabel* shortDescriptionLabel = new QLabel(tr("Short Description:"), this);
        layout->addWidget(shortDescriptionLabel, 2, 0, 1, 1);

        shortDescriptionEdit_.setToolTip(tr("Set the short description for the element"));
        layout->addWidget(&shortDescriptionEdit_, 2, 1, 1, 1);
    }

    QLabel* descriptionLabel = new QLabel(tr("Description:"), this);
    layout->addWidget(descriptionLabel, 3, 0, 1, 1, Qt::AlignTop);

    descriptionEdit_.setToolTip(tr("Set the description for the element"));
    layout->addWidget(&descriptionEdit_, 3, 1, 1, 1);
    layout->setRowStretch(3,1);
}
