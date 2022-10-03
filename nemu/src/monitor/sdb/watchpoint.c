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

#include "sdb.h"

#define NR_WP 32


static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}
WP* new_wp(){
  WP* temp=free_;
  //assert(free_!=NULL);
  if(free_!=NULL)
  {free_=free_->next;}
  if(head==NULL){head=temp;temp->next=NULL;}
  WP* head_temp=head;
    while(head_temp!=NULL){
       if(head_temp->next==NULL){head_temp->next=temp;
       temp->next=NULL;
       break;}
       head_temp=head_temp->next;
    }
  return temp;
}
void free_wp(WP *wp){
    WP* temp=free_;
    while(temp->next!=NULL){
       if(temp->next==NULL){temp->next=wp;
       wp->next=NULL;
       return;}
       temp=temp->next;
    }
}
/*void set_exp(char* args,WP* wp){
wp->exp=args;}
void set_pastv(word_t args,WP* wp){
wp->past_val=args;}
void set_nowv(word_t args,WP* wp){
wp->now_val=args;}
int getNO(WP *wp){return wp->NO;}
void set_wp(char* args){
	WP* new=new_wp();
	new->exp=args;
	new->past_val=expr(args,false);
	new->now_val=expr(args,false);
	printf("%d",new->NO);
}*/
/* TODO: Implement the functionality of watchpoint */

