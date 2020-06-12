/*

Copyright (c) 2017 Paul Austin - SDG

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <stdio.h>
#include <cstring>
#include <ctype.h>
#include "tstring.h"

namespace teak {
//-----------------------------------------------------------------------------
tstring::tstring(const char* cstr) {
    _begin = cstr;
    _end = cstr + strlen(cstr);
}
//-----------------------------------------------------------------------------
bool tstring::equal(const char* cstr) const {
    if ((int)strlen(cstr) != length())
        return false;

    return strncmp(cstr, begin(), length()) == 0;
}
//-----------------------------------------------------------------------------
bool tstring::equal(tstring& tstr) const {
    if (tstr.length() != length())
        return false;

    return strncmp(tstr.begin(), begin(), length()) == 0;
}
//-----------------------------------------------------------------------------
void tstring::skip_space() {
    while (isspace(peek_char())) {
        _begin++;
    }
}
//-----------------------------------------------------------------------------
bool tstring::match_char(char c) {
    if (peek_char() == c) {
        _begin++;
        return true;
    } else {
        return false;
    }
}
//-----------------------------------------------------------------------------
tstring_category tstring::read_token(tstring& token) {

    skip_space();

    if (empty()) {
        token._begin = token._end = NULL;
        return tscEmpty;
    }

    token._begin = _begin;
    char c = *_begin++;
    token._end = _begin;

    if (c == '(') {
        return tscOpenParen;
    } else if (c == ')') {
        return tscCloseParen;
    } else if (isalpha(c)) {
        // TODO support dotted names
        while (_begin < _end && isalnum(*_begin)) {
            _begin++;
            token._end = _begin;
        }
        if (!empty() && *_begin == ':') {
            // Eat the colon.
            _begin++;
            return tscNameDef;
        } else {
            return tscTextSymbol;
        }
    } else if (isdigit(c)) {
        if (length() == 0) {
            // Simple case is done
            return tscINumberSymbol;
        } else {
            return finish_read_number_token(token);
        }
    } else if (c == '\'') {
        token._begin = _begin++;
        while (peek_char() != '\'') {
            _begin++;
        }
        if (*_begin == '\'') {
            token._end = _begin;
            _begin++;
            return tscQString;
        } else {
            // never found closing quote
            token._end = token._begin;
            return tscOther;
        }
    } else {
        // unsupported charcter, return it as err token
        return tscOther;
    }
}
//-----------------------------------------------------------------------------
tstring_category tstring::finish_read_number_token(tstring& token) {

    // Numbers that are more than one character long may actually be
    // '0' followed by a radix indicator. Check second digit for radix indicator.
    // also look for radix specific fractional part.

    tstring_category tsc = tscINumberSymbol;
    bool valid = false;
    if (match_char('x')) {
        // hexidecimal
        valid = read_hex_text_chunk(token, false);
        if (valid && match_char('.')) {
            tsc = tscFNumberSymbol;
            valid = read_hex_text_chunk(token, false);
        }
    } else if (match_char('b')) {
        // binary
        valid = read_bin_text_chunk(token, false);
        if (valid && match_char('.')) {
            tsc = tscFNumberSymbol;
            valid = read_bin_text_chunk(token, false);
        }
    } else {
        valid = read_decimal_text_chunk(token, true);
        if (valid && match_char('.')) {
            tsc = tscFNumberSymbol;
            valid = read_decimal_text_chunk(token, false);
        }
    }
    token._end = _begin;
    valid = valid_token_end(valid);
    return valid ? tsc : tscOther;
}
//-----------------------------------------------------------------------------
bool tstring::read_decimal_text_chunk(tstring& token, bool valid) {
    token = *this;
    while (isdigit(peek_char())) {
        _begin++;
        valid |= true;
    }
    token._end = _begin;
    return valid;
}
//-----------------------------------------------------------------------------
bool tstring::read_hex_text_chunk(tstring& token, bool valid) {
    token = *this;
    while (isxdigit(peek_char())) {
        _begin++;
        valid |= true;
    }
    token._end = _begin;
    return valid;
}
//-----------------------------------------------------------------------------
bool tstring::read_bin_text_chunk(tstring& token, bool valid) {
    token = *this;
    while (is_bin_char(peek_char())) {
        _begin++;
        valid |= true;
    }
    token._end = _begin;
    return valid;
}
//-----------------------------------------------------------------------------
bool tstring::valid_token_end(bool valid) {
    char c = peek_char();
    return (c == '(' || c == ')' || isspace(c) || c == 0);
}
//-----------------------------------------------------------------------------
int tstring::to_integer() {
    tstring temp = *this;
    return temp.read_integer();
}
//-----------------------------------------------------------------------------
int tstring::read_integer(int* pradix) {
    int num = 0;
    char radix = peek_char(1);

    if ((length() <= 1) || isdigit(radix) || radix == '.') {
        radix = 'd';
        while (isdigit(peek_char())) {
            num = (10 * num) + dec_char_val(get_char());
        }
    } else if (radix == 'x') {
        skip_chars(2);
        while (isxdigit(peek_char())) {
            num = (16 * num) + hex_char_val(get_char());
        }
    } else if (radix == 'b') {
        skip_chars(2);
        while (is_bin_char(peek_char())) {
            num = (2 * num) + bin_char_val(get_char());
        }
    }
    if (pradix) {
        *pradix = radix;
    }
    return num;
}
//-----------------------------------------------------------------------------
float tstring::to_float() {

    tstring temp = *this;
    int radix;
    float fnum = (float) temp.read_integer(&radix);
    float fraction = 1.0;
    if (temp.match_char('.')) {
        if (radix == 'd') {
            while (isdigit(temp.peek_char())) {
                fraction = fraction / 10;
                fnum = fnum + (fraction * dec_char_val(temp.get_char()));
            }
        } else if (radix == 'x') {
            while (isdigit(temp.peek_char())) {
                fraction = fraction / 16;
                fnum = fnum + (fraction * dec_char_val(temp.get_char()));
            }
        } else if (radix == 'b') {
            while (isdigit(temp.peek_char())) {
                fraction = fraction / 2;
                fnum = fnum + (fraction * dec_char_val(temp.get_char()));
            }
        }
    }
    return fnum;
}
//-----------------------------------------------------------------------------
int hex_char_val(char c) {
    if (c >= '0' && c <= '9') {
        return (c - '0');
    } else if (c >= 'a' && c <= 'f') {
        return (c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
        return (c - 'A' + 10);
    } else {
        return 0;
    }
}

} // namespace teak
