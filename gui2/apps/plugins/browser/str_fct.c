/*
* str_fct.c
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#define    toLower(chr)  ((chr>64 && chr<91)?chr+32:chr)

int namesort(char **s1,char **s2)
{
    char * st1=*s1;
    char * st2=*s2;
    while(*st1!=0 && *st2!=0 && toLower(*st1) == toLower(*st2))
    {
        st1++;
        st2++;
    }

    return (toLower(*st1) - toLower(*st2));
}

void strlwr(char *s)
{
    while (*s)
    {
        *s = tolower(*s);
        s++;
    }
}
