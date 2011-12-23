/* 
 *
 *  Created on: 23.1.2011
 *      Author: Antti Kamppi
 */

#include "componenttreeitem.h"

#include <models/model.h>
#include <models/fileset.h>
#include <models/file.h>
#include <models/function.h>
#include <models/choice.h>
#include <models/modelparameter.h>
#include <models/parameter.h>
#include <models/memorymap.h>
#include <models/addressspace.h>
#include <models/remapstate.h>
#include <models/view.h>
#include <models/port.h>
#include <models/businterface.h>
#include <models/channel.h>
#include <models/cpu.h>
#include <models/otherclockdriver.h>
#include <models/componentgenerator.h>
#include <models/memorymapitem.h>
#include <models/addressblock.h>
#include <models/bank.h>
#include <models/subspacemap.h>
#include <models/generaldeclarations.h>

#include <LibraryManager/libraryinterface.h>

#include <QSharedPointer>
#include <QFileInfo>
#include <QApplication>
#include <QFileDialog>
#include <QStringList>

#include <QDebug>

#include <typeinfo>

ComponentTreeItem::ComponentTreeItem(TreeItemType type, 
									 void* elementPointer,
									 QSharedPointer<Component> component,
									 LibraryInterface* handler,
									 ComponentTreeItem* parentItem):
QObject(parentItem), 
type_(type),
text_(), 
dataPointer_(elementPointer),
component_(component),
parent_(parentItem), 
childItems_(), 
isValid_(true),
handler_(handler) {

    bool platformComp = component->getComponentImplementation() == KactusAttribute::KTS_SW &&
                        component->getComponentSWType() == KactusAttribute::KTS_SW_PLATFORM;
    bool endpointComp = component->getComponentImplementation() == KactusAttribute::KTS_SW &&
                        component->getComponentSWType() == KactusAttribute::KTS_SW_ENDPOINTS;
    bool hwComp = component->getComponentImplementation() == KactusAttribute::KTS_HW;

	// the type defines how the construction goes
	switch (type) {

		// if this is the root item
	case ComponentTreeItem::COMPONENT: {

		text_ = tr("Component");

		// create the child items for the root item

		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::GENERAL, component.data(), component, handler, this));

        if (!endpointComp)
        {
		    childItems_.append(new ComponentTreeItem(
			    ComponentTreeItem::FILESETS, 0, component, handler, this));
        }

// 		childItems_.append(new ComponentTreeItem(
// 			ComponentTreeItem::CHOICES, 0, component, this));

		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::MODELPARAMETERS, 0, component, handler, this));

		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::PARAMETERS, 0, component, handler, this));

// 		childItems_.append(new ComponentTreeItem(
// 			ComponentTreeItem::MEMORYMAPS, 0, component, this));

// 		childItems_.append(new ComponentTreeItem(
// 			ComponentTreeItem::ADDRESSSPACES, 0, component, this));

// 		childItems_.append(new ComponentTreeItem(
// 			ComponentTreeItem::REMAPSTATES, 0, component, this));

        if (!endpointComp)
        {
		    childItems_.append(new ComponentTreeItem(
			    ComponentTreeItem::VIEWS, 0, component, handler, this));
        }

        if (hwComp)
        {
		    childItems_.append(new ComponentTreeItem(
			    ComponentTreeItem::PORTS, 0, component, handler, this));
        }

        if (hwComp || platformComp)
        {
		    childItems_.append(new ComponentTreeItem(
			    ComponentTreeItem::BUSINTERFACES, 0, component, handler, this));
        }

        if (hwComp)
        {
		    childItems_.append(new ComponentTreeItem(
			    ComponentTreeItem::CHANNELS, 0, component, handler, this));
        }

