/*
 * Pattern.h
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
 * Purpose: a regular expression object (wrapper over PCRE2)
 *
 * Created: 12/01/2015
 *
 */


#ifndef QS_PATTERN_H
#define QS_PATTERN_H

#include "string.hpp"
#include "extra/onigmo/onigmo.h"

namespace runtime {



class regex
{
public:

    enum option
    {
        none               = ONIG_OPTION_NONE,
        ignore_case        = ONIG_OPTION_IGNORECASE,
        extended           = ONIG_OPTION_EXTEND,
        multiline          = ONIG_OPTION_MULTILINE,
        dot_all            = ONIG_OPTION_DOTALL,
        greedy             = ONIG_OPTION_FIND_LONGEST,
        dont_capture_group = ONIG_OPTION_DONT_CAPTURE_GROUP,
        capture_group      = ONIG_OPTION_CAPTURE_GROUP,
    };

    regex(const string &pattern) :
           regex(pattern, 0)
    { }

    regex(const string &pattern, int flags);

    regex(const string &pattern, const string &str_flags);

    ~regex();

    bool has_match() const;

    bool match(const string &subject, int start_offset = 0);

    int capture_count();

    string capture(int nth);

    int start_offset(int nth = 0) const;

    int end_offset(int nth = 0) const;

    const char *to_string() const;

    string pattern() const;

private:

    regex_t *m_regex;
    OnigErrorInfo m_error;
    OnigRegion *m_region;
    int  m_flags;
    OnigPosition m_match_result;
    string m_pattern, m_subject;

    int parse_flags(const string &options);
};



} // runtime


#endif // QS_PATTERN_H
