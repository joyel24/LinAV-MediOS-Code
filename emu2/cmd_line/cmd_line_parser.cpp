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

void add_cmd_fct(char * name,int(*fct)(int,char**),char * help_str)
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

void toLower(char * str)
{
    while(*str)
    {
        if(*str >= 'A' && *str <= 'Z')
            *str = *str + ('a'-'A');
        str++;
    }
}

int execute_line (char *line)
{
  register int i;
  COMMAND *command;
  char *word;
  int res;
  char * argv[50];
  int argc;

  i = 0;
  while (line[i] && whitespace (line[i]))
    i++;
  word = line + i;

  while (line[i] && !whitespace (line[i]))
    i++;

  if (line[i])
    line[i++] = '\0';

  toLower(word);
  command = find_command (word);

  if (!command)
    {
      fprintf (stderr, "%s: No such command for Emu.\n", word);
      return 0;
    }

  while (whitespace (line[i]))
    i++;

  //word = line + i;
  
  argc=parse_args(argv,line + i);
  //printf("Found cmd, %d args\n",argc);

  res=command->func(argc,argv);
  
  return res;
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

int parse_args(char ** argv, char * str)
{
  int k=0;int i=0;
  
  while(1)
  {
    /* erase spaces at begin */
    while(str[i] == ' ') i++;

    if(str[i] == '\0') /* we have reached end of string */
        break;
    
    /* we are at the start of a new arg */
    argv[k] = str+i;
    k++;

    /* looking for the end of arg */
    while(str[i] != '\0' && str[i] != ' ') i++;

    if(str[i] == '\0') /* we have reached end of string */
        break;

    /* add end of string at end of arg */
    str[i] = '\0';
    i++;
  }
  return k;
}

bool is_number(char * str)
{    
    int cnt=0;
    bool is_hex = false;
    bool res=true;
    if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
    {
        str +=2;
        is_hex=true;
    }
    
    while(str[cnt])
    {
        if(str[cnt] >= '0' && str[cnt] <= '9')
            cnt++;
        else if(is_hex && str[cnt] >= 'A' && str[cnt] <= 'F')
            cnt++;
        else if(is_hex && str[cnt] >= 'a' && str[cnt] <= 'f')
            cnt++;
        else
        {
            res=false;
            break;
        }
    }
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
    
    while(1)
    {
        if(string[cnt] >= '0' && string[cnt] <= '9') /* we have a normal digit */
        {
            if(is_hex)
            {
                val = val << 4;
                val |= (string[cnt]-'0')&0xf;
            }
            else
            {
                val = val * 10;
                val += string[cnt]-'0';
            }
            cnt ++;
        }
        else if(is_hex && string[cnt] >= 'A' && string[cnt] <= 'F') /* A - F */
        {
            val = val << 4;
            val |= (string[cnt]-'A'+0xa)&0xf;
            cnt ++;
        }
        else if(is_hex && string[cnt] >= 'a' && string[cnt] <= 'f') /* a - f */
        {
            val = val << 4;
            val |= (string[cnt]-'a'+0xa)&0xf;
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

