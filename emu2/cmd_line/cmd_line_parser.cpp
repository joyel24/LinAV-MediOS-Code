/* 
*   cmd_line_parser.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stdio.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <cmd_line.h>

COMMAND * head_cmd = NULL;

/* Forward declarations. */
char * stripwhite (char * string);
COMMAND *find_command (char *name);
int execute_line (char *line);
char ** my_completion __P((const char *text,int start,int end));
char * dupstr (char * s);

int done;

void init_cmd_line(void)
{
  /* init of cmd ine */
  rl_readline_name = "EMU> ";
  rl_attempted_completion_function = my_completion;
  
  done = 0;
  head_cmd = NULL;
}

void add_cmd_fct(char * name,int(*fct)(char*),char * help_str)
{
    if(name!=NULL)
    {
        COMMAND * ptr = new COMMAND();
        ptr->name = dupstr(name);
        if(help_str!=NULL)
            ptr->doc = dupstr(help_str);
        else
            ptr->doc=NULL;
        ptr->func = fct;
        ptr->nxt = head_cmd;
        head_cmd =ptr;
    }
}

void print_cmd_list(void)
{
    for(COMMAND * ptr = head_cmd;ptr!=NULL;ptr=ptr->nxt)
    {
        printf("%s: %s\n",ptr->name,ptr->doc!=NULL?ptr->doc:"");
    }
}

char * dupstr (char * s)
{
  char *r;

  r = (char*)malloc (strlen (s) + 1);
  strcpy (r, s);
  return (r);
}

int execute_line (char *line)
{
  register int i;
  COMMAND *command;
  char *word;

  i = 0;
  while (line[i] && whitespace (line[i]))
    i++;
  word = line + i;

  while (line[i] && !whitespace (line[i]))
    i++;

  if (line[i])
    line[i++] = '\0';

  command = find_command (word);

  if (!command)
    {
      fprintf (stderr, "%s: No such command for Emu.\n", word);
      return 0;
    }

  while (whitespace (line[i]))
    i++;

  word = line + i;

  return (command->func(word));
}

COMMAND * find_command (char *name)
{
  COMMAND *ptr ;
  
  for (ptr = head_cmd; ptr!=NULL; ptr=ptr->nxt)
    if (strcmp (name, ptr->name) == 0)
      return (ptr);

  return ((COMMAND *)NULL);
}

char * stripwhite (char * string)
{
  register char *s, *t;

  for (s = string; whitespace (*s); s++)
    ;
    
  if (*s == 0)
    return (s);

  t = s + strlen (s) - 1;
  while (t > s && whitespace (*t))
    t--;
  *++t = '\0';

  return s;
}

int my_atoi(char * string)
{
    bool is_hex = false;
    string=stripwhite(string);
    uint32_t val = 0;
    int cnt=0;
    
    if(string[0] == '0' && (string[1] == 'x' || string[1] == 'X'))
    {
        string +=2;
        is_hex=true;
    }
    
    while(string)
    {
        if(string[cnt] > 0x2F && string[cnt] < 0x3A) /* we have a normal digit */
        {
            if(is_hex)
            {
                val = val << 4;
                val |= (string[cnt]-0x30)&0xf;
            }
            else
            {
                val = val * 10;
                val += string[cnt]-0x30;
            }
            cnt ++;
        }
        else if(is_hex && string[cnt] > 0x40 && string[cnt] < 0x47) /* A - F */
        {
            val = val << 4;
            val |= (string[cnt]-0x37)&0xf;
            cnt ++;
        }
        else if(is_hex && string[cnt] > 0x60 && string[cnt] < 0x67) /* A - F */
        {
            val = val << 4;
            val |= (string[cnt]-0x57)&0xf;
            cnt ++;
        }
        else
            break;
    }
    return val;
}

char * command_generator __P((const char *text, int state))
{
  static COMMAND *ptr;
  static int len;
  char *name;
  
  if (!state)
    {
      ptr=head_cmd;
      len = strlen (text);
    }

  while (ptr)
    {
      name=ptr->name;
      ptr=ptr->nxt;

      if (strncmp (name, text, len) == 0)
        return (dupstr(name));
    }

  return ((char *)NULL);
}

char ** my_completion __P((const char *text,int start,int end))
{
  char **matches;

  matches = (char **)NULL;
  
  if (start == 0)
    matches = rl_completion_matches (text, command_generator);

  return (matches);
}

void cmd_line(void)
{
  char *line, *s;
  done=0;
  while(!done)
  {
      line = readline ("EMU> ");

      if (!line)
        break;
        
      s = stripwhite (line);

      if (*s)
    {
        add_history (s);
        done=execute_line (s);
    }

      free (line);
    }
}