// 		childItems_.append(new ComponentTreeItem(
// 			ComponentTreeItem::CPUS, 0, component, this));

        if (hwComp)
        {
		    childItems_.append(new ComponentTreeItem(
			    ComponentTreeItem::OTHERCLOCKDRIVERS, 0, component, handler, this));

            // HACK! We're using the vlnv as the data pointer to ensure uniqueness. Any better ideas?
            childItems_.append(new ComponentTreeItem(
                ComponentTreeItem::SOFTWARE, component->getVlnv(), component, handler, this));
        }

// 		childItems_.append(new ComponentTreeItem(
// 			ComponentTreeItem::COMPONENTGENERATORS, 0, component, this));
		break;
									   }
	case ComponentTreeItem::GENERAL: {
		text_ = tr("General");
		break;
									 }
	case ComponentTreeItem::FILESETS: {
		text_ = tr("File sets");

		// get filesets from the component
		QList<QSharedPointer<FileSet> > list = component_->getFileSets();
		for (int i = 0; i < list.size(); ++i) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::FILESET, list.at(i).data(),
				component, handler, this));
		}

		break;
									  }
	case ComponentTreeItem::FILESET: {

		// get pointer to the matching file set
		FileSet* fileSet = static_cast<FileSet*>(dataPointer_);
		
		// set the text that is shown in the editor
		text_ = fileSet->getName();

		QStringList errorList;
		isValid_ = fileSet->isValid(errorList, tr("edited component"), false);

		// File set always has three children: Default file builders, Files and Functions
		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::DEFAULTFILEBUILDERS, &fileSet->getDefaultFileBuilders(), 
			component, handler, this));
		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::FILES, 0, component, handler, this));
// 		childItems_.append(new ComponentTreeItem(
// 			ComponentTreeItem::FUNCTIONS, 0, component, this));

		// get files from the fileSet and append them to this item
		QList<QSharedPointer<File> > fileList = fileSet->getFiles();
		for (int i = 0; i < fileList.size(); ++i) {

			// add child to "Files" child item
			childItems_.value(1)->addChild(new ComponentTreeItem(
				ComponentTreeItem::FILE, fileList.at(i).data(), component, handler, childItems_.at(1)));
		}

		// get the functions defined for the file set
