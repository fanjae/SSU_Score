#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "ssu_extern.h"
#include "ssu_file_load.h"
#include "ssu_testing_problem.h"
#include "ssu_create_result.h"
#include "ssu_word.h"

int *stu_folder_length; // 학생 폴더 길이
int *ans_file_length; // 정답(파일) 개수에 대한 길이
int *pro_file_length; // 문제(파일) 개수에 대한 길이
int problem_count=0; // 문제의 개수
int option_state[5]; // 옵션의 상태
int parameter_state[5]; // 옵션(파라미터)의 개수
int student_count=0; // 학생의 수 
	
char *student_file_dir; // 학생 디렉토리
char *trueset_file_dir; // 정답 디렉토리

double all_blank_score; // 모든 빈칸 문제에 대한 점수
double all_program_score; // 모든 프로그램 문제에 대한 점수
double sum;

struct answer_data *ans_table; // 정답 정보를 담는 table.
struct problem_data *pro_table; // 프로그램 정보를 담는 table.
struct student_data *stu_table; // 학생 정보를 담는 table.

#define BUFFER_SIZE 1024

int problem_setting(char *tmp)
{
	char *temp = "ssu_ans_list.txt";
	char *temp2 = "ssu_pro_list.txt";
	char *temp3 = "ssu_stu_folder_list.txt";
	char *temp4 = "/score_table.csv";
	char *dir,*dir2,*dir3,*dir4;
	char character;

	int fd, fd2;
	int len;
	int ch_len = 0;
	int index = 0;

	len = strlen(temp) ;
	dir = (char *) malloc(len + 1);	
	strcat(dir,temp);

	len = strlen(temp2);
	dir2 = (char *) malloc(len + 1);
	strcat(dir2,temp2);


	len = strlen(temp3);
	dir3 = (char *) malloc(len + 1) ;
	strcat(dir3,temp3);

	len = strlen(tmp) + strlen(temp4) ;
	dir4 = (char *) malloc(len + 1);
	strcat(dir4,tmp);
	strcat(dir4,temp4);

	/* 아래는 ssu_ans_list.txt로 부터 정답 폴더의 목록을 받아온다.
	정답 폴더의 개수를 받아온 다음에는 정답 폴더의 길이를 받아온다.
	정답 폴더의 길이를 받아온 다음에는 정답 table에 정답 폴더 이름을 
	동적 할당 하여 넣어주는 과정이다. */

	if ((fd = open(dir, O_RDONLY, 0755)) < 0) { // ssu_ans_list.txt를 open한다. // open이 안되었으면 error 처리.
		fprintf(stderr, "File Open Error : %s\n", dir);
		exit(1);
	}

	// 문제 개수를 ssu_ans_list.txt로 부터 count 해온다.
	while(1) 
	{
		if (read(fd, &character, 1) > 0) {
			if (character == '\n') {
				problem_count++;
			}
		}
		else
			break;
    }
	close(fd);

	// 정답 테이블에는 정답 파일의 개수 만큼 동적 할당을 받는다.
	// 정답 파일 길이에도 정답 파일의 개수 만큼 동적 할당을 받는다.
	ans_table = (struct answer_data *) malloc( sizeof (struct answer_data) * problem_count);
	ans_file_length = (int *) malloc( sizeof (int) * problem_count); 
	
	if ((fd = open(dir, O_RDONLY, 0755)) < 0) {
		fprintf(stderr, "File Open Error : %s\n", dir);
		exit(1);
	}

	ch_len = 0;
	index = 0;
	// ssu_ans_list.txt를 읽어와서 해당 문제 폴더의 길이를 읽어온다.
	while(1)
	{
		if (read(fd, &character, 1) > 0) {
			ch_len++;
			if (character == '\n') { // 엔터값을 만나기 전까지가 해당 문제에 대한 길이다.
				ans_file_length[index] = ch_len-1;
				ch_len = 0;
				index++;
			}
		}
		else
			break;
	}
	close(fd);

	// 정답 table의 정답 폴더 이름을 길이만큼 동적 할당한다.
	for(int i=0; i<problem_count; i++)  
		ans_table[i].p_name = (char *) malloc(ans_file_length[i] + 1);  

	if ((fd = open(dir, O_RDONLY, 0755)) < 0) {
		fprintf(stderr, "File Open Error : %s\n", dir);
		exit(1);
	}
	
	ch_len = 0;
	index = 0;
	// ssu_ans_list.txt를 읽어와서 해당 문제 폴더의 이름을 넣어준다.	
	while(1)
	{
		if (read(fd, &character, 1) > 0) {
			ans_table[index].p_name[ch_len] = character;
			ch_len++;
			if (character == '\n') {
				ans_table[index].p_name[ch_len-1] = 0;
				ch_len = 0;
				index++;
			}
		}
		else
			break;
	}
	close(fd);
	/* 정답 table 관련 작업은 여기서 끝이 난다. */


	/* 아래는 ssu_pro_list.txt로 부터 문제 정보의 목록을 받아온다.
	문제(파일)의 개수를 받아온 다음에는 문제 정보의 길이를 받아온다.
	문제(파일)의 길이를 받아온 다음에는 문제 table에 문제 폴더 이름을 
	동적 할당 하여 넣어주는 과정이다. */

	// 문제 파일 리스트를 load 하여 ssu_pro_list.txt에 불러온다. 
	if(pro_file_list_load() != 0) { 
		fprintf(stderr,"Function Error : pro_file_list_load\n");	
		exit(1);
	}
	
	// 문제 table에 문제의 개수만큼 동적 할당을 받는다.
	// 이는 문제 길이도 동일하다. 문제의 길이를 받는 table도 문제의 개수만큼 동적 할당을 받는다.
	pro_table = (struct problem_data *) malloc( sizeof (struct problem_data) * problem_count);
	pro_file_length = (int *) malloc( sizeof (int) * problem_count); 

	if ((fd = open(dir2, O_RDONLY, 0755)) < 0) { // ssu_pro_list.txt를 읽어온다.
		fprintf(stderr, "File Open Error : %s\n", dir2);
		exit(1);
	}

	index = 0;
	ch_len = 0;
	while(1)  
	{
		if (read(fd, &character, 1) > 0) {
			ch_len++;
			if (character == '\n') {
				pro_file_length[index++] = ch_len-1;
				ch_len = 0;
			}
		}
		else
			break;
	}
	close(fd);

	// 문제 이름에 문제 길이 만큼 동적 할당한다.
	for(int i=0; i<problem_count; i++)  
		pro_table[i].p_name = (char *) malloc(pro_file_length[i] + 1);

	if ((fd = open(dir2, O_RDONLY, 0755)) < 0) { // ssu_pro_list.txt를 읽어온다.
		fprintf(stderr, "File Open Error : %s\n", dir2);
		exit(1);
	}	

	ch_len = 0;
	index = 0;
	// ssu_pro_list.txt를 읽어와서 문제(파일)의 이름을 넣어준다.
	while(1)
	{
		if (read(fd, &character, 1) > 0) {
			pro_table[index].p_name[ch_len] = character;
			ch_len++;

			if (character == '\n') { // 엔터값 이전까지가 실질적인 길이이다.
				pro_table[index].p_name[ch_len-1] = 0;
				ch_len = 0;
				index++;
			}
		}
		else
			break;
	}
	close(fd);
	/* 문제(파일) table 관련 정보는 여기서 끝이 난다. */
	/* 아래는 ssu_stu_folder_list.txt로 부터 학생 정보의 목록을 받아온다.
	학생(폴더)의 개수를 받아온 다음에는 문제 정보의 길이를 받아온다.
	학생(폴더)의 길이를 받아온 다음에는 문제 table에 문제 폴더 이름을 
	동적 할당 하여 넣어주는 과정이다. */
	if(stu_folder_list_load(student_file_dir) != 0) { // ssu_stu_folder_list에 먼저 학생 폴더를 읽어온다.
		fprintf(stderr, "Function Error : stu_folder_list_load\n");
		exit(1);
	}
	if ((fd = open(dir3, O_RDONLY, 0755)) < 0) { // ssu_stu_folder_list.txt를 읽어온다.
		fprintf(stderr, "It's Wrong? File Open Error : %s\n", dir3);
		exit(1);
	}
	
	// ssu_stu_folder_list.txt로 부터 학생의 수를 읽어온다.
	while(1) 
	{
		if (read(fd, &character, 1) > 0) {
			if (character == '\n') 
				student_count++;
		}
		else
			break;
    }
	close(fd);

	// 학생 table에 학생의 수 만큼 동적할당을 받는다.
	// 학생 folder의 길이도 학생의 수 만큼 동적할당을 받는다.
	stu_table = (struct student_data *) malloc( sizeof(struct student_data) * student_count);
	stu_folder_length = (int *) malloc( sizeof(int) * (student_count)); 
	
	if ((fd = open(dir3, O_RDONLY, 0755)) < 0) { // ssu_stu_folder_list.txt를 읽어온다.
		fprintf(stderr, "File Open Error : %s\n", dir3);
		exit(1);
	}
	fd = open(dir3,O_RDONLY,0755);

	ch_len = 0;
	index = 0;
	// ssu_stu_folder_list.txt를 읽어와서 학생 folder에 길이를 잰다. 
	while(1)
	{
		if (read(fd, &character, 1) > 0) {
			ch_len++;
			if (character == '\n') {
				stu_folder_length[index++] = ch_len-1;
				ch_len = 0;
			}
		}
		else
			break;
	}
	close(fd);

	// 학생 table의 문제별 folder의 길이와 문제 개수 만큼 동적 할당한다. 
	for(int i=0; i<student_count; i++) { 
		stu_table[i].f_name = (char *) malloc(stu_folder_length[i] + 1);
		stu_table[i].score = (double *) malloc(sizeof(double) * problem_count);
	}	
	
	if ((fd = open(dir3, O_RDONLY, 0755)) < 0) { // ssu_stu_folder_list.txt를 읽어온다.
		fprintf(stderr, "File Open Error : %s\n", dir3);
		exit(1);
	}
	ch_len = 0;
	index = 0;
	// ssu_stu_folder_list.txt를 읽어와서 학생 table에 폴더 이름을 넣어준다. 
	while(1)
	{
		if (read(fd, &character, 1) > 0) {
			stu_table[index].f_name[ch_len] = character;
			ch_len++;
			if (character == '\n') {
				stu_table[index].f_name[ch_len-1] = 0;
				ch_len = 0;
				index++;
			}
		}
		else
			break;
	}
	close(fd);
	/* 학생(폴더) 관련 정보는 여기서 끝난다. */ 
	
	if(pro_type_set() != 0) { // 문제의 타입을 결정한다 (빈칸인가 프로그램 채점문제인가)
		fprintf(stderr,"Function Error : pro_type_set\n");	
		exit(1);
	}

	system("rm ssu_pro_list.txt");
	system("rm ssu_ans_list.txt");
	system("rm ssu_stu_folder_list.txt");


	if ((fd2 = open(dir4, O_RDONLY, 0755)) < 0) {
		
	}
	else {
		int temp_file_count = 0;
		int **temp_file_length;

		char **temp_folder_length;
		char **temp_score_length;
		while(1) {
			char ch;
			if(read(fd2,&ch,1) > 0) {
				if(ch == '\n')
					temp_file_count++;
			}
			else
				break;
		}

		temp_file_length = (int **) malloc(sizeof(int *) * 2); // file의 길이를 받을 변수로 이를 동적할당 한다.
		temp_file_length[0] = (int *) malloc(sizeof(int) * temp_file_count); // 문제 이름에 대한 변수 동적할당.
		temp_file_length[1] = (int *) malloc(sizeof(int) * temp_file_count); // 문제 점수에 대한 변수 동적할당.

		temp_folder_length = (char **) malloc(sizeof(char *) * temp_file_count); // 문제 이름에 대한 동적할당.
		temp_score_length = (char **) malloc(sizeof(char *) * temp_file_count); // 문제 점수에 대한 동적할당.
	
		// 임시 문제의 개수 만큼 동적할당 한다.
		for(int i=0; i<temp_file_count; i++) {
			temp_folder_length[i] = (char *) malloc(sizeof(char) * temp_file_length[0][i] + 1);
			temp_score_length[i] = (char *) malloc(sizeof(char) * temp_file_length[1][i] + 1);
		}
		
		if(temp_file_count > problem_count)	{
			fprintf(stderr,"Error! score_table problem list is more than ANS_DIR problem_count\n");
			fprintf(stderr,"Remove score_csv\n");
			score_table_csv_remove(trueset_file_dir);
		}
		else if(temp_file_count < problem_count) {
			fprintf(stderr,"Error! score_table problem list is less than ANS_DIR problem count\n");
			fprintf(stderr,"Remove score_csv.\n");
			score_table_csv_remove(trueset_file_dir);
		}
		free(temp_file_length);
		free(temp_folder_length);
		free(temp_score_length);
	}
	close(fd2);

	if ((fd2 = open(dir4, O_RDONLY, 0755)) < 0) { // score_table.csv를 읽어온다. (실패시 존재하지 않으므로 써야한다.)
		int type;
		char temp_data[10];

		printf("score_table.csv file doesn't exist in TRUEDIR!\n");
		printf("1. input blank question and program question's socre. ex) 0.5 1\n");
		printf("2. input all question's score. ex) Input value of 1-1: 0.1\n");
		printf("select type >> ");
		scanf("%d", &type);
		if(type == 1) {
			printf("Input value of blank question : ");
			scanf("%lf", &all_blank_score);
			printf("Input value of program question : ");
			scanf("%lf", &all_program_score);
			
			for(int i = 0;  i < problem_count; i++) { // 문제 개수 만큼 반복하면서 문제의 type을 확인한다.
				if(ans_table[i].type == 1)
					ans_table[i].score = all_blank_score;
				else if(ans_table[i].type == 2)
					ans_table[i].score = all_program_score;
			}
		}
		else {
			for(int i = 0; i < problem_count; i++) { //  문제 개수 만큼 반복하면서 문제 점수를 받아온다.
				printf("Input of %s: ",pro_table[i].p_name);
				scanf("%lf",&ans_table[i].score);
			}
		}

		if((fd2 = open(dir4, O_WRONLY | O_CREAT,0644)) < 0) { // score_table.csv를 쓰기 모드로 불러온다.
			fprintf(stderr,"File Open Error for : %s\n",dir4);
			exit(1);
		}
		for(int i = 0; i < problem_count; i++) { // table에 ,로 구분하여 써낸다.
			len = strlen(pro_table[i].p_name);
			sprintf(temp_data, "%.2lf", ans_table[i].score);
			write(fd2,(char *)pro_table[i].p_name,len);
			write(fd2,",",1);
			len = strlen(temp_data);
			write(fd2,(char *)temp_data,len);
			write(fd2,"\n",1);	
		}
	} 
	else { // score_table.csv가 기존에 존재하여 읽어온 경우

		int temp_file_count=0;
		int **temp_file_length; // file의 길이를 받을 임시 변수 첫번째에는 문제 이름, 두번째에는 점수를 받음
		char **temp_folder_length; // 문제의 이름의 길이를 받아올 임시 변수
		char **temp_score_length; // 문제의 점수의 길이를 받아올 임시 변수
		
		while(1) {
			char ch;
			if(read(fd2,&ch,1) > 0) {
				if(ch == '\n')
					temp_file_count++;
			}
			else
				break;
		}

		temp_file_length = (int **) malloc(sizeof(int *) * 2); // file의 길이를 받을 변수로 이를 동적할당 한다.
		temp_file_length[0] = (int *) malloc(sizeof(int) * temp_file_count); // 문제 이름에 대한 변수 동적할당.
		temp_file_length[1] = (int *) malloc(sizeof(int) * temp_file_count); // 문제 점수에 대한 변수 동적할당.

		temp_folder_length = (char **) malloc(sizeof(char *) * temp_file_count); // 문제 이름에 대한 동적할당.
		temp_score_length = (char **) malloc(sizeof(char *) * temp_file_count); // 문제 점수에 대한 동적할당.
		lseek(fd2,0,SEEK_SET);

		ch_len=0;
		index = 0;
		while(1) {
			char ch;
			if(read(fd2,&ch,1) > 0) { // score_table.csv를 읽어온다.
				ch_len++;
				if(ch == ',') { // 1번째 ','까지는 문제 이름에 대한 정보이다.
					temp_file_length[0][index] = ch_len-1;		
					ch_len = 0;
				}
				else if(ch == '\n')	{ // ','를 기준으로 엔터값 까지는 문제 점수에 대한 정보이다.
					temp_file_length[1][index] = ch_len-1;
					index++;
					ch_len = 0;
				}
			}
			else
				break;
		}
		// 임시 문제의 개수 만큼 동적할당 한다.
		for(int i=0; i<temp_file_count; i++) {
			temp_folder_length[i] = (char *) malloc(sizeof(char) * temp_file_length[0][i] + 1);
			temp_score_length[i] = (char *) malloc(sizeof(char) * temp_file_length[1][i] + 1);
		}
		
		lseek(fd2,0,SEEK_SET);
		int flag=0;
		index = 0;
		ch_len = 0;
		while(1) {
			if(read(fd2,&character,1) > 0) { // score_table.csv를 읽어온다.
				if(character == ',' && flag == 0) { // ','까지의 길이를 재어 문제 길이를 잰다.
					temp_folder_length[index][ch_len] = 0;
					ch_len = 0;
					flag = 1;
				}
				else if(character == '\n' && flag == 1) { // '\n' 까지의 길이를 재어 점수 길이를 잰다.
					temp_score_length[index][ch_len] = 0;
					index++;
					ch_len = 0;
					flag = 0;
				}
				else {
					if(flag == 0) // ','까지의 과정은 임시 문제 이름에 추가
						temp_folder_length[index][ch_len] = character;
					else if(flag == 1) // '\n' 까지의 과정은 임시  점수에 추가
						temp_score_length[index][ch_len] = character;
					ch_len++;
				}
			}
			else
				break;
		}
		close(fd2);
	
		// temp로 부터 읽어온 문제에 대해서 정답파일에 존재하는지 확인한다.
		for(int i = 0; i < temp_file_count; i++) {
			int flag = 0;
			for(int j = 0; j < problem_count; j++) {
				if(strcmp(temp_folder_length[i], pro_table[i].p_name) == 0)	{
					ans_table[i].score = atof(temp_score_length[i]);
					flag = 1;
					break;		
				}
			}
			if(flag == 0) // 정답파일에 그런 문제가 없는 경우에 대한 오류처리.
			{
				fprintf(stderr,"problem : %s is not exist in %s\n",temp_folder_length[i],trueset_file_dir);
				exit(1);
			}
		}
		free(temp_file_length); // 동적할당 반환
		free(temp_folder_length); // 동적할당 반환
		free(temp_score_length); // 동적할당 반환
	}

	/* 채점을 시작 하는 부분 */

	printf("grading student's test papers..\n");
	if(ans_checking(trueset_file_dir) != 0) {
		fprintf(stderr,"Function Error ans_checking!!\n");
		exit(1);
	}
	double sum = 0;
	for(int i=0; i<student_count; i++)
	{
		stu_table[i].sum = 0;
		for(int j=0; j<problem_count; j++)
		{
			if(ans_table[j].type == 1) // 빈칸 채점 과정 시작
			{
				stu_table[i].score[j] = ans_word_checking(i, j);
				stu_table[i].sum += stu_table[i].score[j];
			}
			else if(ans_table[j].type == 2) // 프로그램 채점 과정 시작
			{
				stu_table[i].score[j] = auto_problem_marking(i, j);	
				stu_table[i].sum += stu_table[i].score[j];
			} 
		}
		if(option_state[1] == 0) // p옵션이 없는 경우
			printf("%s is finished..\n",stu_table[i].f_name);
		if(option_state[1] == 1) // p 옵션이 있는 경우
			printf("%s is finished.. score : %.2lf\n",stu_table[i].f_name,stu_table[i].sum);	
		sum += stu_table[i].sum;
	}
	if(option_state[1] == 1) // p 옵션이 있는 경우
		printf("Total average : %.2lf\n", sum / student_count);
	if(option_state[2] == 1) {
		if(parameter_state[2] > 5) // -c 옵션이 있는 경우(채점 진행 후)
			parameter_state[2] = 5;
		for(int k = 0; k < parameter_state[2]; k++) 
			for(int j = 0; j < student_count; j++) 
				if(strcmp(option_list[2][k], stu_table[j].f_name) == 0) 
					printf("%s's score : %.2lf\n",stu_table[j].f_name,stu_table[j].sum);
				
	}
	score_result_create(); // 스코어 테이블 생성
	system("rm ssu_pro_open_err_test.txt");
	system("rm ssu_pro_open_test.txt");
}	
