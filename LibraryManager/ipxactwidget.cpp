/* 
 *
 *  Created on: 14.7.2010
 *      Author: Antti Kamppi
 */

#include "ipxactwidget.h"
#include "ipxactmodel.h"

#include <QWidget>
#include <QDomDocument>
#include <QTreeView>
#include <QFile>
#include <QVBoxLayout>

#include <QDebug>

// TODO add these to QSettings
const int WIDGET_WIDTH = 700;
const int WIDGET_HEIGHT = 800;

const int ELEMENT_NAME_WIDTH = 200;
const int ATTRIBUTE_WIDTH = 240;
const int VALUE_WIDTH = 200;

// the constructor
IPXactWidget::IPXactWidget(const QString &filePath, QWidget *parent,
		Qt::WindowFlags f):	QWidget(parent, f), model_(0), view_(0),
		layout_(0) {

	if (filePath.isEmpty()) {
		return;
	}
	QFile file(filePath);
	if (file.open(QIODevice::ReadOnly)) {
		QDomDocument document;
		if (document.setContent(&file)) {

			// create model and view and connect them
			model_ = QSharedPointer<IPXactModel>(
					new IPXactModel(document, this));
			view_ = QSharedPointer<QTreeView>(new QTreeView(this));
			view_->setModel(model_.data());

			// set properties for the treeview
			view_->setColumnWidth(0, ELEMENT_NAME_WIDTH);
			view_->setColumnWidth(1, ATTRIBUTE_WIDTH);
			view_->setColumnWidth(2, VALUE_WIDTH);

			// create layout and add view to it
			layout_ = QSharedPointer<QVBoxLayout>(new QVBoxLayout);
			layout_->addWidget(view_.data());
			this->setLayout(layout_.data());
		}
		file.close();
	}
}

// the destructor
IPXactWidget::~IPXactWidget() {
}

QSize IPXactWidget::sizeHint() const {
	return QSize(WIDGET_WIDTH, WIDGET_HEIGHT);
}
