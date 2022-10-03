/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include "../../../include/memory/paddr.h"
static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}



static int cmd_si(char *args) {
	char *arg = strtok(NULL, " ");
	if (arg!=NULL){
	int N;
	sscanf(arg,"%d",&N);
	cpu_exec(N);
	}
	else
	{cpu_exec(1);}
  return 0;
}


static int cmd_info(char *args) {
	char *arg = strtok(NULL, " ");
	// arg is a string that points to a character that can be extracted directly using *
	if(*arg=='r'){
	isa_reg_display();
	}
	else if(*arg=='w'){
	wp_display();
	}
  return 0;
}

static int cmd_x(char *args) {
	int fir;
	sscanf(strtok(NULL, " "),"%d",&fir);
	uint32_t sec;
	char *exp=strtok(NULL, " ");
	//sscanf(strtok(NULL, " "),"%x",&sec);
	sec=expr(exp,false);
	for(int i=0;i<fir;i++){
	if(i%4==0){
	printf("\n0x%08x:\t", sec);
	}
	printf("0x%08x\t", paddr_read(sec,4));
	sec+=4;
	}
	printf("\n");
	
	
  return 0;
}

static int cmd_p(char *args) {
	//printf("p EXPR_%s\n", args);
	char *arg=strtok(NULL," ");
	printf("%u\n",expr(arg,false));
  return 0;
}

static int cmd_w(char *args) {
	//expr(args,false)
	WP *new=new_wp();
	//new->exp=args;
	for(int i=0;i<strlen(args);i++){
	new->exp[i]=args[i];
	}
	new->now_val=expr(args,false);
	printf("watchpoint %d: %s\n",new->NO,new->exp);
  return 0;
}

static int cmd_d(char *args) {
	//printf("d N\n");
	int N;
	sscanf(strtok(NULL, " "),"%d",&N);
	free_wp(N);
	
  return 0;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si","Let the program step through N instructions and then pause execution,when N is not given, the default is 1", cmd_si},
  { "info","Print status register and the print information monitoring points",cmd_info},
  { "x","Work out the value of the expression EXPR, using the result as the starting memory. Address, which outputs n consecutive 4 bytes in hexadecimal form", cmd_x},
  {"p","Find the value of the expression EXPR", cmd_p},
  {"w","Pauses program execution when the value of the expression EXPR changes", cmd_w},
  {"d","Delete the number of N monitoring points", cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    cmd_si(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);
    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    //printf("%c",*strtok(NULL," ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
      	      //printf("%s",cmd_table[i].name);
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
