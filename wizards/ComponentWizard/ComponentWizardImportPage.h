//-----------------------------------------------------------------------------
// File: ComponentWizardImportPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Import page for the component wizard.
//-----------------------------------------------------------------------------

#ifndef COMPONENTWIZARDVHDLIMPORTPAGE_H
#define COMPONENTWIZARDVHDLIMPORTPAGE_H

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <wizards/ComponentWizard/InstanceData.h>

#include <QWizardPage>
#include <QSharedPointer>
#include <QVariant>

class Component;
class LibraryInterface;
class ImportEditor;
class Kactus2Value;
class Kactus2Group;
class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! Import page for the component wizard.
//-----------------------------------------------------------------------------
class ComponentWizardImportPage : public QWizardPage
{
    Q_OBJECT        
public:

    /*!
     *  The constructor.
     *
     *      @param [in] component               Pointer to the component being edited.
     *      @param [in] handler                 Pointer to the instance which manages the library.
     *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] busInterface            Interface for accessing bus interfaces.
     *      @param [in] parent                  The parent wizard.
     */
    ComponentWizardImportPage(QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer <ComponentParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        BusInterfaceInterface* busInterface,
        QWidget* parent);

    /*!
     *  Destructor.
     */
    ~ComponentWizardImportPage();

    /*!
     *  Returns the ID of the next page.
     */
    virtual int nextId() const;

    /*!
     *  Initialize the page to contain correct files to select the top-vhdl.
     */
    virtual void initializePage();


    /*!
     *  Check if the settings on the page are valid and user can move to next page.
     *
     *      @return True if all settings are valid, otherwise false.
     */
    virtual bool isComplete() const;

signals:

     //! Emitted when a new component has been imported.
    void componentChanged(QSharedPointer<Component>);

    /*!
     *  Increase the number of references to the selected parameter.
     *
     *      @param [in] id      The selected parameter.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Decrease the number of references to the selected parameter.
     *
     *      @param [in] id      The selected parameter.
     */
    void decreaseReferences(QString const& id);

private slots:

    /*!
     *  Handle the change in imported component.
     *
     *      @param [in] newComponent    The new component.
     */
    void onComponentChange(QSharedPointer<Component> newComponent);

private:
	// Disable copying.
	ComponentWizardImportPage(ComponentWizardImportPage const& rhs);
	ComponentWizardImportPage& operator=(ComponentWizardImportPage const& rhs);
    
    /*!
     *  Get the list of imported instances from the selected component.
     *
     *      @param [in] newComponent    The selected component.
     *
     *      @return List of imported instances from the selected component.
     */
    QVector<InstanceData::instanceData> getVerilogInstancesFromComponent(QSharedPointer<Component> newComponent)
        const;

    /*!
     *  Get the list of imported instances from the selected vendor extension.
     *
     *      @param [in] instancesGroup  Extension group for component instances.
     *
     *      @return List of imported instances from the selected extension group.
     */
    QVector<InstanceData::instanceData> getVerilogInstancesFromExtension(
        QSharedPointer<Kactus2Group> instancesGroup) const;

    /*!
     *  Get the string for the selected instance parameter.
     *
     *      @param [in] stringID        ID of the selected parameter.
     *      @param [in] extensionGroup  Vendor extension group containing the instance.
     *
     *      @return Instance data matching the ID.
     */
    QString getInstanceString(QString const& stringID, QSharedPointer<Kactus2Group> extensionGroup) const;

    /*!
     *  Get the string pairs for the selected instance parameter pair.
     *
     *      @param [in] itemGroupID     ID of the selected item group.
     *      @param [in] itemID          ID of the item pair.
     *      @param [in] extensionGroup  Vendor extension group for the parameter pairs.
     *
     *      @return Parameter pair data for the selected ID.
     */
    QVector<QPair<QString, QString> > getInstanceStringPairs(QString const& itemGroupID, QString const& itemID,
        QSharedPointer<Kactus2Group> extensionGroup) const;

    /*!
     *  Get a single value from the selected extension group.
     *
     *      @param [in] valueID         ID of the selected item.
     *      @param [in] extensionGroup  Vendor extension group for the parameter pairs.
     *
     *      @return Value of the selected ID.
     */
    QString getSingleValueFromGroup(QString const& valueID, QSharedPointer<Kactus2Group> extensionGroup) const;

	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

	//! Editor to set the generics and ports.
	ImportEditor* editor_;
};

#endif // COMPONENTWIZARDVHDLIMPORTPAGE_H
