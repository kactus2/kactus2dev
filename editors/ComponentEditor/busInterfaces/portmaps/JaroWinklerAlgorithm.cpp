//-----------------------------------------------------------------------------
// File: JaroWinklerAlgorithm.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 24.10.2016
//
// Description:
// Implementation of Jaro-Winkler distance algorithm for string similarity measurement.
//-----------------------------------------------------------------------------

#include "JaroWinklerAlgorithm.h"

#include <QtMath>

//-----------------------------------------------------------------------------
// Function: JaroWinklerAlgorithm::calculateJaroDistance()
//-----------------------------------------------------------------------------
double JaroWinklerAlgorithm::calculateJaroDistance(QString const& firstString, QString const& secondString)
{
    QString firstMatch = Details::findMatchingCharacters(firstString, secondString);
    QString secondMatch = Details::findMatchingCharacters(secondString, firstString);

    double matchingCharacters = firstMatch.size();
    
    double jaroDistance = 0;

    if (matchingCharacters != 0 && firstMatch.size() == secondMatch.size())
    {
        double transpositions = Details::getTranspositions(firstMatch, secondMatch);

        double firstPart = matchingCharacters / firstString.length();
        double secondPart = matchingCharacters / secondString.length();
        double transpositionPart = (matchingCharacters - transpositions) / matchingCharacters;

        jaroDistance = (firstPart + secondPart + transpositionPart) / 3;
    }

    return jaroDistance;
}

//-----------------------------------------------------------------------------
// Function: JaroWinklerAlgorithm::calculateJaroWinklerDistance()
//-----------------------------------------------------------------------------
double JaroWinklerAlgorithm::calculateJaroWinklerDistance(QString const& firstString, QString const& secondString, 
    double winklerScalingFactor)
{
    double jaroDistance = calculateJaroDistance(firstString, secondString);
    double winklerDistance = Details::calculateWinklerDistance(firstString, secondString,
        jaroDistance, winklerScalingFactor);

    return jaroDistance + winklerDistance;
}

//-----------------------------------------------------------------------------
// Function: JaroWinklerAlgorithm::Details::findMatchingCharacters()
//-----------------------------------------------------------------------------
QString JaroWinklerAlgorithm::Details::findMatchingCharacters(QString const& searched,
    QString const& reference)
{
    int matchingDistance = qMax(1, qMax(searched.length(), reference.length())/2 - 1);

    QString matchingCharacters = "";

    for (int i = 0; i < searched.size(); ++i)
    {
        int startPosition = qMax(0, i - matchingDistance);
        int endPosition = qMin(i + matchingDistance, reference.size() - 1);

        int sectionLength = endPosition - startPosition + 1;

        QStringRef referenceSection = reference.midRef(startPosition, sectionLength);
        if (referenceSection.contains(searched.at(i), Qt::CaseInsensitive))
        {
            matchingCharacters.append(searched.at(i).toLower());
        }
    }

    return matchingCharacters;
}

//-----------------------------------------------------------------------------
// Function: JaroWinklerAlgorithm::Details::getTranspositions()
//-----------------------------------------------------------------------------
int JaroWinklerAlgorithm::Details::getTranspositions(QString const& firstMatch, QString const& secondMatch)
{
    int transpositions = 0;
    for (int i = 0; i < firstMatch.size(); i++)
    {
        if (firstMatch.at(i) != secondMatch.at(i))
        {
            transpositions++;
        }
    }

    return transpositions;
}

//-----------------------------------------------------------------------------
// Function: JaroWinklerAlgorithm::Details::calculateWinklerDistance()
//-----------------------------------------------------------------------------
double JaroWinklerAlgorithm::Details::calculateWinklerDistance(QString const& firstString,
    QString const& secondString, double jaroDistance, double scalingFactor)
{
    int prefixLength = Details::findCommonPrefixLength(firstString, secondString);

    return prefixLength*scalingFactor*(1 - jaroDistance);
}

//-----------------------------------------------------------------------------
// Function: JaroWinklerAlgorithm::Details::findCommonPrefixLength()
//-----------------------------------------------------------------------------
int JaroWinklerAlgorithm::Details::findCommonPrefixLength(QString const& firstString, QString const& secondString)
{
    const int MAX_PREFIX_LENGTH = 4;
    const int MAX_STEPS = qMin(MAX_PREFIX_LENGTH, qMin(firstString.length(), secondString.length()));

    int prefixLength = 0;

    for (int i = 0; i < MAX_STEPS && i == prefixLength; i++)
    {
        if (firstString.at(i).toLower() == secondString.at(i).toLower())
        {
            prefixLength++;
        }
    }  

    return prefixLength;
}
