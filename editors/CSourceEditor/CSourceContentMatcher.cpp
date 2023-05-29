//-----------------------------------------------------------------------------
// File: CSourceContentMatcher.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 11.1.2011
//
// Description:
// MCAPI-specific content matcher.
//-----------------------------------------------------------------------------

#include "CSourceContentMatcher.h"

#include <QTextCursor>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/kactusExtensions/ApiDefinition.h>
#include <IPXACTmodels/kactusExtensions/ApiFunction.h>
#include <IPXACTmodels/kactusExtensions/ApiFunctionParameter.h>
#include <IPXACTmodels/kactusExtensions/ComDefinition.h>
#include <IPXACTmodels/kactusExtensions/ComInterface.h>

#include <common/widgets/assistedTextEdit/TextContentAssistWidget.h>

//-----------------------------------------------------------------------------
// Function: CSourceContentMatcher()
//-----------------------------------------------------------------------------
CSourceContentMatcher::CSourceContentMatcher()
    : ownerComponent_(),
      sourceApiDefinitions_(),
      sourceComDefinitions_(),
      lastMatchType_(MATCH_NONE),
      curAssist_(0),
      tooltipText_()
{
    icons_[MCAPI_CONTENT_FUNC] = QIcon(":icons/common/graphics/mcapi-func.png");
    icons_[MCAPI_CONTENT_TYPENAME] = QIcon(":icons/common/graphics/mcapi-type.png");
    icons_[MCAPI_CONTENT_NODE_ID] = QIcon(":icons/common/graphics/mcapi-node.png");
    icons_[MCAPI_CONTENT_PORT_ID] = QIcon(":icons/common/graphics/mcapi-port.png");
    icons_[MCAPI_CONTENT_ENDPOINT] = QIcon(":icons/common/graphics/mcapi-endpoint.png");
    icons_[MCAPI_CONTENT_ENDPOINT_HANDLE] = QIcon(":icons/common/graphics/mcapi-endpoint_handle.png");
}

//-----------------------------------------------------------------------------
// Function: ~CSourceContentMatcher()
//-----------------------------------------------------------------------------
CSourceContentMatcher::~CSourceContentMatcher()
{
}

//-----------------------------------------------------------------------------
// Function: lookForwardMatch()
//-----------------------------------------------------------------------------
bool CSourceContentMatcher::lookForwardMatch(QString const& text)
{
    int index = 0;
    int toolTipIndex = 0;
    return (lastMatchType_ != MATCH_NONE &&
            enumerateMatches(text, 0, index, 0, toolTipIndex) == lastMatchType_);
}

//-----------------------------------------------------------------------------
// Function: fillWithContent()
//-----------------------------------------------------------------------------
bool CSourceContentMatcher::fillWithContent(QString const& text, TextContentAssistWidget& assist,
                                          int& startIndex, QString& toolTipText, int& toolTipIndex)
{
    curAssist_ = &assist;

    toolTipText.clear();    
    lastMatchType_ = enumerateMatches(text, &CSourceContentMatcher::addMatchToAssist,
                                       startIndex, &toolTipText, toolTipIndex);

    curAssist_->sortItems();

    return (lastMatchType_ != MATCH_NONE);
}

