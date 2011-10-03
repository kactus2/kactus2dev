//-----------------------------------------------------------------------------
// File: MCAPIContentMatcher.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 11.1.2011
//
// Description:
// MCAPI-specific content matcher.
//-----------------------------------------------------------------------------

#ifndef MCAPICONTENTMATCHER_H
#define MCAPICONTENTMATCHER_H

#include <QIcon>
#include <QList>
#include <QStringList>

#include "MCAPIDesignerTypes.h"
#include "MCAPIFunctionSet.h"
#include "common/widgets/assistedTextEdit/ITextContentMatcher.h"

//-----------------------------------------------------------------------------
//! MCAPIContentMatcher class.
//-----------------------------------------------------------------------------
class MCAPIContentMatcher : public ITextContentMatcher
{
public:
    /*!
     *  Constructor.
     */
    MCAPIContentMatcher();

    /*!
     *  Destructor.
     */
    ~MCAPIContentMatcher();

    /*!
     *  Sets the name of the local node.
     *
     *      @param [in] name The name of the local node (id constant).
     */
    void setLocalNodeName(QString const& name);

    /*!
     *  Adds an endpoint.
     *
     *      @param [in] name      The name of the endpoint.
     *      @param [in] type      The endpoint type.
     *      @param [in] nodeName  The name of the node (id constant).
     *      @param [in] portName  The name of the port node (id constant).
     */
    void addEndpoint(QString const& name, MCAPIEndpointDirection type, QString const& nodeName,
                     QString const& portName);

    /*!
     *  Adds a connection between two endpoints.
     *
     *      @param [in] localEndpoint   The name of the local endpoint.
     *      @param [in] remoteEndpoint  The name of the remote endpoint.
     *      @param [in] type            The connection type.
     */
    void addConnection(QString const& localEndpoint, QString const& remoteEndpoint,
                       MCAPIDataType type);

    /*!
     *  Looks forward for matching contents in the given text.
     *
     *      @param [in] text The text.
     *
     *      @return True, if there is a match. Otherwise false.
     */
    bool lookForwardMatch(QString const& text);

    /*!
     *  Fills the content assist with content based on the given text.
     *
     *      @param [in]  text The text.
     *      @param [in]  assist        The content assist to fill with content.
     *      @param [out] startIndex    The start index of the selection to which the content applies.
     *      @param [out] toolTipText   The tool tip hint text. Empty if no tool tip available.
     *      @param [out] toolTipIndex  The index which specifies there start position for the tool tip hint.
     *
     *      @return The content state (what kind of content was detected).
     */
    bool fillWithContent(QString const& text, TextContentAssistWidget& assist,
                         int& startIndex, QString& toolTipText, int& toolTipIndex);

private:
    //! MatchExecFunc definition.
    typedef void (*MatchExecFunc)(TextContentAssistWidget& assist, QString const& match, QIcon const& icon);

    //-----------------------------------------------------------------------------
    //! MatchType enumeration.
    //-----------------------------------------------------------------------------
    enum MatchType
    {
        MATCH_NONE = 0,
        MATCH_FUNCTION,
        MATCH_PARAM
    };

    //-----------------------------------------------------------------------------
    //! EndpointDesc structure.
    //-----------------------------------------------------------------------------
    struct EndpointDesc
    {
        QString name;               //!< The name of the endpoint.
        MCAPIEndpointDirection type;     //!< The endpoint type.
        QString portName;           //!< The name of the port (id constant).

        /*!
         *  Constructor.
         */
        EndpointDesc(QString const& name_, MCAPIEndpointDirection type_,
                     QString const& portName_) : name(name_), type(type_), portName(portName_)
        {
        }
    };

    //! EndpointDescList type definition.
    typedef QList<EndpointDesc> EndpointDescList;

    //-----------------------------------------------------------------------------
    //! NodeDesc structure.
    //-----------------------------------------------------------------------------
    struct NodeDesc
    {
        QString name;                   //!< The name of the node (id constant).
        EndpointDescList endpoints;     //!< Available endpoints on this node.

        /*!
         *  Default constructor.
         */
        NodeDesc() : name(), endpoints()
        {
        }

        /*!
         *  Constructor which sets the name of the node.
         */
        NodeDesc(QString const& name_) : name(name_), endpoints()
        {
        }
    };

