/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-19 frank256
*
* License (BSD):
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice, this
*    list of conditions and the following disclaimer in the documentation and/or
*    other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef LGUI_UTF8_H
#define LGUI_UTF8_H

#include <string>

namespace lgui {
/** Some helper functions for working with UTF-8 compatible strings.
 *  All positions used here are byte offsets unless stated otherwise!
 */
namespace utf8
{
    /** Move pos (byte offset!) to the start of the next valid UTF-8 code point.
     *  @return false if no next code points exists, true otherwise
     */
    bool next_cp(const std::string& str, size_t& pos);

    /** Move pos (byte offset!) to the start of the previous valid UTF-8 code
     *  point.
     *  @return false if no previous code point exists, true otherwise
     */
    bool prev_cp(const std::string& str, size_t& pos);

    /** Return the length of the string counted in UTF-8 code points.
     * @return the length of the string counted in code points
     */
    size_t length_cps(const std::string& str);

    /** Return the length of a substring counted in UTF-8 code points.
     * @param str the string
     * @param startoffs the offset to start counting
     * @param endoffs the offset to end counting (exclusive,
     *        codepoints starting here will not be counted)
     * @return the length of the substring counted in code points
     */
    size_t length_cps_substr(const std::string& str, size_t startoffs,
                             size_t endoffs= std::string::npos);


    /** Get the number of bytes (width) a character will occupy when
     *  encoded in UTF-8.
     * @return the number of bytes c will occupy as a UTF-8 code point
     */
    size_t cp_width(int32_t c);

    /** Encode a character as a UTF-8 code point.
     * @return c encoded as a UTF-8 code point
     */
    std::string encode_chr(int32_t c);

    /** Searches for the first occurence of a unicode character in an
     *  UTF-8 string.
     * @param str the string to search
     * @param c the unicode character to search for
     * @param pos the position where to start searching (inclusive)
     * @return the offset where c has been found or std::string::npos
     */
    size_t find_chr(const std::string& str, int32_t c, size_t pos=0);

    /** Searches *backwards* for the first occurence of the a character in a
     *  UTF-8 string.
     * @param str the string to search
     * @param c the unicode character to search for
     * @param pos the position where to start searching (inclusive!)
     * @return the offset where c has been found or std::string::npos
     */
    size_t rfind_chr(const std::string& str, int32_t c, size_t pos=std::string::npos);

    /** Appends the unicode character c to the end of string str.
     */
    void append_chr(std::string& str, int32_t c);

    /** Insert the unicode character c into str at position (offset!) p.
     *  @return the number of bytes inserted or 0 on error
     */
    size_t insert_chr(std::string& str, size_t pos, int32_t c);

    /** Tries to read a unicode character from the code point starting at pos.
     *  @return the unicode character read, -1 if the end of string has been reached
     *          or -2 on error (invalid codepoint)
     */
    int32_t get_at_offs(const std::string& str, size_t pos);

    /** Tries to remove a unicode character from the string starting at pos.
     *  Will only remove valid unicode codepoints.
     *  @return true if the character has been successfully removed
     */
    bool remove_chr(std::string& str, size_t pos);

    /** Replaces all occurences of find in str with replace.
     *  Not strictly related to UTF-8.
     *  @param str the string to work on
     *  @param find the string to search for
     *  @param replace the string to replace `find` with when found
     *  @param start_pos the position to start searching at (byte offset!)
     */
    void replace_all(std::string& str, const std::string& find, const std::string& replace,
                     size_t start_pos = 0);

    size_t skip_to_next_word_boundary(const std::string& str, size_t offs, bool backwards);
}

}
#endif // LGUI_UTF8_H
