/* 
 *  	Created on: 11.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapseditor.cpp
 *		Project: Kactus 2
 */

#include "memorymapseditor.h"

#include <QHBoxLayout>

MemoryMapsEditor::MemoryMapsEditor( QSharedPointer<Component> component,
								   QWidget *parent ):
ItemEditor(component, parent),
view_(this) {


	//view_.setModel(&model_);

	int colWidth = 70;
	view_.setColumnWidth(0, colWidth);
	view_.setColumnWidth(1, colWidth);
	view_.setColumnWidth(2, colWidth);
	view_.setColumnWidth(3, colWidth);
	view_.setColumnWidth(4, colWidth);
	view_.setColumnWidth(5, colWidth);
	view_.setColumnWidth(6, colWidth);
	view_.setColumnWidth(7, colWidth);
	view_.setColumnWidth(8, colWidth);
	view_.setColumnWidth(9, colWidth);
	view_.setColumnWidth(10, colWidth);
	view_.setColumnWidth(11, colWidth);
	view_.setColumnWidth(12, colWidth);
	view_.setColumnWidth(13, colWidth);
	view_.setColumnWidth(14, colWidth);
	view_.setSortingEnabled(true);

}

MemoryMapsEditor::~MemoryMapsEditor() {
}

bool MemoryMapsEditor::isValid() const {
	return true;
}

void MemoryMapsEditor::refresh() {

}
