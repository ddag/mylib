/* $Id: urlcode.c,v 1.1 2001/11/07 15:37:02 jabley Exp $
 *
 * urlencode/decode stdin to stdout as per RFC2396
 *
 * Copyright (c) 1998-2001 by Joseph Nicholas Abley
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND JOSEPH NICHOLAS ABLEY ("THE
 * AUTHOR") DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 *   Joseph Nicholas Abley
 *   10805 Old River Road
 *   Komoka, ON  N0L 1R0
 *   CANADA
 *
 * jabley@automagic.org
 *
 */

#include <stdio.h>
#include <string.h>

/* return 1 for characters that RFC2396 says should be escaped */
static int should_encode(int c) 
{
  /* RFC2396 section 2.4.3: control */
  if (c < 0x21 || c > 0x7e)
    return(1);
  /* RFC2396 section 2.4.3: space, delims, unwise */
  if (strchr(" <>#%\"{}|\\^[]`", c) != NULL)
    return(1);
  return(0);
}


/* decode a hex digit into binary */
static int hexdigit(int c) 
{
  if (c >= '0' && c <= '9')
    return(c - '0');
  if (c >= 'a' && c <= 'f')
    return(c - 'a' + 10);
  if (c >= 'A' && c <= 'F')
    return(c - 'A' + 10);
  return(-1);
}


/* urlencode */
void urlencode(const char *input, char *output) 
{
    int i = 0;
    while (*input) 
    {
        if (should_encode(*input)) 
        {
            sprintf(&output[i], "%%%02X", *input);
            i += 3; // %FF
        }
        else
            output[i++] = *input;
        input++;
    }
    output[i] = 0;
}


/* urldecode */
void urldecode(const char *input, char *output) 
{
    int i = 0;
    while (*input) 
    {
        if (*input == '%') 
        {
            output[i++] = 0x10 * hexdigit(input[1]) + hexdigit(input[2]);
            input += 3;
        }
        else
        {
            output[i++] = *input;
            input++;
        }
    }
    output[i] = 0;
}

