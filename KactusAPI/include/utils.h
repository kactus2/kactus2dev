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

#include <QObject>
#include <QRegularExpression>

namespace Utils
{
    //! Contains firmness search settings.
    struct ImplementationOptions
    {
        //! If true then hardware components should be included in search.
        bool hw_{ true };

        //! If true then software components should be included in search.
        bool sw_{ true };

        //! If true then system components should be included in search.
        bool system_{ true };

        /*! The default constructor
         *
         * Constructs struct with all options set to true.
        */
        KACTUS2_API ImplementationOptions() = default;
    };

    //! Contains the search settings for hierarchy.
    struct HierarchyOptions
    {
        //! If true then global objects should be included in search.
        bool flat_{ true };

        //! If true then product objects should be included in search.
        bool product_{ true };

        //! If true then board objects should be included in search.
        bool board_{ true };

        //! If true then chip objects should be included in search.
        bool chip_{ true };

        //! If true then soc objects should be included in search.
        bool soc_{ true };

        //! If true then ip objects should be included in search.
        bool ip_{ true };

        /*! The default constructor.
         *
         * Constructs struct with all options set to true.
        */
        KACTUS2_API HierarchyOptions() = default;
    };

    //! Contains the Re-usability search filters.
    struct FirmnessOptions
    {
        //! If true then templates should be included in search.
        bool templates_{ true };

        //! If true then mutables should be included in search.
        bool mutable_{ true };

        //! If true then fixeds should be included in search.
        bool fixed_{ true };

        /*! The default constructor
         *
         * Constructs struct with all options set to true
        */
        KACTUS2_API FirmnessOptions() = default;
    };

    //! Contains the search options for document types.
    struct TypeOptions
    {
        //! If true then components should be included in search.
        bool components_{ true };

        //! If true then bus definitions should be included in search.
        bool buses_{ true };

        //! If true then catalogs should be included in search.
        bool catalogs_{ true };

        //! If true then APIs/Coms should be included in search.
        bool apis_{ true };

        //! If true then other IP-Xact types should be included in search.
        bool advanced_{ true };

        /*! The default constructor
         *
         * Constructs struct with all options set to true.
        */
        KACTUS2_API TypeOptions() = default;
    };

    struct ValidityOptions
    {
        //! If true then valid objects should be included in search.
        bool valid_{ true };

        //! If true then invalid objects should be included in search.
        bool invalid_{ true };

        /*! The default constructor
         *
         * Constructs struct with all options set to true.
         */
        KACTUS2_API ValidityOptions() = default;
    };

    struct RevisionOptions
    {
        //! If true then 2014 std objects should be included in search.
        bool std14_{ true };

        //! If true then 2022 std objects should be included in search.
        bool std22_{ true };

        /*! The default constructor
         *
         * Constructs struct with all options set to true.
         */
        KACTUS2_API RevisionOptions() = default;
    };

    struct FilterOptions
    {
        //! Contains Type settings.
        Utils::TypeOptions type{};

        //! Contains Implementation settings.
        Utils::ImplementationOptions implementation{};

        //! Contains Hierarchy settings.
        Utils::HierarchyOptions hierarchy{};

        //! Contains Firmness settings.
        Utils::FirmnessOptions firmness{};

        //! Contains Revision settings.
        Utils::RevisionOptions revision{};

        //! Contains Validity settings.
        Utils::ValidityOptions validity{};

        /*! The default constructor
         *
         * Constructs struct with all options set to true.
        */
        KACTUS2_API FilterOptions() = default;
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
