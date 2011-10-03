//-----------------------------------------------------------------------------
// File: MCAPIFunctionSet.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.1.2011
//
// Description:
// MCAPI function set for easy management of function content.
//-----------------------------------------------------------------------------

#ifndef MCAPIFUNCTIONSET_H
#define MCAPIFUNCTIONSET_H

#include <QString>
#include <QList>

#include "MCAPIFunctionDesc.h"

//-----------------------------------------------------------------------------
//! MCAPIFunctionSet class.
//-----------------------------------------------------------------------------
class MCAPIFunctionSet
{
public:
    /*!
     *  Constructor.
     */
    MCAPIFunctionSet();

    /*!
     *  Destructor.
     */
     ~MCAPIFunctionSet();

     /*!
      *  Retrieves the function description at the given index.
      */
     MCAPIFunctionDesc const& getAt(unsigned int index) const;

    /*!
     *  Finds the function description corresponding to the given function name.
     *
     *      @param [in] name The function name.
     *
     *      @return The corresponding function description, if found. Otherwise 0.
     */
    MCAPIFunctionDesc const* find(QString const& name) const;

    /*!
     *  Returns the number of functions in the set.
     */
    unsigned int getCount() const;

private:
    // Disable copying.
    MCAPIFunctionSet(MCAPIFunctionSet const& rhs);
    MCAPIFunctionSet& operator=(MCAPIFunctionSet const& rhs);

    /*!
     *  Builds the function list.
     */
    void buildFunctionList();

    /*!
     *  Adds a function description to the function set.
     *
     *      @param [in] name        The function name.
     *      @param [in] returnType  The return type of the function.
     *
     *      @return The newly-created function description.
     *
     *      @remarks Assumes that there is no function already with exactly the same name.
     */
    MCAPIFunctionDesc* addFunctionDesc(QString const& name, VariableType returnType = VAR_TYPE_UNDEFINED,
                                       QString const& returnHint = "void");

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of known functions.
    QList<MCAPIFunctionDesc> m_functions;
};

#endif // MCAPIFUNCTIONSET_H