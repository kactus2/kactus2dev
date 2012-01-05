// TODO: check datatypes and use Parse_error exceptions

#include "design.h"

#include <LibraryManager/vlnv.h>
#include "generaldeclarations.h"

#include <QDomNamedNodeMap>
#include <QXmlStreamWriter>

#include <QDebug>

Design::Design(QDomDocument& doc):
LibraryComponent(doc), 
componentInstances_(),
interconnections_(), 
hierConnections_(),
adHocConnections_(),
attributes_() {

	LibraryComponent::vlnv_->setType(VLNV::DESIGN);

    QDomNodeList nodes = doc.childNodes();

    int i;
    for (i = 0; i < nodes.size(); i++) {
	if (nodes.at(i).nodeName() == "spirit:design")
	    break;
    }
    nodes = nodes.at(i).childNodes();

    for (int i = 0; i < nodes.size(); i++) {
        QDomNode node = nodes.at(i);

        if (node.nodeName() == "spirit:componentInstances") {
            QDomNodeList instanceNodes = node.childNodes();
            for (int i = 0; i < instanceNodes.size(); i++) {
                QDomNode instanceNode = instanceNodes.at(i);
                componentInstances_.append(ComponentInstance(instanceNode));
            }
        } else if (node.nodeName() == "spirit:interconnections") {
            QDomNodeList interconnectionNodes = node.childNodes();
            for (int i = 0; i < interconnectionNodes.size(); i++) {
                QDomNode interconnectionNode = interconnectionNodes.at(i);
                if (interconnectionNode.nodeName() == "spirit:interconnection")
                {
                    interconnections_.append(Interconnection(interconnectionNode));
                } else {
                    // TODO spirit::monitorInterconnection
                }
            }
        } else if (node.nodeName() == "spirit:adHocConnections") {
            QDomNodeList adHocNodes = node.childNodes();
            for (int i = 0; i < adHocNodes.size(); i++) {
                QDomNode adHocNode = adHocNodes.at(i);
                adHocConnections_.append(AdHocConnection(adHocNode));
            }
        } else if (node.nodeName() == "spirit:hierConnections") {
            QDomNodeList hierConnectionNodes = node.childNodes();
            for (int i = 0; i < hierConnectionNodes.size(); i++) {
                QDomNode hierConnectionNode = hierConnectionNodes.at(i);
                hierConnections_.append(HierConnection(hierConnectionNode));
            }
        } else if (node.nodeName() == "spirit:vendorExtensions") {
            parseVendorExtensions(node);
        }
    }
}

Design::Design(const VLNV &vlnv):
LibraryComponent(vlnv), 
componentInstances_(),
interconnections_(), 
hierConnections_(),
adHocConnections_(),
attributes_() {
	LibraryComponent::vlnv_->setType(VLNV::DESIGN);
}

Design::Design( const Design& other ):
LibraryComponent(other),
columns_(other.columns_),
componentInstances_(other.componentInstances_),
interconnections_(other.interconnections_),
hierConnections_(other.hierConnections_),
adHocConnections_(other.adHocConnections_),
attributes_(other.attributes_) {

}

Design& Design::operator=( const Design& other ) {
	if (this != &other) {
		LibraryComponent::operator=(other);
		
		columns_ = other.columns_;
		componentInstances_ = other.componentInstances_;
		interconnections_ = other.interconnections_;
		hierConnections_ = other.hierConnections_;
		adHocConnections_ = other.adHocConnections_;
		attributes_ = other.attributes_;
	}
	return *this;
}

Design::~Design() {
}

QSharedPointer<LibraryComponent> Design::clone() const {
	return QSharedPointer<LibraryComponent>(new Design(*this));
}

