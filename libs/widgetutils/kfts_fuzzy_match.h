/*
    SPDX-FileCopyrightText: 2017 Forrest Smith
    SPDX-FileCopyrightText: 2020 Waqar Ahmed

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KFTS_FUZZY_MATCH_H
#define KFTS_FUZZY_MATCH_H

#include <QString>

namespace kfts
{
Q_DECL_UNUSED static bool fuzzy_match_simple(const QString pattern, const QString str);

Q_DECL_UNUSED static bool fuzzy_match(const QString pattern, const QString str, int &outScore);
Q_DECL_UNUSED static bool fuzzy_match(const QString pattern, const QString str, int &outScore, uint8_t *matches, int maxMatches);

Q_DECL_UNUSED static bool fuzzy_match_sequential(const QString pattern, const QString str, int &outScore);
Q_DECL_UNUSED static QString to_fuzzy_matched_display_string(const QString pattern, QString &str, const QString &htmlTag, const QString &htmlTagClose);
}

namespace kfts
{
namespace fuzzy_internal
{
static bool fuzzy_match_recursive(QString::const_iterator pattern,
                                  QString::const_iterator str,
                                  int &outScore,
                                  const QString::const_iterator strBegin,
                                  const QString::const_iterator strEnd,
                                  const QString::const_iterator patternEnd,
                                  uint8_t const *srcMatches,
                                  uint8_t *newMatches,
                                  int maxMatches,
                                  int nextMatch,
                                  int &recursionCount,
                                  int seqBonus = 15);
}

static bool fuzzy_match_simple(const QString pattern, const QString str)
{
    auto patternIt = pattern.cbegin();
    for (auto strIt = str.cbegin(); strIt != str.cend() && patternIt != pattern.cend(); ++strIt) {
        if (strIt->toLower() == patternIt->toLower()) {
            ++patternIt;
        }
    }
    return patternIt == pattern.cend();
}

static bool fuzzy_match(const QString pattern, const QString str, int &outScore)
{
    uint8_t matches[256];
    return fuzzy_match(pattern, str, outScore, matches, sizeof(matches));
}

static bool fuzzy_match(const QString pattern, const QString str, int &outScore, uint8_t *matches, int maxMatches)
{
    int recursionCount = 0;

    auto strIt = str.cbegin();
    auto patternIt = pattern.cbegin();
    const auto patternEnd = pattern.cend();
    const auto strEnd = str.cend();

    return fuzzy_internal::fuzzy_match_recursive(patternIt, strIt, outScore, strIt, strEnd, patternEnd, nullptr, matches, maxMatches, 0, recursionCount);
}

static bool fuzzy_match_sequential(const QString pattern, const QString str, int &outScore)
{
    int recursionCount = 0;
    uint8_t matches[256];
    auto maxMatches = sizeof(matches);
    auto strIt = str.cbegin();
    auto patternIt = pattern.cbegin();
    const auto patternEnd = pattern.cend();
    const auto strEnd = str.cend();

    return fuzzy_internal::fuzzy_match_recursive(patternIt, strIt, outScore, strIt, strEnd, patternEnd, nullptr, matches, maxMatches, 0, recursionCount, 40);
}

static bool fuzzy_internal::fuzzy_match_recursive(QString::const_iterator pattern,
                                                  QString::const_iterator str,
                                                  int &outScore,
                                                  const QString::const_iterator strBegin,
                                                  const QString::const_iterator strEnd,
                                                  const QString::const_iterator patternEnd,
                                                  const uint8_t *srcMatches,
                                                  uint8_t *matches,
                                                  int maxMatches,
                                                  int nextMatch,
                                                  int &recursionCount,
                                                  int seqBonus)
{
    static constexpr int recursionLimit = 10;
    ++recursionCount;
    if (recursionCount >= recursionLimit) {
        return false;
    }

    if (pattern == patternEnd || str == strEnd) {
        return false;
    }

    bool recursiveMatch = false;
    uint8_t bestRecursiveMatches[256];
    int bestRecursiveScore = 0;

    bool first_match = true;
    while (pattern != patternEnd && str != strEnd) {
        if (pattern->toLower() == str->toLower()) {
            if (nextMatch >= maxMatches) {
                return false;
            }

            if (first_match && srcMatches) {
                memcpy(matches, srcMatches, nextMatch);
                first_match = false;
            }

            uint8_t recursiveMatches[256];
            int recursiveScore;
            auto strNextChar = std::next(str);
            if (fuzzy_match_recursive(pattern,
                                      strNextChar,
                                      recursiveScore,
                                      strBegin,
                                      strEnd,
                                      patternEnd,
                                      matches,
                                      recursiveMatches,
                                      sizeof(recursiveMatches),
                                      nextMatch,
                                      recursionCount)) {
                if (!recursiveMatch || recursiveScore > bestRecursiveScore) {
                    memcpy(bestRecursiveMatches, recursiveMatches, 256);
                    bestRecursiveScore = recursiveScore;
                }
                recursiveMatch = true;
            }

            matches[nextMatch++] = (uint8_t)(std::distance(strBegin, str));
            ++pattern;
        }
        ++str;
    }

    bool matched = pattern == patternEnd ? true : false;

    if (matched) {
        int sequential_bonus = seqBonus;
        static constexpr int separator_bonus = 30;
        static constexpr int camel_bonus = 30;
        static constexpr int first_letter_bonus = 15;

        static constexpr int leading_letter_penalty = -5;
        static constexpr int max_leading_letter_penalty = -15;
        static constexpr int unmatched_letter_penalty = -1;

        while (str != strEnd) {
            ++str;
        }

        outScore = 100;

        int penalty = leading_letter_penalty * matches[0];
        if (penalty < max_leading_letter_penalty) {
            penalty = max_leading_letter_penalty;
        }
        outScore += penalty;

        const int unmatched = (int)(std::distance(strBegin, str)) - nextMatch;
        outScore += unmatched_letter_penalty * unmatched;

        for (int i = 0; i < nextMatch; ++i) {
            uint8_t currIdx = matches[i];

            if (i > 0) {
                uint8_t prevIdx = matches[i - 1];

                if (currIdx == (prevIdx + 1)) {
                    outScore += sequential_bonus;
                }
            }

            if (currIdx > 0) {
                QChar neighbor = *(strBegin + currIdx - 1);
                QChar curr = *(strBegin + currIdx);
                if (neighbor.isLower() && curr.isUpper()) {
                    outScore += camel_bonus;
                }

                bool neighborSeparator = neighbor == QLatin1Char('_') || neighbor == QLatin1Char(' ');
                if (neighborSeparator) {
                    outScore += separator_bonus;
                }
            } else {
                outScore += first_letter_bonus;
            }
        }
    }

    if (recursiveMatch && (!matched || bestRecursiveScore > outScore)) {
        memcpy(matches, bestRecursiveMatches, maxMatches);
        outScore = bestRecursiveScore;
        return true;
    } else if (matched) {
        return true;
    } else {
        return false;
    }
}

static QString to_fuzzy_matched_display_string(const QString pattern, QString &str, const QString &htmlTag, const QString &htmlTagClose)
{
    int j = 0;
    for (int i = 0; i < str.size() && j < pattern.size(); ++i) {
        if (str.at(i).toLower() == pattern.at(j).toLower()) {
            str.replace(i, 1, htmlTag + str.at(i) + htmlTagClose);
            i += htmlTag.size() + htmlTagClose.size();
            ++j;
        }
    }
    return str;
}
}

#endif
