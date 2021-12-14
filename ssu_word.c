#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "ssu_extern.h"
#define SIZE 105

extern struct answer_data *ans_table;
extern struct problem_data *pro_table;
extern struct student_data *stu_table;

extern char *student_file_dir;
extern char *trueset_file_dir;

extern int *stu_folder_length;
extern int *pro_file_length;
extern int _ans_file_length;
extern int student_count;
extern int problem_count;

int Precedence(char *temp)
{
	if((strcmp(temp,".") == 0) || (strcmp(temp,"->") == 0))
		return 30;
	else if((strcmp(temp,"!") == 0) || (strcmp(temp,"~") == 0)) /*(strcmp(temp,"&") == 0) */
		return 28;
	else if((strcmp(temp,"*") == 0) || (strcmp(temp,"/") == 0) || (strcmp(temp,"%") == 0))
		return 26;
	else if((strcmp(temp,"+") == 0) || (strcmp(temp,"-") == 0))
		return 24;
	else if((strcmp(temp,"<<") == 0) || (strcmp(temp,">>") == 0))
		return 22;
	else if((strcmp(temp,"<") == 0) || (strcmp(temp,"<=") == 0) || (strcmp(temp,">") == 0) || (strcmp(temp,">=") == 0))
		return 20;
	else if((strcmp(temp,"==") == 0) || (strcmp(temp,"!=") == 0))
		return 18;
	else if((strcmp(temp,"^") == 0))
		return 16;
	else if((strcmp(temp,"|") == 0))
		return 14;
	else if((strcmp(temp,"&&") == 0))
		return 12;
	else if((strcmp(temp,"||") == 0))
		return 10;
	else if((strcmp(temp,"=") == 0) || (strcmp(temp,"+=") == 0) || (strcmp(temp,"-=") == 0) || (strcmp(temp,"*=") == 0) || (strcmp(temp,"/=") == 0) || (strcmp(temp,"%=") == 0) || (strcmp(temp,"<<=") == 0) || (strcmp(temp,">>=") == 0) || (strcmp(temp,"&=") == 0) || (strcmp(temp,"^=") == 0) || (strcmp(temp,"!=") == 0))
		return 8;
	else if((strcmp(temp,"(") == 0))
		return 6;
	return -1;
}
int isalphabet(char temp) // 숫자 알파벳인가를 판단하는 함수
{
	if(temp >= '0' && temp <= '9') // '0' 부터 '9' 까지
		return 2;
	if((temp >= 'a' && temp <= 'z') || (temp >= 'A' && temp <= 'Z') || (temp == '_')) // _ 이거나 알파벳인가
		return 1;
	else
		return 0;
}
int isoper(char temp) // 연산자 계열인가를 판단하는 함수
{
	if(temp == '-' || temp == '/' || temp == '%') // -나 /인 경우(교환 법칙 성립 하지 않음.)
		return 3;
	else if(temp == '+' || temp == '*' || temp == '>' || temp == '<' || temp == '!')
		return 4;
	else if(temp == '=' || temp == '|' || temp == '&' || temp == '~' || temp == '^')
		return 5;
	else if(temp == '(' || temp == ')')
		return 6;
	else if(temp == '<' || temp == '>')
		return 7;
	else if(temp == '#' || temp == '.' || temp == '"' || temp == ',' || temp == 36 || temp == 39 || temp == 46 || temp == 58 || temp == 59 || temp == 63 || temp == 64)
		return 8;
	else
		return 0;
}
double ans_word_checking(int x,int y)
{
	char *ans_word_file_temp;
	char *stu_word_file_temp;	

	char ans_word_data[SIZE][500]; // 정답 data를 담는 공간
	char ans_word_token[SIZE][SIZE][SIZE]; // 정답 token을 담는 공간
	char stu_word_token[SIZE][500]; // 학생 token을 담는 공간
	char stu_word_data[500]; // 학생 data 담는 공간
	
	int ans_token_count[SIZE] = {0}; // 정답 token 개수를 세는 함수
	int ans_word_count; // ans_word 개수를 세는 함수
	int ans_token_len[SIZE] = {0}; // 토큰 길이를 세는 함수
	int stu_token_len[SIZE] = {0}; // 학생 토큰 길이를 세는 함수
	int stu_token_count = 0; // 토큰 개수를 세는 함수
	int ans_word_file_len;
	int stu_word_file_len;

	int i,j,k,l;
	int fd,fd2;

	char *ans_temp,*ans_temp2;
	char *stu_temp;
	char *fp;
	ssize_t size;

	ans_word_file_len = strlen(trueset_file_dir) + strlen("/") + strlen(ans_table[y].p_name) + strlen("/") + strlen(pro_table[y].p_name);
	ans_word_file_temp = (char *) malloc(ans_word_file_len + 1);
	sprintf(ans_word_file_temp,"%s/%s/%s",trueset_file_dir,ans_table[y].p_name,pro_table[y].p_name);

	stu_word_file_len = strlen(student_file_dir) + strlen("/") + strlen(stu_table[x].f_name) + strlen("/") + strlen(pro_table[y].p_name);
	stu_word_file_temp = (char *) malloc(stu_word_file_len + 1);
	sprintf(stu_word_file_temp,"%s/%s/%s",student_file_dir,stu_table[x].f_name,pro_table[y].p_name);
	if(access(ans_word_file_temp, F_OK) < 0) { // fname이 존재하지 않으면 오류 출력
		fprintf(stderr,"%s doesn't exist. \n", ans_word_file_temp);
		exit(1);
	}

	if(access(stu_word_file_temp,F_OK) < 0) { // fname2가 존재하지 않으면 0점 처리
		return 0.0;	
	}

	if((fd = open(ans_word_file_temp,O_RDONLY,0755)) < 0) { // fname이 읽기로 여는데 실패시 오류출력.
		fprintf(stderr,"File Open Error : %s \n", ans_word_file_temp);
		exit(1);
	}
	
	if((fd2 = open(stu_word_file_temp, O_RDONLY, 0755)) < 0) { // fname2가 읽기로 여는데 실패시 오류출력.
		fprintf(stderr,"File Open Error : %s \n", stu_word_file_temp);
		exit(1);
	}
	size = lseek(fd,0,SEEK_END);
	lseek(fd,0,SEEK_SET);
	
	ans_temp = (char *) malloc(size+1);
	read(fd,ans_temp,size);
	ans_temp[size] = 0;
	ans_temp2 = ans_temp;
	fp = strtok(ans_temp2,":"); // : 으로 구분해야하기 때문에 strtok를 사용
	i = j = k = l = 0;
	while(fp != NULL) {
		if(fp[j] == 0) {
			ans_word_data[i][j] = 0; // ans_word_data 의 끝에 null값 초기화 
			ans_token_len[i] = k; // 길이를 잰다.
			i++;
			j=0;
			k=0;
			fp = strtok(NULL,":");
			continue;
		}
		if(fp[j] == ' ' || fp[j] == '\n') { // 띄어쓰기나 엔터값인 경우
			if(fp[j+1] == 0) { // 다음값이 null값이면 받지 않는다.
				j++;
				continue;
			}
			if(j == 0) { // 맨앞 공백 무시
				j++;
				continue;
			}
			if(fp[j-1] == ' ' || fp[j-1] == '\n') { // 이전값이 띄어쓰기나 엔터값인 경우 다음으로 넘어간다(이는 한칸만 받도록 하기 위함)
				j++;
				continue;
			}
			ans_word_data[i][k] = fp[j];
			j++;
			k++;
			continue;
		}
		ans_word_data[i][k] = fp[j];   
		j++;
		k++;
	}
	ans_word_count = i;
	i = j = k = l = 0;
	for(int for_i = 0; for_i < ans_word_count; for_i++) {
		for(int for_j = 0; for_j <= ans_token_len[for_i] ; for_j++) {
			if(ans_word_data[for_i][for_j] == 0) {
				ans_word_token[for_i][k][l] = 0;
				k = 0;
				l = 0;
				break;
			}
			if(ans_word_data[for_i][for_j] == ' ') { // 공백인 경우 그냥 지나감
				continue;
			}
			if(isalphabet(ans_word_data[for_i][for_j]) >= 1) { // 알파벳이나 숫자인 경우
				if(isoper(ans_word_data[for_i][for_j-1]) >= 2) { // 이전 연산자 계열인 토큰 별도  처리
					ans_word_token[for_i][k][l] = 0;
					ans_token_count[for_i]++;
					k++;
					l = 0;
					ans_word_token[for_i][k][l] = ans_word_data[for_i][for_j];
					l++;
					continue;
				}
				else if(ans_word_data[for_i][for_j-1] == ' ') { // 이전 연산자가 공백이었던 경우
					if(isoper(ans_word_data[for_i][for_j-2]) >= 2) { // 그 이전 연산자를 본다.
						ans_word_token[for_i][k][l] = 0;
						ans_token_count[for_i]++;
						k++;
						l = 0;
						ans_word_token[for_i][k][l] = ans_word_data[for_i][for_j];
						l++;
						continue;
					}
					ans_word_token[for_i][k][l] = 0;
					ans_token_count[for_i]++;
					k++;
					l = 0;
				}				
				ans_word_token[for_i][k][l] = ans_word_data[for_i][for_j];
				l++;
				continue;
			}
			else if(isoper(ans_word_data[for_i][for_j]) >= 2) { // 연산자인 경우
				if(isalphabet(ans_word_data[for_i][for_j-1]) >= 1) { // 이전값이 알파벳인 경우 토큰 별도 처리
					/*
					if(ans_word_data[for_i][for_j] == '(') {
						ans_word_token[for_i][k][l] = ans_word_data[for_i][for_j];
						l++;
						continue;
					}
					*/
					ans_word_token[for_i][k][l] = 0;
					ans_token_count[for_i]++;
					k++;
					l = 0;
					ans_word_token[for_i][k][l] = ans_word_data[for_i][for_j];
					l++;
					continue;
				}
				
				else if(ans_word_data[for_i][for_j-1] == ' ') { // 이전값이 공백인 경우 별도 토큰 처리
					ans_word_token[for_i][k][l] = 0;
					ans_token_count[for_i]++;
					k++;
					l = 0;
					ans_word_token[for_i][k][l] = ans_word_data[for_i][for_j];
					l++;
					continue;
				}
				ans_word_token[for_i][k][l] = ans_word_data[for_i][for_j];
				l++;
				continue;
			}
		}
	}
	size = lseek(fd2,0,SEEK_END);
	lseek(fd2,0,SEEK_SET);

	stu_temp = (char *) malloc(size+1);
	read(fd2,stu_temp,size);
	close(fd);
	close(fd2);

	if(size == 0)
		return 0.0;

	i = j = k = l = 0;
	while(1) {
		if(stu_temp[j] == 0) {
			stu_word_data[l] = 0;
			break;
		}
		if(stu_temp[j] == ' ' || stu_temp[j] == '\n') { // 띄어쓰기나 엔터값인 경우
			if(stu_temp[j+1] == 0) { // 다음 값이 문자의 끝인 경우 받지 않는
				j++;
				continue;
			}
			if(j == 0) {
				j++;
				continue;
			}
			if(stu_temp[j-1] == ' ' || stu_temp[j-1] == '\n') { // 띄어쓰기가 엔터값이 이전에 있었던 경우 무시(1번만 받도록 처리)
				j++;
				continue;
			}
			stu_word_data[l] = stu_temp[j];
			l++;
			j++;
			continue;	
		}
		stu_word_data[l] = stu_temp[j];
		l++;
		j++;
	}
	l = 0;
	for(int for_i = 0; for_i < size ; for_i++) {
		if(stu_word_data[for_i] == 0) {
			stu_word_token[stu_token_count][l] = 0;
			l = 0;
	 		break;
		}
		if(stu_word_data[for_i] == ' ') { // 공백 무시
			if(stu_word_data[for_i+1] == 0) {
				continue;
			}
			if(for_i == 0) {
				continue;
			}
		}
		if(isalphabet(stu_word_data[for_i]) >= 1) { // 알파벳인 경우 처리
			if(isoper(stu_word_data[for_i-1]) >= 2) { // 이전값이 만약 연산자인 경우 토큰 별도 처리
				stu_word_token[stu_token_count][l] = 0;
				stu_token_count++;
				l = 0;
				stu_word_token[stu_token_count][l] = stu_word_data[for_i];
				l++;
				continue;
			}
			else if(stu_word_data[for_i-1] == ' ') { // 이전 값이 공백인 경우
				if(isoper(stu_word_data[for_i-2]) >= 2) { // 그 이전 값을 보고 연산자인 경우 토큰 별도 처리
					stu_word_token[stu_token_count][l] = 0;
					stu_token_count++;
					l = 0;
					stu_word_token[stu_token_count][l] = stu_word_data[for_i];
					l++;
					continue;
				}
				stu_word_token[stu_token_count][l] = 0;
				stu_token_count++;
				l = 0;
			}
			stu_word_token[stu_token_count][l] = stu_word_data[for_i];
			l++;
			continue;
		}
		else if(isoper(stu_word_data[for_i]) >= 2) { // 연산자인 경우 처리
			if(isalphabet(stu_word_data[for_i-1]) >= 1) { // 이전 값이 알파벳인 경우 토큰 별도 처리
				stu_word_token[stu_token_count][l] = 0;
				stu_token_count++;
				l = 0;
				stu_word_token[stu_token_count][l] = stu_word_data[for_i];			
				l++;
				continue;
			}
			else if(stu_word_data[for_i-1] == ' ') { // 이전 값이 공백인 경우
				stu_word_token[stu_token_count][l] = 0; 
				stu_token_count++;
				l = 0;
				stu_word_token[stu_token_count][l] = stu_word_data[for_i];
				l++;
				continue;
			}
			stu_word_token[stu_token_count][l] = stu_word_data[for_i];
			l++;
			continue;
		}
	}
// 정답 문제 토큰 출력용 
/*
	for(int for_i = 0; for_i < ans_word_count; for_i++) 
		for(int for_j = 0; for_j <= ans_token_count[for_i]; for_j++) 
			printf("%s ",ans_word_token[for_i][for_j]);

*/
// 학생 문제 토큰 출력용
/*
	for(int for_i = 0; for_i <= stu_token_count; for_i++)
			printf("%s ",stu_word_token[for_i]);
*/

// 1차 비교(토큰으로 볼때 완전히 일치한가)
	for(int for_i = 0; for_i < ans_word_count; for_i++) {
		if(stu_token_count == ans_token_count[for_i]) {
			int count = 0;
			for(int for_j = 0; for_j <= stu_token_count; for_j++) {
				if((strcmp(stu_word_token[for_j],ans_word_token[for_i][for_j])) == 0) {
					count++;
				}
			}
			if(count == stu_token_count) {
				return ans_table[y].score; // 점수 리턴(하나라도 맞으면 더이상 비교할 이유가 없기때문)
			}
			count = 0;
		}
	}
	return 0.0;
// 정답 후위 표현식 과정
	int ans_top[SIZE] = {0}; 
	int ans_post_len[SIZE] = {0};
	char ans_stack_data[SIZE][500] = {0};
	char ans_post_data[SIZE][SIZE][500] = {0};
	
	for(int for_i = 0; for_i < ans_word_count; for_i++) {
		for(int for_j = 0; for_j <= ans_token_count[for_i]; for_j++) {
			char temp[500];
			char popOp[500];
			strcpy(temp,ans_word_token[for_i][for_j]); // temp에 token 임시로넣음
			if(isalphabet(temp[0]) >= 1) { // 알파벳이면 그냥 후위 표현식 배열에 바로 넣는다.
				strcpy(ans_post_data[for_i][ans_post_len[for_i]++],temp);
			}
			else {
				if(strcmp(temp,"(") == 0) // 소괄호를 stack에 넣는다.
					strcpy(ans_stack_data[ans_top[for_i]++],temp);
				else if(strcmp(temp,")") == 0) // ')' 이면 , '('를 만날때까지 전부 pop
					while(1) {
						strcpy(popOp,ans_stack_data[--ans_top[for_i]]); // pop
						if(strcmp(popOp,"(") == 0)
							break;
						strcpy(ans_post_data[for_i][ans_post_len[for_i]++],popOp); // pop 
					}
				else {
					while(ans_top[for_i] != 0 && (Precedence(ans_stack_data[ans_top[for_i]-1]) >= Precedence(temp))) // 정답이 빈 상황이 아니라면 우선순위 처리
						strcpy(ans_post_data[for_i][ans_post_len[for_i]++],ans_stack_data[--ans_top[for_i]]);
					strcpy(ans_stack_data[ans_top[for_i]++],temp);
				}
			}
		}
		while(ans_top[for_i] != 0) // 정답 스택이 비지 않은 경우 모두 출
			strcpy(ans_post_data[for_i][ans_post_len[for_i]++],ans_stack_data[--ans_top[for_i]]); // stack 맨 위 data를 후위 표현식에 넣음. 
	}

// 정답 후위 표현식 출력
/*
	for(int for_i = 0; for_i < ans_word_count; for_i++) 
		for(int for_j = 0; for_j < ans_post_len[for_i]; for_j++) 
			printf("%s",ans_post_data[for_i][for_j]);
*/
//

	int stu_top = 0;
	int stu_post_len = 0;
	char stu_stack_data[SIZE][500];
	char stu_post_data[SIZE][500];
	for(int for_i = 0; for_i <= stu_token_count; for_i++) { // 토큰 개수 만큼 반복한다.
		char temp[500];
		char popOp[500];
		strcpy(temp,stu_word_token[for_i]);  // 토큰을 임시로  복사한다.
		if(isalphabet(temp[0]) >= 1)	// 알파벳인 경우 그냥 넣는다.
			strcpy(stu_post_data[stu_post_len++],temp);
		else {
			if(strcmp(temp,"(") == 0) // '(' 라면 그냥 넣는다
				strcpy(stu_stack_data[stu_top++],temp);
			else if(strcmp(temp,")") == 0) // ')' 라면 '(' 만날때 까지 반복한다.
				while(1) {
					strcpy(popOp,stu_stack_data[--stu_top]);
					if(strcmp(popOp,"(") == 0)
						break;
					strcpy(stu_post_data[stu_post_len++],popOp);
				}
			else {
				while(stu_top != 0 && (Precedence(stu_stack_data[stu_top-1]) >= Precedence(temp))) // top이 0이 아닌 경우 우선순위 처
					strcpy(stu_post_data[stu_post_len++],stu_stack_data[--stu_top]);
				strcpy(stu_stack_data[stu_top++],temp);
			}
		}
	}
	while(stu_top != 0) 
		strcpy(stu_post_data[stu_post_len++],stu_stack_data[--stu_top]);

// 학생 후위 표현식
/*	printf("\n");
	for(int for_i = 0; for_i <= stu_post_len; for_i++)
	{
		printf("%s",stu_post_data[for_i]);
	}
*/

	for(int for_i = 0; for_i < ans_word_count; for_i++) {
		if(ans_post_len[for_i] == stu_post_len) {
			for(int for_j = 0; for_j <= stu_post_len; for_j++) {
				char temp[500];
				if(((strcmp(stu_post_data[for_j],">") == 0)	&& (strcmp(ans_post_data[for_i][for_j],"<") == 0))) { // '>' 와 '<'에 대한 교환 처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j-2],ans_post_data[for_i][for_j-1])) == 0) {
						strcpy(temp,"<");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}
				else if(((strcmp(stu_post_data[for_j],">=") == 0) && (strcmp(ans_post_data[for_i][for_j],"<=") == 0))) { // '>=' 와 '<='에 대한 교환 처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j-2],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,"<=");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}
				else if(((strcmp(stu_post_data[for_j],"<") == 0) && (strcmp(ans_post_data[for_i][for_j],">") == 0))) { // '<' 와 '>'에 대한 교환 처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j-2],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,">");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}
				else if(((strcmp(stu_post_data[for_j],"<=") == 0) && (strcmp(ans_post_data[for_i][for_j],">=") == 0))) { // '<=' 와 '>='에 대한 교환 처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j-2],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,">=");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}
				else if(((strcmp(stu_post_data[for_j],"+") == 0) && (strcmp(ans_post_data[for_i][for_j],"+") == 0))) { // '+'에 대한 교환처리를 하는 과정이다
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j-2],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,"+");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}
/*
				else if(((strcmp(stu_post_data[for_j],"*") == 0) && (strcmp(ans_post_data[for_i][for_j],"*") == 0))) { // '*'에 대한 교환처리를 하는 과정이다
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j+1],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,"*");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}
*/
				else if(((strcmp(stu_post_data[for_j],"+=") == 0) && (strcmp(ans_post_data[for_i][for_j],"+=") == 0))) { // '+='에 대한 교환처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j+1],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,"+=");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}
				/*
				else if(((strcmp(stu_post_data[for_j],"*=") == 0) && (strcmp(ans_post_data[for_i][for_j],"*=") == 0))) { // '*='에 대한 교환처리 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j+1],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,"*=");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}*/
				else if(((strcmp(stu_post_data[for_j],"==") == 0) && (strcmp(ans_post_data[for_i][for_j],"==") == 0))) { // '=='에 대한 교환처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j+1],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,"==");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}
				else if(((strcmp(stu_post_data[for_j],"!=") == 0) && (strcmp(ans_post_data[for_i][for_j],"!=") == 0))) { // '!='에 대한 교환처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j+1],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,"!=");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}

			}	
		}
	}						

	for(int for_i = 0; for_i < ans_word_count; for_i++) { // 정답 개수를 세서 비교하여 답인지 확인한다.
		if(ans_post_len[for_i] == stu_post_len) {
			int count = 0;
			for(int for_j = 0; for_j <= stu_post_len; for_j++) {
				if(strcmp(ans_post_data[for_i][for_j],stu_post_data[for_j]) == 0)
					count++;
			}
			if(count-1 == stu_post_len || count == stu_post_len) {
				return ans_table[y].score;
			}
		}
	}

	for(int for_i = 0; for_i <ans_word_count; for_i++) { // &&이나 ||인 경우에 대한 교환 처리.
		if(ans_post_len[for_i] == stu_post_len) {
			char temp[6][500];
			for(int for_j = 0; for_j <= stu_post_len; for_j++) {
				if((strcmp(stu_post_data[for_j],"&&") == 0) || (strcmp(stu_post_data[for_j],"||") == 0)) {
					strcpy(temp[2],stu_post_data[for_j-1]);
					strcpy(temp[1],stu_post_data[for_j-2]);
					strcpy(temp[0],stu_post_data[for_j-3]);
					strcpy(temp[5],stu_post_data[for_j-4]);
					strcpy(temp[4],stu_post_data[for_j-5]);
					strcpy(temp[3],stu_post_data[for_j-6]);
					for(int for_k = 0; for_k < 6; for_k++) {
						strcpy(stu_post_data[for_j-6+for_k],temp[for_k]);
					}
				}
			}

		}
	}

	/* 위에서 사용했던 교환법칙과 동일하다. */

	for(int for_i = 0; for_i < ans_word_count; for_i++) {
		if(ans_post_len[for_i] == stu_post_len) {
			for(int for_j = 0; for_j <= stu_post_len; for_j++) {
				char temp[500];
				if(((strcmp(stu_post_data[for_j],">") == 0)	&& (strcmp(ans_post_data[for_i][for_j],"<") == 0))) { // '>'와 '<'에 대한 교환처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j-2],ans_post_data[for_i][for_j-1])) == 0) {
						strcpy(temp,"<");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}
				else if(((strcmp(stu_post_data[for_j],">=") == 0) && (strcmp(ans_post_data[for_i][for_j],"<=") == 0))) { // '>='와 '<='에 대한 교환처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j-2],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,"<=");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}
				else if(((strcmp(stu_post_data[for_j],"<") == 0) && (strcmp(ans_post_data[for_i][for_j],">") == 0))) { // '<'와 '>'에 대한 교환처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j-2],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,">");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}
				else if(((strcmp(stu_post_data[for_j],"<=") == 0) && (strcmp(ans_post_data[for_i][for_j],">=") == 0))) { // '<='와 '>='에 대한 교환처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j-2],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,">=");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}
				else if(((strcmp(stu_post_data[for_j],"+") == 0) && (strcmp(ans_post_data[for_i][for_j],"+") == 0))) { // '+'에 대한 교환처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j-2],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,"+");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}
/*
				else if(((strcmp(stu_post_data[for_j],"*") == 0) && (strcmp(ans_post_data[for_i][for_j],"*") == 0))) { // '*'에 대한 교환처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j+1],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,"*");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}
*/
				else if(((strcmp(stu_post_data[for_j],"+=") == 0) && (strcmp(ans_post_data[for_i][for_j],"+=") == 0))) { // '+='에 대한 교환처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j+1],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,"+=");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}
				/*
				else if(((strcmp(stu_post_data[for_j],"*=") == 0) && (strcmp(ans_post_data[for_i][for_j],"*=") == 0))) { // '*='에 대한 교환처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j+1],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,"*=");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}*/
				else if(((strcmp(stu_post_data[for_j],"==") == 0) && (strcmp(ans_post_data[for_i][for_j],"==") == 0))) { // '=='에 대한 교환처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j+1],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,"==");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}
				else if(((strcmp(stu_post_data[for_j],"!=") == 0) && (strcmp(ans_post_data[for_i][for_j],"!=") == 0))) { // '!='에 대한 교환처리를 하는 과정이다.
					if((strcmp(stu_post_data[for_j-1],ans_post_data[for_i][for_j-2]) == 0) && (strcmp(stu_post_data[for_j+1],ans_post_data[for_i][for_j-1]) == 0)) {
						strcpy(temp,"!=");
						strcpy(stu_post_data[for_j],temp);
						strcpy(temp,stu_post_data[for_j-1]);
						strcpy(stu_post_data[for_j-1],stu_post_data[for_j-2]);
						strcpy(stu_post_data[for_j-2],temp);
					}
				}

			}	
		}
	}						

	for(int for_i = 0; for_i < ans_word_count; for_i++) { // 교환 이후 정답 개수를 세서 정답인지 확인한다.
		if(ans_post_len[for_i] == stu_post_len) {
			int count = 0;
			for(int for_j = 0; for_j <= stu_post_len; for_j++) {
				if(strcmp(ans_post_data[for_i][for_j],stu_post_data[for_j]) == 0)
					count++;
			}
			if(count-1 == stu_post_len || count == stu_post_len) {
				return ans_table[y].score;
			}
		}
	}

	return 0.0;
}	