void Design::write(QFile& file)
{
	QXmlStreamWriter xmlWriter(&file);

	xmlWriter.setAutoFormatting(true);
	xmlWriter.setAutoFormattingIndent(-1);

	LibraryComponent::write(xmlWriter);

	// set the attributes
	setXMLNameSpaceAttributes(attributes_);

	// write the attributes for the spirit:designConfiguration element
	General::writeAttributes(xmlWriter, attributes_);

	LibraryComponent::writeVLNV(xmlWriter);

	if (!componentInstances_.isEmpty()) {
		xmlWriter.writeStartElement("spirit:componentInstances");

		foreach (ComponentInstance inst, componentInstances_) {
			xmlWriter.writeStartElement("spirit:componentInstance");

			xmlWriter.writeTextElement("spirit:instanceName",
				inst.instanceName);
			xmlWriter.writeTextElement("spirit:displayName",
				inst.displayName);
			xmlWriter.writeTextElement("spirit:description",
				inst.description);
			xmlWriter.writeEmptyElement("spirit:componentRef");
			General::writeVLNVAttributes(xmlWriter,
				&inst.componentRef);

			QMapIterator<QString, QString> i(inst.configurableElementValues);
			xmlWriter.writeStartElement("spirit:configurableElementValues");
			while (i.hasNext()) {
				i.next();

				xmlWriter.writeStartElement("spirit:configurableElementValue");
				xmlWriter.writeAttribute("spirit:referenceId", i.key());
				xmlWriter.writeCharacters(i.value());
				xmlWriter.writeEndElement();
			}

			xmlWriter.writeEndElement();

			// Write custom data to vendor extensions.
			xmlWriter.writeStartElement("spirit:vendorExtensions");

			// Write the component position.
			writePosition(xmlWriter, inst.position);

			// Write the port positions.
			QMapIterator<QString, QPointF> itrPortPos(inst.portPositions);
			xmlWriter.writeStartElement("kactus2:portPositions");

			while (itrPortPos.hasNext())
			{
				itrPortPos.next();

				xmlWriter.writeStartElement("kactus2:portPosition");
				xmlWriter.writeAttribute("kactus2:busRef", itrPortPos.key());
				writePosition(xmlWriter, itrPortPos.value());
				xmlWriter.writeEndElement();
			}

			xmlWriter.writeEndElement(); // kactus2:portPositions

			// Write the MCAPI node ID if specified.
			if (inst.mcapiNodeID != -1)
			{
				xmlWriter.writeEmptyElement("kactus2:mcapiNodeId");
				xmlWriter.writeAttribute("value", QString::number(inst.mcapiNodeID));
			}

			if (inst.endpointsExpanded)
			{
				xmlWriter.writeEmptyElement("kactus2:endpointsExpanded");
			}

			if (inst.imported)
			{
				xmlWriter.writeEmptyElement("kactus2:imported");
			}

			xmlWriter.writeEndElement(); // spirit:vendorExtensions
			xmlWriter.writeEndElement(); // spirit:componentInstance
		}

		xmlWriter.writeEndElement();
	}

	if (!interconnections_.isEmpty()) {
		xmlWriter.writeStartElement("spirit:interconnections");

		foreach (Interconnection inter, interconnections_) {
			xmlWriter.writeStartElement("spirit:interconnection");

			xmlWriter.writeTextElement("spirit:name",
				inter.name);
			xmlWriter.writeTextElement("spirit:displayName",
				inter.displayName);
			xmlWriter.writeTextElement("spirit:description",
				inter.description);

			xmlWriter.writeEmptyElement("spirit:activeInterface");
			xmlWriter.writeAttribute("spirit:componentRef",
				inter.interface1.componentRef);
			xmlWriter.writeAttribute("spirit:busRef",
				inter.interface1.busRef);

			xmlWriter.writeEmptyElement("spirit:activeInterface");
			xmlWriter.writeAttribute("spirit:componentRef",
				inter.interface2.componentRef);
			xmlWriter.writeAttribute("spirit:busRef",
				inter.interface2.busRef);

			xmlWriter.writeEndElement();
		}

		xmlWriter.writeEndElement();
	}

	if (!hierConnections_.isEmpty()) {
		xmlWriter.writeStartElement("spirit:hierConnections");

		foreach (HierConnection hier, hierConnections_) {
			xmlWriter.writeStartElement("spirit:hierConnection");
			xmlWriter.writeAttribute("spirit:interfaceRef",
				hier.interfaceRef);

			xmlWriter.writeEmptyElement("spirit:interface");
			xmlWriter.writeAttribute("spirit:componentRef",
				hier.interface_.componentRef);
			xmlWriter.writeAttribute("spirit:busRef",
				hier.interface_.busRef);

			// Write custom data to vendor extensions.
			xmlWriter.writeStartElement("spirit:vendorExtensions");
			writePosition(xmlWriter, hier.position);
			writeDirection(xmlWriter, hier.direction);

			if (!hier.route.empty())
			{
				xmlWriter.writeStartElement("kactus2:route");

				foreach (QPointF const& point, hier.route)
				{
					writePosition(xmlWriter, point);
				}

				xmlWriter.writeEndElement();
			}

			xmlWriter.writeEndElement();

			xmlWriter.writeEndElement();
		}

		xmlWriter.writeEndElement();
	}

	if (!adHocConnections_.isEmpty()) {
		xmlWriter.writeStartElement("spirit:adHocConnections");

		foreach (AdHocConnection adhoc, adHocConnections_) {
			xmlWriter.writeStartElement("spirit:adHocConnection");

			xmlWriter.writeTextElement("spirit:name",
				adhoc.name);
			xmlWriter.writeTextElement("spirit:displayName",
				adhoc.displayName);
			xmlWriter.writeTextElement("spirit:description",
				adhoc.description);

			foreach (PortRef portRef, adhoc.internalPortReferences) {
				xmlWriter.writeEmptyElement(
					"spirit:internalPortReference");

				xmlWriter.writeAttribute("spirit:componentRef",
					portRef.componentRef);
				xmlWriter.writeAttribute("spirit:portRef",
					portRef.portRef);
			}

			foreach (PortRef portRef, adhoc.externalPortReferences) {
				xmlWriter.writeEmptyElement(
					"spirit:externalPortReference");

				xmlWriter.writeAttribute("spirit:portRef",
					portRef.portRef);
			}

			xmlWriter.writeEndElement();
		}

		xmlWriter.writeEndElement();
	}

	xmlWriter.writeStartElement("spirit:vendorExtensions");

	if (!columns_.isEmpty())
	{
		xmlWriter.writeStartElement("kactus2:columnLayout");

		foreach (ColumnDesc const& columnDesc, columns_)
		{
			xmlWriter.writeEmptyElement("kactus2:column");
			xmlWriter.writeAttribute("name", columnDesc.name);
			xmlWriter.writeAttribute("contentType", QString::number(columnDesc.contentType));
			xmlWriter.writeAttribute("allowedItems", QString::number(columnDesc.allowedItems));
		}

		xmlWriter.writeEndElement();

		xmlWriter.writeStartElement("kactus2:routes");

		foreach (Interconnection const& conn, interconnections_)
		{
			if (!conn.route.empty())
			{
				xmlWriter.writeStartElement("kactus2:route");
				xmlWriter.writeAttribute("kactus2:connRef", conn.name);

				foreach (QPointF const& point, conn.route)
				{
					writePosition(xmlWriter, point);
				}

				xmlWriter.writeEndElement();
			}
		}

		xmlWriter.writeEndElement();
	}

	xmlWriter.writeEndElement();

	xmlWriter.writeEndElement();
	xmlWriter.writeEndDocument();
}

