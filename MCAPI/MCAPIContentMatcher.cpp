//-----------------------------------------------------------------------------
// File: MCAPIContentMatcher.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 11.1.2011
//
// Description:
// MCAPI-specific content matcher.
//-----------------------------------------------------------------------------

#include "MCAPIContentMatcher.h"

#include <algorithm>
#include <QTextCursor>

#include <common/widgets/assistedTextEdit/TextContentAssistWidget.h>

namespace
{
    //! Number of type names.
    int const TYPENAME_COUNT = 27;

    // The type names.
    QString const TYPENAMES[TYPENAME_COUNT] =
    {
        "void",
        "int",
        "float",
        "double",
        "unsigned",
        "signed",
        "char",
        "union",
        "mcapi_int_t",
        "mcapi_uint_t",
        "mcapi_uint8_t",
        "mcapi_uint16_t",
        "mcapi_uint32_t",
        "mcapi_uint64_t",
        "mcapi_boolean_t",
        "mcapi_endpoint_t",
        "mcapi_node_t",
        "mcapi_port_t",
        "mcapi_version_t",
        "mcapi_status_t",
        "mcapi_request_t",
        "mcapi_priority_t",
        "mcapi_timeout_t",
        "mcapi_pktchan_recv_hndl_t",
        "mcapi_pktchan_send_hndl_t",
        "mcapi_sclchan_recv_hndl_t",
        "mcapi_sclchan_send_hndl_t",
    };

    //! The name of the status variable.
    QString const STATUS_VARIABLE_NAME = "&status";
}

//-----------------------------------------------------------------------------
// Static member initializations.
//-----------------------------------------------------------------------------
MCAPIFunctionSet MCAPIContentMatcher::s_functionSet;

//-----------------------------------------------------------------------------
// Function: MCAPIContentMatcher()
//-----------------------------------------------------------------------------
MCAPIContentMatcher::MCAPIContentMatcher() : m_localNode(), m_remoteNodes(), m_connections(),
                                             m_lastMatchType(MATCH_NONE), curAssist_(0),
                                             m_tooltipText()
{
    m_icons[MCAPI_CONTENT_FUNC] = QIcon(":icons/graphics/mcapi-func.png");
    m_icons[MCAPI_CONTENT_TYPENAME] = QIcon(":icons/graphics/mcapi-type.png");
    m_icons[MCAPI_CONTENT_NODE_ID] = QIcon(":icons/graphics/mcapi-node.png");
    m_icons[MCAPI_CONTENT_PORT_ID] = QIcon(":icons/graphics/mcapi-port.png");
    m_icons[MCAPI_CONTENT_ENDPOINT] = QIcon(":icons/graphics/mcapi-endpoint.png");
}

//-----------------------------------------------------------------------------
// Function: ~MCAPIContentMatcher()
//-----------------------------------------------------------------------------
MCAPIContentMatcher::~MCAPIContentMatcher()
{
}

//-----------------------------------------------------------------------------
// Function: setSenderName()
//-----------------------------------------------------------------------------
void MCAPIContentMatcher::setLocalNodeName(QString const& name)
{
    m_localNode.name = name;
}

//-----------------------------------------------------------------------------
// Function: addEndpoint()
//-----------------------------------------------------------------------------
void MCAPIContentMatcher::addEndpoint(QString const& name, MCAPIEndpointDirection type,
                                      QString const& nodeName, QString const& portName)
{
    EndpointDesc endpointDesc(name, type, portName);

    // Check if the node is the local node.
    if (m_localNode.name == nodeName)
    {
        m_localNode.endpoints.push_back(endpointDesc);
        return;
    }

    // Otherwise find out the correct remote node.
    QList<NodeDesc>::iterator node = m_remoteNodes.begin();

    while (node != m_remoteNodes.end())
    {
        if (node->name == nodeName)
        {
            // If the receiver was found, just add the endpoint descriptor to its list of endpoints.
            node->endpoints.push_back(endpointDesc);
            return;
        }

        ++node;
    }

    // If the receiver node was not found, add it to the list.
    m_remoteNodes.push_back(NodeDesc(nodeName));
    m_remoteNodes.back().endpoints.push_back(endpointDesc);
}

//-----------------------------------------------------------------------------
// Function: addConnection()
//-----------------------------------------------------------------------------
void MCAPIContentMatcher::addConnection(QString const& localEndpoint, QString const& remoteEndpoint,
                                        MCAPIDataType type)
{
    m_connections.push_back(ConnectionDesc(localEndpoint, remoteEndpoint, type));
}

