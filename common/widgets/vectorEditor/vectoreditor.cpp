/* 
 *
 *  Created on: 12.4.2011
 *      Author: Antti Kamppi
 * 		filename: vectoreditor.cpp
 */

#include "vectoreditor.h"

#include <models/vector.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>

VectorEditor::VectorEditor(const QString& title,
						   QWidget *parent): 
QGroupBox(title, parent), 
vector_(),
enableBox_(tr("enable"), this),
left_(this), right_(this) {

	left_.setMinimum(0);
	left_.setDisabled(true);
	right_.setMinimum(0);
	right_.setDisabled(true);

	QLabel* leftLabel = new QLabel(tr("Left Bound"), this);
	QLabel* rightLabel = new QLabel(tr("Right bound"), this);

	QGridLayout* boundLayout = new QGridLayout();
	boundLayout->addWidget(leftLabel, 0, 0, 1, 1);
	boundLayout->addWidget(&left_, 0, 1, 1, 1);
	boundLayout->addWidget(rightLabel, 1, 0, 1, 1);
	boundLayout->addWidget(&right_, 1, 1, 1, 1);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(&enableBox_);
	topLayout->addLayout(boundLayout);

	// when user changes the state of enable-check box 
	connect(&enableBox_, SIGNAL(toggled(bool)),
		this, SLOT(onToggled(bool)), Qt::UniqueConnection);

	connect(&left_, SIGNAL(valueChanged(int)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&right_, SIGNAL(valueChanged(int)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

VectorEditor::~VectorEditor() {
}

bool VectorEditor::isValid() const {
	if (left_.value() < 0 || right_.value() < 0)
		return false;

	return true;
}

void VectorEditor::restoreChanges() {

	// if vector is defined
	if (vector_) {

		enableBox_.setChecked(true);
		left_.setValue(vector_->getLeft());
		right_.setValue(vector_->getRight());
	}
	// if vector is undefined
	else {
		enableBox_.setChecked(false);
		left_.setValue(0);
		right_.setValue(0);
	}
}

QSharedPointer<Vector> VectorEditor::applyChanges() {

	// if vector is to be defined
	if (enableBox_.isChecked()) {

		// if vector is not yet defined
		if (!vector_) {
			vector_ = QSharedPointer<Vector>(new Vector());
		}

		// set the left and right bounds for vector
		vector_->setLeft(left_.value());
		vector_->setRight(right_.value());
	}

	// if vector should not exist
	else 
		vector_.clear();

	return vector_;
}

void VectorEditor::onToggled( bool enabled ) {
	left_.setEnabled(enabled);
	right_.setEnabled(enabled);
	emit contentChanged();
}

void VectorEditor::setLeftMax( int max ) {
	left_.setMaximum(max);
	right_.setMaximum(max);
}

void VectorEditor::setRightMax( int max ) {
	right_.setMaximum(max);
}

void VectorEditor::setVector( QSharedPointer<Vector> vector ) {
	vector_ = vector;

	restoreChanges();
}

void VectorEditor::clear() {

	// disable the spin box editors
	onToggled(false);
	left_.setValue(0);
	right_.setValue(0);

	// clear the vector pointer
	vector_.clear();
}

int VectorEditor::getWidth() const {

	// if no vector is specified the width is one bit
	if (!enableBox_.isEnabled())
		return 1;

	return left_.value() - right_.value();
}