bool Design::isValid( QStringList& errorList ) const {
	bool valid = true;
	QString thisIdentifier(QObject::tr("containing design"));

	if (!vlnv_) {
		errorList.append(QObject::tr("No vlnv specified for the design."));
		valid = false;
	}
	else if (!vlnv_->isValid(errorList, thisIdentifier)) {
		valid = false;
	}
	else {
		thisIdentifier = QObject::tr("design %1").arg(vlnv_->toString());
	}

	QStringList instanceNames;
	foreach (Design::ComponentInstance instance, componentInstances_) {
		
		// if there are several instances with same name
		if (instanceNames.contains(instance.instanceName)) {
			errorList.append(QObject::tr("Design contains several instances with"
				" name %1").arg(instance.instanceName));
			valid = false;
		}
		else {
			instanceNames.append(instance.instanceName);
		}

		if (!instance.isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList interconnectionNames;
	foreach (Design::Interconnection interconnection, interconnections_) {
		
		// if there are several interconnections with same name
		if (interconnectionNames.contains(interconnection.name)) {
			errorList.append(QObject::tr("Design contains several interconnections"
				" with name %1").arg(interconnection.name));
			valid = false;
		}
		else {
			interconnectionNames.append(interconnection.name);
		}

		if (!interconnection.isValid(instanceNames, errorList, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList adHocNames;
	foreach (Design::AdHocConnection adHoc, adHocConnections_) {
		if (adHocNames.contains(adHoc.name)) {
			errorList.append(QObject::tr("Design contains several ad hoc connections"
				" with name %1").arg(adHoc.name));
			valid = false;
		}
		else {
			adHocNames.append(adHoc.name);
		}

		if (!adHoc.isValid(instanceNames, errorList, thisIdentifier)) {
			valid = false;
		}
	}

	foreach (Design::HierConnection hierConn, hierConnections_) {
		if (!hierConn.isValid(instanceNames, errorList, thisIdentifier)) {
			valid = false;
		}
	}

	return valid;
}

const QList<Design::ComponentInstance> &Design::getComponentInstances()
{
	return componentInstances_;
}

const QList<Design::Interconnection> &Design::getInterconnections()
{
	return interconnections_;
}

const QList<Design::HierConnection> &Design::getHierarchicalConnections()
{
	return hierConnections_;
}

const QList<Design::AdHocConnection> &Design::getAdHocConnections()
{
	return adHocConnections_;
}

void Design::setComponentInstances(QList<Design::ComponentInstance> const& componentInstances)
{
	componentInstances_ = componentInstances;
}

void Design::setInterconnections(QList<Design::Interconnection> const& interconnections)
{
	interconnections_ = interconnections;
}

void Design::setHierarchicalConnections(QList<Design::HierConnection> const& hierConnections)
{
	hierConnections_ = hierConnections;
}

void Design::setAdHocConnections(QList<Design::AdHocConnection> const& adHocConnections)
{
	adHocConnections_ = adHocConnections;
}

const QStringList Design::getDependentFiles() const
{
	// TODO implement this
	return QStringList();
}

const QList<VLNV> Design::getDependentVLNVs() const
{
	QList<VLNV> instanceList;

	// go through all component instances within the design
	for (int i = 0; i < this->componentInstances_.size(); ++i) {

		// if the pointer is valid and it is not already added to the list of
		// component instances (two instances of same component are not added
		// twice)
		if ((this->componentInstances_.at(i).componentRef.isValid()) &&
			(!instanceList.contains(componentInstances_.at(i).componentRef))) {

				// add the component VLNV to the list
				instanceList.append(componentInstances_.at(i).componentRef);
		}
	}
	return instanceList;
}

//-----------------------------------------------------------------------------
// Function: parseVendorExtensions()
//-----------------------------------------------------------------------------
void Design::parseVendorExtensions(QDomNode &node)
{
	QDomNodeList childNodes = node.childNodes();

	for (int i = 0; i < childNodes.size(); i++)
	{
		QDomNode childNode = childNodes.at(i);

		// If the column layout was found, read the column descriptions.
		if (childNode.nodeName() == "kactus2:columnLayout")
		{
			QDomNodeList columnNodes = childNode.childNodes();

			for (int i = 0; i < columnNodes.size(); ++i)
			{
				if (columnNodes.at(i).nodeName() == "kactus2:column")
				{
					columns_.append(ColumnDesc(columnNodes.at(i)));
				}
			}
		}
		// Otherwise read the interconnection routes if they were found.
		else if (childNode.nodeName() == "kactus2:routes")
		{
			QDomNodeList connNodes = childNode.childNodes();

			for (int i = 0; i < connNodes.size(); ++i)
			{
				QDomNode connNode = connNodes.at(i);

				if (connNode.nodeName() == "kactus2:route")
				{
					QString name = connNode.attributes().namedItem("kactus2:connRef").nodeValue();
					QList<QPointF> route;

					// Parse the route.
					for (int i = 0; i < connNode.childNodes().size(); ++i)
					{
						QDomNode posNode = connNode.childNodes().at(i);
						QPointF pos;

						if (posNode.nodeName() == "kactus2:position")
						{
							pos.setX(posNode.attributes().namedItem("x").nodeValue().toInt());
							pos.setY(posNode.attributes().namedItem("y").nodeValue().toInt());
							route.append(pos);
						}
					}

					// Apply the route to the correct interconnection.
					for (int i = 0; i < interconnections_.size(); ++i)
					{
						if (interconnections_[i].name == name)
						{
							interconnections_[i].route = route;
							break;
						}
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: getColumns()
//-----------------------------------------------------------------------------
QList<Design::ColumnDesc> const& Design::getColumns() const
{
	return columns_;
}

//-----------------------------------------------------------------------------
// Function: setColumns()
//-----------------------------------------------------------------------------
void Design::setColumns(QList<ColumnDesc> const& columns)
{
	columns_ = columns;
}

//-----------------------------------------------------------------------------
// Function: writePosition()
//-----------------------------------------------------------------------------
void Design::writePosition(QXmlStreamWriter& xmlWriter, QPointF const& pos)
{
	xmlWriter.writeEmptyElement("kactus2:position");
	xmlWriter.writeAttribute("x", QString::number(int(pos.x())));
	xmlWriter.writeAttribute("y", QString::number(int(pos.y())));
}

//-----------------------------------------------------------------------------
// Function: writeDirection()
//-----------------------------------------------------------------------------
void Design::writeDirection(QXmlStreamWriter& xmlWriter, QVector2D const& dir)
{
	xmlWriter.writeEmptyElement("kactus2:direction");
	xmlWriter.writeAttribute("x", QString::number(int(dir.x())));
	xmlWriter.writeAttribute("y", QString::number(int(dir.y())));
}

QList<VLNV> Design::getComponents() const {

	QList<VLNV> list;

	foreach (Design::ComponentInstance const& instance, componentInstances_) {
		if (instance.componentRef.isValid())
			list.append(instance.componentRef);
	}
	return list;
}

void Design::setVlnv( const VLNV& vlnv ) {
	LibraryComponent::setVlnv(vlnv);
	LibraryComponent::vlnv_->setType(VLNV::DESIGN);
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc()
//-----------------------------------------------------------------------------
Design::ColumnDesc::ColumnDesc(const QDomNode& node) : name(), contentType(COLUMN_CONTENT_IO),
                                                 allowedItems(CIT_NONE)
{
    // Read the column description data from the attributes.
    QDomNamedNodeMap attributeMap =  node.attributes();

    name = attributeMap.namedItem("name").nodeValue();
    contentType = static_cast<ColumnContentType>(attributeMap.namedItem("contentType").nodeValue().toInt());
    allowedItems = attributeMap.namedItem("allowedItems").nodeValue().toInt();
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc()
//-----------------------------------------------------------------------------
Design::ColumnDesc::ColumnDesc(QString const& name, ColumnContentType contentType,
                               unsigned int allowedItems) : name(name), contentType(contentType),
                                                            allowedItems(allowedItems)
{
}

Design::ColumnDesc::ColumnDesc( const ColumnDesc& other ):
name(other.name),
contentType(other.contentType),
allowedItems(other.allowedItems) {
}

Design::ColumnDesc& Design::ColumnDesc::operator=( const ColumnDesc& other ) {
	if (this != &other) {
		name = other.name;
		contentType = other.contentType;
		allowedItems = other.allowedItems;
	}
	return *this;
}

Design::ComponentInstance::ComponentInstance(
    QDomNode& componentInstanceNode)
    : instanceName(), displayName(), description(), componentRef(),
      configurableElementValues(), portPositions(), mcapiNodeID(-1), endpointsExpanded(false), imported(false)
{
    QDomNodeList nodes = componentInstanceNode.childNodes();
    for (int i = 0; i < nodes.size(); i++) {
        QDomNode node = nodes.at(i);

        if (node.nodeName() == "spirit:instanceName") {
            instanceName = node.firstChild().nodeValue();
        } else if (node.nodeName() == "spirit:displayName") {
            displayName = node.firstChild().nodeValue();
        } else if (node.nodeName() == "spirit:description") {
            description = node.firstChild().nodeValue();
        } else if (node.nodeName() == "spirit:componentRef") {
            componentRef = General::createVLNV(node, VLNV::COMPONENT);
        } else if (node.nodeName() == "spirit:configurableElementValues") {
            for (int i = 0; i < node.childNodes().size(); i++) {
                QDomNode confNode = node.childNodes().at(i);

                QString value = confNode.firstChild().nodeValue();

                QDomNamedNodeMap attributes = confNode.attributes();
                QString reference = 
                    attributes.namedItem("spirit:referenceId").nodeValue();
                configurableElementValues.insert(reference, value);
            }
        } else if (node.nodeName() == "spirit:vendorExtensions") {
            for (int i = 0; i < node.childNodes().size(); ++i)
            {
                QDomNode childNode = node.childNodes().at(i);

                if (childNode.nodeName() == "kactus2:position")
                {
                    QDomNamedNodeMap attributeMap = childNode.attributes();
                    position.setX(attributeMap.namedItem("x").nodeValue().toInt());
                    position.setY(attributeMap.namedItem("y").nodeValue().toInt());
                }
                else if (childNode.nodeName() == "kactus2:portPositions")
                {
                    parsePortPositions(childNode);
                }
                else if (childNode.nodeName() == "kactus2:mcapiNodeId")
                {
                    mcapiNodeID = childNode.attributes().namedItem("value").nodeValue().toInt();
                }
                else if (childNode.nodeName() == "kactus2:endpointsExpanded")
                {
                    endpointsExpanded = true;
                }
                else if (childNode.nodeName() == "kactus2:imported")
                {
                    imported = true;
                }
            }
        }
    }
}

Design::ComponentInstance::ComponentInstance(
    QString instanceName, QString displayName, QString description,
    VLNV const& componentRef, QPointF const& position)
    : instanceName(instanceName), displayName(displayName),
      description(description), componentRef(componentRef),
      configurableElementValues(), position(position), mcapiNodeID(-1),
      endpointsExpanded(false), imported(false)
{
}

Design::ComponentInstance::ComponentInstance( const ComponentInstance& other ):
instanceName(other.instanceName),
displayName(other.displayName),
description(other.description),
componentRef(other.componentRef),
configurableElementValues(other.configurableElementValues),
position(other.position),
portPositions(other.portPositions),
mcapiNodeID(other.mcapiNodeID), endpointsExpanded(other.endpointsExpanded), imported(other.imported) {
}

Design::ComponentInstance& Design::ComponentInstance::operator=( const ComponentInstance& other ) {
	if (this != &other) {
		instanceName = other.instanceName;
		displayName = other.displayName;
		description = other.description;
		componentRef = other.componentRef;
		configurableElementValues = other.configurableElementValues;
		position = other.position;
		portPositions = other.portPositions;
        mcapiNodeID = other.mcapiNodeID;
        endpointsExpanded = other.endpointsExpanded;
        imported = other.imported;
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: parsePortPositions()
//-----------------------------------------------------------------------------
void Design::ComponentInstance::parsePortPositions(QDomNode& node)
{
    for (int i = 0; i < node.childNodes().size(); ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.nodeName() == "kactus2:portPosition")
        {
            QString name = childNode.attributes().namedItem("kactus2:busRef").nodeValue();
            QPointF pos;

            if (childNode.childNodes().size() > 0 &&
                childNode.childNodes().at(0).nodeName() == "kactus2:position")
            {
                QDomNode posNode = childNode.childNodes().at(0);
                pos.setX(posNode.attributes().namedItem("x").nodeValue().toInt());
                pos.setY(posNode.attributes().namedItem("y").nodeValue().toInt());
            }

            portPositions[name] = pos;
        }
    }
}

bool Design::ComponentInstance::isValid( QStringList& errorList, 
										const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("component instance %1").arg(instanceName));

	if (instanceName.isEmpty()) {
		errorList.append(QObject::tr("No name specified for component instance "
			"within %1").arg(parentIdentifier));
		valid = false;
	}

	if (!componentRef.isValid(errorList, thisIdentifier)) {
		valid = false;
	}

	for (QMap<QString, QString>::const_iterator i = configurableElementValues.begin();
		i != configurableElementValues.end(); ++i) {

			if (i.key().isEmpty()) {
				errorList.append(QObject::tr("No configurable element value set"
					" for %1 within %2").arg(thisIdentifier).arg(parentIdentifier));
				valid = false;
			}
			if (i.value().isEmpty()) {
				errorList.append(QObject::tr("No reference id set for configurable"
					" element value in %1 within %2").arg(thisIdentifier).arg(
					parentIdentifier));
				valid = false;
			}
	}

	return valid;
}

Design::Interface::Interface(QDomNode &interfaceNode):
componentRef(""), 
busRef("")
{
    QDomNamedNodeMap attributes = interfaceNode.attributes();

    componentRef = attributes.namedItem("spirit:componentRef").nodeValue();
    busRef = attributes.namedItem("spirit:busRef").nodeValue();
}

Design::Interface::Interface(QString componentRef_, QString busRef_)
    : componentRef(componentRef_), busRef(busRef_)
{
}

Design::Interface::Interface( const Interface& other ):
componentRef(other.componentRef),
busRef(other.busRef) {
}

Design::Interface& Design::Interface::operator=( const Interface& other ) {
	if (this != &other) {
		componentRef = other.componentRef;
		busRef = other.busRef;
	}
	return *this;
}

bool Design::Interface::isValid( const QStringList& instanceNames,
								QStringList& errorList,
								const QString& parentIdentifier ) const {
	bool valid = true;
	
	if (componentRef.isEmpty()) {
		errorList.append(QObject::tr("No component reference set for active"
			" interface within %1").arg(parentIdentifier));
		valid = false;
	}
	else if (!instanceNames.contains(componentRef)) {
		errorList.append(QObject::tr("Active interface within %1 contains "
			"reference to component instance %2 that does not exist.").arg(
			parentIdentifier).arg(componentRef));
		valid = false;
	}

	if (busRef.isEmpty()) {
		errorList.append(QObject::tr("No bus reference set for active interface"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	return valid;
}

Design::Interconnection::Interconnection(QDomNode &interconnectionNode)
    : name(), displayName(), description(),
      interface1(QString(), QString()), interface2(QString(), QString())
{
    QDomNodeList nodes = interconnectionNode.childNodes();

    for (int i = 0; i < nodes.size(); i++) {
	QDomNode node = nodes.at(i);

	if (node.nodeName() == "spirit:name") {
	    name = node.firstChild().nodeValue();
	} else if (node.nodeName() == "spirit:displayName") {
	    displayName = node.firstChild().nodeValue();
	} else if (node.nodeName() == "spirit:description") {
	    description = node.firstChild().nodeValue();
	} else if (node.nodeName() == "spirit:activeInterface") {
	    if (interface1.componentRef.isNull())
		interface1 = Interface(node);
	    else
		interface2 = Interface(node);
	} // TODO: spirit:monitoredActiveInterface spirit:monitorInterface
    }
}

Design::Interconnection::Interconnection(QString name,
					 Interface interface1,
					 Interface interface2,
                     QList<QPointF> const& route,
					 QString displayName,
					 QString description)
    : name(name), displayName(displayName), description(description),
      interface1(interface1), interface2(interface2), route(route)
{
}

Design::Interconnection::Interconnection( const Interconnection& other ):
name(other.name),
displayName(other.displayName),
description(other.description),
interface1(other.interface1),
interface2(other.interface2),
route(other.route) {
}

Design::Interconnection& Design::Interconnection::operator=( const Interconnection& other ) {
	if (this != &other) {
		name = other.name;
		displayName = other.displayName;
		description = other.description;
		interface1 = other.interface1;
		interface2 = other.interface2;
		route = other.route;
	}
	return *this;
}

bool Design::Interconnection::isValid( const QStringList& instanceNames, 
									  QStringList& errorList,
									  const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("interconnection within %1").arg(
		parentIdentifier));

	if (name.isEmpty()) {
		errorList.append(QObject::tr("No name specified for interconnection"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	if (!interface1.isValid(instanceNames, errorList, thisIdentifier)) {
		valid = false;
	}

	if (!interface2.isValid(instanceNames, errorList, thisIdentifier)) {
		valid = false;
	}

	return valid;
}

Design::HierConnection::HierConnection(QDomNode &hierConnectionNode)
    : interfaceRef(), interface_(QString(""), QString("")), position()
{
    QDomNamedNodeMap attributes = hierConnectionNode.attributes();

    interfaceRef = attributes.namedItem("spirit:interfaceRef").nodeValue();

    for (int i = 0; i < hierConnectionNode.childNodes().size(); i++) {
        QDomNode node = hierConnectionNode.childNodes().at(i);

        if (node.nodeName() == "spirit:interface")
            interface_ = Interface(node);
        else if (node.nodeName() == "spirit:vendorExtensions")
        {
            for (int i = 0; i < node.childNodes().size(); ++i)
            {
                QDomNode childNode = node.childNodes().at(i);

                if (childNode.nodeName() == "kactus2:position")
                {
                    QDomNamedNodeMap attributeMap = childNode.attributes();
                    position.setX(attributeMap.namedItem("x").nodeValue().toInt());
                    position.setY(attributeMap.namedItem("y").nodeValue().toInt());
                }
                else if (childNode.nodeName() == "kactus2:direction")
                {
                    QDomNamedNodeMap attributeMap = childNode.attributes();
                    direction.setX(attributeMap.namedItem("x").nodeValue().toInt());
                    direction.setY(attributeMap.namedItem("y").nodeValue().toInt());
                }
                else if (childNode.nodeName() == "kactus2:route")
                {
                    // Parse the route.
                    for (int i = 0; i < childNode.childNodes().size(); ++i)
                    {
                        QDomNode posNode = childNode.childNodes().at(i);
                        QPointF pos;

                        if (posNode.nodeName() == "kactus2:position")
                        {
                            pos.setX(posNode.attributes().namedItem("x").nodeValue().toInt());
                            pos.setY(posNode.attributes().namedItem("y").nodeValue().toInt());
                            route.append(pos);
                        }
                    }
                }
            }
        }
    }
}

Design::HierConnection::HierConnection(QString interfaceRef_,
                                       Interface interface_,
                                       QPointF const& position,
                                       QVector2D const& direction,
                                       QList<QPointF> const& route)
    : interfaceRef(interfaceRef_), interface_(interface_),
      position(position), direction(direction), route(route)
{
}

Design::HierConnection::HierConnection( const HierConnection& other ):
interfaceRef(other.interfaceRef),
interface_(other.interface_),
position(other.position),
direction(other.direction),
route(other.route) {
}

Design::HierConnection& Design::HierConnection::operator=( const HierConnection& other ) {
	if (this != &other) {
		interfaceRef = other.interfaceRef;
		interface_ = other.interface_;
		position = other.position;
		direction = other.direction;
		route = other.route;
	}
	return *this;
}

bool Design::HierConnection::isValid( const QStringList& instanceNames,
									 QStringList& errorList,
									 const QString& parentIdentifier ) const {
	bool valid = true;

	if (interfaceRef.isEmpty()) {
		errorList.append(QObject::tr("No interface reference set for hier connection"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	if (!interface_.isValid(instanceNames, errorList, QObject::tr("hier connection"
		" within %1").arg(parentIdentifier))) {
			valid = false;
	}

	return valid;
}

Design::PortRef::PortRef(QDomNode &portReferenceNode)
    : portRef(), componentRef(), left(-1), right(-1)
{
    QDomNamedNodeMap attributes = portReferenceNode.attributes();

    portRef = attributes.namedItem("spirit:portRef").nodeValue();

    if (portReferenceNode.nodeName() == "spirit:internalPortReference")
	componentRef = attributes.namedItem("spirit:componentRef").nodeValue();

    if (attributes.contains("spirit:left"))
	left = attributes.namedItem("spirit:left").nodeValue().toInt();
    if (attributes.contains("spirit:right"))
	right = attributes.namedItem("spirit:right").nodeValue().toInt();
}

Design::PortRef::PortRef(QString portRef,
			 QString componentRef,
			 int left,
			 int right): 
portRef(portRef),
componentRef(componentRef),
left(left),
right(right)
{
    
}

Design::PortRef::PortRef( const PortRef& other ):
portRef(other.portRef),
componentRef(other.componentRef),
left(other.left),
right(other.right) {
}

Design::PortRef& Design::PortRef::operator=( const PortRef& other ) {
	if (this != &other) {
		portRef	= other.portRef;
		componentRef = other.componentRef;
		left = other.left;
		right = other.right;
	}
	return *this;
}

bool Design::PortRef::isValid( bool externalRef,
							  const QStringList& instanceNames, 
							  QStringList& errorList, 
							  const QString& parentIdentifier ) const {
	bool valid = true;

	// if this is internal refenrece then the component ref must exist
	if (!externalRef) {
		if (componentRef.isEmpty()) {
			errorList.append(QObject::tr("No component reference set for internal"
				" port reference within %1").arg(parentIdentifier));
			valid = false;
		}
		else if (!instanceNames.contains(componentRef)) {
			errorList.append(QObject::tr("The internal port reference in %1 "
				"contained a component reference to instance %2 that does not exist").arg(
				parentIdentifier).arg(componentRef));
			valid = false;
		}

		if (portRef.isEmpty()) {
			errorList.append(QObject::tr("No port reference set for internal port"
				" reference within %1").arg(parentIdentifier));
			valid = false;
		}
	}
	else {
		if (portRef.isEmpty()) {
			errorList.append(QObject::tr("No port reference set for external port"
				" reference within %1").arg(parentIdentifier));
			valid = false;
		}
	}

	return valid;
}

Design::AdHocConnection::AdHocConnection(QDomNode &adHocConnectionNode)
    : name(), displayName(), description(), tiedValue(),
      internalPortReferences(), externalPortReferences()
{
    for (int i = 0; i < adHocConnectionNode.childNodes().size(); i++) {
	QDomNode node = adHocConnectionNode.childNodes().at(i);

	if (node.nodeName() == "spirit:name") {
	    name = node.firstChild().nodeValue();
	} else if (node.nodeName() == "spirit:displayName") {
	    displayName = node.firstChild().nodeValue();
	} else if (node.nodeName() == "spirit:description") {
	    description = node.firstChild().nodeValue();
	} else if (node.nodeName() == "spirit:internalPortReference") {
	    internalPortReferences.append(PortRef(node));
	} else if (node.nodeName() == "spirit:externalPortReference") {
	    externalPortReferences.append(PortRef(node));
	}
    }

    QDomNamedNodeMap attributes = adHocConnectionNode.attributes();

    if (attributes.contains("spirit:tiedValue")) {
    	tiedValue = attributes.namedItem("spirit:tiedValue").nodeValue();
    }
}

Design::AdHocConnection::AdHocConnection(QString name,
					 QString displayName,
					 QString description,
					 QString tiedValue,
					 QList<PortRef> internalPortReferences,
					 QList<PortRef> externalPortReferences):
name(name), 
displayName(displayName), 
description(description),
tiedValue(tiedValue), 
internalPortReferences(internalPortReferences),
externalPortReferences(externalPortReferences)
{
}

Design::AdHocConnection::AdHocConnection( const AdHocConnection& other ):
name(other.name),
displayName(other.displayName),
description(other.description),
tiedValue(other.tiedValue),
internalPortReferences(other.internalPortReferences),
externalPortReferences(other.externalPortReferences) {
}

Design::AdHocConnection& Design::AdHocConnection::operator=( const AdHocConnection& other ) {
	if (this != &other) {
		name = other.name;
		displayName = other.displayName;
		description = other.description;
		tiedValue = other.tiedValue;
		internalPortReferences = other.internalPortReferences;
		externalPortReferences = other.externalPortReferences;
	}
	return *this;
}

bool Design::AdHocConnection::isValid( const QStringList& instanceNames, 
									  QStringList& errorList, 
									  const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("ad hoc connection %1").arg(name));

	if (name.isEmpty()) {
		errorList.append(QObject::tr("No name specified for ad hoc connection"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	if (internalPortReferences.isEmpty()) {
		errorList.append(QObject::tr("At least one internal port reference must be"
			" listed in %1 within %2").arg(thisIdentifier).arg(parentIdentifier));
		valid = false;
	}
	else {
		foreach (PortRef portRef, internalPortReferences) {
			if (!portRef.isValid(false, instanceNames, errorList, thisIdentifier)) {
				valid = false;
			}
		}
	}

	foreach (PortRef portRef, externalPortReferences) {
		if (!portRef.isValid(true, instanceNames, errorList, thisIdentifier)) {
			valid = false;
		}
	}

	return valid;
}