//-----------------------------------------------------------------------------
// Function: lookForwardMatch()
//-----------------------------------------------------------------------------
bool MCAPIContentMatcher::lookForwardMatch(QString const& text)
{
    int index = 0;
    int toolTipIndex = 0;
    return (m_lastMatchType != MATCH_NONE &&
            enumerateMatches(text, 0, index, 0, toolTipIndex) == m_lastMatchType);
}

//-----------------------------------------------------------------------------
// Function: fillWithContent()
//-----------------------------------------------------------------------------
 bool MCAPIContentMatcher::fillWithContent(QString const& text, TextContentAssistWidget& assist,
                                           int& startIndex, QString& toolTipText, int& toolTipIndex)
{
    curAssist_ = &assist;

    toolTipText.clear();    
    m_lastMatchType = enumerateMatches(text, &MCAPIContentMatcher::addMatchToAssist,
                                       startIndex, &toolTipText, toolTipIndex);

    curAssist_->sortItems();

    return (m_lastMatchType != MATCH_NONE);
}

//-----------------------------------------------------------------------------
// Function: enumerateMatches()
//-----------------------------------------------------------------------------
MCAPIContentMatcher::MatchType MCAPIContentMatcher::enumerateMatches(QString const& text, MatchExecFunc func,
                                                                     int& startIndex, QString* toolTipText,
                                                                     int& toolTipIndex)
{
    static QRegExp funcCallExp("[a-z|A-Z|_][a-z|A-Z|0-9|_]*\\([a-z|A-Z|0-9|_|,|&|\\s]*$");

    bool exactMatch = false;
    int count = 0;
    
    // Enumerate function parameters if a function call is found.
    if (funcCallExp.indexIn(text) >= 0)
    {
        count = enumerateFunctionParams(text, func, startIndex, exactMatch, toolTipText, toolTipIndex);

        if (count > 0 && !(count == 1 && exactMatch))
        {
            return MATCH_FUNCTION;
        }
    }
    // If not found, enumerate plain names.
    else
    {
        count = enumerateNames(text, func, startIndex, exactMatch);

        if (count > 0 && !(count == 1 && exactMatch))
        {
            return MATCH_PARAM;
        }
    }

    return MATCH_NONE;
}

//-----------------------------------------------------------------------------
// Function: enumerateNames()
//-----------------------------------------------------------------------------
int MCAPIContentMatcher::enumerateNames(QString const &text, MatchExecFunc func, int &startIndex,
                                        bool& exactMatch)
{
    exactMatch = false;
    int count = 0;

    // Check if the last word could be a name.
    static QRegExp lastWordExp("[a-z|A-Z|_][a-z|A-Z|0-9|_]*$");
    startIndex = lastWordExp.indexIn(text, 0);

    if (startIndex >= 0)
    {
        QString word = text.mid(startIndex, lastWordExp.matchedLength());

        // Search for functions that start with the retrieved word.
        QRegExp matchExp("^" + word.toLower() + ".*");

        for (unsigned int i = 0; i < s_functionSet.getCount(); ++i)
        {
            if (tryMatchIdentifier(s_functionSet.getAt(i).getName(), MCAPI_CONTENT_FUNC, matchExp, func, count))
            {
                // Check if this was an exact match.
                if (!exactMatch && s_functionSet.getAt(i).getName() == word)
                {
                    exactMatch = true;
                }
            }
        }

        // Search for types that start with the retrieved word.
        for (int i = 0; i < TYPENAME_COUNT; ++i)
        {
            if (tryMatchIdentifier(TYPENAMES[i], MCAPI_CONTENT_TYPENAME, matchExp, func, count))
            {
                // Check if this was an exact match.
                if (!exactMatch && TYPENAMES[i] == word)
                {
                    exactMatch = true;
                }
            }
        }
    }

    return count;
}