    //-----------------------------------------------------------------------------
    //! ConnectionDesc structure.
    //-----------------------------------------------------------------------------
    struct ConnectionDesc
    {
        QString localEndpoint;      //!< The name of the local endpoint.
        QString remoteEndpoint;     //!< The name of the remote endpoint.
        MCAPIDataType type;   //!< The connection type.

        /*!
         *  Constructor.
         */
        ConnectionDesc(QString const& localEndpoint_, QString const& remoteEndpoint_,
                       MCAPIDataType type_) : localEndpoint(localEndpoint_),
                                                    remoteEndpoint(remoteEndpoint_), type(type_)
        {
        }
    };

    //! ConnectionDescList type definition.
    typedef QList<ConnectionDesc> ConnectionDescList;

    // Disable copying.
    MCAPIContentMatcher(MCAPIContentMatcher const& rhs);
    MCAPIContentMatcher& operator=(MCAPIContentMatcher const& rhs);

    /*!
     *  Searches for matches in the given text.
     *
     *      @param [in]  text          The text to search.
     *      @param [in]  func          A function that is executed for each match. Can be null.
     *      @param [out] startIndex    The start index of the part of the text to which the matches apply.
     *      @param [out] toolTipText   The tool tip hint text. Empty if no tool tip available.
     *      @param [out] toolTipIndex  The cursor position in the text to indicate correct
     *                                 placement for the tool tip hint.
     *
     *      @return The content state (what kind of content was detected).
     */
    MatchType enumerateMatches(QString const& text, MatchExecFunc func, int& startIndex,
                               QString* toolTipText, int& toolTipIndex);

    /*!
     *  Enumerates matching names (function or type) in the given text.
     *
     *      @param [in]  text        The text to search.
     *      @param [in]  func        A function that is executed for each match. Can be null.
     *      @param [out] startIndex  The start index of the part of the text to which the matches apply.
     *      @param [out] exactMatch  If true, there was an exact match. Otherwise false.
     *
     *      @return The number of matches found.
     */
    int enumerateNames(QString const &text, MatchExecFunc func, int &startIndex, bool& exactMatch);

    /*!
     *  Enumerates matching function parameters in the given text.
     *
     *      @param [in]  text          The text to search.
     *      @param [in]  func          A function that is executed for each match. Can be null.
     *      @param [out] startIndex    The start index of the part of the text to which the matches apply.
     *      @param [out] exactMatch    If true, there was an exact match. Otherwise false.
     *      @param [out] toolTipText   The tool tip hint text. Empty if no tool tip available.
     *      @param [out] toolTipIndex  The cursor position in the text to indicate correct
     *                                 placement for the tool tip hint.
     *
     *      @return The number of matches found.
     *
     *      @remarks Assumes that there is valid function call up to the cursor.
     */
    int enumerateFunctionParams(QString const &text, MatchExecFunc func, int &startIndex,
                                bool& exactMatch, QString* toolTipText, int& toolTipIndex);

    /*!
     *  Adds the given match to the content assist.
     *
     *      @param [in] assist  The content assist.
     *      @param [in] match   The match to add.
     *      @param [in] type    The icon for the match.
     */
    static void addMatchToAssist(TextContentAssistWidget& assist, QString const& match, QIcon const& icon);

    /*!
     *  Finds the node description corresponding to the given node name.
     *
     *      @param [in] name The node name.
     *
     *      @return The corresponding node description, if found. Otherwise 0.
     */
    NodeDesc const* findNodeDesc(QString const& nodeName) const;

    /*!
     *  Finds the endpoint description corresponding to the given endpoint name.
     *
     *      @param [in] name The endpoint name.
     *
     *      @return The corresponding endpoint description, if found. Otherwise 0.
     */
    EndpointDesc const* findEndpointDesc(QString const& name) const;

    /*!
     *  Finds a connection where the local endpoint has the given name.
     *
     *      @param [in] name The name of the local endpoint.
     *
     *      @return The corresponding connection description, if found. Otherwise 0.
     */
    ConnectionDesc const* findConnectionDesc(QString const& name) const;

    /*!
     *  Extracts parameter names to a list from a parameter list string.
     *
     *      @param [in]  paramsListStr  The parameter list string.
     *      @param [out] params         The extracted parameter names.
     */
    static void extractParams(QString const& paramsListStr, QStringList& params);

