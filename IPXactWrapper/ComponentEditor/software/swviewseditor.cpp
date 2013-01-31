/* 
 *  	Created on: 27.6.2012
 *      Author: Antti Kamppi
 * 		filename: swviewseditor.cpp
 *		Project: Kactus 2
 */

#include "swviewseditor.h"
#include "swviewsdelegate.h"
#include <common/widgets/summaryLabel/summarylabel.h>

#include <QVBoxLayout>

SWViewsEditor::SWViewsEditor(QSharedPointer<Component> component, 
	LibraryInterface* handler,
							 QWidget* parent /*= 0*/ ):
ItemEditor(component, handler, parent),
view_(this),
proxy_(this),
model_(component, this) {

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Software views"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	// items can not be dragged
	view_.setItemsDraggable(false);

	view_.setItemDelegate(new SWViewsDelegate(this));
	view_.setColumnWidth(SWViewsDelegate::NAME_COLUMN, SWViewsEditor::NAME_COLUMN_WIDTH);
	view_.setColumnWidth(SWViewsDelegate::HIER_REF_COLUMN, SWViewsEditor::HIER_REF_COLUMN_WIDTH);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(viewAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(viewRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

SWViewsEditor::~SWViewsEditor() {
}

bool SWViewsEditor::isValid() const {
	return model_.isValid();
}

void SWViewsEditor::refresh() {
	view_.update();
}

void SWViewsEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/swviews.html");
}