//-----------------------------------------------------------------------------
// Function: enumerateFunctionParams()
//-----------------------------------------------------------------------------
int MCAPIContentMatcher::enumerateFunctionParams(QString const &text, MatchExecFunc func,
                                                 int &startIndex, bool& exactMatch,
                                                 QString* toolTipText, int& toolTipIndex)
{
    // Used static regular expressions.
    static QRegExp funcCallExp("[a-z|A-Z|_][a-z|A-Z|0-9|_]*\\([a-z|A-Z|0-9|_|,|&|\\s]*$");
    static QRegExp lastParamExp("[(|,]\\s*(&)?([a-z|A-Z|0-9|_][a-z|A-Z|0-9|_]*)?$");
    static QRegExp callStartExp("\\([a-z|A-Z|0-9|_|,|&|\\s]*$");
    
    exactMatch = false;
    int count = 0;

    // Find the beginning of the function call.
    int funcStartIndex = funcCallExp.indexIn(text, 0);
    Q_ASSERT(funcStartIndex >= 0);
    
    // Search for the last param.
    int index = lastParamExp.indexIn(text, funcStartIndex);

    if (index >= 0)
    {
        // If found, discard the leading whitespaces, left parentheses and commas.
        startIndex = text.indexOf(QRegExp("[&|a-z|A-Z|0-9|_]"), index);

        if (startIndex < 0)
        {
            startIndex = index + lastParamExp.matchedLength();
        }

        QString word = text.mid(startIndex, lastParamExp.matchedLength() - (startIndex - index));
        
        // Extract the function name & parameter list from the text.
        index = callStartExp.indexIn(text, funcStartIndex);
        Q_ASSERT(index >= 0);

        QString funcName = text.mid(funcStartIndex, index - funcStartIndex);
        QString paramListText = text.mid(index + 1);

        // Extract previous parameters to a list for easy access.
        QStringList params;
        extractParams(paramListText, params);

        // Based on the function description, try to match content.
        MCAPIFunctionDesc const* funcDesc = s_functionSet.find(funcName);

        if (funcDesc != 0 &&
            static_cast<unsigned int>(params.count()) < funcDesc->getParamCount())
        {
            // Generate a tooltip text.
            if (toolTipText != 0)
            {
                funcDesc->generateToolTipText(params.count(), *toolTipText);
                toolTipIndex = index + 1;
            }

            // Try to match the parameter to the corresponding function.
            tryMatchParam(funcDesc, params, word, func, count);
        }
    }

    return count;
}

//-----------------------------------------------------------------------------
// Function: addMatchToAssist()
//-----------------------------------------------------------------------------
void MCAPIContentMatcher::addMatchToAssist(TextContentAssistWidget& assist, QString const& match,
                                           QIcon const& icon)
{
    assist.addItem(new QListWidgetItem(icon, match));
}

