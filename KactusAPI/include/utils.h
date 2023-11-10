/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 *         filename: utils.h
 *         
 *         Description: This file contains declarations for general purpose 
 *         functions that can be used in the whole software.
 */

#ifndef UTILS_H
#define UTILS_H

#include <KactusAPI/KactusAPIGlobal.h>

#include <QRegularExpression>
#include <QObject>

namespace Utils
{
    //! Contains firmness search settings.
    struct ImplementationOptions
    {
        //! If true then hardware components should be included in search.
        bool hw_;

        //! If true then software components should be included in search.
        bool sw_;

        //! If true then system components should be included in search.
        bool system_;

        /*! The default constructor
         *
         * Constructs struct with all options set to true.
        */
        KACTUS2_API ImplementationOptions();
    };

    //! Contains the search settings for hierarchy.
    struct HierarchyOptions
    {
        //! If true then global objects should be included in search.
        bool flat_;

        //! If true then product objects should be included in search.
        bool product_;

        //! If true then board objects should be included in search.
        bool board_;

        //! If true then chip objects should be included in search.
        bool chip_;

        //! If true then soc objects should be included in search.
        bool soc_;

        //! If true then ip objects should be included in search.
        bool ip_;

        /*! The default constructor.
         *
         * Constructs struct with all options set to true.
        */
        KACTUS2_API HierarchyOptions();
    };

    //! Contains the Re-usability search filters.
    struct FirmnessOptions
    {
        //! If true then templates should be included in search.
        bool templates_;

        //! If true then mutables should be included in search.
        bool mutable_;

        //! If true then fixeds should be included in search.
        bool fixed_;

        /*! The default constructor
         *
         * Constructs struct with all options set to true
        */
        KACTUS2_API FirmnessOptions();
    };

    //! Contains the search options for document types.
    struct TypeOptions
    {
        //! If true then components should be included in search.
        bool components_;

        //! If true then bus definitions should be included in search.
        bool buses_;

        //! If true then catalogs should be included in search.
        bool catalogs_;

        //! If true then APIs/Coms should be included in search.
        bool apis_;

        //! If true then other IP-Xact types should be included in search.
        bool advanced_;

        /*! The default constructor
         *
         * Constructs struct with all options set to true.
        */
        KACTUS2_API TypeOptions();
    };

    struct FilterOptions
    {
        //! Contains Type settings.
        Utils::TypeOptions type;

        //! Contains Implementation settings.
        Utils::ImplementationOptions implementation;

        //! Contains Hierarchy settings.
        Utils::HierarchyOptions hierarchy;

        //! Contains Firmness settings.
        Utils::FirmnessOptions firmness;

        /*! The default constructor
         *
         * Constructs struct with all options set to true.
        */
        KACTUS2_API FilterOptions() : type(), implementation(), hierarchy(), firmness() {};
    };


    //! Regular expression to validate URLs.
    inline const QRegularExpression URL_VALIDITY_REG_EXP(
        "^[a-z]{3,9}[:][/]{2}[a-z0-9]+([-.][a-z0-9]+)*([/][-a-z0-9_.;,?=&%#~+]*)*$",
        QRegularExpression::CaseInsensitiveOption);

    /*!
     *  Retrieves and returns the name of the current user.
     */
    KACTUS2_API QString getCurrentUser();
}

#endif // UTILS_H
