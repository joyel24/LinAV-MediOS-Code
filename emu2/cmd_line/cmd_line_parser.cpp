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

int done;

void init_cmd_line(void)
{
  /* init of cmd ine */
  rl_readline_name = "EMU> ";
  rl_attempted_completion_function = my_completion;
  
  done = 0;
  head_cmd = NULL;
}

void add_cmd_fct(char * name,int(*fct)(char*))
{
    COMMAND * ptr = new COMMAND();
    ptr->name = name;
    ptr->func = fct;
    ptr->nxt = head_cmd;
    head_cmd =ptr;
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
      return (-1);
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

