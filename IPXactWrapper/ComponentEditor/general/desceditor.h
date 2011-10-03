//-----------------------------------------------------------------------------
// File: desceditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 7.2.2011
//
// Description:
// Description editor for the component editor.
//-----------------------------------------------------------------------------

#ifndef DESCEDITOR_H
#define DESCEDITOR_H

#include <QGroupBox>
#include <QPlainTextEdit>

//-----------------------------------------------------------------------------
//! DescEditor class.
//-----------------------------------------------------------------------------
class DescEditor : public QGroupBox
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    DescEditor(QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~DescEditor();

    /*!
     *  Sets the description to the widget.
     *
     *      @param [in] The description string.
     */
    void setDescription(QString const& desc);

    /*!
     *  Returns the description string.
     */
    QString getDescription() const;

signals:
    //! Emitted when the contents of the editor have changed.
    void contentChanged();

private:
    // Disable copying.
    DescEditor(DescEditor const& rhs);
    DescEditor& operator=(DescEditor const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The text edit for the description.
    QPlainTextEdit* m_descEdit;
};

#endif // DESCEDITOR_H