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



enum {
  TK_NOTYPE = 256, TK_EQ,TK_INT,TK_INEQ,TK_AND,TK_POINT,TK_HEX,TK_REG

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
  {"==", TK_EQ},        // equal
  {"\\-", '-'},		
  {"\\*", '*'},
  {"\\/", '/'},
  {"\\(", '('},
  {"\\)", ')'},
  {"!=",TK_INEQ},       //inequal
  {"&&",TK_AND},	//and
  {"^0x[0-9a-zA-Z]+",TK_HEX},
  {"^[0-9]+", TK_INT},
  {"^\\$[\\$0-9a-zA-Z]+$",TK_REG},

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
        case TK_INEQ:
        case TK_AND:
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
        case TK_HEX:
            tokens[nr_token].type = rules[i].token_type;
            for (int l = 0; l < substr_len;l++){
                tokens[nr_token].str[l]=substr_start[l];
                } 
                //printf("%s\n",tokens[nr_token].str);
                nr_token++;
                break;
        case TK_REG:
        	tokens[nr_token].type = rules[i].token_type;
        	for(int l=0;l<substr_len-1;l++){
        	tokens[nr_token].str[l]=substr_start[l+1];
        	}
        	nr_token++;
        	break;
    
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
  //printf("%d\n",oprand(0,nr_token-1));
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();

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
        if (tokens[i].type!=TK_INT&&tokens[i].type!=TK_POINT&&tokens[i].type!=TK_HEX)
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
            for (int j = q; j > i; j--)
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
            if (num(tokens[result].type) >= num(tokens[i].type))
            {
                result = i;
            }
        }
    }
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
        if(tokens[p].type==TK_REG){
            return isa_reg_str2val(tokens[p].str,false);
        }
        else if(tokens[p].type==TK_HEX){
           ;
        }
        else{
        sscanf(tokens[p].str, "%u", &data);
        }
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
	//printf("%c\n",tokens[op].type);
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
        case TK_EQ:
            return val1 == val2;
        case TK_INEQ:
            return val1 != val2;
        case TK_AND:
            return val1 && val2;
        default:
            assert(0);
        }
    }
    return 0;
}
