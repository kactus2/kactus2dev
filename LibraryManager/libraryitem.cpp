/* 
 *  Created on: 28.6.2010
 *  Author: kamppia
 */

#include "libraryitem.h"
#include "vlnv.h"

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QObject>
#include <stdexcept>
#include <QMutexLocker>

#include <QDebug>

// constructor for the root item
LibraryItem::LibraryItem(const QString& name, QObject* parent):
QObject(parent),
name_(name), 
level_(LibraryItem::ROOT),
childItems_(),
parentItem_(0),
vlnv_(NULL),
isValid_(true) {
}

// The constructor
LibraryItem::LibraryItem(VLNV* vlnv, Level level, LibraryItem *parent): 
QObject(parent), 
name_(), 
level_(level),
childItems_(), 
parentItem_(parent),
vlnv_(NULL),  
isValid_(vlnv->documentIsValid()) {

	QString childName;

	// choose name for the item in the tree
	switch (level) {
	case TYPE:
		name_ = VLNV::type2Print(vlnv->getType());
		childName = vlnv->getVendor();
		break;
	case VENDOR:
		name_ = vlnv->getVendor();
		childName =vlnv->getLibrary();
		break;
	case LIBRARY:
		name_ = vlnv->getLibrary();
		childName = vlnv->getName();
		break;
	case NAME:
		name_ = vlnv->getName();
		childName = vlnv->getVersion();
		break;
	case VERSION:
		name_ = vlnv->getVersion();
		vlnv_ = vlnv;
		break;
		// invalid level
	default:
		return;
	}

	// if the vlnv_ is null then the item has child items
	if (!vlnv_) {

		// a new item can't yet have children so no need to search for
		// one with a same name
		childItems_.push_back(new LibraryItem(vlnv,
						static_cast<Level>(static_cast<int>(level) + 1), this));
	}
}

// The destructor
LibraryItem::~LibraryItem() {
	for (int i = 0; i < childItems_.size(); ++i) {
		delete childItems_.at(i);
	}
	childItems_.clear();
}

void LibraryItem::createChild( VLNV* vlnv, Level level ) {
	QString childName;

	// choose name for the item in the tree
	switch (level) {
	case ROOT:
		childName = VLNV::type2Print(vlnv->getType());
		break;
	case TYPE:
		childName = vlnv->getVendor();
		break;
	case VENDOR:
		childName = vlnv->getLibrary();
		break;
	case LIBRARY:
		childName = vlnv->getName();
		break;
	case NAME:
		childName = vlnv->getVersion();
		break;
		// invalid level
	default:
		return;
	}

	// if the child to be created is valid then this item must always be valid 
	// too. Only if all children are invalid this can be invalid too
	if (vlnv->documentIsValid())
		isValid_ = true;

	// find a child with a same name
	for (int i = 0; i < childItems_.size(); ++i) {

		// if a child with the same name is found
		if (childItems_[i]->getName() == childName) {
			childItems_[i]->createChild(vlnv,
				static_cast<Level>(static_cast<int>(level) + 1));
			return;
		}
	}

	// no child with same name was found so create a new child
	LibraryItem* newItem = new LibraryItem(vlnv,
		static_cast<Level>(static_cast<int>(level) +1), this);
	childItems_.push_back(newItem);
	
	return;
}

// Get the name of the item
QString LibraryItem::getName() const {
	return name_;
}

// get the parent of the item
LibraryItem *LibraryItem::parent() const {
	return parentItem_;
}

// get the level of the item in the hierarchy
LibraryItem::Level LibraryItem::getLevel() const {
	return level_;
}

LibraryItem *LibraryItem::child(int index) const {
	return childItems_[index];
}

int LibraryItem::row() {
	if (parentItem_) {
		return parentItem_->childItems_.indexOf(this);
	}

	// if parent does not exist then invalid index is returned
	return -1;
}

int LibraryItem::getNumberOfChildren() const {
	return childItems_.size();
}

VLNV* LibraryItem::getVLNV() const {
	if (level_ != VERSION) {
		return 0;
	}
	return vlnv_;
}

int LibraryItem::getIndexOf(LibraryItem* item) {
	return childItems_.indexOf(item);
}

int LibraryItem::getRow() {
	return parentItem_->getIndexOf(this);
}

bool LibraryItem::hasChildren() const {
	return (childItems_.size() != 0);
}

