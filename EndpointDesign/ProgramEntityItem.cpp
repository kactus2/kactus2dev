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
#include <MCAPI/CSourceTextEdit.h>
#include <MCAPI/MCAPIDesignerTypes.h>

#include "EndpointDesignDiagram.h"
#include "EndpointItem.h"
#include "EndpointStack.h"
#include "EndpointConnection.h"
#include "AppPlaceholderItem.h"
#include "ApplicationItem.h"
#include "MappingComponentItem.h"

#include <QDir>
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
    : SWComponentItem(QRectF(-WIDTH / 2, 0, WIDTH, TOP_MARGIN + BOTTOM_MARGIN), 0,
                      component, instanceName, displayName, description, configurableElementValues),
      endpointStack_(0), appPlaceholder_(0), appItem_(0), importedIcon_(0), fixed_(false)
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
    QMap< QString, QSharedPointer<Port> > const& endpointDefinitions = componentModel()->getPorts();
    QMapIterator< QString, QSharedPointer<Port> > curEndpointDef(endpointDefinitions);

    while (curEndpointDef.hasNext())
    {
        QSharedPointer<Port const> endpointDef = *curEndpointDef.next();

        MCAPIDataType type = MCAPI_TYPE_MESSAGE;
		stringToValue(endpointDef->getTypeName(), type);

        MCAPIEndpointDirection dir = MCAPI_ENDPOINT_IN;
        stringToValue(General::direction2Str(endpointDef->getDirection()), dir);

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
        QString localEndpointName = endpoint->getName();

        matcher->addEndpoint(localEndpointName, endpoint->getMCAPIDirection(),
                             name().toUpper(), endpoint->getName().toUpper() + "_PORT");

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

            Port* port = appItem_->componentModel()->getPort(endpoint->getName());

            if (port == 0)
            {
                emit errorMessage(tr("Endpoint '%1' not found in the SW application instance '%2'").arg(endpoint->getName(), appItem_->name()));
                continue;
            }

            QString remoteEndpointName = port->getRemoteEndpointName();
            
            matcher->addEndpoint(remoteEndpointName, remoteEndpoint->getMCAPIDirection(),
                                 remoteEndpoint->getParentProgramEntity()->name().toUpper(),
                                 remoteEndpointName.toUpper() + "_PORT");
            matcher->addConnection(localEndpointName, remoteEndpointName,
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
    if (fixed_)
    {
        endpointStack_->setPos(0.0f, FIXED_TOP_MARGIN);
        setRect(-WIDTH / 2, 0, WIDTH, FIXED_TOP_MARGIN + height + BOTTOM_MARGIN);
    }
    else
    {
        endpointStack_->setPos(0.0f, TOP_MARGIN);
        setRect(-WIDTH / 2, 0, WIDTH, TOP_MARGIN + height + BOTTOM_MARGIN);
    }

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
    generateCode(getGenSourceLocation());

    CSourceWriter writer(filename, createIndentString());

    if (!writer.open())
    {
        return;
    }

    // Write includes.
    writer.writeInclude("stdlib.h");
    writer.writeInclude("stdio.h");
    writer.writeLine();

    writer.writeLine("// This header includes the Kactus2 generated MCAPI code.");
    writer.writeInclude("ktsmcapicode.h");
    writer.writeLine();

    writer.writeLine("int main(int argc, char* argv[])");
    writer.beginBlock();
    writer.writeLine("// Initialize MCAPI.");

    writer.writeLine("if (initializeMCAPI() != 0)");
    writer.beginBlock();
    writer.writeLine("// TODO: Write error handling code.");
    writer.writeLine("return EXIT_FAILURE;");
    writer.endBlock();
    writer.writeLine();

    writer.writeLine("if (createConnections() != 0)");
    writer.beginBlock();
    writer.writeLine("// TODO: Write error handling code.");
    writer.writeLine("return EXIT_FAILURE;");
    writer.endBlock();
    writer.writeLine();

    writer.writeLine("// TODO: Write your application code here.");
    writer.writeLine();
    writer.writeLine("// Close connections and finalize MCAPI before exiting.");
    writer.writeLine("if (closeConnections() != 0)");
    writer.beginBlock();
    writer.writeLine("// TODO: Write error handling code.");
    writer.writeLine("return EXIT_FAILURE;");
    writer.endBlock();
    writer.writeLine();
    writer.writeLine("mcapi_finalize(&status);");
    writer.writeLine("return EXIT_SUCCESS;");

    writer.endBlock();
}

//-----------------------------------------------------------------------------
// Function: generateCode()
//-----------------------------------------------------------------------------
void ProgramEntityItem::generateCode(QString const& dir)
{
    // Create the directory if it does not exist.
    if (!QDir(dir).exists())
    {
        QDir().mkpath(dir);
    }

    // Retrieve the list of connected remote nodes.
    QList<ProgramEntityItem*> remoteNodes;

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

            if (!remoteNodes.contains(remoteEndpoint->getParentProgramEntity()))
            {
                remoteNodes.append(remoteEndpoint->getParentProgramEntity());
            }
        }
    }

    generateHeader(dir + "/ktsmcapicode.h");
    generateSource(dir + "/ktsmcapicode.c", remoteNodes);
}

