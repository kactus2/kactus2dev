//-----------------------------------------------------------------------------
// File: MCAPIFunctionDesc.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.1.2011
//
// Description:
// MCAPI function description class.
//-----------------------------------------------------------------------------

#include "MCAPIFunctionDesc.h"

//-----------------------------------------------------------------------------
// Function: MCAPIFunctionDesc()
//-----------------------------------------------------------------------------
MCAPIFunctionDesc::MCAPIFunctionDesc(QString name, VariableType returnType,
                                     QString const& returnHint) : m_name(name),
                                                                  m_returnValueDesc(returnType, returnHint),
                                                                  m_params()
{
}

//-----------------------------------------------------------------------------
// Function: MCAPIFunctionDesc()
//-----------------------------------------------------------------------------
MCAPIFunctionDesc::MCAPIFunctionDesc(MCAPIFunctionDesc const& rhs) : m_name(rhs.m_name),
                                                                     m_returnValueDesc(rhs.m_returnValueDesc),
                                                                     m_params(rhs.m_params)
{
}

//-----------------------------------------------------------------------------
// Function: ~MCAPIFunctionDesc()
//-----------------------------------------------------------------------------
MCAPIFunctionDesc::~MCAPIFunctionDesc()
{
}

//-----------------------------------------------------------------------------
// Function: addParam()
//-----------------------------------------------------------------------------
void MCAPIFunctionDesc::addParam(unsigned int desc, QString const& hint)
{
    m_params.push_back(VariableDesc(desc, hint));
}

//-----------------------------------------------------------------------------
// Function: generateToolTip()
//-----------------------------------------------------------------------------
void MCAPIFunctionDesc::generateToolTipText(unsigned int paramIndex, QString& text) const
{
    text.clear();

    text += "<p style='white-space:pre'>";

    // First add the return value hint and the function name.
    text += m_returnValueDesc.hint + ' ' + m_name + '(';

    // Then add the function parameters.
    for (int i = 0; i < m_params.size(); ++i)
    {
        // Highlight the parameter of the given index.
        if (i == paramIndex)
        {
            text += "<b>" + m_params.at(i).hint + "</b>";
        }
        else
        {
            text += m_params.at(i).hint;
        }

        // Add comma only if this is not the last parameter.
        if (i < m_params.size() - 1)
        {
            text += ", ";
        }
    }

    text += ')';
}

//-----------------------------------------------------------------------------
// Function: getName()
//-----------------------------------------------------------------------------
QString const& MCAPIFunctionDesc::getName() const
{
    return m_name;
}

//-----------------------------------------------------------------------------
// Function: getParamType()
//-----------------------------------------------------------------------------
unsigned int MCAPIFunctionDesc::getParamType(unsigned int index) const
{
    Q_ASSERT(index < getParamCount());
    return m_params.at(index).type;
}

//-----------------------------------------------------------------------------
// Function: getParamCount()
//-----------------------------------------------------------------------------
unsigned int MCAPIFunctionDesc::getParamCount() const
{
    return m_params.count();
}

//-----------------------------------------------------------------------------
// Function: operator=()
//-----------------------------------------------------------------------------
MCAPIFunctionDesc& MCAPIFunctionDesc::operator=(MCAPIFunctionDesc const& rhs)
{
    if (this != &rhs)
    {
        m_name = rhs.m_name;
        m_returnValueDesc = rhs.m_returnValueDesc;
        m_params = rhs.m_params;
    }
    
    return *this;
}