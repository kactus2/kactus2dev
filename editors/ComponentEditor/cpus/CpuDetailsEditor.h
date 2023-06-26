//-----------------------------------------------------------------------------
// File: CpuDetailsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 16.6.2023
//
// Description:
// Editor for cpu properties.
//-----------------------------------------------------------------------------

#ifndef CPU_DETAILS_EDITOR_H
#define CPU_DETAILS_EDITOR_H

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>

#include <common/widgets/EnumCollectionEditor/EnumCollectionEditor.h>

#include <KactusAPI/include/ExpressionParser.h>
#include <KactusAPI/include/ParameterFinder.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Cpu.h>

#include <QGroupBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Editor for cpu properties.
//-----------------------------------------------------------------------------
class CpuDetailsEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor
     *
     *      @param [in] component	The containing component.
     *      @param [in] cpu			The cpu whose properties to edit.
     *      @param [in] finder		The parameter finder to use.
	 *      @param [in] parent      The owner of this editor.
     */
    CpuDetailsEditor(QSharedPointer<Component> component, 
		QSharedPointer<Cpu> cpu, 
		QSharedPointer<ParameterFinder> finder,
		QSharedPointer<ExpressionParser> expressionParser,
		QWidget* parent = nullptr);

	/*!
     *  The destructor
     */
	virtual ~CpuDetailsEditor() = default;

    //! No copying
    CpuDetailsEditor(const CpuDetailsEditor& other) = delete;

    

    //! No assignment
    CpuDetailsEditor& operator=(const CpuDetailsEditor& other) = delete;

	/*! Restore the changes made in the editor back to ones in the model.
	*
	*/
	void refresh();


signals:

	//! Emitted when contents of the model change
	void contentChanged();

    /*!
     *  Increase the amount of references made to the given parameter.
     *
     *      @param [in] id  The id of the given parameter.
     */
    void increaseReferences(QString const& id) const;

    /*!
     *  Decrease the amount of references made to the given parameter.
     *
     *      @param [in] id  The id the given parameter.
     */
    void decreaseReferences(QString const& id) const;

private slots:

    //! Handler for changes in isPresent.
    void onIsPresentChanged();

    //! Handler for changes in address space references.
    void onAddressSpaceRefsChanged();

    //! Handler for changes in range.
    void onRangeChanged();

    //! Handler for changes in width.
    void onWidthChanged();

    //! Handler for changes in address unit bits.
    void onAddressUnitChanged();

	//! Handler for changes in memory map reference.
	void onMemoryMapRefChanged(QString const& selectedMemoryMap);

private:

    QString formattedValueFor(QString const& expression) const;
    
    void createEditors(QSharedPointer<ParameterFinder> finder);

    //! Sets the editor layout.
    void setupLayout();

	QSharedPointer<Component> component_;

	//! The edited cpu.
    QSharedPointer<Cpu> cpu_;

    //! The expression parser to use.
	QSharedPointer<ExpressionParser> expressionParser_;

    //! Editor for isPresent.
	ExpressionEditor* presenceEditor_ = nullptr;

    //! Editor for selecting address space references.
	EnumCollectionEditor* addressSpaceRefEditor_ = nullptr;

    //! Combo box to select an memory map within component
    ReferenceSelector* memoryMapRefSelector_ = nullptr;

    //! Editor for cpu address space range.
    ExpressionEditor* rangeEditor_ = nullptr;

    //! Editor for cpu address space width.
	ExpressionEditor* widthEditor_ = nullptr;

    //! Editor for cpu address space address unit bits.
	ExpressionEditor* aubEditor_ = nullptr;
};

#endif // CPU_DETAILS_EDITOR_H