//-----------------------------------------------------------------------------
// Function: indent()
//-----------------------------------------------------------------------------
QString ProgramEntityItem::createIndentString()
{
    QSettings settings;

    // Read indentation settings.
    CSourceTextEdit::IndentStyle style =
        static_cast<CSourceTextEdit::IndentStyle>(settings.value("editor/indentStyle",
                                                               CSourceTextEdit::INDENT_STYLE_SPACES).toInt());
    unsigned int width = settings.value("editor/indentWidth", 4).toInt();

    QString indent;

    if (style == CSourceTextEdit::INDENT_STYLE_SPACES)
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
    // Disable application if the underlying mapping component is not a CPU.
    if (!getMappingComponent()->componentModel()->isCpu())
    {
        return;
    }

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
// Function: getMappingComponent()
//-----------------------------------------------------------------------------
MappingComponentItem const* ProgramEntityItem::getMappingComponent() const
{
    return dynamic_cast<MappingComponentItem const*>(parentItem());
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
            importedIcon_->setToolTip(tr("Auto-synced"));

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

//-----------------------------------------------------------------------------
// Function: setFixed()
//-----------------------------------------------------------------------------
void ProgramEntityItem::setFixed(bool fixed)
{
    fixed_ = fixed;
    appPlaceholder_->setVisible(!fixed);

    onEndpointStackChange(endpointStack_->getVisibleHeight());
}

//-----------------------------------------------------------------------------
// Function: generateHeader()
//-----------------------------------------------------------------------------
void ProgramEntityItem::generateHeader(QString const& filename)
{
    CSourceWriter writer(filename, createIndentString());

    if (!writer.open())
    {
        return;
    }

    // Write the Kactus 2 generated code.
    writer.writeLine("// DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE OVERWRITTEN BY KACTUS2.");
    writer.writeLine();

    writer.writeLine("#ifndef KTSMCAPICODE_H");
    writer.writeLine("#define KTSMCAPICODE_H");
    writer.writeLine();

    writer.writeInclude("mcapi.h");
    writer.writeLine();

    writer.writeHeaderComment("Node data.");
    writer.writeLine();

    // Write local endpoint variables.
    writer.writeLine("// Local endpoints.");
    
    foreach (EndpointItem* endpoint, endpointStack_->getEndpoints())
    {
        writer.writeLine("extern mcapi_endpoint_t " + endpoint->getName() + ";");

        if (endpoint->getConnectionType() == MCAPI_TYPE_PACKET)
        {
            if (endpoint->getMCAPIDirection() == MCAPI_ENDPOINT_OUT)
            {
                writer.writeLine("extern mcapi_pktchan_send_hndl_t " + endpoint->getName() + "_handle;");
            }
            else
            {
                writer.writeLine("extern mcapi_pktchan_recv_hndl_t " + endpoint->getName() + "_handle;");
            }
        }
        else if (endpoint->getConnectionType() == MCAPI_TYPE_SCALAR)
        {
            if (endpoint->getMCAPIDirection() == MCAPI_ENDPOINT_OUT)
            {
                writer.writeLine("extern mcapi_sclchan_send_hndl_t " + endpoint->getName() + "_handle;");
            }
            else
            {
                writer.writeLine("extern mcapi_sclchan_recv_hndl_t " + endpoint->getName() + "_handle;");
            }
        }

        writer.writeLine();
    }

    writer.writeLine();

    // Write remote endpoint variables.
    writer.writeLine("// Remote endpoints.");

    foreach (EndpointItem* endpoint, endpointStack_->getEndpoints())
    {
        Port* port = appItem_->componentModel()->getPort(endpoint->getName());
        Q_ASSERT(port != 0);

        writer.writeLine("extern mcapi_endpoint_t " + port->getRemoteEndpointName() + ";");
    }

    writer.writeLine();

    // Write the other variables.
    writer.writeLine("// Other variables.");
    writer.writeLine("extern mcapi_status_t status;");
    writer.writeLine();

    writer.writeHeaderComment("Functions.");
    writer.writeLine();

    // Write the initialization function prototype.
    writer.writeLine("/*");
    writer.writeLine(" *  Initializes the MCAPI system and endpoints.");
    writer.writeLine(" *");
    writer.writeLine(" *        @return 0 if successful. -1 in case of an error.");
    writer.writeLine(" */");
    writer.writeLine("int initializeMCAPI();");
    writer.writeLine();

    writer.writeLine("/*");
    writer.writeLine(" *  Creates and opens all connections.");
    writer.writeLine(" *");
    writer.writeLine(" *        @return 0 if successful. -1 in case of an error.");
    writer.writeLine(" */");
    writer.writeLine("int createConnections();");
    writer.writeLine();

    writer.writeLine("/*");
    writer.writeLine(" *  Closes all connections.");
    writer.writeLine(" *");
    writer.writeLine(" *        @return 0 if successful. -1 in case of an error.");
    writer.writeLine(" */");
    writer.writeLine("int closeConnections();");
    writer.writeLine();

    writer.writeLine("#endif // KTSMCAPICODE_H");
    writer.writeLine();
}

//-----------------------------------------------------------------------------
// Function: generateSource()
//-----------------------------------------------------------------------------
void ProgramEntityItem::generateSource(QString const& filename, QList<ProgramEntityItem*> const& remoteNodes)
{
    CSourceWriter writer(filename, createIndentString());

    if (!writer.open())
    {
        return;
    }

    // Write the Kactus 2 generated code.
    writer.writeLine("// DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE OVERWRITTEN BY KACTUS2.");
    writer.writeLine();

    writer.writeInclude("ktsmcapicode.h", true);
    writer.writeLine();
    writer.writeHeaderComment("Constants.");
    writer.writeLine();

    writer.writeLine("// Local node ID.");
    writer.writeLine("const mcapi_node_t " + name().toUpper() + " = " +
                     QString::number(getMappingComponent()->getID()) + ";");
    writer.writeLine();

    // Write the remote node IDs.
    if (!remoteNodes.empty())
    {
        writer.writeLine("// Remote node IDs.");

        foreach (ProgramEntityItem* node, remoteNodes)
        {
            writer.writeLine("const mcapi_node_t " + node->name().toUpper() +
                             " = " + QString::number(node->getMappingComponent()->getID()) + ";");
        }

        writer.writeLine();
    }

    // Write the local port IDs.
    writer.writeLine("// Local port IDs.");

    foreach (EndpointItem* endpoint, endpointStack_->getEndpoints())
    {
        writer.writeLine("const mcapi_port_t " + endpoint->getName().toUpper() + "_PORT = " +
                         QString::number(endpoint->getPortID()) + ";");
    }

    writer.writeLine();

    // Write the remote port IDs.
    writer.writeLine("// Remote port IDs.");

    foreach (EndpointItem* endpoint, endpointStack_->getEndpoints())
    {
        Port* port = appItem_->componentModel()->getPort(endpoint->getName());
        Q_ASSERT(port != 0);

        int portNumber = -1;

        if (endpoint->isConnected())
        {
            EndpointItem* remoteEndpoint = endpoint->getConnections().first()->getEndpoint2();

            if (remoteEndpoint == endpoint)
            {
                remoteEndpoint = endpoint->getConnections().first()->getEndpoint1();
            }

            portNumber = remoteEndpoint->getPortID();
        }
        
        writer.writeLine("const mcapi_port_t " + port->getRemoteEndpointName().toUpper() + "_PORT = " +
                         QString::number(portNumber) + ";");
    }

    writer.writeLine();

    writer.writeHeaderComment("Global variables.");
    writer.writeLine();

    // Write local endpoint variables.
    writer.writeLine("// Local endpoints.");

    foreach (EndpointItem* endpoint, endpointStack_->getEndpoints())
    {
        writer.writeLine("mcapi_endpoint_t " + endpoint->getName() + ";");

        if (endpoint->getConnectionType() == MCAPI_TYPE_PACKET)
        {
            if (endpoint->getMCAPIDirection() == MCAPI_ENDPOINT_OUT)
            {
                writer.writeLine("mcapi_pktchan_send_hndl_t " + endpoint->getName() + "_handle;");
            }
            else
            {
                writer.writeLine("mcapi_pktchan_recv_hndl_t " + endpoint->getName() + "_handle;");
            }
        }
        else
        {
            if (endpoint->getMCAPIDirection() == MCAPI_ENDPOINT_OUT)
            {
                writer.writeLine("mcapi_sclchan_send_hndl_t " + endpoint->getName() + "_handle;");
            }
            else
            {
                writer.writeLine("mcapi_sclchan_recv_hndl_t " + endpoint->getName() + "_handle;");
            }
        }

        writer.writeLine();
    }

    writer.writeLine();

    // Write remote endpoint variables.
    writer.writeLine("// Remote endpoints.");

    foreach (EndpointItem* endpoint, endpointStack_->getEndpoints())
    {
        Port* port = appItem_->componentModel()->getPort(endpoint->getName());
        Q_ASSERT(port != 0);

        writer.writeLine("mcapi_endpoint_t " + port->getRemoteEndpointName() + ";");
    }

    writer.writeLine();

    // Write the other variables.
    writer.writeLine("// Other variables.");
    writer.writeLine("mcapi_status_t status;");
    writer.writeLine();

    generateInitializeMCAPIFunc(writer);
    generateCreateConnectionsFunc(writer);
    generateCloseConnectionsFunc(writer);

    writer.writeLine();
}

//-----------------------------------------------------------------------------
// Function: updateGeneratedCode()
//-----------------------------------------------------------------------------
void ProgramEntityItem::updateGeneratedCode()
{
    // Generated code should be updated only if the code files already exist.
    if (QFileInfo(getGenSourceLocation() + "/ktsmcapicode.h").exists())
    {
        generateCode(getGenSourceLocation());
    }
}

//-----------------------------------------------------------------------------
// Function: getGenSourceLocation()
//-----------------------------------------------------------------------------
QString ProgramEntityItem::getGenSourceLocation() const
{
    return getMappingComponent()->getFileLocation() + '/' + name();
}

//-----------------------------------------------------------------------------
// Function: generateCreateConnectionsFunc()
//-----------------------------------------------------------------------------
void ProgramEntityItem::generateCreateConnectionsFunc(CSourceWriter& writer)
{
    // Write the createConnections() function.
    writer.writeHeaderComment("Function: createConnections()");
    writer.writeLine("int createConnections()");

    writer.beginBlock();
    writer.writeLine("mcapi_request_t request;");
    writer.writeLine("size_t size = 0;");
    writer.writeLine();

    foreach (EndpointItem* endpoint, endpointStack_->getEndpoints())
    {
        if (endpoint->isConnected())
        {
            Port* port = appItem_->componentModel()->getPort(endpoint->getName());
            Q_ASSERT(port != 0);

            switch (endpoint->getConnectionType())
            {
            case MCAPI_TYPE_PACKET:
                {
                    if (endpoint->getMCAPIDirection() == MCAPI_ENDPOINT_OUT)
                    {
                        // Sender is responsible of calling the connect function.
                        writer.writeLine("mcapi_connect_pktchan_i(" + endpoint->getName() + ", " +
                            port->getRemoteEndpointName() + ", &request, &status);");
                        writer.writeLine();

                        writeStatusCheck(writer);
                        writeWaitCall(writer, "request", "size");

                        writer.writeLine("mcapi_open_pktchan_send_i(&" + endpoint->getName() + "_handle, " +
                                         endpoint->getName() + ", &request, &status);");
                        writer.writeLine();

                        writeStatusCheck(writer);
                        writeWaitCall(writer, "request", "size");
                    }
                    else
                    {
                        writer.writeLine("mcapi_open_pktchan_recv_i(&" + endpoint->getName() + "_handle, " +
                                         endpoint->getName() + ", &request, &status);");
                        writer.writeLine();

                        writeStatusCheck(writer);
                        writeWaitCall(writer, "request", "size");
                    }

                    break;
                }

            case MCAPI_TYPE_SCALAR:
                {
                    if (endpoint->getMCAPIDirection() == MCAPI_ENDPOINT_OUT)
                    {
                        writer.writeLine("mcapi_connect_sclchan_i(" + endpoint->getName() + ", " +
                            port->getRemoteEndpointName() + ", &request, &status);");
                        writer.writeLine();

                        writeStatusCheck(writer);
                        writeWaitCall(writer, "request", "size");

                        writer.writeLine("mcapi_open_sclchan_send_i(&" + endpoint->getName() + "_handle, " +
                            endpoint->getName() + ", &request, &status);");
                        writer.writeLine();

                        writeStatusCheck(writer);
                        writeWaitCall(writer, "request", "size");
                    }
                    else
                    {
                        writer.writeLine("mcapi_open_sclchan_recv_i(&" + endpoint->getName() + "_handle, " +
                            endpoint->getName() + ", &request, &status);");
                        writer.writeLine();

                        writeStatusCheck(writer);
                        writeWaitCall(writer, "request", "size");
                    }

                    break;
                }
            }
        }
    }

    writer.writeLine("return 0;");
    writer.endBlock();
    writer.writeLine();
}

//-----------------------------------------------------------------------------
// Function: ProgramEntityItem::generateCloseConnectionsFunc()
//-----------------------------------------------------------------------------
void ProgramEntityItem::generateCloseConnectionsFunc(CSourceWriter& writer)
{
    // Write the createConnections() function.
    writer.writeHeaderComment("Function: closeConnections()");
    writer.writeLine("int closeConnections()");

    writer.beginBlock();
    writer.writeLine("mcapi_request_t request;");
    writer.writeLine("size_t size = 0;");
    writer.writeLine();

    foreach (EndpointItem* endpoint, endpointStack_->getEndpoints())
    {
        if (endpoint->isConnected())
        {
            Port* port = appItem_->componentModel()->getPort(endpoint->getName());
            Q_ASSERT(port != 0);

            switch (endpoint->getConnectionType())
            {
            case MCAPI_TYPE_PACKET:
                {
                    if (endpoint->getMCAPIDirection() == MCAPI_ENDPOINT_OUT)
                    {
                        writer.writeLine("mcapi_pktchan_send_close_i(" + endpoint->getName() +
                                         "_handle, &request, &status);");
                        writer.writeLine();
                    }
                    else
                    {
                        writer.writeLine("mcapi_pktchan_recv_close_i(" + endpoint->getName() +
                                         "_handle, &request, &status);");
                        writer.writeLine();
                    }

                    writeStatusCheck(writer);
                    writeWaitCall(writer, "request", "size");
                    break;
                }

            case MCAPI_TYPE_SCALAR:
                {
                    if (endpoint->getMCAPIDirection() == MCAPI_ENDPOINT_OUT)
                    {
                        writer.writeLine("mcapi_sclchan_send_close_i(" + endpoint->getName() +
                                         "_handle, &request, &status);");
                        writer.writeLine();
                    }
                    else
                    {
                        writer.writeLine("mcapi_sclchan_recv_close_i(" + endpoint->getName() +
                                         "_handle, &request, &status);");
                        writer.writeLine();
                    }

                    writeStatusCheck(writer);
                    writeWaitCall(writer, "request", "size");
                    break;
                }
            }
        }
    }

    writer.writeLine("return 0;");
    writer.endBlock();
    writer.writeLine();
}

//-----------------------------------------------------------------------------
// Function: ProgramEntityItem::generateInitializeMCAPIFunc()
//-----------------------------------------------------------------------------
void ProgramEntityItem::generateInitializeMCAPIFunc(CSourceWriter& writer)
{
    // Write the initialization function.
    writer.writeHeaderComment("Function: initializeMCAPI()");
    writer.writeLine("int initializeMCAPI()");

    writer.beginBlock();
    writer.writeLine("mcapi_version_t version;");
    writer.writeLine();
    writer.writeLine("// Initialize MCAPI implementation.");
    writer.writeLine("mcapi_initialize(" + name().toUpper() + ", &version, &status);");
    writer.writeLine();

    writer.writeLine("if (status != MCAPI_SUCCESS)");
    writer.beginBlock();
    writer.writeLine("return -1;");
    writer.endBlock();
    writer.writeLine();

    // Write function calls to create local endpoints.
    writer.writeLine("// Create local endpoints.");

    foreach (EndpointItem* endpoint, endpointStack_->getEndpoints())
    {
        writer.writeLine(endpoint->getName() + " = mcapi_create_endpoint(" +
            endpoint->getName().toUpper() + "_PORT, &status);");
        writer.writeLine();

        writer.writeLine("if (status != MCAPI_SUCCESS)");
        writer.beginBlock();
        writer.writeLine("return -1;");
        writer.endBlock();
        writer.writeLine();
    }

    // Write function calls to retrieve remote endpoints.
    writer.writeLine("// Retrieve remote endpoints.");

    foreach (EndpointItem* endpoint, endpointStack_->getEndpoints())
    {
        if (endpoint->isConnected())
        {
            Port* port = appItem_->componentModel()->getPort(endpoint->getName());
            Q_ASSERT(port != 0);

            EndpointItem* remoteEndpoint = endpoint->getConnections().first()->getEndpoint2();

            if (remoteEndpoint == endpoint)
            {
                remoteEndpoint = endpoint->getConnections().first()->getEndpoint1();
            }

            writer.writeLine(port->getRemoteEndpointName() +
                " = mcapi_get_endpoint(" +
                remoteEndpoint->getParentProgramEntity()->name().toUpper() +
                ", " + port->getRemoteEndpointName().toUpper() + "_PORT, &status);");
            writer.writeLine();

            writer.writeLine("if (status != MCAPI_SUCCESS)");
            writer.beginBlock();
            writer.writeLine("return -1;");
            writer.endBlock();
            writer.writeLine();
        }
    }

    writer.writeLine("return 0;");
    writer.endBlock();
    writer.writeLine();
}

//-----------------------------------------------------------------------------
// Function: ProgramEntityItem::writeWaitCall()
//-----------------------------------------------------------------------------
void ProgramEntityItem::writeWaitCall(CSourceWriter& writer, QString const& requestName,
                                      QString const& sizeName)
{
    writer.writeLine("if (mcapi_wait(&" + requestName + ", &" + sizeName + ", &status, MCAPI_INFINITE) == MCAPI_FALSE)");
    writer.beginBlock();
    writer.writeLine("return -1;");
    writer.endBlock();
    writer.writeLine();
}

//-----------------------------------------------------------------------------
// Function: ProgramEntityItem::writeStatusCheck()
//-----------------------------------------------------------------------------
void ProgramEntityItem::writeStatusCheck(CSourceWriter &writer)
{
    writer.writeLine("if (status != MCAPI_SUCCESS)");
    writer.beginBlock();
    writer.writeLine("return -1;");
    writer.endBlock();
    writer.writeLine();
}
