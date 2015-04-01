//-----------------------------------------------------------------------------
// File: ParameterEditorHeaderView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.12.2014
//
// Description:
// Header view for parameter editor.
//-----------------------------------------------------------------------------

#ifndef PARAMETEREDITORHEADERVIEW_H
#define PARAMETEREDITORHEADERVIEW_H

#include <QHeaderView>

//-----------------------------------------------------------------------------
//! Header view used to drawing the header.
//-----------------------------------------------------------------------------
class ParameterEditorHeaderView : public QHeaderView
{

public:
    /*!
     *  The constructor for the header.
     *
     *      @param [in] orientation     The headers orientation.
     *      @param [in] parent          The parent of the header.
     */
    ParameterEditorHeaderView(Qt::Orientation orientation, QWidget* parent = 0);

    virtual ~ParameterEditorHeaderView();

protected:
    
    /*!
     *  The re-implemented paint section.
     *
     *      @param [in] painter         The given painter.
     *      @param [in] rect            The given rectangle.
     *      @param [in] logicalIndex    Specifies the section.
     */
    virtual void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;

    /*!
     *  Gets the description column.
     *
     *      @return     The index of description column.
     */
    virtual int descriptionColumn() const;

    /*!
     *  Gets the value column.
     *
     *      @return     The index of value column.
     */
    virtual int valueColumn() const;

    /*!
     *  Gets the maximum value column.
     *
     *      @return     The index of maximum value column.
     */
    virtual int maximumColumn() const;

    /*!
     *  Gets the column index for array right.
     *
     *      @return     The index of array right column.
     */
    virtual int arrayRightColumn() const;

private:

    //! Disable copying and assignment
    ParameterEditorHeaderView(const ParameterEditorHeaderView& other);
    ParameterEditorHeaderView& operator=(const ParameterEditorHeaderView& other);
};

#endif // PARAMETEREDITORHEADERVIEW_H