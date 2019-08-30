/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QStyle>

//-----------------------------------------------------------------------------
//! A space determined self adjusting layout.
//-----------------------------------------------------------------------------
class FlowLayout : public QLayout
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent      The parent widget.
     *      @param [in] margin      Margin around the layout.
     *      @param [in] hSpacing    Horizontal spacing of the items.
     *      @param [in] vSpacing    Vertical spacing of the items.
     */
    explicit FlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);

    /*!
     *  The constructor.
     *
     *      @param [in] margin      Margin around the layout.
     *      @param [in] hSpacing    Horizontal spacing of the items.
     *      @param [in] vSpacing    Vertical spacing of the items.
     */
    explicit FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);

    /*!
     *  The destructor.
     */
    ~FlowLayout();

    /*!
     *  Add a layout item to the layout.
     *
     *      @param [in] item    The selected layout item.
     */
    void addItem(QLayoutItem *item) override;

    /*!
     *  Get the current horizontal spacing.
     *
     *      @return The current horizontal spacing.
     */
    int horizontalSpacing() const;

    /*!
     *  Get the current vertical spacing.
     *
     *      @return The current vertical spacing.
     */
    int verticalSpacing() const;

    /*!
     *  Returns the direction in which the layout can use more space than allocated.
     *
     *      @return 0.
     */
    Qt::Orientations expandingDirections() const override;

    /*!
     *  Checks whether the layouts preferred height depends on width.
     *
     *      @return True.
     */
    bool hasHeightForWidth() const override;

    /*!
     *  Returns the preferred height for the selected width.
     *
     *      @param [in] width   The selected width.
     *
     *      @return The preferred height for the selected width.
     */
    int heightForWidth(int width) const override;

    /*!
     *  Get the number of items in the layout.
     *
     *      @return The number of items in the layout.
     */
    int count() const override;

    /*!
     *  Get the item at the selected index.
     *
     *      @param [in] index   Index of the selected item.
     *
     *      @return Item at the selected index.
     */
    QLayoutItem *itemAt(int index) const override;

    /*!
     *  Get the minimum size of this layout.
     *
     *      @return The minimum size of this layout.
     */
    QSize minimumSize() const override;

    /*!
     *  Calculates the geometry of the layouts items.
     *
     *      @param [in] rect    Rectangle of the available area.
     */
    void setGeometry(const QRect &rect) override;

    /*!
     *  Get the preferred size of the layout.
     *
     *      @return The preferred size of the layout.
     */
    QSize sizeHint() const override;

    /*!
     *  Remove the selected item from the layout.
     *
     *      @param [in] index   Index of the selected item.
     *
     *      @return The removed item.
     */
    QLayoutItem *takeAt(int index) override;

private:

    /*!
     *  Handles the layout.
     *
     *      @param [in] rect        Rectangle containing the layout.
     *      @param [in] testOnly    Holds whether the placement is being tested.
     */
    int doLayout(const QRect &rect, bool testOnly) const;

    /*!
     *  Get the default spacing.
     *
     *      @param [in] pm  The selected pixel metric.
     */
    int smartSpacing(QStyle::PixelMetric pm) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the items in the layout.
    QList<QLayoutItem *> itemList_;

    //! Horizontal item spacing in the layout.
    int m_hSpace_;

    //! Vertical item spacing in the layout.
    int m_vSpace_;
};
//! [0]

#endif // FLOWLAYOUT_H