//-----------------------------------------------------------------------------
// File: AccessPolicyInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 20.9.2023
//
// Description:
// Interface for editing access policies.
//-----------------------------------------------------------------------------

#ifndef ACCESSPOLICYINTERFACE_H
#define ACCESSPOLICYINTERFACE_H

#include <KactusAPI/KactusAPIGlobal.h>

#include <IPXACTmodels/common/AccessTypes.h>

#include <QList>
#include <QSharedPointer>

class AccessPolicy;
class ModeReferenceInterface;
class ModeReference;
class Mode;

//-----------------------------------------------------------------------------
//! Interface for editing access policies.
//-----------------------------------------------------------------------------
class KACTUS2_API AccessPolicyInterface
{
public:

    /*!
     *	The constructor.
     *  
     *      @param [in] modeRefInterface     Interface for accessing mode references.
     */
    explicit AccessPolicyInterface(ModeReferenceInterface* modeRefInterface);

    virtual ~AccessPolicyInterface() = default;

    //! No copying.
    AccessPolicyInterface(AccessPolicyInterface const& other) = delete;
    AccessPolicyInterface& operator=(AccessPolicyInterface const& other) = delete;

    /*!
     *	Set available access policies.
     *  
     *      @param [in] accessPolicies     Access policies to set.
     */
    void setAccessPolicies(QSharedPointer<QList<QSharedPointer<AccessPolicy> > > accessPolicies);

    /*!
     *	Set the available component modes for the interface.
     *  
     *      @param [in] componentModes     The available component modes.
     */
    void setComponentModes(QSharedPointer<QList<QSharedPointer<Mode> > > componentModes);

    /*!
     *	Get the access policy count.
     *	    
     * 	    @return The number of available access policies.
     */
    int getAccessPolicyCount() const;

    /*!
     *	Add a new access policy.
     */
    void addAccessPolicy(int row);

    /*!
     *	Remove an access policy given by index.
     *  
     *      @param [in] accessPolicyIndex     The index of the access policy to remove.
     *	    
     * 	    @return True, if the removal was successful, otherwise false.
     */
    bool removeAccessPolicy(int accessPolicyIndex);

    /*!
     *	Set the access value for a given accesspolicy.
     *  
     *      @param [in] newAccess             The new access value to set.
     *      @param [in] accessPolicyIndex     The index of the access policy to set the access for.
     *	    
     * 	    @return True, if the access value was successfully set, otherwise false.
     */
    bool setAccess(std::string const& newAccess, int accessPolicyIndex);

    /*!
     *	Get the access value as a string of a given access policy.
     *  
     *      @param [in] accessPolicyIndex     The index of the access policy.
     *
     * 	    @return The access value as a string.
     */
    std::string getAccessString(int accessPolicyIndex) const;

    /*!
     *	Get the access value of a given access policy.
     *  
     *      @param [in] accessPolicyIndex     The index of the access policy.
     *	    
     * 	    @return The access value.
     */
    AccessTypes::Access getAccess(int accessPolicyIndex) const;

    /*!
     *	Get the mode reference interface.
     *  
     * 	    @return The mode reference interface.
     */
    ModeReferenceInterface* getModeReferenceInterface() const;

    /*!
     *	Get the mode references of given access policy.
     *  
     *      @param [in] accessPolicyIndex     The index of the access policy whose mode references to get.
     *	    
     * 	    @return  The mode references of access policy given by index.
     */
    std::vector<std::pair<unsigned int, std::string> > getAccesPolicyModeReferences(int accessPolicyIndex) const;

    /*!
     *	Set mode references of an access policy.
     *  
     *      @param [in] accessPolicyIndex       The index of the access policy whose mode references to be set.
     *      @param [in] newModeRefs             The modereferences to be set.
     *	    
     * 	    @return True, if the operation was successful, otherwise false.
     */
    bool setAccessPolicyModeReferences(int accessPolicyIndex, std::vector<std::pair<unsigned int, std::string> > const& newModeRefs);

    /*!
     *	Get the mode references in use by other than selected access policy given by index.
     *  
     *      @param [in] accessPolicyIndex     The index of the access policy whose mode references are not to be
     *                                        included in the returned list.
     *	    
     * 	    @return All other mode references in use.
     */
    std::vector<std::pair<unsigned int, std::string> > getModeReferencesInUse(int accessPolicyIndex) const;

    /*!
     *	Validates the mode references of selected access policy. Compares against all mode references 
     *  in other access policies.
     *  
     *      @param [in] accessPolicyIndex     The index of the access policy whose mode references are validated.
     *	    
     * 	    @return True, if the mode references of selected accesspolicy are valid, otherwise false.
     */
    bool accessPolicyHasValidModeReferences(int accessPolicyIndex) const;

private:

    //! Available access policies.
    QSharedPointer<QList<QSharedPointer<AccessPolicy> > > accessPolicies_ =
        QSharedPointer<QList<QSharedPointer<AccessPolicy> > >(new QList<QSharedPointer<AccessPolicy> >());
    
    //! The interface for accessing access policy mode references.
    ModeReferenceInterface* modeRefInterface_;

    //! Available component modes.
    QSharedPointer<QList<QSharedPointer<Mode> > > componentModes_;
};


#endif // ACCESSPOLICYINTERFACE_H