// 		QList<QSharedPointer<Function> > functionList = fileSet->getFunctions();
// 		for (int i = 0; i < functionList.size(); ++i) {
// 			
// 			childItems_.value(2)->addChild(new ComponentTreeItem(
// 				ComponentTreeItem::FUNCTION, functionList.at(i).data(), component, childItems_.at(2)));
// 		}

		break;
									 }
	case ComponentTreeItem::FILES: {
		text_ = tr("Files");
		break;
								   }
	case ComponentTreeItem::FILE: {
		File* file = static_cast<File*>(dataPointer_);
		
		// set the text shown in the editor
		//QFileInfo fileInfo(file->getName());
		text_ = file->getName();
		
		QStringList errorList;
		isValid_ = file->isValid(errorList, tr("edited component"), true);

		break;
								  }
	case ComponentTreeItem::FUNCTIONS: {
		text_ = tr("Functions");
		break;
									   }
	case ComponentTreeItem::FUNCTION: {
		Function* functionP = static_cast<Function*>(dataPointer_);
		
		// if entrypoint has been defined
		if (!functionP->getEntryPoint().isEmpty()) {
			text_ = functionP->getEntryPoint();
		}
		// if no entrypoint is defined then use the only mandatory field in function
		else {
			text_ = functionP->getFileRef();
		}
									  }
	case ComponentTreeItem::DEFAULTFILEBUILDERS: {
		text_ = tr("Default file builders");
		QList<QSharedPointer<FileBuilder> >* builders = 
			static_cast<QList<QSharedPointer<FileBuilder> >*>(dataPointer_);
		
		// check all default file builders and if at least one is not in valid state.
		QStringList errorList;
		foreach (QSharedPointer<FileBuilder> builder, *builders) {
			if (!builder->isValid(errorList, tr("edited component"))) {
				isValid_ = false;
			}
		}
		break;
												 }
	case ComponentTreeItem::CHOICES: {
		text_ = tr("Choices");

		// get choices from the component
		QList<QSharedPointer<Choice> > list = component_->getChoices();
		for (int i = 0; i < list.size(); ++i) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::CHOICE, list.at(i).data(), component, handler, this));
		}
		break;
									 }
	case ComponentTreeItem::CHOICE: {
		Choice* choice = static_cast<Choice*>(dataPointer_);

		// set the text shown in editor
		text_ = choice->getName();
		break;
									}

	case ComponentTreeItem::MODELPARAMETERS: {
        if (endpointComp)
        {
            text_ = tr("Endpoints");
        }
        else
        {
		    text_ = tr("Model parameters");
        }

		QMap<QString, QSharedPointer<ModelParameter> >* modelParams = 
			component_->getModel()->getModelParametersPointer();
		dataPointer_ = modelParams;

		// if at least one model parameter is invalid
		foreach (QSharedPointer<ModelParameter> modelParam, *modelParams) {
			if (!modelParam->isValid()) {
				isValid_ = false;
			}
		}

		break;
											 }
	case ComponentTreeItem::PARAMETERS: {
		text_ = tr("Parameters");

		QList<QSharedPointer<Parameter> >* params = &component_->getParameters();
		dataPointer_ = params;

		// if at least one parameter is invalid then show this as red
		foreach (QSharedPointer<Parameter> param, *params) {
			if (!param->isValid()) {
				isValid_ = false;
				break;
			}
		}
		break;
										}
	case ComponentTreeItem::MEMORYMAPS: {
		text_ = tr("Memory maps");

		// get memoryMaps from the component and add them to the model
		QList<QSharedPointer<MemoryMap> > list = component_->getMemoryMaps();
		for (int i = 0; i < list.size(); ++i) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::MEMORYMAP, list.at(i).data(), component, handler, this));
		}
		break;
										}
	case ComponentTreeItem::MEMORYMAP: {
		MemoryMap* memMap = static_cast<MemoryMap*>(dataPointer_);
		Q_ASSERT_X(memMap, "ComponentTreeItem constructor in case MEMORYMAP",
			"static_cast failed to give valid MemoryMap-pointer");

		// memory map always has 3 child-items: Address blocks, banks and subspace maps
		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::ADDRESSBLOCKS, 0, component, handler, this));
		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::BANKS, 0, component, handler, this));
		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::SUBSPACEMAPS, 0, component, handler, this));

		text_ = memMap->getName();

		// get list of addressBlocks, banks and subspaceMaps
		QList<QSharedPointer<MemoryMapItem> > list = memMap->getItems();
		for (int i = 0; i < list.size(); ++i) {
			
			// find out what class the MemoryMapItem actually is
			// (this is done because MemoryMapItem is base-class for 
			// AddressBlock, Bank and SubspaceMap
			
			// if object was addressBlock
			if (typeid(*list.at(i).data()).name() == typeid(AddressBlock).name()) {
				
				// search the addressBlocks child item where the new addressBlock is added to
				for (int j = 0; j < childItems_.size(); ++j) {

					if (childItems_.at(j)->type() == ComponentTreeItem::ADDRESSBLOCKS) {
						
						// create the new ComponentTreeItem that matches the addressBlock
						ComponentTreeItem* item = new ComponentTreeItem(
							ComponentTreeItem::ADDRESSBLOCK, list.at(i).data(), 
							component, handler, childItems_.at(j));
						
						// add the created item for the addressBlocks item
						childItems_.at(j)->addChild(item);
						break;
					}

				}
			}

			// if object was Bank
			else if (typeid(*list.at(i).data()).name() == typeid(Bank).name()) {
				
				// search the banks child item where the new bank is added to
				for (int j = 0; j < childItems_.size(); ++j) {

					if (childItems_.at(j)->type() == ComponentTreeItem::BANKS) {

						// create the new ComponentTreeItem that matches the addressBlock
						ComponentTreeItem* item = new ComponentTreeItem(
							ComponentTreeItem::BANK, list.at(i).data(), component, 
							handler, childItems_.at(j));

						// add the created item for the addressBlocks item
						childItems_.at(j)->addChild(item);
						break;
					}

				}
			}

			// if object was SubspaceMap
			else if (typeid(*list.at(i).data()).name() == typeid(SubspaceMap).name()) {
				
				// search the addressBlocks child item where the new addressBlock is added to
				for (int j = 0; j < childItems_.size(); ++j) {

					if (childItems_.at(j)->type() == ComponentTreeItem::SUBSPACEMAPS) {

						// create the new ComponentTreeItem that matches the addressBlock
						ComponentTreeItem* item = new ComponentTreeItem(
							ComponentTreeItem::SUBSPACEMAP, list.at(i).data(), component, handler, childItems_.at(j));

						// add the created item for the addressBlocks item
						childItems_.at(j)->addChild(item);
						break;
					}

				}
			}

			// TODO remove this in final
			else {
				qDebug() << "UnIdentified item in memorymap";
			}
		}
		break;
									   }
	case ComponentTreeItem::ADDRESSBLOCKS: {
		text_ = tr("Address blocks");
		break;
										   }
	case ComponentTreeItem::ADDRESSBLOCK: {
		AddressBlock* addrBlock = static_cast<AddressBlock*>(dataPointer_);
		Q_ASSERT_X(addrBlock, "ComponentTreeItem constructor in case ADDRESSBLOCK",
			"static_cast failed to give valid AddressBlock-pointer");

		text_ = addrBlock->getName();
		break;
										  }
	case ComponentTreeItem::BANKS: {
		text_ = tr("Banks");
		break;
								   }
	case ComponentTreeItem::BANK: {
		Bank* bank = static_cast<Bank*>(dataPointer_);
		Q_ASSERT_X(bank, "ComponentTreeItem constructor in case BANK",
			"static_cast failed to give valid Bank-pointer");

		text_ = bank->getName();

		// bank always has 3 child-items: Address blocks, banks and subspace maps
		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::ADDRESSBLOCKS, 0, component, handler, this));
		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::BANKS, 0, component, handler, this));
		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::SUBSPACEMAPS, 0, component, handler, this));

		// get list of addressBlocks, banks and subspaceMaps
		QList<QSharedPointer<MemoryMapItem> > list = bank->getItems();
		for (int i = 0; i < list.size(); ++i) {

			// find out what class the MemoryMapItem actually is
			// (this is done because MemoryMapItem is base-class for 
			// AddressBlock, Bank and SubspaceMap

			// if object was addressBlock
			if (typeid(*list.at(i).data()).name() == typeid(AddressBlock).name()) {

				// search the addressBlocks child item where the new addressBlock is added to
				for (int j = 0; j < childItems_.size(); ++j) {

					if (childItems_.at(j)->type() == ComponentTreeItem::ADDRESSBLOCKS) {

						// create the new ComponentTreeItem that matches the addressBlock
						ComponentTreeItem* item = new ComponentTreeItem(
							ComponentTreeItem::ADDRESSBLOCK, list.at(i).data(),
							component, handler, childItems_.at(j));

						// add the created item for the addressBlocks item
						childItems_.at(j)->addChild(item);
						break;
					}

				}
			}

			// if object was Bank
			else if (typeid(*list.at(i).data()).name() == typeid(Bank).name()) {

				// search the banks child item where the new bank is added to
				for (int j = 0; j < childItems_.size(); ++j) {

					if (childItems_.at(j)->type() == ComponentTreeItem::BANKS) {

						// create the new ComponentTreeItem that matches the addressBlock
						ComponentTreeItem* item = new ComponentTreeItem(
							ComponentTreeItem::BANK, list.at(i).data(), component,
							handler, childItems_.at(j));

						// add the created item for the addressBlocks item
						childItems_.at(j)->addChild(item);
						break;
					}

				}
			}

			// if object was SubspaceMap
			else if (typeid(*list.at(i).data()).name() == typeid(SubspaceMap).name()) {

				// search the addressBlocks child item where the new addressBlock is added to
				for (int j = 0; j < childItems_.size(); ++j) {

					if (childItems_.at(j)->type() == ComponentTreeItem::SUBSPACEMAPS) {

						// create the new ComponentTreeItem that matches the addressBlock
						ComponentTreeItem* item = new ComponentTreeItem(
							ComponentTreeItem::SUBSPACEMAP, list.at(i).data(), 
							component, handler, childItems_.at(j));

						// add the created item for the addressBlocks item
						childItems_.at(j)->addChild(item);
						break;
					}

				}
			}

			// TODO remove this in final
			else {
				qDebug() << "UnIdentified item in memorymap";
			}
		}
		break;
								  }
	case ComponentTreeItem::SUBSPACEMAPS: {
		text_ = tr("Subspace maps");
		break;
										  }
	case ComponentTreeItem::SUBSPACEMAP: {
		SubspaceMap* subMap = static_cast<SubspaceMap*>(dataPointer_);
		Q_ASSERT_X(subMap, "ComponentTreeItem constructor in case SUBSPACEMAP",
			"static_cast failed to give valid SubspaceMap-pointer");

		text_ = subMap->getName();
		break;
								   }
	case ComponentTreeItem::ADDRESSSPACES: {
		text_ = tr("Address spaces");

		// get addressSpaces from the component and add them to the model
		QList<QSharedPointer<AddressSpace> > list = component_->getAddressSpaces();
		for (int i = 0; i < list.size(); ++i) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::ADDRESSSPACE, list.at(i).data(), component, handler, this));
		}
		break;
										   }
	case ComponentTreeItem::ADDRESSSPACE: {
		AddressSpace* addrSpace = static_cast<AddressSpace*>(dataPointer_);
		Q_ASSERT_X(addrSpace, "ComponentTreeItem constructor in case ADDRESSSPACE",
			"static_cast failed to give valid addressSpace-pointer");

		text_ = addrSpace->getName();
		break;
										  }
	case ComponentTreeItem::REMAPSTATES: {
		text_ = tr("Remap states");

		// get remapStates from the component and add them to the model
		QList<QSharedPointer<RemapState> > list = component_->getRemapStates();
		for (int i = 0; i < list.size(); ++i) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::REMAPSTATE, list.at(i).data(), component, handler, this));
		}
		break;
										 }
	case ComponentTreeItem::REMAPSTATE: {
		RemapState* remapState = static_cast<RemapState*>(dataPointer_);
		Q_ASSERT_X(remapState, "ComponentTreeItem constructor in case REMAPSTATE",
			"static_cast failed to give valid RemapState-pointer");

		text_ = remapState->getName();
		break;
										}
	case ComponentTreeItem::VIEWS: {
		text_ = tr("Views");

		// get views from the component through model and set them to the model
		QList<QSharedPointer<View> > list = component_->getViews();
		for (int i = 0; i < list.size(); ++i) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::VIEW, list.at(i).data(), component, handler, this));
		}
		break;
								   }
	case ComponentTreeItem::VIEW: {
		View* view = static_cast<View*>(dataPointer_);
		Q_ASSERT_X(view, "ComponentTreeItem constructor in case VIEW",
			"static_cast failed to give valid View-pointer");

		text_ = view->getName();

		QStringList errorList;
		isValid_ = view->isValid(component_->getFileSetNames(), errorList, 
			tr("edited component"));

		break;
								  }
	case ComponentTreeItem::PORTS: {
		text_ = tr("Ports");

		QMap<QString, QSharedPointer<Port> >* ports =
			component_->getModel()->getPortsPointer();
		dataPointer_ = ports;

		bool hasViews = component_->hasViews();

		// if at least one port is invalid
		foreach (QSharedPointer<Port> port, *ports) {
			if (!port->isValid(hasViews)) {
				isValid_ = false;
				break;
			}
		}
		break;
								   }
	case ComponentTreeItem::BUSINTERFACES: {

        if (platformComp)
        {
            text_ = tr("APIs");
        }
        else
        {
		    text_ = tr("Bus interfaces");
        }

		// get list of keys for busInterfaces QMap
		QList<QString> busKeys = component_->getBusInterfaces().keys();

		// go through each key
		for (int i = 0; i < busKeys.size(); ++i) {
			
			// append the item that matches the key
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::BUSINTERFACE, component_->getBusInterface(busKeys.at(i)),
				component, handler, this));
		}
		break;
                                           }

    case ComponentTreeItem::BUSINTERFACE: {
		BusInterface* busInterface = static_cast<BusInterface*>(dataPointer_);
		Q_ASSERT_X(busInterface, "ComponentTreeItem constructor in case BUSINTERFACE",
			"static_cast failed to give valid BusInterface-pointer");

		text_ = busInterface->getName();

		break;
										  }
	case ComponentTreeItem::CHANNELS: {
		text_ = tr("Channels");

		// get the channels from the component and add them to the model
		QList<QSharedPointer<Channel> > list = component_->getChannels();
		for (int i = 0; i < list.size(); ++i) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::CHANNEL, list.at(i).data(), component, handler, this));
		}
		break;
									  }
	case ComponentTreeItem::CHANNEL: {
		Channel* channel = static_cast<Channel*>(dataPointer_);
		Q_ASSERT_X(channel, "ComponentTreeItem constructor in case CHANNEL",
			"static_cast failed to give valid Channel-pointer");

		text_ = channel->getName();

		break;
									 }
	case ComponentTreeItem::CPUS: {
		text_ = tr("Cpus");

		// get the cpus from the component and add them to the model
		QList<QSharedPointer<Cpu> > list = component_->getCpus();
		for (int i = 0; i < list.size(); ++i) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::CPU, list.at(i).data(), component, handler, this));
		}
		break;
								  }
	case ComponentTreeItem::CPU: {
		Cpu* cpu = static_cast<Cpu*>(dataPointer_);
		Q_ASSERT_X(cpu, "ComponentTreeItem constructor in case CPU",
			"static_cast failed to give valid Cpu-pointer");

		text_ = cpu->getName();

		break;
								 }
	case ComponentTreeItem::OTHERCLOCKDRIVERS: {
		text_ = tr("Other clock drivers");

		QList<QSharedPointer<OtherClockDriver> >* drivers = 
			&component_->getOtherClockDrivers();
		dataPointer_ = drivers;

		// if one of the drivers is invalid
		foreach (QSharedPointer<OtherClockDriver> driver, *drivers) {
			if (!driver->isValid()) {
				isValid_ = false;
				break;
			}
		}
		break;
											   }
	case ComponentTreeItem::COMPONENTGENERATORS: {
		text_ = tr("Component generators");
		break;
												 }

    case ComponentTreeItem::SOFTWARE: {
        text_ = tr("Software mappings");
        //dataPointer_ = 0;
        break;
                                      }

	default: {

		// TODO remove this
		qDebug() << "Unsupported type in ComponentTreeItem constructor";
		break;
			 }
	}

}

