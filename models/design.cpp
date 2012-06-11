// TODO: check datatypes

#include "design.h"

#include "XmlUtils.h"

#include <common/validators/vhdlNameValidator/vhdlnamevalidator.h>
#include <LibraryManager/vlnv.h>
#include "generaldeclarations.h"

#include <QDomNamedNodeMap>
#include <QXmlStreamWriter>

#include <QDebug>

//-----------------------------------------------------------------------------
// Function: Design::Design()
//-----------------------------------------------------------------------------
Design::Design(QDomDocument& doc)
    : LibraryComponent(doc), 
      componentInstances_(),
      swInstances_(),
      interconnections_(), 
      hierConnections_(),
      adHocConnections_(),
      portAdHocVisibilities_(),
      attributes_(),
      apiDependencies_(),
      hierApiDependencies_(),
      comConnections_(),
      hierComConnections_()
{
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

        if (node.nodeName() == "spirit:componentInstances")
        {
            QDomNodeList instanceNodes = node.childNodes();
            for (int i = 0; i < instanceNodes.size(); i++)
            {
                QDomNode instanceNode = instanceNodes.at(i);
                componentInstances_.append(ComponentInstance(instanceNode));
            }
        }
        else if (node.nodeName() == "spirit:interconnections")
        {
            QDomNodeList interconnectionNodes = node.childNodes();

            for (int i = 0; i < interconnectionNodes.size(); i++)
            {
                QDomNode interconnectionNode = interconnectionNodes.at(i);

                if (interconnectionNode.nodeName() == "spirit:interconnection")
                {
                    interconnections_.append(Interconnection(interconnectionNode));
                }
                else
                {
                    // TODO spirit::monitorInterconnection
                }
            }
        }
        else if (node.nodeName() == "spirit:adHocConnections")
        {
            QDomNodeList adHocNodes = node.childNodes();

            for (int i = 0; i < adHocNodes.size(); i++)
            {
                QDomNode adHocNode = adHocNodes.at(i);
                adHocConnections_.append(AdHocConnection(adHocNode));
            }
        }
        else if (node.nodeName() == "spirit:hierConnections")
        {
            QDomNodeList hierConnectionNodes = node.childNodes();

            for (int i = 0; i < hierConnectionNodes.size(); i++)
            {
                QDomNode hierConnectionNode = hierConnectionNodes.at(i);
                hierConnections_.append(HierConnection(hierConnectionNode));
            }
        }
        else if (node.nodeName() == "spirit:vendorExtensions")
        {
            parseVendorExtensions(node);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Design::Design()
//-----------------------------------------------------------------------------
Design::Design(const VLNV &vlnv)
    : LibraryComponent(vlnv), 
      componentInstances_(),
      swInstances_(),
      interconnections_(), 
      hierConnections_(),
      adHocConnections_(),
      portAdHocVisibilities_(),
      attributes_(),
      apiDependencies_(),
      hierApiDependencies_(),
      comConnections_(),
      hierComConnections_()
{
	LibraryComponent::vlnv_->setType(VLNV::DESIGN);
}

//-----------------------------------------------------------------------------
// Function: Design::Design()
//-----------------------------------------------------------------------------
Design::Design(Design const& other)
    : LibraryComponent(other),
      columns_(other.columns_),
      componentInstances_(other.componentInstances_),
      swInstances_(other.swInstances_),
      interconnections_(other.interconnections_),
      hierConnections_(other.hierConnections_),
      adHocConnections_(other.adHocConnections_),
      portAdHocVisibilities_(other.portAdHocVisibilities_),
      adHocPortPositions_(other.adHocPortPositions_),
      attributes_(other.attributes_),
      apiDependencies_(other.apiDependencies_),
      hierApiDependencies_(other.hierApiDependencies_),
      comConnections_(other.comConnections_),
      hierComConnections_(other.hierComConnections_)
{
}

//-----------------------------------------------------------------------------
// Function: Design::operator=()
//-----------------------------------------------------------------------------
Design& Design::operator=( const Design& other )
{
	if (this != &other) {
		LibraryComponent::operator=(other);
		
		columns_ = other.columns_;
		componentInstances_ = other.componentInstances_;
        swInstances_ = other.swInstances_;
		interconnections_ = other.interconnections_;
		hierConnections_ = other.hierConnections_;
		adHocConnections_ = other.adHocConnections_;
        portAdHocVisibilities_ = other.portAdHocVisibilities_;
        adHocPortPositions_ = other.adHocPortPositions_;
		attributes_ = other.attributes_;
        apiDependencies_ = other.apiDependencies_;
        hierApiDependencies_ = other.hierApiDependencies_;
        comConnections_ = other.comConnections_;
        hierComConnections_ = other.hierComConnections_;
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Design::~Design()
//-----------------------------------------------------------------------------
Design::~Design()
{
}

//-----------------------------------------------------------------------------
// Function: Design::close()
//-----------------------------------------------------------------------------
QSharedPointer<LibraryComponent> Design::clone() const {
	return QSharedPointer<LibraryComponent>(new Design(*this));
}

//-----------------------------------------------------------------------------
// Function: Design::write()
//-----------------------------------------------------------------------------
void Design::write(QFile& file)
{
	QXmlStreamWriter writer(&file);

	writer.setAutoFormatting(true);
	writer.setAutoFormattingIndent(-1);

	LibraryComponent::write(writer);

	// set the attributes
	setXMLNameSpaceAttributes(attributes_);

	// write the attributes for the spirit:designConfiguration element
	General::writeAttributes(writer, attributes_);

	LibraryComponent::writeVLNV(writer);

	if (!componentInstances_.isEmpty()) {
		writer.writeStartElement("spirit:componentInstances");

		foreach (ComponentInstance inst, componentInstances_) {
			writer.writeStartElement("spirit:componentInstance");

			writer.writeTextElement("spirit:instanceName",
				inst.instanceName);
			writer.writeTextElement("spirit:displayName",
				inst.displayName);
			writer.writeTextElement("spirit:description",
				inst.description);
			writer.writeEmptyElement("spirit:componentRef");
			General::writeVLNVAttributes(writer,
				&inst.componentRef);

			QMapIterator<QString, QString> i(inst.configurableElementValues);
			writer.writeStartElement("spirit:configurableElementValues");
			while (i.hasNext()) {
				i.next();

				writer.writeStartElement("spirit:configurableElementValue");
				writer.writeAttribute("spirit:referenceId", i.key());
				writer.writeCharacters(i.value());
				writer.writeEndElement();
			}

			writer.writeEndElement();

			// Write custom data to vendor extensions.
			writer.writeStartElement("spirit:vendorExtensions");

			// Write the component position.
            XmlUtils::writePosition(writer, inst.position);

			// Write the bus, API and COM interface positions.
            XmlUtils::writePositionsMap(writer, inst.portPositions, "kactus2:portPosition", "kactus2:busRef");
            XmlUtils::writePositionsMap(writer, inst.apiInterfacePositions, "kactus2:apiInterfacePosition", "kactus2:apiRef");
            XmlUtils::writePositionsMap(writer, inst.apiInterfacePositions, "kactus2:comInterfacePosition", "kactus2:comRef");            

            // Write the port ad-hoc visibilities.
            QMap<QString, bool> const& adHocVisibilities = inst.portAdHocVisibilities;
            QMap<QString, QPointF> const& adHocPortPositions = inst.adHocPortPositions;
            XmlUtils::writeAdHocVisibilities(writer, adHocVisibilities, adHocPortPositions);

			// Write the MCAPI node ID if specified.
			if (inst.mcapiNodeID != -1)
			{
				writer.writeEmptyElement("kactus2:mcapiNodeId");
				writer.writeAttribute("value", QString::number(inst.mcapiNodeID));
			}

			if (inst.endpointsExpanded)
			{
				writer.writeEmptyElement("kactus2:endpointsExpanded");
			}

			if (inst.imported)
			{
				writer.writeEmptyElement("kactus2:imported");
			}

            writer.writeStartElement("kactus2:propertyValues");

            // Write property values.
            QMapIterator<QString, QString> iter(inst.swPropertyValues);

            while (iter.hasNext())
            {
                iter.next();

                writer.writeEmptyElement("kactus2:propertyValue");
                writer.writeAttribute("kactus2:name", iter.key());
                writer.writeAttribute("kactus2:value", iter.value());
            }

            writer.writeEndElement(); // kactus2:propertyValues

			writer.writeEndElement(); // spirit:vendorExtensions
			writer.writeEndElement(); // spirit:componentInstance
		}

		writer.writeEndElement();
	}

	if (!interconnections_.isEmpty()) {
		writer.writeStartElement("spirit:interconnections");

		foreach (Interconnection inter, interconnections_) {
			writer.writeStartElement("spirit:interconnection");

			writer.writeTextElement("spirit:name",
				inter.name);
			writer.writeTextElement("spirit:displayName",
				inter.displayName);
			writer.writeTextElement("spirit:description",
				inter.description);

			writer.writeEmptyElement("spirit:activeInterface");
			writer.writeAttribute("spirit:componentRef",
				inter.interface1.componentRef);
			writer.writeAttribute("spirit:busRef",
				inter.interface1.busRef);

			writer.writeEmptyElement("spirit:activeInterface");
			writer.writeAttribute("spirit:componentRef",
				inter.interface2.componentRef);
			writer.writeAttribute("spirit:busRef",
				inter.interface2.busRef);

			writer.writeEndElement();
		}

		writer.writeEndElement();
	}

	if (!hierConnections_.isEmpty()) {
		writer.writeStartElement("spirit:hierConnections");

		foreach (HierConnection hier, hierConnections_) {
			writer.writeStartElement("spirit:hierConnection");
			writer.writeAttribute("spirit:interfaceRef",
				hier.interfaceRef);

			writer.writeEmptyElement("spirit:interface");
			writer.writeAttribute("spirit:componentRef",
				hier.interface_.componentRef);
			writer.writeAttribute("spirit:busRef",
				hier.interface_.busRef);

			// Write custom data to vendor extensions.
			writer.writeStartElement("spirit:vendorExtensions");

			XmlUtils::writePosition(writer, hier.position);
			XmlUtils::writeDirection(writer, hier.direction);

			if (!hier.route.empty())
			{
				writer.writeStartElement("kactus2:route");
                
                if (hier.offPage)
                {
                    writer.writeAttribute("kactus2:offPage", "true");
                }
                else
                {
                    writer.writeAttribute("kactus2:offPage", "false");
                }

				foreach (QPointF const& point, hier.route)
				{
					XmlUtils::writePosition(writer, point);
				}

				writer.writeEndElement();
			}

			writer.writeEndElement();

			writer.writeEndElement();
		}

		writer.writeEndElement();
	}

	if (!adHocConnections_.isEmpty()) {
		writer.writeStartElement("spirit:adHocConnections");

		foreach (AdHocConnection adhoc, adHocConnections_)
        {
			writer.writeStartElement("spirit:adHocConnection");

			writer.writeTextElement("spirit:name", adhoc.name);
			writer.writeTextElement("spirit:displayName", adhoc.displayName);
			writer.writeTextElement("spirit:description", adhoc.description);

			foreach (PortRef portRef, adhoc.internalPortReferences)
            {
				writer.writeEmptyElement("spirit:internalPortReference");

				writer.writeAttribute("spirit:componentRef", portRef.componentRef);
				writer.writeAttribute("spirit:portRef", portRef.portRef);

                if (portRef.left >= 0)
                {
                    writer.writeAttribute("spirit:left", QString::number(portRef.left));
                }

                if (portRef.right >= 0)
                {
                    writer.writeAttribute("spirit:right", QString::number(portRef.right));
                }
			}

			foreach (PortRef portRef, adhoc.externalPortReferences)
            {
				writer.writeEmptyElement("spirit:externalPortReference");

				writer.writeAttribute("spirit:portRef", portRef.portRef);

                if (portRef.left >= 0)
                {
                    writer.writeAttribute("spirit:left", QString::number(portRef.left));
                }

                if (portRef.right >= 0)
                {
                    writer.writeAttribute("spirit:right", QString::number(portRef.right));
                }
			}

			writer.writeEndElement();
		}

		writer.writeEndElement();
	}

	writer.writeStartElement("spirit:vendorExtensions");

    // Write SW instances if any.
    if (!swInstances_.empty())
    {
        writer.writeStartElement("kactus2:swInstances");

        foreach (SWInstance const& instance, swInstances_)
        {
            instance.write(writer);
        }
        
        writer.writeEndElement(); // kactus2:swInstances
    }

    // Write API dependencies if any.
    if (!apiDependencies_.empty())
    {
        writer.writeStartElement("kactus2:apiDependencies");

        foreach (ApiDependency const& dependency, apiDependencies_)
        {
            dependency.write(writer);
        }

        writer.writeEndElement(); // kactus2:apiDependencies
    }

    // Write hierarchical API dependencies if any.
    if (!hierApiDependencies_.empty())
    {
        writer.writeStartElement("kactus2:hierApiDependencies");

        foreach (HierApiDependency const& dependency, hierApiDependencies_)
        {
            dependency.write(writer);
        }

        writer.writeEndElement(); // kactus2:hierApiDependencies
    }

    // Write COM connections if any.
    if (!comConnections_.empty())
    {
        writer.writeStartElement("kactus2:comConnections");

        foreach (ComConnection const& conn, comConnections_)
        {
            conn.write(writer);
        }

        writer.writeEndElement(); // kactus2:comConnections
    }

    // Write hierarchical COM connections if any.
    if (!hierComConnections_.empty())
    {
        writer.writeStartElement("kactus2:hierComConnections");

        foreach (HierComConnection const& conn, hierComConnections_)
        {
            conn.write(writer);
        }

        writer.writeEndElement(); // kactus2:hierComConnections
    }

	if (!columns_.isEmpty())
	{
		writer.writeStartElement("kactus2:columnLayout");

		foreach (ColumnDesc const& columnDesc, columns_)
		{
            columnDesc.write(writer);
		}

		writer.writeEndElement(); // kactus2:columnLayout
	}

    writer.writeStartElement("kactus2:routes");

    foreach (Interconnection const& conn, interconnections_)
    {
        if (!conn.route.empty())
        {
            writer.writeStartElement("kactus2:route");
            writer.writeAttribute("kactus2:connRef", conn.name);

            if (conn.offPage)
            {
                writer.writeAttribute("kactus2:offPage", "true");
            }
            else
            {
                writer.writeAttribute("kactus2:offPage", "false");
            }

            foreach (QPointF const& point, conn.route)
            {
                XmlUtils::writePosition(writer, point);
            }

            writer.writeEndElement();
        }
    }

    foreach (AdHocConnection const& conn, adHocConnections_)
    {
        if (!conn.route.empty())
        {
            writer.writeStartElement("kactus2:route");
            writer.writeAttribute("kactus2:connRef", conn.name);

            if (conn.offPage)
            {
                writer.writeAttribute("kactus2:offPage", "true");
            }
            else
            {
                writer.writeAttribute("kactus2:offPage", "false");
            }

            foreach (QPointF const& point, conn.route)
            {
                XmlUtils::writePosition(writer, point);
            }

            writer.writeEndElement();
        }
    }

    writer.writeEndElement(); // kactus2:routes

    // Write the top-level component's port ad-hoc visibilities.
    XmlUtils::writeAdHocVisibilities(writer, portAdHocVisibilities_, adHocPortPositions_);

	writer.writeEndElement(); // kactus2:vendorExtensions

	writer.writeEndElement();
	writer.writeEndDocument();
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

    QStringList swInstanceNames;

    foreach (SWInstance const& instance, swInstances_)
    {
        if (swInstanceNames.contains(instance.getInstanceName()))
        {
            errorList.append(QObject::tr("Design contains several SW instances "
                                         "with name %1").arg(instance.getInstanceName()));
            valid = false;
        }
        else
        {
            swInstanceNames.append(instance.getInstanceName());
        }

        if (!instance.isValid(errorList, instanceNames, thisIdentifier))
        {
            valid = false;
        }
    }

	QStringList interconnectionNames;
	foreach (Design::Interconnection interconnection, interconnections_) {
		
		// if there are several interconnections with same name
		if (interconnectionNames.contains(interconnection.name)) {
			errorList.append(QObject::tr("Design contains several interconnections"
				" with name '%1'").arg(interconnection.name));
			valid = false;
		}
		else {
			interconnectionNames.append(interconnection.name);
		}

		if (!interconnection.isValid(instanceNames, errorList, thisIdentifier)) {
			valid = false;
		}
	}

    QStringList comConnectionNames;
    QStringList allInstanceNames = instanceNames + swInstanceNames;

    foreach (ComConnection const& conn, comConnections_)
    {
        if (comConnectionNames.contains(conn.getName()))
        {
            errorList.append(QObject::tr("Design contains several COM connections "
                                         "with name '%1'").arg(conn.getName()));
            valid = false;
        }
        else
        {
            comConnectionNames.append(conn.getName());
        }

        if (!conn.isValid(errorList, allInstanceNames, thisIdentifier))
        {
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

bool Design::isValid() const {
	if (!vlnv_) {
		return false;
	}
	else if (!vlnv_->isValid()) {
		return false;
	}

	QStringList instanceNames;
	foreach (Design::ComponentInstance instance, componentInstances_) {

		// if there are several instances with same name
		if (instanceNames.contains(instance.instanceName)) {
			return false;
		}
		else {
			instanceNames.append(instance.instanceName);
		}

		if (!instance.isValid()) {
			return false;
		}
	}

	QStringList interconnectionNames;
	foreach (Design::Interconnection interconnection, interconnections_) {

		// if there are several interconnections with same name
		if (interconnectionNames.contains(interconnection.name)) {
			return false;
		}
		else {
			interconnectionNames.append(interconnection.name);
		}

		if (!interconnection.isValid(instanceNames)) {
			return false;
		}
	}

	QStringList adHocNames;
	foreach (Design::AdHocConnection adHoc, adHocConnections_) {
		if (adHocNames.contains(adHoc.name)) {
			return false;
		}
		else {
			adHocNames.append(adHoc.name);
		}

		if (!adHoc.isValid(instanceNames)) {
			return false;
		}
	}

	foreach (Design::HierConnection hierConn, hierConnections_) {
		if (!hierConn.isValid(instanceNames)) {
			return false;
		}
	}

	return true;
}

QList<Design::ComponentInstance> const& Design::getComponentInstances() const
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
                    QString offPageValue = connNode.attributes().namedItem("kactus2:offPage").nodeValue();
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

					// Apply the route to the correct interconnection or ad-hoc connection.
                    bool found = false;

					for (int i = 0; i < interconnections_.size(); ++i)
					{
						if (interconnections_[i].name == name)
						{
							interconnections_[i].route = route;
                            interconnections_[i].offPage = offPageValue == "true";
                            found = true;
							break;
						}
					}

                    if (!found)
                    {
                        for (int i = 0; i < adHocConnections_.size(); ++i)
                        {
                            if (adHocConnections_[i].name == name)
                            {
                                adHocConnections_[i].route = route;
                                adHocConnections_[i].offPage = offPageValue == "true";
                                break;
                            }
                        }
                    }
				}
			}
		}
        else if (childNode.nodeName() == "kactus2:adHocVisibilities")
        {
            for (int i = 0; i < childNode.childNodes().size(); ++i)
            {
                QDomNode adHocNode = childNode.childNodes().at(i);

                if (adHocNode.nodeName() == "kactus2:adHocVisible")
                {
                    QString name = adHocNode.attributes().namedItem("portName").nodeValue();
                    portAdHocVisibilities_[name] = true;

                    QPointF pos;
                    pos.setX(adHocNode.attributes().namedItem("x").nodeValue().toInt());
                    pos.setY(adHocNode.attributes().namedItem("y").nodeValue().toInt());

                    adHocPortPositions_[name] = pos;
                }
            }
        }
        else if (childNode.nodeName() == "kactus2:swInstances")
        {
            for (int j = 0; j < childNode.childNodes().count(); ++j)
            {
                QDomNode swNode = childNode.childNodes().at(j);

                if (swNode.nodeName() == "kactus2:swInstance")
                {
                    swInstances_.append(SWInstance(swNode));
                }
            }
        }
        else if (childNode.nodeName() == "kactus2:apiDependencies")
        {
            for (int j = 0; j < childNode.childNodes().count(); ++j)
            {
                QDomNode apiNode = childNode.childNodes().at(j);

                if (apiNode.nodeName() == "kactus2:apiDependency")
                {
                    apiDependencies_.append(ApiDependency(apiNode));
                }
            }
        }
        else if (childNode.nodeName() == "kactus2:hierApiDependencies")
        {
            for (int j = 0; j < childNode.childNodes().count(); ++j)
            {
                QDomNode apiNode = childNode.childNodes().at(j);

                if (apiNode.nodeName() == "kactus2:hierApiDependency")
                {
                    hierApiDependencies_.append(HierApiDependency(apiNode));
                }
            }
        }
        else if (childNode.nodeName() == "kactus2:comConnections")
        {
            for (int j = 0; j < childNode.childNodes().count(); ++j)
            {
                QDomNode comNode = childNode.childNodes().at(j);

                if (comNode.nodeName() == "kactus2:comConnection")
                {
                    comConnections_.append(ComConnection(comNode));
                }
            }
        }
        else if (childNode.nodeName() == "kactus2:hierComConnections")
        {
            for (int j = 0; j < childNode.childNodes().count(); ++j)
            {
                QDomNode comNode = childNode.childNodes().at(j);

                if (comNode.nodeName() == "kactus2:hierComConnection")
                {
                    hierComConnections_.append(HierComConnection(comNode));
                }
            }
        }
	}
}

//-----------------------------------------------------------------------------
// Function: Design::getPortAdHocVisibilities()
//-----------------------------------------------------------------------------
QMap<QString, bool> const& Design::getPortAdHocVisibilities() const
{
    return portAdHocVisibilities_;
}

//-----------------------------------------------------------------------------
// Function: Design::getAdHocPortPositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> const& Design::getAdHocPortPositions() const
{
    return adHocPortPositions_;
}

//-----------------------------------------------------------------------------
// Function: getColumns()
//-----------------------------------------------------------------------------
QList<ColumnDesc> const& Design::getColumns() const
{
	return columns_;
}

//-----------------------------------------------------------------------------
// Function: Design::setPortAdHocVisibilities()
//-----------------------------------------------------------------------------
void Design::setPortAdHocVisibilities(QMap<QString, bool> const& portAdHocVisibilities)
{
    portAdHocVisibilities_ = portAdHocVisibilities;
}

//-----------------------------------------------------------------------------
// Function: Design::setAdHocPortPositions()
//-----------------------------------------------------------------------------
void Design::setAdHocPortPositions(QMap<QString, QPointF> const& val)
{
    adHocPortPositions_ = val;
}

//-----------------------------------------------------------------------------
// Function: setColumns()
//-----------------------------------------------------------------------------
void Design::setColumns(QList<ColumnDesc> const& columns)
{
	columns_ = columns;
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
// Function: Design::setApiDependencies()
//-----------------------------------------------------------------------------
void Design::setApiDependencies(QList<ApiDependency> const& apiDependencies)
{
    apiDependencies_ = apiDependencies;
}

//-----------------------------------------------------------------------------
// Function: Design::setApiDependencies()
//-----------------------------------------------------------------------------
void Design::setHierApiDependencies(QList<HierApiDependency> const& hierApiDependencies)
{
    hierApiDependencies_ = hierApiDependencies;
}

//-----------------------------------------------------------------------------
// Function: Design::setComConnections()
//-----------------------------------------------------------------------------
void Design::setComConnections(QList<ComConnection> const& comConnections)
{
    comConnections_ = comConnections;
}

//-----------------------------------------------------------------------------
// Function: Design::setHierComConnections()
//-----------------------------------------------------------------------------
void Design::setHierComConnections(QList<HierComConnection> const& hierComConnections)
{
    hierComConnections_ = hierComConnections;
}

//-----------------------------------------------------------------------------
// Function: Design::getSWInstances()
//-----------------------------------------------------------------------------
QList<SWInstance> const& Design::getSWInstances() const
{
    return swInstances_;
}

//-----------------------------------------------------------------------------
// Function: Design::setSWInstances()
//-----------------------------------------------------------------------------
void Design::setSWInstances(QList<SWInstance> const& swInstances)
{
    swInstances_ = swInstances;
}

//-----------------------------------------------------------------------------
// Function: Design::getApiDependencies()
//-----------------------------------------------------------------------------
QList<ApiDependency> const& Design::getApiDependencies() const
{
    return apiDependencies_;
}

//-----------------------------------------------------------------------------
// Function: Design::getHierApiDependencies()
//-----------------------------------------------------------------------------
QList<HierApiDependency> const& Design::getHierApiDependencies() const
{
    return hierApiDependencies_;
}

//-----------------------------------------------------------------------------
// Function: Design::getComConnections()
//-----------------------------------------------------------------------------
QList<ComConnection> const& Design::getComConnections() const
{
    return comConnections_;
}

//-----------------------------------------------------------------------------
// Function: Design::getHierComConnections()
//-----------------------------------------------------------------------------
QList<HierComConnection> const& Design::getHierComConnections() const
{
    return hierComConnections_;
}

Design::ComponentInstance::ComponentInstance(
    QDomNode& componentInstanceNode)
    : instanceName(), displayName(), description(), componentRef(),
      configurableElementValues(), portPositions(), portAdHocVisibilities(),
      adHocPortPositions(), mcapiNodeID(-1), endpointsExpanded(false), imported(false),
      swPropertyValues()
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
        } else if (node.nodeName() == "spirit:vendorExtensions")
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
                else if (childNode.nodeName() == "kactus2:portPositions")
                {
                    XmlUtils::parsePositionsMap(childNode, "kactus2:portPosition",
                                                "kactus2:busRef", portPositions);
                }
                else if (childNode.nodeName() == "kactus2:apiInterfacePositions")
                {
                    XmlUtils::parsePositionsMap(childNode, "kactus2:apiInterfacePosition",
                                                "kactus2:apiRef", apiInterfacePositions);
                }
                else if (childNode.nodeName() == "kactus2:comInterfacePositions")
                {
                    XmlUtils::parsePositionsMap(childNode, "kactus2:comInterfacePosition",
                                                "kactus2:comRef", comInterfacePositions);
                }
                else if (childNode.nodeName() == "kactus2:adHocVisibilities")
                {
                    XmlUtils::parseAdHocVisibilities(childNode, portAdHocVisibilities, adHocPortPositions);
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
                else if (childNode.nodeName() == "kactus2:propertyValues")
                {
                    parsePropertyValues(childNode);
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
      configurableElementValues(), position(position), portAdHocVisibilities(),
      adHocPortPositions(), mcapiNodeID(-1), endpointsExpanded(false), imported(false)
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
portAdHocVisibilities(other.portAdHocVisibilities),
adHocPortPositions(other.adHocPortPositions),
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
        portAdHocVisibilities = other.portAdHocVisibilities;
        adHocPortPositions = other.adHocPortPositions;
        mcapiNodeID = other.mcapiNodeID;
        endpointsExpanded = other.endpointsExpanded;
        imported = other.imported;
	}
	return *this;
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

	// if the instance name contains characters that are not allowed in vhdl
	VhdlNameValidator nameValidator;
	int pos = 0;
	QString instName(instanceName);
	if (nameValidator.validate(instName, pos) != QValidator::Acceptable) {
		errorList.append(QObject::tr("The instance name %1 contains illegal "
			"characters.").arg(instName));
		valid = false;
	}

	if (!componentRef.isValid(errorList, thisIdentifier)) {
		valid = false;
	}

	for (QMap<QString, QString>::const_iterator i = configurableElementValues.begin();
		i != configurableElementValues.end(); ++i) {

			if (i.key().isEmpty()) {
				errorList.append(QObject::tr("No reference id set for configurable"
					" element value in %1 within %2").arg(thisIdentifier).arg(
					parentIdentifier));
				valid = false;
			}
			if (i.value().isEmpty()) {
				errorList.append(QObject::tr("No configurable element value set"
					" for %1 within %2").arg(thisIdentifier).arg(parentIdentifier));
				valid = false;
			}
	}

	return valid;
}

bool Design::ComponentInstance::isValid() const {
	if (instanceName.isEmpty()) {
		return false;
	}

	// if the instance name contains characters that are not allowed in vhdl
	VhdlNameValidator nameValidator;
	int pos = 0;
	QString instName(instanceName);
	if (nameValidator.validate(instName, pos) != QValidator::Acceptable) {
		return false;
	}

	if (!componentRef.isValid()) {
		return false;
	}

	for (QMap<QString, QString>::const_iterator i = configurableElementValues.begin();
		i != configurableElementValues.end(); ++i) {

			if (i.key().isEmpty()) {
				return false;
			}
			if (i.value().isEmpty()) {
				return false;
			}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Function: Design::ComponentInstance::parsePropertyValues()
//-----------------------------------------------------------------------------
void Design::ComponentInstance::parsePropertyValues(QDomNode& node)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode propNode = node.childNodes().at(i);

        if (propNode.nodeName() == "kactus2:propertyValue")
        {
            QString name = propNode.attributes().namedItem("kactus2:name").nodeValue();
            QString value = propNode.attributes().namedItem("kactus2:value").nodeValue();

            swPropertyValues.insert(name, value);
        }
    }
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

bool Design::Interface::isValid( const QStringList& instanceNames ) const {
	if (componentRef.isEmpty()) {
		return false;
	}
	else if (!instanceNames.contains(componentRef)) {
		return false;
	}

	if (busRef.isEmpty()) {
		return false;
	}

	return true;
}

Design::Interconnection::Interconnection(QDomNode &interconnectionNode)
    : name(), displayName(), description(),
      interface1(QString(), QString()), interface2(QString(), QString()), route(), offPage(false)
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
                     bool offPage,
					 QString displayName,
					 QString description)
    : name(name), displayName(displayName), description(description),
      interface1(interface1), interface2(interface2), route(route), offPage(offPage)
{
}

Design::Interconnection::Interconnection( const Interconnection& other ):
name(other.name),
displayName(other.displayName),
description(other.description),
interface1(other.interface1),
interface2(other.interface2),
route(other.route),
offPage(other.offPage)
{
}

Design::Interconnection& Design::Interconnection::operator=( const Interconnection& other ) {
	if (this != &other) {
		name = other.name;
		displayName = other.displayName;
		description = other.description;
		interface1 = other.interface1;
		interface2 = other.interface2;
		route = other.route;
        offPage = other.offPage;
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

bool Design::Interconnection::isValid( const QStringList& instanceNames ) const {
	if (name.isEmpty()) {
		return false;
	}

	if (!interface1.isValid(instanceNames)) {
		return false;
	}

	if (!interface2.isValid(instanceNames)) {
		return false;
	}
	return true;
}

Design::HierConnection::HierConnection(QDomNode &hierConnectionNode)
    : interfaceRef(), interface_(QString(""), QString("")), position(), route(), offPage(false)
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
                    offPage = childNode.attributes().namedItem("kactus2:offPage").nodeValue() == "true";

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
                                       QList<QPointF> const& route,
                                       bool offPage)
    : interfaceRef(interfaceRef_), interface_(interface_),
      position(position), direction(direction), route(route), offPage(offPage)
{
}

Design::HierConnection::HierConnection( const HierConnection& other ):
interfaceRef(other.interfaceRef),
interface_(other.interface_),
position(other.position),
direction(other.direction),
route(other.route),
offPage(other.offPage) {
}

Design::HierConnection& Design::HierConnection::operator=( const HierConnection& other ) {
	if (this != &other) {
		interfaceRef = other.interfaceRef;
		interface_ = other.interface_;
		position = other.position;
		direction = other.direction;
		route = other.route;
        offPage = other.offPage;
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

bool Design::HierConnection::isValid( const QStringList& instanceNames ) const {
	if (interfaceRef.isEmpty()) {
		return false;
	}

	if (!interface_.isValid(instanceNames)) {
		return false;
	}

	return true;
}

Design::PortRef::PortRef(QDomNode &portReferenceNode)
    : portRef(), componentRef(), left(-1), right(-1)
{
    QDomNamedNodeMap attributes = portReferenceNode.attributes();

    portRef = attributes.namedItem("spirit:portRef").nodeValue();

    if (portReferenceNode.nodeName() == "spirit:internalPortReference")
    {
	    componentRef = attributes.namedItem("spirit:componentRef").nodeValue();
    }

    if (attributes.contains("spirit:left"))
    {
	    left = attributes.namedItem("spirit:left").nodeValue().toInt();
    }

    if (attributes.contains("spirit:right"))
    {
	    right = attributes.namedItem("spirit:right").nodeValue().toInt();
    }
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

	// if this is internal reference then the component ref must exist
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

bool Design::PortRef::isValid( bool externalRef, const QStringList& instanceNames ) const {
	// if this is internal reference then the component ref must exist
	if (!externalRef) {
		if (componentRef.isEmpty()) {
			return false;
		}
		else if (!instanceNames.contains(componentRef)) {
			return false;
		}

		if (portRef.isEmpty()) {
			return false;
		}
	}
	else {
		if (portRef.isEmpty()) {
			return false;
		}
	}
	return true;
}

Design::AdHocConnection::AdHocConnection(QDomNode &adHocConnectionNode)
    : name(), displayName(), description(), tiedValue(),
      internalPortReferences(), externalPortReferences(),
      route(), offPage(false)
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
					 QList<PortRef> externalPortReferences,
                     QList<QPointF> const& route, bool offPage):
name(name), 
displayName(displayName), 
description(description),
tiedValue(tiedValue), 
internalPortReferences(internalPortReferences),
externalPortReferences(externalPortReferences),
route(route), offPage(offPage)
{
}

Design::AdHocConnection::AdHocConnection( const AdHocConnection& other ):
name(other.name),
displayName(other.displayName),
description(other.description),
tiedValue(other.tiedValue),
internalPortReferences(other.internalPortReferences),
externalPortReferences(other.externalPortReferences),
route(other.route), offPage(other.offPage) {
}

Design::AdHocConnection& Design::AdHocConnection::operator=( const AdHocConnection& other ) {
	if (this != &other) {
		name = other.name;
		displayName = other.displayName;
		description = other.description;
		tiedValue = other.tiedValue;
		internalPortReferences = other.internalPortReferences;
		externalPortReferences = other.externalPortReferences;
        route = other.route;
        offPage = other.offPage;
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

bool Design::AdHocConnection::isValid( const QStringList& instanceNames ) const {
	if (name.isEmpty()) {
		return false;
	}

	if (internalPortReferences.isEmpty()) {
		return false;
	}
	else {
		foreach (PortRef portRef, internalPortReferences) {
			if (!portRef.isValid(false, instanceNames)) {
				return false;
			}
		}
	}

	foreach (PortRef portRef, externalPortReferences) {
		if (!portRef.isValid(true, instanceNames)) {
			return false;
		}
	}

	return true;
}
