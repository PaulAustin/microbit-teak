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

#ifndef tstring_h
#define tstring_h

namespace teak {
//-------------------------------------------------------------------
enum tstring_category {
    tscEmpty,        // Zero length
    tscOther,        // Not yet categorized
    tscOpenParen,    // '('
    tscCloseParen,   // ')'
    tscNameDef,      // simple name followed by colon 'name:''
    tscQString,      // quoted string, with quotes removed
    tscTextSymbol,   // abc, abc123
    tscINumberSymbol, // 123
    tscFNumberSymbol, // 123.5
};

// A readonly string the begin and end point can be moved,
// but target string is readonly.
// TeakString categories.
class tstring {
  private:
    const char* _begin;
    const char* _end;

  public:
    // Constructors
    tstring() { _begin = _end = 0; }
    tstring(const char* cstr);
    tstring(const char* begin, const char* end) { _begin = begin; _end = end; };

    // Properties
    const char* begin() const { return _begin; };
    const char* end() const { return _end; }
    int length() const { return (int)(_end - _begin); };
    bool empty() const { return (!(_end > _begin)); }
    bool equal(const char* cstr) const;
    bool equal(tstring& tstr) const;

    // Parsing methods
    void skip_space();
    bool match_char(char c);
    void skip_chars(int i) { _begin += i; }
    char peek_char(int i) { return (_begin + i < _end) ? *(_begin + i) : 0; }
    char peek_char() { return (_begin < _end) ? *_begin : 0; }
    char get_char() { return (_begin < _end) ? *_begin++ : 0; }

    int read_integer(int* pradix = 0);
    float read_float();
    int to_integer();
    float to_float();
    tstring_category read_token(tstring& token);

    // Internal parsing helper methods
    tstring_category finish_read_number_token(tstring& token);
    bool valid_token_end(bool valid);
    bool read_decimal_text_chunk(tstring& token, bool valid);
    bool read_hex_text_chunk(tstring& token, bool valid);
    bool read_bin_text_chunk(tstring& token, bool valid);

};

inline bool is_bin_char(char c)  { return ((c=='1') | ( c== '0')); }
inline int dec_char_val(char c)  { return (c >= '0' && c <= '9') ? c - '0' : 0; }
inline int bin_char_val(char c)  { return (c >= '0' && c <= '9') ? c - '0' : 0; }
inline int is_in_range(char c, char low, char high)  { return (c >= low) && (c <= high); }
int hex_char_val(char c);

class tstringBuffer : public tstring {
    const char* _maxEnd;
};

#define TSTR_DOT_STAR(_ts_) _ts_.length(), _ts_.begin()

} // namespace teak
#endif // tstring_h
