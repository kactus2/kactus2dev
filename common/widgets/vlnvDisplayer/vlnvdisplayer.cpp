/* 
 *
 *  Created on: 11.3.2011
 *      Author: Antti Kamppi
 * 		filename: vlnvdisplayer.cpp
 */

#include "vlnvdisplayer.h"

#include <QGridLayout>
#include <QHBoxLayout>

VLNVDisplayer::VLNVDisplayer(QWidget *parent, const VLNV& vlnv): QGroupBox(parent),
vendor_(this), library_(this), name_(this), version_(this) {

	QLabel* vendor = new QLabel(tr("Vendor:"), this);
	QLabel* library = new QLabel(tr("Library:"), this);
	QLabel* name = new QLabel(tr("Name:"), this);
	QLabel* version = new QLabel(tr("Version:"), this);

	QGridLayout* layout = new QGridLayout();
	layout->addWidget(vendor, 0, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(library, 1, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(name, 2, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(version, 3, 0, 1, 1, Qt::AlignLeft);

	layout->addWidget(&vendor_, 0, 1, 1, 1, Qt::AlignLeft);
	layout->addWidget(&library_, 1, 1, 1, 1, Qt::AlignLeft);
	layout->addWidget(&name_, 2, 1, 1, 1, Qt::AlignLeft);
	layout->addWidget(&version_, 3, 1, 1, 1, Qt::AlignLeft);

	setVLNV(vlnv);

	QHBoxLayout* topLayout = new QHBoxLayout(this);
	topLayout->addLayout(layout);
	topLayout->addStretch();

	setFlat(true);
}

VLNVDisplayer::VLNVDisplayer( QWidget *parent, const VLNV* vlnv ):QGroupBox(parent),
vendor_(this), library_(this), name_(this), version_(this) {

	QLabel* vendor = new QLabel(tr("Vendor:"), this);
	QLabel* library = new QLabel(tr("Library:"), this);
	QLabel* name = new QLabel(tr("Name:"), this);
	QLabel* version = new QLabel(tr("Version:"), this);

	QGridLayout* layout = new QGridLayout();
	layout->addWidget(vendor, 0, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(library, 1, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(name, 2, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(version, 3, 0, 1, 1, Qt::AlignLeft);

	layout->addWidget(&vendor_, 0, 1, 1, 1, Qt::AlignLeft);
	layout->addWidget(&library_, 1, 1, 1, 1, Qt::AlignLeft);
	layout->addWidget(&name_, 2, 1, 1, 1, Qt::AlignLeft);
	layout->addWidget(&version_, 3, 1, 1, 1, Qt::AlignLeft);

	// if vlnv pointer is valid
	if (vlnv->isValid()) 
    {
		setVLNV(*vlnv);
    }
    else
    {
        vendor_.setText("UNSET");
        library_.setText("UNSET");
        name_.setText("UNSET");
        version_.setText("UNSET");
    }

	setFlat(true);

    QHBoxLayout* topLayout = new QHBoxLayout(this);
    topLayout->addLayout(layout);
    topLayout->addStretch();

	// this widget can not be expanded
	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

VLNVDisplayer::VLNVDisplayer( QWidget* parent ) : QGroupBox(parent), vendor_(this),
                                                  library_(this), name_(this), version_(this)
{
    QLabel* vendor = new QLabel(tr("Vendor:"), this);
    QLabel* library = new QLabel(tr("Library:"), this);
    QLabel* name = new QLabel(tr("Name:"), this);
    QLabel* version = new QLabel(tr("Version:"), this);

    QGridLayout* layout = new QGridLayout();
    layout->addWidget(vendor, 0, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(library, 1, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(name, 2, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(version, 3, 0, 1, 1, Qt::AlignLeft);

    layout->addWidget(&vendor_, 0, 1, 1, 1, Qt::AlignLeft);
    layout->addWidget(&library_, 1, 1, 1, 1, Qt::AlignLeft);
    layout->addWidget(&name_, 2, 1, 1, 1, Qt::AlignLeft);
    layout->addWidget(&version_, 3, 1, 1, 1, Qt::AlignLeft);

    QHBoxLayout* topLayout = new QHBoxLayout(this);
    topLayout->addLayout(layout);
    topLayout->addStretch();

    setFlat(true);
}

VLNVDisplayer::~VLNVDisplayer() {
}

void VLNVDisplayer::setVLNV( const VLNV& vlnv ) {
	vendor_.setText(vlnv.getVendor());
	library_.setText(vlnv.getLibrary());
	name_.setText(vlnv.getName());
	version_.setText(vlnv.getVersion());
}