void LibraryItem::getVLNVs( QList<VLNV*>& vlnvList ) {
	
	// if this is a leaf-object
	if (vlnv_)
		vlnvList.append(vlnv_);

	// if this item has child-items but no vlnv itself
	else {
		for (int i = 0; i < childItems_.size(); ++i) {
			childItems_.value(i)->getVLNVs(vlnvList);
		}
	}
}

void LibraryItem::removeChild(LibraryItem* childItem ) {

	Q_ASSERT_X(childItem, "LibraryItem::removeChild()",
		"Null LibraryItem-pointer");

	if (childItems_.contains(childItem)) {
		int index = childItems_.indexOf(childItem);

		delete childItems_.value(index);
		childItems_.removeAt(index);
	}

}

LibraryItem* LibraryItem::findHighestUnique( LibraryItem* childItem ) {

	// if this is the root item
	if (!parentItem_)
		return childItem;

	else if (!childItem)
		return parentItem_->findHighestUnique(this);

	// if the childItem is the only child
	else if (childItems_.contains(childItem) && childItems_.size() == 1) {
		
		// this is the highest to remove unless parent item is also removed
		return parentItem_->findHighestUnique(this);
	}

	return childItem;
}

LibraryItem* LibraryItem::findHighestUnique( const VLNV* vlnv ) {

	// search all children
	for (int i = 0; i < childItems_.size(); ++i) {

		switch (level_) {

		case ROOT: {
			if (childItems_.at(i)->getName() == VLNV::type2Print(vlnv->getType())) {
				return childItems_.at(i)->findHighestUnique(vlnv);
			}
			continue;
				   }
		case TYPE: {
			if (childItems_.at(i)->getName() == vlnv->getVendor()) {
				return childItems_.at(i)->findHighestUnique(vlnv);
			}
			continue;
				   }
		case VENDOR: {
			if (childItems_.at(i)->getName() == vlnv->getLibrary()) {
				return childItems_.at(i)->findHighestUnique(vlnv);
			}
			continue;
					 }
		case LIBRARY: {
			if (childItems_.at(i)->getName() == vlnv->getName()) {
				return childItems_.at(i)->findHighestUnique(vlnv);
			}
			continue;
					  }
		case NAME: {
			if (childItems_.at(i)->getName() == vlnv->getVersion()) {
				return childItems_.at(i)->findHighestUnique(vlnv);
			}
			continue;
				   }
		case VERSION: {
			return parentItem_->findHighestUnique(this);
					  }
		}
	}

	// if there was no child that matched the vlnv then this must be the highest 
	// unique that can be used.
	return this;
}

bool LibraryItem::isValid() {
	
	// if this item identifies the vlnv then it specifies the setting directly
	if (vlnv_) {
		isValid_ = vlnv_->documentIsValid();
		return isValid_;
	}

	return isValid_;
}

void LibraryItem::setValidity( bool valid ) {
	isValid_ = valid;

	// if this item identifies the vlnv then set the setting for vlnv also
	if (vlnv_)
		vlnv_->setDocumentValid(valid);
}

LibraryItem* LibraryItem::findItem( const VLNV& vlnv ) {
	QString searchedChildName;
	
	// choose name for the item in the tree
	switch (level_) {
	case ROOT:
		searchedChildName = VLNV::type2Print(vlnv.getType());
		break;
	case TYPE:
		searchedChildName = vlnv.getVendor();
		break;
	case VENDOR:
		searchedChildName = vlnv.getLibrary();
		break;
	case LIBRARY:
		searchedChildName = vlnv.getName();
		break;
	case NAME:
		searchedChildName = vlnv.getVersion();
		break;
	case VERSION:

		// if this item represents the specified vlnv
		if (*vlnv_ == vlnv)
			return this;
		
		// if vlnv was not found
		return 0;
	
	// invalid level
	default:
		return 0;
	}

	// search all child items
	for (int i = 0; i <childItems_.size(); ++i) {

		// if child item was one that was searched for
		if (childItems_.value(i)->getName() == searchedChildName)
			return childItems_.value(i)->findItem(vlnv);
	}

	// if no match was found
	return 0;
}

QString LibraryItem::getTypeStr() const {

	// if this is hierarchy that contains the type
	if (level_ == LibraryItem::TYPE)
		return name_;
	
	// if root is asked for the type it can't tell it
	else if (level_ == LibraryItem::ROOT)
		return QString();

	// ask parent to tell the type
	else
		return parentItem_->getTypeStr();
}

