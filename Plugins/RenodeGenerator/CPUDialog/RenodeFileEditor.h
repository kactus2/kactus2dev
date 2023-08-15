//-----------------------------------------------------------------------------
// File: RenodeFileEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.05.2023
//
// Description:
// Editor selecting files and file names for Renode generation.
//-----------------------------------------------------------------------------

#ifndef RENODEFILEEDITOR_H
#define RENODEFILEEDITOR_H

#include <QWidget>
#include <QJsonObject>

class RenodeFileSelectionGroup;

#include <QLineEdit>

//-----------------------------------------------------------------------------
//! Editor selecting files and file names for Renode generation.
//-----------------------------------------------------------------------------
class RenodeFileEditor : public QWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] configurationObject     JSON object containing the editor configuration.
     *      @param [in] parent                  Pointer to the owner of this widget.
     */
    RenodeFileEditor(QJsonObject const& configurationObject, QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~RenodeFileEditor() = default;

    /*!
     *  Check if the CPU file should be written.
     *
     *      @return True, if the CPU file should be written, false otherwise.
     */
    bool writeCpu();

    /*!
     *  Check if the memory file should be written.
     *
     *      @return True, if the memory file should be written, false otherwise.
     */
    bool writeMemory();

    /*!
     *  Check if the peripherals file should be written.
     *
     *      @return True, if the peripherals file should be written, false otherwise.
     */
    bool writePeripherals();

    /*!
     *  Get the name of the CPU file.
     *
     *      @return Name of the CPU file.
     */
    QString getCpuFileName() const;

    /*!
     *  Get the current text in the CPU editor.
     *
     *      @return Current text in the CPU editor.
     */
    QString getCpuEditorText() const;

    /*!
     *  Get the name of the memory file.
     *
     *      @return Name of the memory file.
     */
    QString getmemoryFileName() const;

    /*!
     *  Get the current text in the memory editor.
     *
     *      @return Current text in the memory editor.
     */
    QString getMemoryEditorText() const;

    /*!
     *  Get the name of the peripherals file.
     *
     *      @return Name of the peripherals file.
     */
    QString getPeripheralFileName() const;

    /*!
     *  Get the current text in the peripherals editor.
     *
     *      @return Current text in the peripherals editor.
     */
    QString getPeripheralEditorText() const;

    //! No copying. No assignment.
    RenodeFileEditor(const RenodeFileEditor& other) = delete;
    RenodeFileEditor& operator=(const RenodeFileEditor& other) = delete;

public slots:

    /*!
     *  Change the placeholder text in the file editors to match the new CPU name.
     *
     *      @param [in] newCpuName  The name of the new CPU.
     */
    void changeFileNames(QString const& newCpuName);

private:

    /*!
     *  Template for getting the file name from the selected editor.
     *
     *      @param [in] editor  The selected file editor.
     *
     *      @return Name of the selected file.
     */
    template <typename fileEditor>
    QString getFileName(fileEditor* editor) const;

    /*!
     *  Apply the configuration to file name editors.
     *
     *      @param [in] configurationObject     JSON object containing the editor configuration.
     */
    void applyConfigurations(QJsonObject const& configurationObject);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Group for selecting files to generate.
    RenodeFileSelectionGroup* fileSelectionGroup_;

    //! Editor for CPU file name.
    QLineEdit* cpuFileNameEditor_;

    //! Editor for memory file name.
    QLineEdit* memoryFileNameEditor_;

    //! Editor for peripherals file name.
    QLineEdit* peripheralFileNameEditor_;
};

#endif // RENODEFILEEDITOR_H