ComponentTreeItem::~ComponentTreeItem() {
}

ComponentTreeItem* ComponentTreeItem::child(int index) {

	// if index is invalid
	if (childItems_.size() <= index || index < 0) {
		return 0;
	}

	return childItems_.value(index);
}

ComponentTreeItem* ComponentTreeItem::parent() {
	return parent_;
}

int ComponentTreeItem::row() {

	// if parent exists (this in not root item)
	if (parent_) {
		return parent_->getIndexOf(this);
	}

	// if this was root item
	return -1;
}

void* ComponentTreeItem::data() {
	return dataPointer_;
}

QString ComponentTreeItem::text() const {
	return text_;
}

bool ComponentTreeItem::hasChildren() {
	return !childItems_.isEmpty();
}

int ComponentTreeItem::getIndexOf(ComponentTreeItem *child) {
	return childItems_.indexOf(child);
}

int ComponentTreeItem::rowCount() const {
	return childItems_.size();
}

QFont ComponentTreeItem::getFont() const {

	QFont font(QApplication::font());

	switch (type_) {
case ComponentTreeItem::GENERAL: 
case ComponentTreeItem::FILESETS: 
case ComponentTreeItem::CHOICES: 
case ComponentTreeItem::MODELPARAMETERS:
case ComponentTreeItem::PARAMETERS: 
case ComponentTreeItem::MEMORYMAPS: 
case ComponentTreeItem::ADDRESSSPACES: 
case ComponentTreeItem::REMAPSTATES: 
case ComponentTreeItem::VIEWS: 
case ComponentTreeItem::PORTS: 
case ComponentTreeItem::BUSINTERFACES:
case ComponentTreeItem::CHANNELS: 
case ComponentTreeItem::CPUS: 
case ComponentTreeItem::OTHERCLOCKDRIVERS: 
case ComponentTreeItem::COMPONENTGENERATORS:
case ComponentTreeItem::SOFTWARE: {
	font.setPointSize(font.pointSize() + 2);
	font.setBold(true);
	return font;
											 }
default: {
	return font;
		 }
	}
}

