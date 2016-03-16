//-----------------------------------------------------------------------------
// File: ComponentInstantiation.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.09.2015
//
// Description:
// Describes the ipxact:componentInstantiation element.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANTIATION_H
#define COMPONENTINSTANTIATION_H

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/common/FileBuilder.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Describes the ipxact:componentInstantiation element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComponentInstantiation : public NameGroup, public Extendable
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] name            Name of the instantiation.
     *      @param [in] displayName     Display name of the instantiation.
     *      @param [in] description     Description of the instantiation.
     */
    ComponentInstantiation(QString const& name = QString(), QString const& displayName = QString(),
        QString const& description = QString());

    /*!
     *  The copy constructor.
     */
    ComponentInstantiation(const ComponentInstantiation& other);

    /*!
     *  The assignment operator.
     */
    ComponentInstantiation& operator=(const ComponentInstantiation& other);

    /*!
     *  The destructor.
     */
    ~ComponentInstantiation();

    /*!
     *  Check if the instantiation is virtual.
     *
     *      @return True, if the instantiation is virtual, false otherwise.
     */
    bool isVirtual() const;

    /*!
     *  Set the virtuality status of the instantiation.
     *
     *      @param [in] virtualStatus   The virtuality status of the instantiation.
     */
    void setVirtual(bool virtualStatus);

    /*!
     *  Get the used HDL.
     *
     *      @return The used HDL.
     */
    QString getLanguage() const;

    /*!
     *  Set the used HDL.
     *
     *      @param [in] newLanguage     The used HDL.
     */
    void setLanguage(QString const& newLanguage);
   
    /*!
     *  Check if the language is strict.
     *
     *      @return True, if the language is strict, false otherwise.
     */
    bool isLanguageStrict() const;

    /*!
     *  Set the language strictness.
     *
     *      @param [in] virtualStatus   The language strictness.
     */
    void setLanguageStrictness(bool strictness);

    /*!
     *  Get the name of the library in which the model should be compiled.
     *
     *      @return The name of the library.
     */
    QString getLibraryName() const;

    /*!
     *  Set the name of the library in which the model should be compiled.
     *
     *      @param [in] newLanguage     The new name of the library.
     */
    void setLibraryName(QString const& newLibraryName);

    /*!
     *  Get the name of the VHDL package containing the interface of the model.
     *
     *      @return The name of the VHDL package.
     */
    QString getPackageName() const;

    /*!
     *  Set the name of the VHDL package containing the interface of the model.
     *
     *      @param [in] newPackageName  The name of the VHDL package.
     */
    void setPackageName(QString const& newPackageName);

    /*!
     *  Get the module name or the VHDL entity name.
     *
     *      @return The module or VHDL entity name.
     */
    QString getModuleName() const;

    /*!
     *  Set the module name or the VHDL entity name.
     *
     *      @param [in] newModuleName   The module or VHDL entity name.
     */
    void setModuleName(QString const& newModuleName);

    /*!
     *  Get the name of the VHDL architecture.
     *
     *      @return The name of the VHDL architecture.
     */
    QString getArchitectureName() const;

    /*!
     *  Set the name of the VHDL architecture.
     *
     *      @param [in] newArchitectureName     The name of the VHDL architecture.
     */
    void setArchitectureName(QString const& newArchitectureName);

    /*!
     *  Get the name of configuration.
     *
     *      @return The name of the configuration.
     */
    QString getConfigurationName() const;

    /*!
     *  Set the name of configuration.
     *
     *      @param [in] newConfigurationName    The name of the configuration.
     */
    void setConfigurationName(QString const& newConfigurationName);

    /*!
     *  Get the module parameters.
     *
     *      @return Pointer to a list containing module parameters.
     */
    QSharedPointer<QList<QSharedPointer<ModuleParameter> > > getModuleParameters() const;

    /*!
     *  Set the module parameters.
     *
     *      @param [in] newModuleParameters     The new module parameters.
     */
    void setModuleParameters(QSharedPointer<QList<QSharedPointer<ModuleParameter> > > newModuleParameters);

    /*!
     *  Get the default file builders.
     *
     *      @return A pointer to a list containing the default file builders.
     */
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > getDefaultFileBuilders() const;

    /*!
     *  Set the default file builders.
     *
     *      @param [in] newDefaultFileBuilders  The new file builders.
     */
    void setDefaultFileBuilders(QSharedPointer<QList<QSharedPointer<FileBuilder> > > newDefaultFileBuilders);

    /*!
     *  Get the file set references.
     *
     *      @return Pointer to a list containing referenced file set names.
     */
    QSharedPointer<QStringList> getFileSetReferences() const;

    /*!
     *  Set the file set references.
     *
     *      @param [in] newFileSetReferences    The names of the referenced file sets.
     */
    void setFileSetReferences(QSharedPointer<QStringList> newFileSetReferences);

    /*!
     *  Get the parameters.
     *
     *      @return A pointer to a list containing parameters.
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters() const;

    /*!
     *  Set the parameters.
     *
     *      @param [in] newParameters   The new parameters.
     */
    void setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters);

private:

    /*!
     *  Copy module parameters.
     */
    void copyModuleParameters(const ComponentInstantiation& other) const;

    /*!
     *  Copy default file builders.
     */
    void copyDefaultFileBuilders(const ComponentInstantiation& other) const;

    /*!
     *  Copy file set references.
     */
    void copyFileSetReferences(const ComponentInstantiation& other) const;

    /*!
     *  Copy parameters.
     */
    void copyParameters(const ComponentInstantiation& other) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The virtual status of the instantiation.
    bool virtuality_;

    //! The used HDL.
    QString language_;

    //! Determines whether the language shall be strictly enforced or not.
    bool languageStrictness_;

    //! The name of the library.
    QString libraryName_;

    //! The name of the VHDL package containing the interface of the model.
    QString packageName_;

    //! The name of the module or VHDL entity.
    QString moduleName_;

    //! The name of the VHDL architecture.
    QString architectureName_;

    //! The configuration.
    QString configurationName_;

    //! A list of module parameters.
    QSharedPointer<QList<QSharedPointer<ModuleParameter> > > moduleParameters_;

    //! A list of default file builders.
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > defaultFileBuilders_;

    //! A list of file set references.
    QSharedPointer<QStringList> fileSetReferences_;

    //! A list of instantiation parameters.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;
};

#endif // DESIGNINSTANTIATION_H