//-----------------------------------------------------------------------------
// Function: enumerateMatches()
//-----------------------------------------------------------------------------
CSourceContentMatcher::MatchType CSourceContentMatcher::enumerateMatches(QString const& text, MatchExecFunc func,
                                                                     int& startIndex, QString* toolTipText,
                                                                     int& toolTipIndex)
{
    static QRegularExpression funcCallExp("[a-z|A-Z|_][a-z|A-Z|0-9|_]*\\([a-z|A-Z|0-9|_|.|,|&|\\s]*$");

    bool exactMatch = false;
    int count = 0;
    
    // Enumerate function parameters if a function call is found.
    if (text.indexOf(funcCallExp) >= 0)
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
int CSourceContentMatcher::enumerateNames(QString const &text, MatchExecFunc func, int &startIndex,
                                        bool& exactMatch)
{
    exactMatch = false;
    int count = 0;

    // Check if the last word could be a name.
    static QRegularExpression lastWordExp("[a-z|A-Z|_][a-z|A-Z|0-9|_]*$");
    startIndex = text.indexOf(lastWordExp, 0);
    auto wordMatch = lastWordExp.match(text);

    if (wordMatch.hasMatch())
    {
        QString word = wordMatch.captured(); //text.mid(startIndex, wordMatch.capturedLength());
        QRegularExpression matchExp("^" + QRegularExpression::escape(word.toLower()) + ".*");

        for (QSharedPointer<ApiDefinition const> apiDef : sourceApiDefinitions_)
        {
			// Search for functions that start with the retrieved word.
			for ( QSharedPointer<ApiFunction> apiFunc : *apiDef->getFunctions() )
            {
                if (tryMatchIdentifier(apiFunc->name(), MCAPI_CONTENT_FUNC, matchExp, func, count))
                {
                    // Check if this was an exact match.
                    if (!exactMatch && apiFunc->name() == word)
                    {
                        exactMatch = true;
                    }
                }
            }

            // Search for types that start with the retrieved word.
            for (QString const& dataType : *apiDef->getDataTypes())
            {
                if (tryMatchIdentifier(dataType, MCAPI_CONTENT_TYPENAME, matchExp, func, count))
                {
                    // Check if this was an exact match.
                    if (!exactMatch && dataType == word)
                    {
                        exactMatch = true;
                    }
                }
            }
        }
    }

    return count;
}

//-----------------------------------------------------------------------------
// Function: enumerateFunctionParams()
//-----------------------------------------------------------------------------
int CSourceContentMatcher::enumerateFunctionParams(QString const &text, MatchExecFunc func,
                                                 int &startIndex, bool& exactMatch,
                                                 QString* toolTipText, int& toolTipIndex)
{
    // Used static regular expressions.
    static QRegularExpression funcCallExp("[a-z|A-Z|_][a-z|A-Z|0-9|_]*\\([a-z|A-Z|0-9|_|,|.|&|\\s]*$");
    static QRegularExpression lastParamExp("[(|,]\\s*(&)?([a-z|A-Z|0-9|_][a-z|A-Z|0-9|_|.]*)?$");
    static QRegularExpression callStartExp("\\([a-z|A-Z|0-9|_|,|.|&|\\s]*$");
    
    exactMatch = false;
    int count = 0;

    // Find the beginning of the function call.
    int funcStartIndex = text.indexOf(funcCallExp, 0);
    Q_ASSERT(funcStartIndex >= 0);
    
    // Search for the last param.
    int index = text.indexOf(lastParamExp, funcStartIndex);
    auto paramMatch = lastParamExp.match(text, funcStartIndex);

    if (index >= 0)
    {
        // If found, discard the leading whitespaces, left parentheses and commas.
        startIndex = text.indexOf(QRegularExpression("[&|a-z|A-Z|0-9|_]"), index);

        if (startIndex < 0)
        {
            startIndex = index + paramMatch.capturedLength();
        }

        QString word = text.mid(startIndex, paramMatch.capturedLength() - (startIndex - index));
        
        // Extract the function name & parameter list from the text.
        index = text.indexOf(callStartExp, funcStartIndex);
        Q_ASSERT(index >= 0);

        QString funcName = text.mid(funcStartIndex, index - funcStartIndex);
        QString paramListText = text.mid(index + 1);

        // Extract previous parameters to a list for easy access.
        QStringList params;
        extractParams(paramListText, params);

        // Based on the function description, try to match content.
        ApiFunction const* matchingApiFunc = 0;

        for (QSharedPointer<ApiDefinition const> apiDef : sourceApiDefinitions_)
		{
			for ( QSharedPointer<ApiFunction> apiFunc : *apiDef->getFunctions() )
            {
                if (apiFunc->name() == funcName)
                {
                    matchingApiFunc = apiFunc.data();
                    break;
                }
            }
        }

        if (matchingApiFunc != 0 && params.count() < matchingApiFunc->getParamCount())
        {
            // Generate a tooltip text.
            if (toolTipText != 0)
            {
                matchingApiFunc->generateToolTipText(params.count(), *toolTipText);
                toolTipIndex = index + 1;
            }

            tryMatchParam(matchingApiFunc, params, word, func, count);
        }
    }

    return count;
}

//-----------------------------------------------------------------------------
// Function: addMatchToAssist()
//-----------------------------------------------------------------------------
void CSourceContentMatcher::addMatchToAssist(TextContentAssistWidget& assist, QString const& match,
                                           QIcon const& icon)
{
    if (assist.findItems(match, Qt::MatchExactly).isEmpty())
    {
        assist.addItem(new QListWidgetItem(icon, match));
    }
}

//-----------------------------------------------------------------------------
// Function: CSourceContentMatcher::tryMatchParam()
//-----------------------------------------------------------------------------
void CSourceContentMatcher::tryMatchParam(ApiFunction const* apiFuncDesc, QStringList const& params,
                                        QString const& word, MatchExecFunc func, int& count)
{
    // Find out the parameter index.
    int paramIndex = params.count();

    QRegularExpression matchExp("^" + QRegularExpression::escape(word.toLower()) + ".*");

    // Based on the function parameter description, try to find matching content.
    QSharedPointer<ApiFunctionParameter const> apiParam = apiFuncDesc->getParam(paramIndex);

    // Check if the parameter is dependent on another parameter.
    if (apiParam->getDependentParameterIndex() >= 0)
    {
        int index =  apiParam->getDependentParameterIndex();

        if (index < params.count() && index < apiFuncDesc->getParamCount())
        {
            QSharedPointer<ApiFunctionParameter const> dependentParam =
                apiFuncDesc->getParam(apiParam->getDependentParameterIndex());

            // Find the matching COM interface.
            foreach (QSharedPointer<ComInterface> comIf, ownerComponent_->getComInterfaces())
            {
                QString value = comIf->name();

                if (dependentParam->getContentSource() != "Name")
                {
                    value = QString::fromStdString(comIf->getPropertyValues().value(dependentParam->getContentSource().toStdString(), std::string()));
                }

                if (value == params.at(index) &&
                    (comIf->getDirection() == dependentParam->getComDirection() ||
                     dependentParam->getComDirection() == DirectionTypes::DIRECTION_INVALID) &&
                    (comIf->getTransferType() == dependentParam->getComTransferType() ||
                     dependentParam->getComTransferType() == "" ||
                     dependentParam->getComTransferType() == "any"))
                {
                    QString identifier  = "";

                    if (apiParam->getContentSource() == "Name")
                    {
                        identifier = comIf->name();
                    }
                    else
                    {
                        identifier = QString::fromStdString(comIf->getPropertyValues().value(apiParam->getContentSource().toStdString(), ""));
                    }

                    if (apiParam->getType().contains('*'))
                    {
                        identifier = "&" + identifier;
                    }

                    tryMatchIdentifier(identifier, MCAPI_CONTENT_ENDPOINT, matchExp, func, count);                    
                    break;
                }
            }
        }
    }
    else if (!apiParam->getContentSource().isEmpty())
    {
        // Otherwise enumerate all COM interfaces that match with the requirements.
        foreach (QSharedPointer<ComInterface> comIf, ownerComponent_->getComInterfaces())
        {
            if ((comIf->getDirection() == apiParam->getComDirection() ||
                apiParam->getComDirection() == DirectionTypes::DIRECTION_INVALID) &&
                (comIf->getTransferType() == apiParam->getComTransferType() ||
                 apiParam->getComTransferType() == "" ||
                 apiParam->getComTransferType() == "any"))
            {
                QString identifier = "";

                if (apiParam->getContentSource() == "Name")
                {
                    identifier = comIf->name();
                }
                else
                {
                    identifier = QString::fromStdString(comIf->getPropertyValues().value(apiParam->getContentSource().toStdString(), std::string()));
                }

                if (apiParam->getType().contains('*'))
                {
                    identifier = "&" + identifier;
                }

                tryMatchIdentifier(identifier, MCAPI_CONTENT_ENDPOINT, matchExp, func, count);
            }
        }
    }
}


//-----------------------------------------------------------------------------
// Function: tryMatchIdentifier()
//-----------------------------------------------------------------------------
bool CSourceContentMatcher::tryMatchIdentifier(QString const& identifier, MCAPIContentType type,
                                             QRegularExpression& exp, MatchExecFunc func, int& count)
{
    if (identifier.toLower().contains(exp))
    {
        if (func != 0)
        {
            func(*curAssist_, identifier, icons_[type]);
        }

        ++count;

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: extractParams()
//-----------------------------------------------------------------------------
void CSourceContentMatcher::extractParams(QString const& paramsListStr, QStringList& params)
{
    static QRegularExpression paramExtractExp("([a-z|A-Z|0-9|_][^,]*)?(?=\\s*[,|$])");

    // Clear the parameter list.
    params.clear();

    // Extract parameters one-by-one.
    auto paramMatch = paramExtractExp.match(paramsListStr);

    while (paramMatch.hasMatch())
    {
        params.push_back(paramMatch.captured());

        // Find the next comma to prepare for extracting the next parameter.
        int index = paramMatch.capturedStart();
        int nextComma = paramsListStr.indexOf(',', index + paramMatch.capturedLength());

        // If not found, there are no more complete parameters (only the one that is
        // being written at the moment).
        if (nextComma < 0)
        {
            break;
        }

        paramMatch = paramExtractExp.match(paramsListStr, nextComma + 1);
    }
}

//-----------------------------------------------------------------------------
// Function: CSourceContentMatcher::addSourceApiDefinition()
//-----------------------------------------------------------------------------
void CSourceContentMatcher::addSourceApiDefinition(QSharedPointer<ApiDefinition const> apiDef)
{
    sourceApiDefinitions_.append(apiDef);
}

//-----------------------------------------------------------------------------
// Function: CSourceContentMatcher::addSourceComDefinition()
//-----------------------------------------------------------------------------
void CSourceContentMatcher::addSourceComDefinition(QSharedPointer<ComDefinition const> comDef)
{
    sourceComDefinitions_.append(comDef);
}

//-----------------------------------------------------------------------------
// Function: CSourceContentMatcher::setOwner()
//-----------------------------------------------------------------------------
void CSourceContentMatcher::setOwner(QSharedPointer<Component> component)
{
    ownerComponent_ = component;
}