    /*!
     *  Tries to match all available content to the parameter.
     *
     *      @param [in] funcDesc   The function description.
     *      @param [in] params     The previous parameters for the function.
     *      @param [in] word       The partial content of the parameter to match.
     *      @param [in] func       The match execution function.
     *      @param [in/out] count  The accumulated number of matches.
     */
    void tryMatchParam(MCAPIFunctionDesc const* funcDesc, QStringList const& params, QString const& word,
                       MatchExecFunc func, int& count);
    /*!
     *  Called for each enumerated node.
     *
     *      @param [in]     desc   The enumerated node desc.
     *      @param [in      exp    The regular expression to use for matching.
     *      @param [in]     func   The match execution function.
     *      @param [in/out] count  The accumulated number of matches.
     */
    void tryMatchNodeParam(NodeDesc const& desc, QRegExp& exp, MatchExecFunc func, int& count);

    /*!
     *  Called for each enumerated port.
     *
     *      @param [in]     desc   The enumerated port desc.
     *      @param [in      exp    The regular expression to use for matching.
     *      @param [in]     func   The match execution function.
     *      @param [in/out] count  The accumulated number of matches.
     */
    void tryMatchPortParam(EndpointDesc const& desc, QRegExp& exp, MatchExecFunc func, int& count);

    /*!
     *  Tries to match any endpoints of the given node to the parameter description.
     *
     *      @param [in]     node       The node.
     *      @param [in]     paramDesc  The parameter description.
     *      @param [in]     matchExp   The regular expression to use for matching.
     *      @param [in]     func       The match execution function.
     *      @param [in/out] count      The accumulated number of matches.
     */
    void tryMatchEndpoints(NodeDesc const& node, unsigned int paramDesc, QRegExp matchExp,
                           MatchExecFunc func, int& count);

    /*!
     *  Tries to match an identifier to the regular expression.
     *
     *      @param [in]     identifier The identifier.
     *      @param [in]     type       The identifier's MCAPI content type.
     *      @param [in]     exp        The regular expression to use for matching.
     *      @param [in]     func       The match execution function.
     *      @param [in/out] count      The accumulated number of matches.
     *
     *      @return True, if the identifier matched with the regular expression. Otherwise false.
     */
    bool tryMatchIdentifier(QString const& identifier, MCAPIContentType type,
                            QRegExp& exp, MatchExecFunc func, int& count);

    /*!
     *  Checks the parameter desc against the MCAPI types for validity.
     *
     *      @param [in] paramDesc     The parameter description flags.
     *      @param [in] endpointType  The MCAPI endpoint type.
     *      @param [in] connType      The MCAPI connection type.
     *
     *      @return True, if the parameter matches with the MCAPI types. Otherwise false.
     */
    static bool isEndpointValid(unsigned int paramDesc, MCAPIEndpointDirection endpointType,
                                MCAPIDataType connType);

    /*!
     *  Checks whether the connection is valid so that content matching should be performed.
     *
     *      @param [in]  endpointName       The name of one of the endpoints in the connection.
     *      @param [in]  localDesc          The parameter description of the local endpoint.
     *      @param [in]  remoteDesc         The parameter description of the remote endpoint.
     *      @param [out] otherEndpointName  The name of the other (missing) endpoint.
     *
     *      @return True, if the connection is valid. Otherwise false.
     */
    bool isConnectionValid(QString const& endpointName, unsigned int localDesc, unsigned int remoteDesc,
                           QString& otherEndpointName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    static MCAPIFunctionSet s_functionSet;

    //! Content icons.
    QIcon m_icons[MCAPI_CONTENT_TYPE_COUNT];

    //! The sender node description.
    NodeDesc m_localNode;

    //! The receiver node descriptions.
    QList<NodeDesc> m_remoteNodes;

    //! Available endpoint connections.
    ConnectionDescList m_connections;

    //! Last found match type.
    MatchType m_lastMatchType;

    //! The current assist that is being filled with content.
    TextContentAssistWidget* curAssist_;

    //! The generated tooltip text.
    QString m_tooltipText;
};

//-----------------------------------------------------------------------------

#endif // MCAPICONTENTMATCHER_H