void LibraryItem::setVlnv( VLNV& vlnv ) {

	switch (level_) {
		case LibraryItem::ROOT: {
			return;
								}
		case LibraryItem::TYPE: {
			vlnv.setType(name_);
			return;
								}
		case LibraryItem::VENDOR: {
			vlnv.setVendor(name_);
			parentItem_->setVlnv(vlnv);
			return;
								  }
		case LibraryItem::LIBRARY: {
			vlnv.setLibrary(name_);
			parentItem_->setVlnv(vlnv);
			return;
								   }
		case LibraryItem::NAME: {
			vlnv.setName(name_);
			parentItem_->setVlnv(vlnv);
			return;
								}
		case LibraryItem::VERSION: {
			vlnv.setVersion(name_);
			parentItem_->setVlnv(vlnv);
			return;
								   }
								
	}
}

void LibraryItem::clear() {
	qDeleteAll(childItems_);
	childItems_.clear();
}

QList<LibraryItem*> LibraryItem::getVendors( const QRegExpValidator& validator ) const {
	QList<LibraryItem*> list;
	int pos = 0;

	switch (level_) {
		case LibraryItem::ROOT: {
			foreach (LibraryItem* item, childItems_) {
				list += item->getVendors(validator);
			}
			break;
								}
		case LibraryItem::TYPE: {
			foreach (LibraryItem* item, childItems_) {

                                QString name = item->getName();

                                // if the child's name passes the validation
                                if (QValidator::Acceptable == validator.validate(name, pos))
					list.append(item);
			}
			break;
								}
		default:
			Q_ASSERT_X(false, "LibraryItem::getVendors",
				"Switch came to code that should never be executed");
			return list;
			
	}
	// when items are appended to list return it.
	return list;
}

QList<LibraryItem*> LibraryItem::getVendors() const {
	QList<LibraryItem*> list;

	switch (level_) {
		case LibraryItem::ROOT: {
			foreach (LibraryItem* item, childItems_) {
				list += item->getVendors();
			}
			return list;
								}
		case LibraryItem::TYPE: {
			return childItems_;
								}
		default:
			Q_ASSERT_X(false, "LibraryItem::getVendors",
				"Switch came to code that should never be executed");
			return list;
	}
}

QList<LibraryItem*> LibraryItem::getLibraries( const QRegExpValidator& validator ) const {
	QList<LibraryItem*> list;
	int pos = 0;

	switch (level_) {
		case LibraryItem::ROOT:
		case LibraryItem::TYPE: {
			foreach (LibraryItem* item, childItems_) {
				list += item->getLibraries(validator);
			}
			break;
								}
		case LibraryItem::VENDOR: {
			foreach (LibraryItem* item, childItems_) {

                            QString name = item->getName();

                            // if the child's name passes the validation
                                if (QValidator::Acceptable == validator.validate(name, pos))
					list.append(item);
			}
			break;
								  }
		default:
			Q_ASSERT_X(false, "LibraryItem::getVendors",
				"Switch came to code that should never be executed");
			return list;
	}
	// when items are appended to list return it.
	return list;
}

QList<LibraryItem*> LibraryItem::getLibraries() const {
	QList<LibraryItem*> list;

	switch (level_) {
		case LibraryItem::ROOT:
		case LibraryItem::TYPE: {
			foreach (LibraryItem* item, childItems_) {
				list += item->getLibraries();
			}
			return list;
								}
		case LibraryItem::VENDOR: {
			return childItems_;
								  }
		default:
			Q_ASSERT_X(false, "LibraryItem::getVendors",
				"Switch came to code that should never be executed");
			return list;
	}
}

QList<LibraryItem*> LibraryItem::getNames() const {
	QList<LibraryItem*> list;

	switch (level_) {
		case LibraryItem::ROOT:
		case LibraryItem::TYPE: 
		case LibraryItem::VENDOR: {
			foreach (LibraryItem* item, childItems_) {
				list += item->getNames();
			}
			return list;
								}
		case LibraryItem::LIBRARY: {
			return childItems_;
								  }
		default:
			Q_ASSERT_X(false, "LibraryItem::getVendors",
				"Switch came to code that should never be executed");
			return list;
	}
}

QList<LibraryItem*> LibraryItem::getVersions() const {
	QList<LibraryItem*> list;

	switch (level_) {
		case LibraryItem::ROOT:
		case LibraryItem::TYPE: 
		case LibraryItem::VENDOR:
		case LibraryItem::LIBRARY: {
			foreach (LibraryItem* item, childItems_) {
				list += item->getNames();
			}
			return list;
								  }
		case LibraryItem::NAME: {
			return childItems_;
								   }
		default:
			Q_ASSERT_X(false, "LibraryItem::getVendors",
				"Switch came to code that should never be executed");
			return list;
	}
}
