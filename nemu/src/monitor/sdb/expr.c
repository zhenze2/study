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

uint32_t eval(int p, int q);
int check_parentheses(int left, int right);
int num(int c);
int oprand(int p, int q);


/*
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
    L->val[L->topid][strlen(e)] = e[strlen(e)];
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
    L->topid--;
}

int empty(Stack *s)
{
    if (s->topid != -1)
    {
        return 0;
    }
    return 1;
}
void calu(Stack *l, char *p)
{
    char *a1 = l->val[l->topid];
    pop(l);
    char *a2 = l->val[l->topid];
    pop(l);
    float op1;
    float op2;
    sscanf(a1, "%f", &op1);
    sscanf(a2, "%f", &op2);
	//printf("%f,%f\n",op2,op1);
	//printf("%s,%s\n",a2,a1);
    char s[32];
    if (p[0] == '+')
    {

        sprintf(s, "%f", op2 + op1);
        push(l, s);
    }
    else if (p[0] == '-')
    {

        sprintf(s, "%f", op2 - op1);
        push(l, s);
    }
    else if (p[0] == '/')
    {

        sprintf(s, "%f", op2 / op1);
        push(l, s);
    }
    else if (p[0] == '*')
    {

        sprintf(s, "%f", op2 * op1);
        push(l, s);
    }
}
*/







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
        case '(':
        case ')':
        {
            tokens[nr_token].type = rules[i].token_type;
            //tokens[nr_token].str[0]=rules[i].token_type;
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
  //printf("%d\n",check_parentness(0,nr_token-1)); right#
  printf("%d\n",oprand(0,nr_token-1));
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
    /*Stack operand;
    operand.topid = -1;
    Stack data;
    data.topid = -1;
    for (int i = 0; i < nr_token; i++)
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
                push(&data, operand.val[operand.topid]);
                pop(&operand);
            }

            pop(&operand);
        }
        else if (empty(&operand))
        {
            push(&operand, tokens[i].str);
        }
        else if (pri(operand.val[operand.topid][0]) < pri(tokens[i].type))
        {
            push(&operand, tokens[i].str);
        }
        else
        {
            while (!empty(&operand) && pri(operand.val[operand.topid][0]) >=pri(tokens[i].type))
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

    Stack result;
    result.topid = -1;
    while (!empty(&operand))
    {
        if (oper(operand.val[operand.topid]) == 0)
        {
            push(&result, operand.val[operand.topid]);
            operand.topid--;
        }
        else
        {
            calu(&result, operand.val[operand.topid]);
            operand.topid--;
        }
    }
    printf("%s\n", result.val[result.topid]);*/
    printf("%u\n",eval(0,nr_token-1));
  return 0;
}


int op[32] __attribute__((used))={};
int check_parentheses(int left, int right)
{
    if ((tokens[left].type == '(' && tokens[right].type != ')') || (tokens[left].type != '(' && tokens[right].type == ')')||(tokens[left].type != '(' && tokens[right].type != ')'))
    {
        return 0;
    }
    int top = -1;
    for (int i = left; i <= right; i++)
    {
        if (tokens[i].type == '(')
        {
            top++;
            op[top] = '(';
        }
        if (tokens[i].type == ')')
        {
            op[top]=-1;
            top--;
        }
    }
    if (top == -1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
int oprand(int p, int q)
{
    int result = p;
    for (int i = p; i <= q; i++)
    {
        if (tokens[i].type == '+' || tokens[i].type == '-' || tokens[i].type == '*'||tokens[i].type == '/')
        {
            int a = 0;
            int b = 0;
            for (int j = result; j < i; j++)
            {
                if (tokens[j].type == '(')
                {
                    a = 1;
                    break;
                }
            }
            for (int j = p; j > i; j--)
            {
                if (tokens[j].type == ')')
                {
                    b = 1;
                    break;
                }
            }
            if(a*b==1){
                continue;
            }
            if (num(result) >= num(tokens[i].type))
            {
                result = i;
            }
        }
    }
    printf("%c\n",tokens[result].type);
    return result;
}

int num(int c)
{
    switch (c)
    {
    case '+':
    case '-':
        return 0;
    case '*':
    case '/':
        return 1;

    default:
        break;
    }
    return 10;
}
uint32_t eval(int p, int q)
{
    if (p > q)
    {
        printf("This is a bad expression");
        /* Bad expression */
    }
    else if (p == q)
    {
        /* Single token.
         * For now this token should be a number.
         * Return the value of the number.
         */
        uint32_t data;
        sscanf(tokens[p].str, "%u", &data);
        return data;
    }
    else if (check_parentheses(p, q) == 1)
    {
        /* The expression is surrounded by a matched pair of parentheses.
         * If that is the case, just throw away the parentheses.
         */
        return eval(p + 1, q - 1);
    }
    else
    {
        /* We should do more things here. */
        int op = oprand(p, q);
        uint32_t val1 = eval(p, op - 1);
        uint32_t val2 = eval(op + 1, q);
	printf("%c\n",op);
        switch (tokens[op].type)
        {
        case '+':
            return val1 + val2;
        case '-':
            return val1 - val2;
        case '*':
            return val1 * val2;
        case '/':
            return val1 / val2;
        default:
            assert(0);
        }
    }
    return 0;
}
