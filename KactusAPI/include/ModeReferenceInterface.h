//-----------------------------------------------------------------------------
// File: ModeReferenceInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 26.10.2023
//
// Description:
// Interface for accessing mode references.
//-----------------------------------------------------------------------------

#ifndef MODEREFERENCEINTERFACE_H
#define MODEREFERENCEINTERFACE_H

#include <KactusAPI/KactusAPIGlobal.h>

#include <QList>
#include <QSharedPointer>

class ModeReference;
class Mode;

class KACTUS2_API ModeReferenceInterface
{

public:

    ModeReferenceInterface() = default;

    virtual ~ModeReferenceInterface() = default;

    /*!
     *	Set the available mode references.
     *  
     *      @param [in] modeReferences     The mode references to set.
     */
    void setModeReferences(std::vector<std::pair<unsigned int, std::string> > const& modeReferences);

    /*!
     *	Get the number of mode references.
     *
     * 	    @return The number of mode references within the containing element.
     */
    int getModeReferenceCount() const;

    /*!
     *	Get the mode reference value for a selected mode reference.
     *
     *      @param [in] modeReferenceIndex  The index of the mode reference whose reference value to get.
     *
     * 	    @return The mode reference value.
     */
    std::string getModeReferenceValue(int modeReferenceIndex) const;

    /*!
     *	Get the mode reference priority for a selected mode reference.
     *
     *      @param [in] modeReferenceIndex  The index of the mode reference whose priority to get.
     *
     * 	    @return The mode reference priority.
     */
    int getModeReferencePriority(int modeReferenceIndex) const;

    /*!
     *	Set the mode reference value for a selected mode reference.
     *
     *      @param [in] modeReferenceIndex  The index of the mode reference whose reference value to set.
     *      @param [in] newValue            The new reference to set.
     *
     * 	    @return True, if the operation was successful, otherwise false.
     */
    bool setModeReferenceValue(int modeReferenceIndex, std::string const& newValue);

    /*!
     *	Set the mode reference priority for a selected mode reference.
     *
     *      @param [in] modeReferenceIndex  The index of the mode reference whose reference priority to set.
     *      @param [in] newPriority         The new priority value to set.
     *
     * 	    @return True, if the operation was successful, otherwise false.
     */
    bool setModeReferencePriority(int modeReferenceIndex, unsigned int newPriority);

    /*!
     *	Add a new mode reference.
     *
     * 	    @return True, if the operation was successful, otherwise false.
     */
    bool addModeReference(int row);

    /*!
     *	Remove a selected mode reference.
     *
     *      @param [in] modeReferenceIndex      The index of the mode reference to be removed.
     *
     * 	    @return True, if the operation was successful, otherwise false.
     */
    bool removeModeReference(int modeReferenceIndex);

    /*!
     *	Validate the reference value of given mode reference.
     *  
     *      @param [in] modeReferenceIndex     The index of the mode reference to validate.
     *	    
     * 	    @return True, if the mode reference value is valid, otherwise false.
     */
    bool modeReferenceValueIsValid(int modeReferenceIndex) const;
    
    /*!
     *	Validate the priority value of given mode reference.
     *
     *      @param [in] modeReferenceIndex     The index of the mode reference to validate.
     *
     * 	    @return True, if the mode reference priority is valid, otherwise false.
     */
    bool modeReferencePriorityIsValid(int modeReferenceIndex) const;
    
    /*!
     *	Get all mode references of the current element as a vector of strings.
     *  
     * 	    @return Vector containing the mode references.
     */
    std::vector<std::string> getModeReferencesString() const;

    /*!
     *	Set to indicate that the current element is a memory remap.
     *  
     *      @param [in] isRemap     Flag to set, if element is remap.
     */
    void setContainingElementIsRemap(bool isRemap);

    /*!
     *	Set mode references in the containing element, eg. mode references of all remaps within a memory map.
     *  Should set all mode references except for the mode refs of access policy/remap currently being edited.
     *  
     *      @param [in] modeRefs     The mode references to set.
     */
    void setContainingElementModeReferences(std::vector<std::pair<unsigned int, std::string> > const& modeReferences);

    /*!
     *	Get the available component modes.
     *  
     * 	    @return The available component modes.
     */
    std::vector<std::string> getComponentModes() const;

    /*!
     *	Set the available component modes.
     *  
     *      @param [in] newModes     The modes to set.
     */
    void setComponentModes(QSharedPointer<QList<QSharedPointer<Mode> > > newModes);

    /*!
     *	Get all currently set mode references for the current element.
     *  
     * 	    @return The currently set mode references.
     */
    std::vector<std::pair<unsigned int, std::string> > getModeReferences() const;

    /*!
     *	Checks if all mode references are errorless.
     *
     * 	    @return Boolean true if componentModes_ is not empty and every item is errorless, overwise false
     */
    bool areAllModeReferencesValid();

private:
    
    /*!
     *	Get the mode references that are currently in use, except for the mode reference being validated.
     *  
     *      @param [in] modeReferenceIndex     The index of the mode references to validate.
     *	    
     * 	    @return The mode references currently in use by other elements.
     */
    std::vector<std::pair<unsigned int, std::string> > getModeReferencesInUse(int modeReferenceIndex) const;

    //! Should contain all mode references except for the mode refs of access policy/remap currently being edited.
    std::vector<std::pair<unsigned int, std::string> > containingElementModeReferences_;

    //! The mode references of the item currently being edited.
    std::vector<std::pair<unsigned int, std::string> > modeReferences_;

    //! List of available component modes.
    QSharedPointer<QList<QSharedPointer<Mode> > > componentModes_;

    //! For validation purposes. Indicates if the mode refs are within a memory remap or not.
    bool containingElementIsRemap_ = false;

};

#endif // MODEREFERENCEINTERFACE_H
