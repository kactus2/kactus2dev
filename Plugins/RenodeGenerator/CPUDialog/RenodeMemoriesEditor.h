//-----------------------------------------------------------------------------
// File: RenodeMemoriesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 20.03.2023
//
// Description:
// Editor for CPU memory details in renode generation.
//-----------------------------------------------------------------------------

#ifndef RENODEMEMORIESEDITOR_H
#define RENODEMEMORIESEDITOR_H

#include <QWidget>
#include <QObject>
#include <QTableView>

#include <Plugins/RenodeGenerator/CPUDialog/RenodeStructs.h>

class RenodeMemoriesModel;

//-----------------------------------------------------------------------------
//! Editor for CPU memory details in renode generation.
//-----------------------------------------------------------------------------
class RenodeMemoriesEditor : public QWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Pointer to the owner of this widget.
     */
    RenodeMemoriesEditor(QWidget *parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~RenodeMemoriesEditor() = default;

    /*!
     *  Setup the selected CPUs.
     *
     *      @param [in] memories    List of the available memory items.
     */
    void setupMemories(QVector<QSharedPointer<RenodeStructs::cpuMemories> > memories);

    //! No copying. No assignment.
    RenodeMemoriesEditor(const RenodeMemoriesEditor& other) = delete;
    RenodeMemoriesEditor& operator=(const RenodeMemoriesEditor& other) = delete;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The view for the table.
    QTableView* view_;

    //! The containing model.
    RenodeMemoriesModel* model_;
};

#endif // RENODEMEMORIESEDITOR_H