//-----------------------------------------------------------------------------
// Function: findNodeDesc()
//-----------------------------------------------------------------------------
MCAPIContentMatcher::NodeDesc const* MCAPIContentMatcher::findNodeDesc(QString const& name) const
{
    // Check if the name matches with the name of the local node.
    if (m_localNode.name == name)
    {
        return &m_localNode;
    }

    // Otherwise try to find a match from the remote nodes.
    QList<NodeDesc>::const_iterator itrNode = m_remoteNodes.begin();

    while (itrNode != m_remoteNodes.end())
    {
        if (itrNode->name == name)
        {
            return &(*itrNode);
        }

        ++itrNode;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: findNodeDesc()
//-----------------------------------------------------------------------------
MCAPIContentMatcher::EndpointDesc const* MCAPIContentMatcher::findEndpointDesc(QString const& name) const
{
    // Check for the endpoint from the local node first.
    EndpointDescList::const_iterator itrEndpoint = m_localNode.endpoints.begin();

    while (itrEndpoint != m_localNode.endpoints.end())
    {
        if (itrEndpoint->name == name)
        {
            return &(*itrEndpoint);
        }

        ++itrEndpoint;
    }

    // Otherwise try to find it from the remote nodes.
    QList<NodeDesc>::const_iterator itrNode = m_remoteNodes.begin();

    while (itrNode != m_remoteNodes.end())
    {
        itrEndpoint = itrNode->endpoints.begin();

        while (itrEndpoint != itrNode->endpoints.end())
        {
            if (itrEndpoint->name == name)
            {
                return &(*itrEndpoint);
            }

            ++itrEndpoint;
        }
        
        ++itrNode;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: findConnectionDesc()
//-----------------------------------------------------------------------------
MCAPIContentMatcher::ConnectionDesc const* MCAPIContentMatcher::findConnectionDesc(QString const& name) const
{
    ConnectionDescList::const_iterator itrConn = m_connections.begin();

    while (itrConn != m_connections.end())
    {
        if (itrConn->localEndpoint == name || itrConn->remoteEndpoint == name)
        {
            return &(*itrConn);
        }

        ++itrConn;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: tryMatchParam()
//-----------------------------------------------------------------------------
void MCAPIContentMatcher::tryMatchParam(MCAPIFunctionDesc const* funcDesc, QStringList const& params,
                                        QString const& word, MatchExecFunc func, int& count)
{
    // Find out the parameter index.
    int paramIndex = params.count();

    QRegExp matchExp("^" + word.toLower() + ".*");

    // Based on the function parameter description, try to find matching content.
    unsigned int paramDesc = funcDesc->getParamType(paramIndex);

    // Check if the parameter is a local node id.
    if (paramDesc & VAR_TYPE_LOCAL_NODE_ID)
    {
        tryMatchNodeParam(m_localNode, matchExp, func, count);
    }

    // Check if the parameter is a remote node id.
    if (paramDesc & VAR_TYPE_REMOTE_NODE_ID)
    {
        for (int i = 0; i < m_remoteNodes.size(); ++i)
        {
            tryMatchNodeParam(m_remoteNodes.at(i), matchExp, func, count);
        }
    }

    // Check if the parameter is a port id.
    if (paramDesc & VAR_TYPE_PORT_ID)
    {
        // The node is assumed to be the local node if not otherwise specified in the
        // first parameter of the function.
        QString nodeName = m_localNode.name;

        if (funcDesc->getParamCount() > 0 && (funcDesc->getParamType(0) & VAR_TYPE_NODE_ID) &&
            params.count() > 0)
        {
            nodeName = params.at(0);
        }

        NodeDesc const* nodeDesc = findNodeDesc(nodeName);

        if (nodeDesc != 0)
        {
            for (int i = 0; i < nodeDesc->endpoints.size(); ++i)
            {
                tryMatchPortParam(nodeDesc->endpoints.at(i), matchExp, func, count);
            }
        }
    }

    QString otherEndpointName;

    // Check if the parameter is a local endpoint.
    if (paramDesc & VAR_TYPE_LOCAL_ENDPOINT)
    {
        // Check if the remote endpoint has already been specified and the connection is valid.
        if (paramIndex > 0 && (funcDesc->getParamType(0) & VAR_TYPE_REMOTE_ENDPOINT))
        {
            if (isConnectionValid(params.at(0), paramDesc, funcDesc->getParamType(0), otherEndpointName))
            {
                // Try to match with the other endpoint.
                tryMatchIdentifier(otherEndpointName, MCAPI_CONTENT_ENDPOINT, matchExp, func, count);
            }
        }
        else
        {
            // Otherwise browse through the local endpoints, trying to find endpoints that match
            // with the parameter description.
            tryMatchEndpoints(m_localNode, paramDesc, matchExp, func, count);
        }
    }

    // Check if the parameter is a remote endpoint.
    if (paramDesc & VAR_TYPE_REMOTE_ENDPOINT)
    {
        // Check if the local endpoint has already been specified and the connection is valid.
        if (paramIndex > 0 && (funcDesc->getParamType(0) & VAR_TYPE_LOCAL_ENDPOINT))
        {
            if (isConnectionValid(params.at(0), funcDesc->getParamType(0), paramDesc, otherEndpointName))
            {
                // Try to match with the other endpoint.
                tryMatchIdentifier(otherEndpointName, MCAPI_CONTENT_ENDPOINT, matchExp, func, count);
            }
        }
        else
        {
            // Otherwise browse through the remote endpoints, trying to find endpoints that match
            // with the parameter description.
            for (int i = 0; i < m_remoteNodes.size(); ++i)
            {
                tryMatchEndpoints(m_remoteNodes.at(i), paramDesc, matchExp, func, count);
            }
        }
    }

    // Check if the parameter is a status pointer.
    if (paramDesc & VAR_TYPE_STATUS_PTR)
    {
        tryMatchIdentifier(STATUS_VARIABLE_NAME, MCAPI_CONTENT_TYPENAME, matchExp, func, count);
    }
}


//-----------------------------------------------------------------------------
// Function: tryMatchNodeParam()
//-----------------------------------------------------------------------------
void MCAPIContentMatcher::tryMatchNodeParam(NodeDesc const& desc, QRegExp& exp, MatchExecFunc func,
                                            int& count)
{
    tryMatchIdentifier(desc.name, MCAPI_CONTENT_NODE_ID, exp, func, count);
}

//-----------------------------------------------------------------------------
// Function: tryMatchPortParam()
//-----------------------------------------------------------------------------
void MCAPIContentMatcher::tryMatchPortParam(EndpointDesc const& desc, QRegExp& exp, MatchExecFunc func,
                                            int& count)
{
    tryMatchIdentifier(desc.portName, MCAPI_CONTENT_PORT_ID, exp, func, count);
}

//-----------------------------------------------------------------------------
// Function: tryMatchEndpoints()
//-----------------------------------------------------------------------------
void MCAPIContentMatcher::tryMatchEndpoints(NodeDesc const& node, unsigned int paramDesc, QRegExp matchExp,
                                            MatchExecFunc func, int& count)
{
    EndpointDescList::const_iterator itrEndpoint = node.endpoints.begin();

    while (itrEndpoint != node.endpoints.end())
    {
        // Search for a matching connection.
        ConnectionDesc const* connDesc = findConnectionDesc(itrEndpoint->name);

        if (connDesc != 0 && isEndpointValid(paramDesc, itrEndpoint->type, connDesc->type))
        {
            tryMatchIdentifier(itrEndpoint->name, MCAPI_CONTENT_ENDPOINT, matchExp, func, count);
        }

        ++itrEndpoint;
    }
}

//-----------------------------------------------------------------------------
// Function: tryMatchIdentifier()
//-----------------------------------------------------------------------------
bool MCAPIContentMatcher::tryMatchIdentifier(QString const& identifier, MCAPIContentType type,
                                             QRegExp& exp, MatchExecFunc func, int& count)
{
    if (identifier.toLower().contains(exp))
    {
        if (func != 0)
        {
            func(*curAssist_, identifier, m_icons[type]);
        }

        ++count;

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: isEndpointValid()
//-----------------------------------------------------------------------------
bool MCAPIContentMatcher::isEndpointValid(unsigned int paramDesc, MCAPIEndpointDirection endpointType,
                                          MCAPIDataType connType)
{
    // The input/output type must match.
    bool validInOut = (endpointType == MCAPI_ENDPOINT_IN && (paramDesc & VAR_TYPE_CONN_IN)) ||
        (endpointType == MCAPI_ENDPOINT_OUT && (paramDesc & VAR_TYPE_CONN_OUT));

    // The connection type must match.
    bool validConn = (connType == MCAPI_TYPE_SCALAR && (paramDesc & VAR_TYPE_CONN_SCALAR)) ||
        (connType == MCAPI_TYPE_PACKET && (paramDesc & VAR_TYPE_CONN_PACKET)) ||
        (connType == MCAPI_TYPE_MESSAGE && (paramDesc & VAR_TYPE_CONN_MSG));

    return (validInOut && validConn);
}

//-----------------------------------------------------------------------------
// Function: isConnectionValid()
//-----------------------------------------------------------------------------
bool MCAPIContentMatcher::isConnectionValid(QString const& endpointName, unsigned int localDesc,
                                            unsigned int remoteDesc, QString& otherEndpointName) const
{
    // Search for a matching connection.
    ConnectionDesc const* connDesc = findConnectionDesc(endpointName);

    if (connDesc == 0)
    {
        return false;
    }
    
    // Find the endpoint descriptions corresponding to both ends of the connection.
    EndpointDesc const* endpoint1Desc = findEndpointDesc(connDesc->localEndpoint);
    EndpointDesc const* endpoint2Desc = findEndpointDesc(connDesc->remoteEndpoint);
    Q_ASSERT(endpoint1Desc != 0);
    Q_ASSERT(endpoint2Desc != 0);

    // Both endpoints must match with the parameter descriptions.
    if (!isEndpointValid(localDesc, endpoint1Desc->type, connDesc->type) ||
        !isEndpointValid(remoteDesc, endpoint2Desc->type, connDesc->type))
    {
        return false;
    }

    if (connDesc->localEndpoint == endpointName)
    {
        otherEndpointName = connDesc->remoteEndpoint;
    }
    else
    {
        otherEndpointName = connDesc->localEndpoint;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: extractParams()
//-----------------------------------------------------------------------------
void MCAPIContentMatcher::extractParams(QString const& paramsListStr, QStringList& params)
{
    static QRegExp paramExtractExp("([a-z|A-Z|0-9|_][^,]*)?(?=\\s*[,|$])");

    // Clear the parameter list.
    params.clear();

    // Extract parameters one-by-one.
    int index = paramExtractExp.indexIn(paramsListStr);

    while (index >= 0)
    {
        params.push_back(paramsListStr.mid(index, paramExtractExp.matchedLength()));

        // Find the next comma to prepare for extracting the next parameter.
        int nextComma = paramsListStr.indexOf(',', index + paramExtractExp.matchedLength());

        // If not found, there are no more complete parameters (only the one that is
        // being written at the moment).
        if (nextComma < 0)
        {
            break;
        }

        index = paramExtractExp.indexIn(paramsListStr, nextComma + 1);
    }
}
