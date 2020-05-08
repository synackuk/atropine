/* 
 xtoa.c - Functions for converting numbers to ascii, that are normally
          implemented by avr-libc.
 This file is licensed under the the MIT License.
 Copyright (c) 2014 Matthijs Kooijman <matthijs@stdin.nl>
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <plib.h>

char* ultoa(unsigned long val, char* s, int radix) {
  static const char dig[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  char* p, *q;

  q = s;
  do {
    *q++ = dig[val % radix];
    val /= radix;
  } while (val);
  *q = '\0';

  /* Reverse the string (but leave the \0) */
  p = s;
  q--;

  while (p < q) {
    char c = *p;
    *p++ = *q;
    *q-- = c;
  }

  return s;
}