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
void free_wp(int N){
	WP* head_f=head;
	WP *wp;
	if(head_f->NO==N){
		wp=head_f;
		head=head->next;
	}
	while(head_f->next!=NULL){
		//delete the NO N watchpoint
		if(head_f->next->NO==N){
		wp=head_f->next;
		head_f=head_f->next->next;
		break;
	}	
	}
    WP* temp=free_;
    while(temp->next!=NULL){
       if(temp->next==NULL){temp->next=wp;
       wp->next=NULL;
       return;}
       temp=temp->next;
    }
}
WP* gethead(){
	return head;
}
void wp_display(){
	if(head==NULL){
	printf("There is no watch right now.\n");
	}
	else{
		WP *temp=head;
		printf("NO\tvalue\texpression\n");
		while(temp!=NULL){
		printf("%d\t%u\t%s\n",temp->NO,temp->now_val,temp->exp);
		}
	}
}
/* TODO: Implement the functionality of watchpoint */

