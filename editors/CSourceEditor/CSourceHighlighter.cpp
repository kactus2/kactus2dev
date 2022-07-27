//-----------------------------------------------------------------------------
// File: CSourceHighlighter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 21.12.2010
//
// Description:
// MCAPI/C syntax highlighter.
//-----------------------------------------------------------------------------

#include "CSourceHighlighter.h"

#include <IPXACTmodels/kactusExtensions/ApiDefinition.h>
#include <IPXACTmodels/kactusExtensions/ApiFunction.h>

//-----------------------------------------------------------------------------
// Function: CSourceHighlighter()
//-----------------------------------------------------------------------------
CSourceHighlighter::CSourceHighlighter(QTextDocument* parent): LanguageHighlighter(parent)
{
   
}

//-----------------------------------------------------------------------------
// Function: ~CSourceHighlighter()
//-----------------------------------------------------------------------------
CSourceHighlighter::~CSourceHighlighter()
{
}


//-----------------------------------------------------------------------------
// Function: CSourceHighlighter::registerAPI()
//-----------------------------------------------------------------------------
void CSourceHighlighter::registerAPI(QSharedPointer<ApiDefinition const> apiDef)
{
    // Register data type rules.
    foreach (QString const& dataType, *apiDef->getDataTypes())
    {
        addRule(QRegularExpression("\\b" + dataType + "\\b"), API_DATA_TYPES);
    }

	foreach (QSharedPointer<ApiFunction> apiFunc, *apiDef->getFunctions())
    {
        addRule(QRegularExpression("\\b" + apiFunc->name() + "\\b"), API_FUNCTIONS);
    }
}