void ComponentTreeItem::addChild( ComponentTreeItem* child ) {
	childItems_.append(child);
}

ComponentTreeItem::TreeItemType ComponentTreeItem::type() const {
	return type_;
}

QList<ComponentTreeItem*> ComponentTreeItem::getChildItems() {
	return childItems_;
}

void* ComponentTreeItem::getDataPointer() const {
	return dataPointer_;
}

void ComponentTreeItem::setText( const QString newText ) {
	text_ = newText;
}

bool ComponentTreeItem::isValid() const {
	return isValid_;
}

void ComponentTreeItem::setValidity( bool valid ) {
	isValid_ = valid;
}

void ComponentTreeItem::removeChild( ComponentTreeItem* childPointer ) {
	childItems_.removeAll(childPointer);
}

void ComponentTreeItem::getDataPointers( QList<void*>& pointers ) {
	// if the dataPointer is not null then append it to the list.
	if (dataPointer_) {
		pointers.append(dataPointer_);
	}

	// ask all children to append their dataPointers
	foreach (ComponentTreeItem* item, childItems_) {
		item->getDataPointers(pointers);
	}
}

bool ComponentTreeItem::createChild() {

	// the type of parent defines how a child is created
	switch (type_) {
		case ComponentTreeItem::FILESETS: {
			FileSet* fileSet = component_->createFileSet();
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::FILESET, fileSet, component_, handler_, this));
			return true;
										  }
		case ComponentTreeItem::FILES: {
			// get pointer to the file set where the file is added to
			FileSet* fileSet = static_cast<FileSet*>(parent_->getDataPointer());

			QString componentPath = handler_->getPath(*component_->getVlnv());

			QStringList fileList = QFileDialog::getOpenFileNames(
				0, tr("Select files to add"), componentPath);

			foreach (QString filePath, fileList) {

				QString relPath = General::getRelativePath(componentPath, filePath);

				File* file = fileSet->createFile(relPath);
				childItems_.append(new ComponentTreeItem(
					ComponentTreeItem::FILE, file, component_, handler_, this));
			}

			return true;
									   }
		case ComponentTreeItem::CHANNELS: {

			// create a new empty channel
			Channel* chan = component_->createChannel();
			
			// create a child item that represents the channel
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::CHANNEL, chan, component_, handler_, this));

			return true;
										  }
		case ComponentTreeItem::BUSINTERFACES: {

			// create a new empty bus interface
			BusInterface* busif = component_->createBusInterface();

			// create a child item that represents the bus interface
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::BUSINTERFACE, busif, component_, handler_, this));

			return true;
											   }
		case ComponentTreeItem::VIEWS: {

			// create a new empty view
			View* view = component_->createView();

			// create a child item that represents the view
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::VIEW, view, component_, handler_, this));

			return true;
									   }
		// if the item type is such that the child can't be created
		default: {
			return false;
				 }
	}
}

