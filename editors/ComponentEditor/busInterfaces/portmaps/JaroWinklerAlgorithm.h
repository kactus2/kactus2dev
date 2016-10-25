//-----------------------------------------------------------------------------
// File: JaroWinklerAlgorithm.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 24.10.2016
//
// Description:
// Implementation of Jaro-Winkler distance algorithm for string similarity measurement.
//-----------------------------------------------------------------------------

#ifndef JAROWINKLERALGORITHM_H
#define JAROWINKLERALGORITHM_H

#include <QString>

//-----------------------------------------------------------------------------
//! Implementation of Jaro-Winkler distance algorithm for string similarity measurement.
//-----------------------------------------------------------------------------
namespace JaroWinklerAlgorithm
{
    /*!
     *  Calculate a Jaro distance for two strings.
     *
     *      @param [in] firstString     The first string in comparison.
     *      @param [in] secondString    The second string in comparison.
     *
     *      @return The similarity metric between the two strings on a scale of 0 to 1.0 where 1.0 is exact match.
     */
    double calculateJaroDistance(QString const& firstString, QString const& secondString);

    /*!
     *  Calculate a Jaro-Winkler distance for two strings. Favors strings with similar prefixes.
     *
     *      @param [in] firstString             The first string in comparison.
     *      @param [in] secondString            The second string in comparison.
     *      @param [in] winklerScalingFactor    The scaling factor for prefix. Should not exceed 0.25.
     *
     *      @return The similarity metric between the two strings on a scale of 0 to 1.0.
     */
    double calculateJaroWinklerDistance(QString const& firstString, QString const& secondString,
        double winklerScalingFactor = 0.1);

    namespace Details
    {
        /*!
        *  Get the matching characters from the selected strings.
        *
        *      @param [in] searched             The string whose characters are searched in the reference.
        *      @param [in] reference            The string whose characters are matched.
        *
        *      @return The matching characters.
        */
        QString findMatchingCharacters(QString const& searched, QString const& reference);

        /*!
        *  Get the number of transpositions from the matched strings.
        *
        *      @param [in] firstMatch      The first string.
        *      @param [in] secondMatch     The second string.
        *
        *      @return The number of character movements required to make the selected strings equal.
        */
        int getTranspositions(QString const& firstMatch, QString const& secondMatch);
        
        /*!
        *  Calculate the Winkler distance from the Jaro distance.
        *
        *      @param [in] firstString     The first string in comparison.
        *      @param [in] secondString    The second string in comparison.
        *      @param [in] jaroDistance    The calculated Jaro distance.
        *      @param [in] scalingFactor   The scaling factor for the prefix. Should not exceed 0.25.
        *
        *      @return The similarity between two strings, weighted by the length of the prefix and scaling factor.
        */
        double calculateWinklerDistance(QString const& firstString, QString const& secondString,
            double jaroDistance, double scalingFactor);
        
        /*!
         *  Finds the number of characters in a common prefix for two strings.
         *
         *      @param [in] firstString     The first string.
         *      @param [in] secondString    The second string.
         *
         *      @return The number of common characters in the beginning of the strings.
         */
        int findCommonPrefixLength(QString const& firstString, QString const& secondString);
    };
};

//-----------------------------------------------------------------------------

#endif // JAROWINKLERALGORITHM_H
