//-----------------------------------------------------------------------------
// File: RenodeFileSelectionGroup.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.04.2023
//
// Description:
// Group for selecting files to be written in Renode generation.
//-----------------------------------------------------------------------------

#ifndef RENODEFILESELECTIONGROUP_H
#define RENODEFILESELECTIONGROUP_H

#include <QCheckBox>
#include <QGroupBox>
#include <QObject>
#include <QJsonObject>

//-----------------------------------------------------------------------------
//! Group for selecting files to be written in Renode generation.
//-----------------------------------------------------------------------------
class RenodeFileSelectionGroup : public QGroupBox
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] configurationObject     JSON object containing the editor configuration.
     *    @param [in] parent                  Pointer to the owner of this widget.
     */
    RenodeFileSelectionGroup(QJsonObject const& configurationObject, QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~RenodeFileSelectionGroup() = default;

    /*!
     *  Check if the CPU file should be written.
     *
     *    @return True, if the CPU file should be written, false otherwise.
     */
    bool writeCpu();

    /*!
     *  Check if the memory file should be written.
     *
     *    @return True, if the memory file should be written, false otherwise.
     */
    bool writeMemory();

    /*!
     *  Check if the peripherals file should be written.
     *
     *    @return True, if the peripherals file should be written, false otherwise.
     */
    bool writePeripherals();


    //! No copying. No assignment.
    RenodeFileSelectionGroup(const RenodeFileSelectionGroup& other) = delete;
    RenodeFileSelectionGroup& operator=(const RenodeFileSelectionGroup& other) = delete;

private slots:

    /*!
     *  Handle the state change in all check boxes.
     *
     *    @param [in] newState    The new state.
     */
    void onCheckAllStateChange(bool newState);

    /*!
     *  Handle the state change in a single check box. Change the state in the select all -check box accordingly.
     *
     *    @param [in] newState    The new state.
     */
    void onItemClicked(bool newState);

private:

    /*!
     *  Apply the configuration to file selections.
     *
     *    @param [in] configurationObject     JSON object containing the editor configuration.
     */
    void applyConfigurations(QJsonObject const& configurationObject);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Check box for selecting all the other check boxes.
    QCheckBox* selectAll_;

    //! Check box for selecting the CPU file to be written.
    QCheckBox* cpuCheck_;

    //! Check box for selecting the memory file to be written.
    QCheckBox* memoryCheck_;
    
    //! Check box for selecting the peripherals file to be written.
    QCheckBox* peripheralsCheck_;

    //! Container for ease of access to the check boxes.
    QVector<QCheckBox*> boxStorage_;
};

#endif // RENODEFILESELECTIONGROUP_H
