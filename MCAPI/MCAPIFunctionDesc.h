//-----------------------------------------------------------------------------
// File: MCAPIFunctionDesc.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.1.2011
//
// Description:
// MCAPI function description class.
//-----------------------------------------------------------------------------

#ifndef MCAPIFUNCTIONDESC_H
#define MCAPIFUNCTIONDESC_H

#include <QString>
#include <QList>

//-----------------------------------------------------------------------------
//! VariableType enumeration.
//-----------------------------------------------------------------------------
enum VariableType
{
    VAR_TYPE_UNDEFINED          = 0,

    VAR_TYPE_LOCAL_NODE_ID      = 0x01,
    VAR_TYPE_REMOTE_NODE_ID     = 0x02,
    VAR_TYPE_NODE_ID            = VAR_TYPE_LOCAL_NODE_ID |  VAR_TYPE_REMOTE_NODE_ID,

    VAR_TYPE_LOCAL_PORT_ID      = 0x04,
    VAR_TYPE_REMOTE_PORT_ID     = 0x08,
    VAR_TYPE_PORT_ID            = VAR_TYPE_LOCAL_PORT_ID | VAR_TYPE_REMOTE_PORT_ID,

    VAR_TYPE_LOCAL_ENDPOINT     = 0x10,
    VAR_TYPE_REMOTE_ENDPOINT    = 0x20,
    VAR_TYPE_ENDPOINT           = VAR_TYPE_LOCAL_ENDPOINT | VAR_TYPE_REMOTE_ENDPOINT,

    VAR_TYPE_STATUS_PTR         = 0x40,

    // Additional flags
    VAR_TYPE_CONN_IN            = 0x80,
    VAR_TYPE_CONN_OUT           = 0x100,
    VAR_TYPE_CONN_MSG           = 0x200,
    VAR_TYPE_CONN_SCALAR        = 0x400,
    VAR_TYPE_CONN_PACKET        = 0x800,
    VAR_TYPE_CONN_ANY           = VAR_TYPE_CONN_IN | VAR_TYPE_CONN_OUT | VAR_TYPE_CONN_PACKET |
                                  VAR_TYPE_CONN_SCALAR | VAR_TYPE_CONN_MSG,

    //VAR_TYPE_POINTER            = 0x1000,
    /* 3 more flags can be placed here */
};

//-----------------------------------------------------------------------------
//! MCAPIFunctionDesc class.
//-----------------------------------------------------------------------------
class MCAPIFunctionDesc
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] name        The name of the function.
     *      @param [in] returnType  The type of the return value.
     *      @param [in] returnHint  The hint text for the return value.
     */
    MCAPIFunctionDesc(QString name, VariableType returnType, QString const& returnHint);

    /*!
     *  Copy constructor.
     *
     *      @param [in] rhs The function description to copy.
     */
    MCAPIFunctionDesc(MCAPIFunctionDesc const& rhs);

    /*!
     *  Destructor.
     */
    ~MCAPIFunctionDesc();

    /*! 
     *  Adds a parameter to the function description.
     *
     *      @param [in] desc  The parameter description.
     *      @param [in] hint  The hint text for tooltips.
     */
    void addParam(unsigned int desc, QString const& hint);
    
    /*!
     *  Generates a tooltip text for the function prototype.
     *
     *      @param [in]  paramIndex  The index of the parameter to highlight.
     *      @param [out] text        The tool tip text.
     */
    void generateToolTipText(unsigned int paramIndex, QString& text) const;

    /*! 
     *  Returns the name of the function.
     */
    QString const& getName() const;

    /*!
     *  Returns the type of the i:th parameter.
     *
     *      @param [in] index The index of the parameter. Must be on the range 0 to getParamCount() - 1.
     *
     *      @return The parameter type description.
     */
    unsigned int getParamType(unsigned int index) const;

    /*!
     *  Returns the number of parameters for the function.
     */
    unsigned int getParamCount() const;

    /*!
     *  Assignment operator.
     */
    MCAPIFunctionDesc& operator=(MCAPIFunctionDesc const& rhs);

    //-----------------------------------------------------------------------------
    //! VariableDesc structure.
    //-----------------------------------------------------------------------------
    struct VariableDesc
    {
        unsigned int type;  // The variable type description (see VariableType).
        QString hint;       // The hint text for the variable.

        /*!
         *  Constructor.
         */
        VariableDesc(unsigned int type_, QString const& hint_) : type(type_), hint(hint_)
        {
        }
    };

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //!< The name of the function.
    QString m_name;

    //!< The return variable description.
    VariableDesc m_returnValueDesc;

    //!< The function parameters.
    QList<VariableDesc> m_params;
};

//-----------------------------------------------------------------------------

#endif // MCAPIFUNCTIONDESC_H