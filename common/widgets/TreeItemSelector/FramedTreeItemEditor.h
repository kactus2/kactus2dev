//-----------------------------------------------------------------------------
// File: FramedTreeItemEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 6.9.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#ifndef FRAMED_TREEITEMEDITOR_H
#define FRAMED_TREEITEMEDITOR_H

#include <QFrame>
#include <QCheckBox>

#include <common/widgets/TreeItemSelector/TreeItemEditor.h>

#include <editors/ComponentEditor/common/MemoryTreeFactory.h>

//-----------------------------------------------------------------------------
//! Editor for enumerations.
//-----------------------------------------------------------------------------
class FramedTreeItemEditor : public QFrame
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent          The parent widget.
     */
    FramedTreeItemEditor(MemoryTreeFactory* factory, QWidget* parent);

    /*!
     *  Destructor.
     */
    virtual ~FramedTreeItemEditor() = default;

    // Disable copying.
    FramedTreeItemEditor(FramedTreeItemEditor const& rhs) = delete;
    FramedTreeItemEditor& operator=(FramedTreeItemEditor const& rhs) = delete;

    void selectPath(QStringList const& path);

    QStringList selectedPath() const;

signals:

    /*!
     *  Signal for canceling the editing.
     */
    void cancelEditing();

    /*!
     *  Signal for accepting the edits.
     */
    void finishEditing();

private slots:

    void validateSelection();

private:
    /*!
     *  Setup the layout.
     */
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The editor for tree items.
    TreeItemEditor* editor_;


    QPushButton* okButton_ = new QPushButton(this);

    QPushButton* cancelButton_ = new QPushButton(this);
};

#endif // FRAMED_TREEITEMEDITOR_H