bool ComponentTreeItem::canHaveChildren() const {
	switch (type_) {
		case ComponentTreeItem::GENERAL:
		case ComponentTreeItem::FILE:
		case ComponentTreeItem::FILESET:
		case ComponentTreeItem::MODELPARAMETERS:
		case ComponentTreeItem::PORTS:
		case ComponentTreeItem::PARAMETERS:
		case ComponentTreeItem::OTHERCLOCKDRIVERS:
		case ComponentTreeItem::BUSINTERFACE:
		case ComponentTreeItem::VIEW:
		case ComponentTreeItem::DEFAULTFILEBUILDERS:
        case ComponentTreeItem::SOFTWARE: {
			return false;
													 }
		default: {
			return true;
				 }
	}
}



bool ComponentTreeItem::canBeRemoved() const {
	switch (type_) {
		case ComponentTreeItem::GENERAL:
		case ComponentTreeItem::FILESETS:
		case ComponentTreeItem::CHOICES:
		case ComponentTreeItem::MODELPARAMETERS:
		case ComponentTreeItem::PARAMETERS:
		case ComponentTreeItem::MEMORYMAPS:
		case ComponentTreeItem::ADDRESSSPACES:
		case ComponentTreeItem::REMAPSTATES:
		case ComponentTreeItem::VIEWS:
		case ComponentTreeItem::PORTS:
		case ComponentTreeItem::BUSINTERFACES:
		case ComponentTreeItem::CHANNELS:
		case ComponentTreeItem::CPUS:
		case ComponentTreeItem::OTHERCLOCKDRIVERS:
		case ComponentTreeItem::DEFAULTFILEBUILDERS:
		case ComponentTreeItem::FILES:
		case ComponentTreeItem::FUNCTIONS:
		case ComponentTreeItem::COMPONENTGENERATORS:
        case ComponentTreeItem::SOFTWARE: {
			return false;
													 }
		default: {
			return true;
				 }
	}
}

void ComponentTreeItem::moveChild(int sourceIndex, int targetIndex ) {

	// if there is no child in source index.
	if (sourceIndex < 0 || sourceIndex >= childItems_.size())
		return;
	else if (targetIndex < 0 || targetIndex > childItems_.size())
		return;

	ComponentTreeItem* itemToMove = childItems_.takeAt(sourceIndex);

	// if item is moved down then the target must be decremented because on item is
	// removed before adding to the list
	if (targetIndex > sourceIndex)
		--targetIndex;

	childItems_.insert(targetIndex, itemToMove);
	
	switch (type_) {
		// if one of these types then tell the parent item to sort the IP-Xact model
		case ComponentTreeItem::FILES: {
			QStringList fileNames;
			foreach (ComponentTreeItem* child, childItems_) {
				fileNames.append(child->text());
			}
			parent_->sortModel(fileNames);
			return;
									   }
		default:
			return;
	}
}

void ComponentTreeItem::sortModel(const QStringList& idList) {
	
	// currently only file sets have ability to be sorted.
	if (type_ != ComponentTreeItem::FILESET)
		return;

	FileSet* fileSet = static_cast<FileSet*>(dataPointer_);
	fileSet->sortFiles(idList);
}
