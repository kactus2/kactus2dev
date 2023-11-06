//-----------------------------------------------------------------------------
// File: ModeReferenceEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 10.8.2023
//
// Description:
// Editor for mode references.
//-----------------------------------------------------------------------------

#ifndef MODEREFERENCEEDITOR_H
#define MODEREFERENCEEDITOR_H

#include "ModeReferenceDelegate.h"
#include "ModeReferenceModel.h"

class EditableTableView;

class ModeReferenceEditor : public QWidget
{
    Q_OBJECT

public:

    /*!
     *	The constructor.
     *  
     *      @param [in] model        The mode reference model to use.
     *      @param [in] parent       The parent widget.
     */
    ModeReferenceEditor(ModeReferenceModel* model, QWidget* parent);

    virtual ~ModeReferenceEditor() = default;

    ModeReferenceEditor(ModeReferenceEditor& other) = delete;
    ModeReferenceEditor& operator=(ModeReferenceEditor& other) = delete;

    /*!
     *	Set the model mode references.
     *  
     *      @param [in] modeRefs     The mode references to set.
     */
    void setModeRefs(QList<QPair<QString, int> > modeRefs) const;

    /*!
     *	Get the mode referneces from the model.
     *  
     * 	    @return The mode references as a list.
     */
    QList<QPair<QString, int> > getModeRefs() const;

signals:

    void contentChanged();

private:

    /*!
     *	Setup the layout of the editor.
     */
    void setupLayout();

    //! The mode reference model.
    ModeReferenceModel* model_;

    //! The view displaying the modes and priorities in a table.
    EditableTableView* view_;
};

#endif // MODEREFERENCEEDITOR_H
