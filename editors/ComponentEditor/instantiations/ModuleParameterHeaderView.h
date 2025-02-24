//-----------------------------------------------------------------------------
// File: ModuleParameterHeaderView.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 12.12.2014
//
// Description:
// Header view for module parameter editor.
//-----------------------------------------------------------------------------

#ifndef MODELPARAMETEREDITORHEADERVIEW_H
#define MODELPARAMETEREDITORHEADERVIEW_H

#include "editors/ComponentEditor/parameters/ParameterEditorHeaderView.h"

//-----------------------------------------------------------------------------
//! Header view used to drawing the header.
//-----------------------------------------------------------------------------
class ModuleParameterHeaderView : public ParameterEditorHeaderView
{

public:
    /*!
     *  The constructor for the header.
     *
     *    @param [in] orientation     The headers orientation.
     *    @param [in] parent          The parent of the header.
     */
    ModuleParameterHeaderView(Qt::Orientation orientation, QWidget* parent = 0);

    virtual ~ModuleParameterHeaderView();

protected:
    
    /*!
     *  Gets the description column index.
     *
     *    @return     The description column index.
     */
    virtual int descriptionColumn() const;

    /*!
     *  Gets the value column index.
     *
     *    @return     The value column index.
     */
    virtual int valueColumn() const;

    /*!
     *  Gets the maximum column index.
     *
     *    @return     The maximum column index.
     */
    virtual int maximumColumn() const;

    /*!
     *  Gets the index of array right column.
     *
     *    @return     The array right column index.
     */
    virtual int arrayRightColumn() const;

private:

    //! Disable copying and assignment
    ModuleParameterHeaderView(const ParameterEditorHeaderView& other);
    ModuleParameterHeaderView& operator=(const ParameterEditorHeaderView& other);
};

#endif // MODELPARAMETEREDITORHEADERVIEW_H