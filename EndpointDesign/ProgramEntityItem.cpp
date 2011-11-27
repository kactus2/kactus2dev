//-----------------------------------------------------------------------------
// File: ProgramEntityItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 26.2.2011
//
// Description:
// Graphics item for the collection of endpoints and a linked application.
//-----------------------------------------------------------------------------

#include "ProgramEntityItem.h"

#include "EndpointConnection.h"
#include "SystemMoveCommands.h"

#include <common/DiagramUtil.h>
#include <common/layouts/VStackedLayout.h>
#include <common/GenericEditProvider.h>

#include <models/component.h>
#include <models/model.h>

#include <MCAPI/MCAPIContentMatcher.h>
#include <MCAPI/MCAPITextEdit.h>
#include <MCAPI/MCAPIDesignerTypes.h>

#include "EndpointDesignDiagram.h"
#include "EndpointItem.h"
#include "EndpointStack.h"
#include "EndpointConnection.h"
#include "AppPlaceholderItem.h"
#include "ApplicationItem.h"
#include "MappingComponentItem.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QFileInfo>

#include <stdexcept>

//-----------------------------------------------------------------------------
// Function: ProgramEntityItem()
//-----------------------------------------------------------------------------
ProgramEntityItem::ProgramEntityItem(QSharedPointer<Component> component,
                                     QString const& instanceName,
                                     QString const& displayName,
                                     QString const& description,
                                     QMap<QString, QString> const& configurableElementValues)
    : SWComponentItem(QRectF(-WIDTH / 2, 0, WIDTH, TOP_MARGIN + BOTTOM_MARGIN),
                      component, instanceName, displayName, description, configurableElementValues),
      endpointStack_(0), appPlaceholder_(0), appItem_(0), importedIcon_(0)
{
    // Set basic graphics properties.
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);
    
    // Create the application placeholder item.
    appPlaceholder_ = new AppPlaceholderItem(this);
    appPlaceholder_->setPos(0, 40);

    // Create the endpoint stack.
    endpointStack_ = new EndpointStack(this);
    connect(endpointStack_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    endpointStack_->setPos(0, TOP_MARGIN);

    if (componentModel()->getVlnv()->isValid())
    {
        endpointStack_->setEditable(false);
    }

    // Load the existing endpoints from the component.
    QMap< QString, QSharedPointer<ModelParameter> > const& endpointDefinitions =
        componentModel()->getModel()->getModelParameters();

    QMapIterator< QString, QSharedPointer<ModelParameter> > curEndpointDef(endpointDefinitions);

    while (curEndpointDef.hasNext())
    {
        QSharedPointer<ModelParameter const> endpointDef = *curEndpointDef.next();

        MCAPIDataType type = MCAPI_TYPE_MESSAGE;
		stringToValue(endpointDef->getDataType(), type);

        MCAPIEndpointDirection dir = MCAPI_ENDPOINT_IN;
        stringToValue(endpointDef->getValue(), dir);

        endpointStack_->addEndpoint(endpointDef->getName(), dir, type);
    }

    // Determine the correct height for the node.
    onEndpointStackChange(endpointStack_->getVisibleHeight());
    connect(endpointStack_, SIGNAL(visibleHeightChanged(int)), this, SLOT(onEndpointStackChange(int)));

    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: ~ProgramEntityItem()
//-----------------------------------------------------------------------------
ProgramEntityItem::~ProgramEntityItem()
{
}

//-----------------------------------------------------------------------------
// Function: getContentMatcher()
//-----------------------------------------------------------------------------
QSharedPointer<MCAPIContentMatcher> ProgramEntityItem::getContentMatcher() const
{
    // Fill the matcher with current information.
    QSharedPointer<MCAPIContentMatcher> matcher(new MCAPIContentMatcher());

    // Set local node name and add endpoints.
    matcher->setLocalNodeName(name().toUpper());

    foreach (EndpointItem* endpoint, endpointStack_->getEndpoints())
    {
        matcher->addEndpoint(endpoint->getName(), endpoint->getMCAPIDirection(),
                             name().toUpper(), endpoint->getName().toUpper() + "_PORT_ID");

        // If the endpoint has a connection, add the remote endpoint
        // and the actual connection to the content matcher.
        if (endpoint->isConnected())
        {
            // Determine the remote endpoint item.
            EndpointItem* remoteEndpoint = endpoint->getConnections().first()->getEndpoint2();

            if (remoteEndpoint == endpoint)
            {
                remoteEndpoint = endpoint->getConnections().first()->getEndpoint1();
            }
            
            matcher->addEndpoint(remoteEndpoint->getName(), remoteEndpoint->getMCAPIDirection(),
                                 remoteEndpoint->getParentProgramEntity()->name().toUpper(),
                                 remoteEndpoint->getName().toUpper() + "_PORT_ID");
            matcher->addConnection(endpoint->getName(), remoteEndpoint->getName(),
                                   endpoint->getConnectionType());
        }
    }

    return matcher;
}

//-----------------------------------------------------------------------------
// Function: type()
//-----------------------------------------------------------------------------
int ProgramEntityItem::type() const
{
    return Type;
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void ProgramEntityItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mousePressEvent(event);
    setZValue(1001.0);

    oldPos_ = pos();

    // Begin the position update for the connections.
    foreach (EndpointItem* endpoint, getEndpoints())
    {
        foreach (EndpointConnection* conn, endpoint->getConnections())
        {
            if (!conns_.contains(conn))
            {
                conn->beginUpdatePosition();
                conns_.insert(conn);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void ProgramEntityItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<EndpointDesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    QGraphicsItem::mouseMoveEvent(event);
    setZValue(1001.0);

    if (getMappingComponent() != 0)
    {
        getMappingComponent()->onMoveItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void ProgramEntityItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    setZValue(0.0);

    if (getMappingComponent() != 0)
    {
        getMappingComponent()->onReleaseItem(this);

        QSharedPointer<QUndoCommand> cmd;

        if (scenePos() != oldPos_)
        {
            cmd = QSharedPointer<QUndoCommand>(new ProgramEntityMoveCommand(this, oldPos_));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // End the position update for the interconnections.
        foreach (EndpointConnection* conn, conns_)
        {
            conn->endUpdatePosition(cmd.data());
        }

        conns_.clear();

        // Add the undo command to the edit stack only if it has at least some real changes.
        if (cmd->childCount() > 0 || pos() != oldPos_)
        {
            static_cast<EndpointDesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: getSourceFile()
//-----------------------------------------------------------------------------
QString const& ProgramEntityItem::getSourceFile() const
{
    throw std::runtime_error("Unimplemented");
}

//-----------------------------------------------------------------------------
// Function: onEndpointStackChange()
//-----------------------------------------------------------------------------
void ProgramEntityItem::onEndpointStackChange(int height)
{
    setRect(-WIDTH / 2, 0, WIDTH, TOP_MARGIN + height + BOTTOM_MARGIN);

    if (getMappingComponent() != 0)
    {
        getMappingComponent()->updateSize();
    }
}

//-----------------------------------------------------------------------------
// Function: createSource()
//-----------------------------------------------------------------------------
void ProgramEntityItem::createSource(QString const& filename)
{
    // Open the file for writing.
    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    // Create the indentation string.
    QString indent = createIndentString();

    // Start writing the file.
    QTextStream stream(&file);

    // Write the file header comment.
    stream << "/*!\n"
              " *  File: " + QFileInfo(filename).fileName() + "\n"
              " *\n"
              " *  Application code for " + name() + ".\n"
              " */" << endl << endl;

    // Write the #includes.
    stream << "#include <stdlib.h>" << endl;
    stream << "#include <stdio.h>" << endl;
    stream << "#include <mcapi.h>" << endl << endl;

    // Write the Kactus 2 generated MCAPI code.
    generateCode(stream, indent);

    // Write the main() function.
    stream << "int main(int argc, char* argv[])" << endl << "{" << endl
           << indent << "// Initialize the MCAPI." << endl
           << indent << "if (initializeMCAPI() != 0)" << endl
           << indent << "{" << endl
           << indent << indent << "// TODO: Write error handling code." << endl
           << indent << indent << "return EXIT_FAILURE;" << endl
           << indent << "}" << endl << endl
           << indent << "// TODO: Write your application code here." << endl << endl
           << indent << "// Finalize MCAPI before exiting." << endl
           << indent << "mcapi_finalize(&status);" << endl
           << indent << "return EXIT_SUCCESS;" << endl << "}" << endl;

    file.close();
}

//-----------------------------------------------------------------------------
// Function: generateCode()
//-----------------------------------------------------------------------------
void ProgramEntityItem::generateCode(QTextStream& stream, QString const& indent)
{
    // Retrieve the list of connected remote nodes and endpoints.
    QList<ProgramEntityItem*> remoteNodes;
    QList<EndpointItem*> remoteEndpoints;

    foreach (EndpointItem* endpoint, endpointStack_->getEndpoints())
    {
        if (endpoint->isConnected())
        {
            // Determine the remote endpoint item.
            EndpointItem* remoteEndpoint = endpoint->getConnections().first()->getEndpoint2();

            if (remoteEndpoint == endpoint)
            {
                remoteEndpoint = endpoint->getConnections().first()->getEndpoint1();
            }

            remoteEndpoints.append(remoteEndpoint);

            if (!remoteNodes.contains(remoteEndpoint->getParentProgramEntity()))
            {
                remoteNodes.append(remoteEndpoint->getParentProgramEntity());
            }
        }
    }

    // Write the Kactus 2 generated code.
    stream << "//-----------------------------------------------" << endl
           << "// Kactus 2 generated code begin - do not modify!" << endl << endl;

    // Write the node local node ids.
    stream << "// Local node ID." << endl
           << "const mcapi_node_t " << name().toUpper() << " = " << getMappingComponent()->getID() << ";" << endl << endl;

    // Write the remote node IDs.
    stream << "// Remote node IDs." << endl;

    foreach (ProgramEntityItem* node, remoteNodes)
    {
        stream << "const mcapi_node_t " << node->name().toUpper()
               << " = " << node->getMappingComponent()->getID() << ";" << endl;
    }

    // Write the local port IDs.
    stream << endl << "// Local port IDs." << endl;

    foreach (EndpointItem* endpoint, endpointStack_->getEndpoints())
    {
        stream << "const mcapi_port_t " << endpoint->getName().toUpper()
            << "_PORT_ID = " << endpoint->getPortID() << ";" << endl;
    }

    // Write the remote port IDs.
    stream << endl << "// Remote port IDs." << endl;

    foreach (EndpointItem* endpoint, remoteEndpoints)
    {
        stream << "const mcapi_port_t " << endpoint->getName().toUpper()
            << "_PORT_ID = " << endpoint->getPortID() << ";" << endl;
    }

    // Write the local endpoint variables.
    stream << endl << "// Local endpoints." << endl;

    foreach (EndpointItem* endpoint, endpointStack_->getEndpoints())
    {
        stream << "mcapi_endpoint_t " << endpoint->getName() << ";" << endl;
    }

    // Write the remote endpoint variables.
    stream << endl << "// Remote endpoints." << endl;

    foreach (EndpointItem* endpoint, remoteEndpoints)
    {
        stream << "mcapi_endpoint_t " << endpoint->getName() << ";" << endl;
    }

    // Write the other variables.
    stream << endl << "// Other variables." << endl
           << "mcapi_status_t status;" << endl << endl;

    // Write the initialization function.
    stream << "// Initializes the MCAPI system and endpoints." << endl
           << "// Returns zero if successful. In case of an error -1 is returned." << endl
           << "int initializeMCAPI()" << endl << "{" << endl
           << indent << "mcapi_version_t version;" << endl << endl
           << indent << "// Initialize the MCAPI implementation." << endl
           << indent << "mcapi_initialize(" << name().toUpper() << ", &version, &status);" << endl << endl
           << indent << "if (status != MCAPI_SUCCESS)" << endl
           << indent << "{" << endl << indent << indent << "return -1;" << endl
           << indent << "}" << endl << endl;

    // Write function calls to create local endpoints.
    stream << indent << "// Create local endpoints." << endl;

    foreach (EndpointItem* endpoint, endpointStack_->getEndpoints())
    {
        stream << indent << endpoint->getName() << " = mcapi_create_endpoint("
               << endpoint->getName().toUpper() << "_PORT_ID, &status);" << endl << endl
               << indent << "if (status != MCAPI_SUCCESS)" << endl
               << indent << "{" << endl << indent << indent << "return -1;" << endl
               << indent << "}" << endl << endl;
    }

    // Write function call to retrieve remote endpoints.
    stream << indent << "// Retrieve remote endpoints." << endl;

    foreach (EndpointItem* endpoint, remoteEndpoints)
    {
        stream << indent << endpoint->getName() << " = mcapi_get_endpoint("
               << endpoint->getParentProgramEntity()->name().toUpper()
               << ", " << endpoint->getName().toUpper() << "_PORT_ID, &status);" << endl << endl
               << indent << "if (status != MCAPI_SUCCESS)" << endl
               << indent << "{" << endl << indent << indent << "return -1;" << endl
               << indent << "}" << endl << endl;
    }

    stream << indent << "return 0;" << endl
           << "}" << endl << endl;

    stream << "// Kactus 2 generated code end" << endl
           << "//-----------------------------------------------" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: indent()
//-----------------------------------------------------------------------------
QString ProgramEntityItem::createIndentString()
{
    QSettings settings;

    // Read indentation settings.
    MCAPITextEdit::IndentStyle style =
        static_cast<MCAPITextEdit::IndentStyle>(settings.value("editor/indentStyle",
                                                               MCAPITextEdit::INDENT_STYLE_SPACES).toInt());
    unsigned int width = settings.value("editor/indentWidth", 4).toInt();

    QString indent;

    if (style == MCAPITextEdit::INDENT_STYLE_SPACES)
    {
        indent.fill(' ', width);
    }
    else
    {
        indent = "\t";
    }

    return indent;
}

//-----------------------------------------------------------------------------
// Function: setApplication()
//-----------------------------------------------------------------------------
void ProgramEntityItem::setApplication(ApplicationItem* item)
{
    appItem_ = item;

    if (appItem_ != 0)
    {
        appItem_->setParentItem(this);
        appItem_->setPos(appPlaceholder_->pos());
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: getMappingComponent()
//-----------------------------------------------------------------------------
MappingComponentItem* ProgramEntityItem::getMappingComponent()
{
    return dynamic_cast<MappingComponentItem*>(parentItem());
}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
void ProgramEntityItem::updateComponent()
{
    SWComponentItem::updateComponent();

    VLNV* vlnv = componentModel()->getVlnv();

    // Check whether the component is packaged (valid vlnv) or not.
    if (vlnv->isValid())
    {
        setBrush(QBrush(QColor(0xce,0xdf,0xff)));
    }
    else
    {
        setBrush(QBrush(QColor(217, 217, 217)));
    }

    // Show an icon if this is an imported SW component.
    if (isImported())
    {
        if (importedIcon_ == 0)
        {
            importedIcon_ = new QGraphicsPixmapItem(QPixmap(":icons/graphics/imported.png"), this);
            importedIcon_->setToolTip(tr("Imported SW"));
            importedIcon_->setPos(100, 6);
        }
    }
    else if (importedIcon_ != 0)
    {
        delete importedIcon_;
    }
}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
ApplicationItem* ProgramEntityItem::getApplication()
{
    return appItem_;
}

//-----------------------------------------------------------------------------
// Function: getEndpoints()
//-----------------------------------------------------------------------------
QList<EndpointItem*> const& ProgramEntityItem::getEndpoints() const
{
    return endpointStack_->getEndpoints();
}

//-----------------------------------------------------------------------------
// Function: getEndpoint()
//-----------------------------------------------------------------------------
EndpointItem* ProgramEntityItem::getEndpoint(QString const& name)
{
    foreach(EndpointItem* item, endpointStack_->getEndpoints())
    {
        if (item->getName() == name)
        {
            return item;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: setEndpointOrder()
//-----------------------------------------------------------------------------
void ProgramEntityItem::setEndpointOrder(QMap<QString, QPointF> const& positions)
{
    endpointStack_->setEndpointOrder(positions);
}

//-----------------------------------------------------------------------------
// Function: isEndpointsExpanded()
//-----------------------------------------------------------------------------
bool ProgramEntityItem::isEndpointsExpanded() const
{
    return endpointStack_->isExpanded();
}

//-----------------------------------------------------------------------------
// Function: setEndpointsExpanded()
//-----------------------------------------------------------------------------
void ProgramEntityItem::setEndpointsExpanded(bool expanded)
{
    endpointStack_->setExpanded(expanded);
}
