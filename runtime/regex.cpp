/*
 * Pattern.cc
 *
 * This file is part of Quasar (formerly wxBasic).
 *
 * Copyright: (c) 2015 Julien Eychenne <jeychenne@gmail.com>
 *
 * This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser
 * General Public License as published by the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version. This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not,
 * write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * See files LGPL.TXT and README.TXT for more information.
 *
 * Author: Julien Eychenne <jeychenne@gmail.com>
 *
 * Purpose: see header
 *
 * Created: 12/01/2015
 *
 */

#include "regex.hpp"


namespace runtime {

regex::regex(const string &pattern, int flags)
{
    m_pattern = pattern;
    m_flags = flags;
    m_match_result = ONIG_MISMATCH;

    if (pattern.empty())
    {
        throw error("empty pattern in regular expression constructor");
    }

    const UChar *pattern_start = reinterpret_cast<const UChar*>(pattern.data());
    const UChar *pattern_end = reinterpret_cast<const UChar*>(pattern.data() + pattern.size());

    int result = onig_new(&m_regex, pattern_start, pattern_end,
                          flags, ONIG_ENCODING_UTF8, ONIG_SYNTAX_PERL, &m_error);

    if (result != ONIG_NORMAL)
    {
//        onig_error_code_to_str();
        throw error("regular expression creation failed");
    }

    m_region = onig_region_new();
}

regex::regex(const string &pattern, const string &str_flags) :
    regex(pattern, parse_flags(str_flags))
{ }

int regex::parse_flags(const string &options)
{
    int flags = none;

    for (auto &s : options.split("|")) {
        if (s == "ignore_case") {
            flags |= ignore_case;
        } else if (s == "multiline") {
            flags |= multiline;
        } else if (s == "dot_all") {
            flags |= dot_all;
        } else if (s == "extended") {
            flags |= extended;
        } else if (s == "greedy") {
            flags |= greedy;
        } else if (s == "dont_capture_group") {
            flags |= dont_capture_group;
        } else if (s == "capture_group") {
            flags |= capture_group;
        }
    }

    return flags;
}

regex::~regex()
{
    if (m_region) onig_region_free(m_region, 1);
    onig_free(m_regex);
}

bool regex::match(const string &subject, int start_offset)
{
    m_subject = subject;

    onig_region_clear(m_region);
    const UChar *str = reinterpret_cast<const UChar*>(m_subject.data());
    const UChar *end = reinterpret_cast<const UChar*>(m_subject.data() + m_subject.size());
    const UChar *start = reinterpret_cast<const UChar*>(m_subject.data() + start_offset);

    m_match_result = onig_search(m_regex, str, end, start, end, m_region, ONIG_OPTION_DEFAULT);

    return has_match();
}

int regex::capture_count()
{
    // Only return capture count if there is a match
    if (m_regex)
    {
        if (has_match())
            return onig_number_of_captures(m_regex);
        else
            return 0;
    }

    return -1;
}

bool regex::has_match() const
{
    return m_match_result != ONIG_MISMATCH;
}

string regex::capture(int nth)
{
    if (has_match() && nth <= m_region->num_regs)
    {
        auto start = size_t(m_region->beg[nth]);
        auto len = size_t(m_region->end[nth] - start);
        const char *s = m_subject.data() + start;

        return string(s, len);
    }

    return string();
}

int regex::start_offset(int nth) const
{
    if (has_match() && nth <= m_region->num_regs)
    {
        return (int) m_region->beg[nth] + 1; // base 1
    }

    return -1;
}

int regex::end_offset(int nth) const
{
    if (has_match() && nth <= m_region->num_regs)
    {
        // don't add one since onig points at the char after the match
        return (int) m_region->end[nth];
    }
    return -1;
}

const char *regex::to_string() const
{
    return m_pattern.data();
}

string regex::pattern() const
{
    return m_pattern;
}



} // runtime
