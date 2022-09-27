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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>








typedef struct stack
{
    char val[32][32];
    int topid;
} Stack;
int pri(char arg)
{
    if (arg == '-' || arg == '+')
    {
        return 1;
    }
    else if (arg == '/' || arg == '*')
    {
        return 2;
    }
    else
        return 0;
}
void push(Stack *L, char *e)
{
    if (L->topid == 32 - 1)
    {
        return;
    }
    L->topid++;
    for (int i = 0; i < strlen(e); ++i)
    {

        L->val[L->topid][i] = e[i];
    }
}
int oper(char *p)
{
    if (p[0] == '+' || p[0] == '-' || p[0] == '*' || p[0] == '/')
    {
        return 1;
    }
    return 0;
}
void pop(Stack *L)
{
    if (L->topid == -1)
    {
        return;
    }
    --L->topid;
}

int empty(Stack *s)
{
    if (s->topid != -1)
    {
        return 1;
    }
    return 0;
}
void calu(Stack *l, char *p)
{
    char *a1 = l->val[l->topid];
    pop(l);
    char *a2 = l->val[l->topid];
    pop(l);
    float op1, op2;
    sscanf( a1,"%f", &op1);
    sscanf( a2,"%f", &op2);
    char sq[32];
    if (p[0] == '+')
    {
        
        sprintf(sq, "%f", op2 + op1);
        push(l, sq);
    }
    else if (p[0] == '-')
    {

        sprintf(sq, "%f", op2 - op1);
        push(l, sq);
    }
    else if (p[0] == '/')
    {

        sprintf(sq, "%f", op2 / op1);
        push(l, sq);
    }
    else if (p[0] == '*')
    {

        sprintf(sq, "%f", op2 * op1);
        push(l, sq);
    }
}








enum {
  TK_NOTYPE = 256, TK_EQ,TK_INT

  /* TODO: Add more token types ,have done*/

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   * have done
   */
  
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},
  {"\\-", '-'},		// equal
  {"\\*", '*'},
  {"\\/", '/'},
  {"\\(", '('},
  {"\\)", ')'},
  {"[0-9]+", TK_INT}

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
  //printf("___%c_____\n",e[position]);
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         * have done
         */
        switch (rules[i].token_type)
        {
        case TK_NOTYPE:
            break;
        case TK_EQ:
        case '+':
        case '-':
        case '*':
        case '/':
        {
            tokens[nr_token].type = rules[i].token_type;
            tokens[nr_token].str[0]=rules[i].token_type;
            nr_token++;
            break;
        }
        case TK_INT:
        {
            tokens[nr_token].type = rules[i].token_type;
            for (int l = 0; l < substr_len;l++){
                tokens[nr_token].str[l]=substr_start[l];
                } 
                //printf("%s\n",tokens[nr_token].str);
                nr_token++;
                break;
        }
    
          default: TODO();

        }

   

        break;
      }
    }
	
   //printf("%d\n",nr_token);
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  return true;
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
    Stack operand;
    operand.topid = -1;
    Stack data;
    data.topid = -1;
    for (int i = 0; i <= 32; i++)
    {
        if (tokens[i].type == TK_INT)
        {
            push(&data, tokens[i].str);
        }
        else if (tokens[i].type == '(')
        {
            push(&operand, tokens[i].str);
        }
        else if (tokens[i].type == ')')
        {
            while (!empty(&operand) && operand.val[operand.topid][0] != '(')
            {
                push(&operand, operand.val[operand.topid]);
                pop(&operand);
            }

            pop(&operand);
        }
        else if (empty(&operand))
        {
            push(&operand, tokens[i].str);
        }
        else if (pri(tokens[i].type) < pri(tokens[i].type))
        {
            push(&operand, tokens[i].str);
        }
        else
        {
            while (!empty(&operand) && pri(tokens[i].type) >= pri(tokens[i].type))
            {
                push(&data, operand.val[operand.topid]);
                pop(&operand);
            }
            push(&operand, tokens[i].str);
        }
    }
    while (!empty(&data))
    {
        push(&operand, data.val[data.topid]);
        pop(&data);
    }
    while (!empty(&operand))
    {
        printf("%s", operand.val[operand.topid]);
        pop(&operand);
    }
    Stack result;
    result.topid = -1;
    while (empty(&operand) != 0)
    {
        if (oper(operand.val[operand.topid]))
        {
            push(&result, operand.val[operand.topid]);
            operand.topid--;
        }
        else
        {
            calu(&result, operand.val[operand.topid]);
        }
    }
    
    double re;
    sscanf("%f",result.val[result.topid],&re);
    printf("%f\n",re);
  return 0;
}
