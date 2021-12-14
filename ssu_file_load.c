#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ssu_extern.h"

char ***option_list;
char *trueset_file_dir;
char *student_file_dir;
struct answer_data *ans_table;
struct problem_data *pro_table;

int problem_count;

int error_reset(void) // 에러 정보에 대한 리셋을 하는 함수이다.
{
	char *fname = "ssu_load_temp_error.txt";
	char *directory_make_temp;
	char *directory_remove_temp;

	int fd;
	int directory_make_temp_len;
	int directory_remove_temp_len;
	int stdoutcopy = dup(1);
	
	if ((fd = open(fname,O_WRONLY | O_CREAT, 0755)) < 0) {
		fprintf(stderr,"Directory : %s open error!\n",fname);
		exit(1);
	}

	dup2(fd,1);
	close(fd);

	directory_remove_temp_len = strlen("rm -r ") + strlen(option_list[4][0]);
	directory_remove_temp = malloc(directory_remove_temp_len + 1);
	sprintf(directory_remove_temp,"rm -r %s",option_list[4][0]);
	system(directory_remove_temp);

	
	directory_make_temp_len = strlen("mkdir ") + strlen(option_list[4][0]);
	directory_make_temp = malloc(directory_make_temp_len + 1);
	sprintf(directory_make_temp,"mkdir %s",option_list[4][0]);
	system(directory_make_temp);
	dup2(stdoutcopy,1);
	close(stdoutcopy);	

//	free(directory_remove_temp);
//	free(directory_make_temp);
	sleep(1);
	return 0;
}
int directory_not_exist(char *tmp) // directory 존재 여부 확인.
{
	char *fname = "ssu_load_temp_error.txt"; // 임시 파일
	char *file_read_temp;

	int stderrcopy = dup(2);
	int fd,fd2;
	int len;

	len = strlen("cd ") + strlen((char *)tmp);
	file_read_temp = (char *) malloc(len + 1);
	strcat(file_read_temp,"cd ");
	strcat(file_read_temp,tmp);

	if ((fd = open(fname,O_WRONLY | O_CREAT, 0755)) < 0) { // fname 파일을 쓰기모드로 오픈한다.
		fprintf(stderr,"Directory : %s open error!\n",fname);
		exit(1);
	}
	else {
		dup2(fd,2);
		close(fd);

		system(file_read_temp); // (cd ANS_DIR 수행)
		system("rm ssu_load_temp_error.txt"); // 표준에러를 담고 있는 변수를 삭제한다.
		dup2(stderrcopy,2);
		close(stderrcopy);
		if ((fd2 = open(tmp, O_RDONLY, 0755)) < 0) {
			fprintf(stderr,"Directory : %s is not exist!!\n",tmp);
			exit(1);
	 	}
		else {
			// free(file_read_temp); // 동적할당 메모리 반환.
			return 1;
		}
	}
	
}

int err_directory_not_exist(char *tmp) // error를 만들어줄 directory가 존재하는가 확인하는 함수
{
	char *file_read_temp;
	
	int fd;
	int len;
		
	len = strlen((char *) tmp);
	file_read_temp = (char *) malloc(len + 1);
	strcat(file_read_temp,tmp);

	if ((fd = open(file_read_temp, O_RDONLY, 0755)) < 0) { // open 했을때 존재하지 않으면 해당 디렉토리는 없는 파일임.
		fprintf(stderr,"Error! Error Directory : %s is not exist!!\n",tmp);
		exit(1);
	}
	else 
		close(fd);

	free(file_read_temp);
	return 1;


}
int ans_directory_not_exist(char *tmp) // 정답 파일 디렉토리 존재 여부를 확인하는 함수
{
	char *file_read_temp;
	
	int fd;
	int len;
		
	len = strlen(trueset_file_dir) + strlen("/") + strlen((char *) tmp);
	file_read_temp = (char *) malloc(len + 1);
	strcat(file_read_temp,trueset_file_dir);
	strcat(file_read_temp,"/");
	strcat(file_read_temp,tmp);

	if ((fd = open(file_read_temp, O_RDONLY, 0755)) < 0) { // open 했을때 존재하지 않으면 해당 디렉토리는 없는 파일임.
		fprintf(stderr,"Error! Answer Directory :  %s is not exist!!\n",tmp);
		exit(1);
	}
	else 
		close(fd);

	free(file_read_temp); // 동적 할당 값 반환
	return 1;
}

int stu_directory_not_exist(char *tmp) // 학생 디렉토리가 존재하는가 확인하는 함수
{
	char *file_read_temp;

	int fd,fd2;
	int len;

	len = strlen(student_file_dir) + strlen("/") + strlen((char *) tmp);
	file_read_temp = (char *) malloc(len + 1);
	strcat(file_read_temp,student_file_dir);
	strcat(file_read_temp,"/");
	strcat(file_read_temp,tmp);
	if ((fd = open(file_read_temp, O_RDONLY, 0755)) < 0) { // open 했을때 존재하지 않으면 해당 디렉토리는 없는 파일임.
		fprintf(stderr,"Error! Student Directory : %s is not exist\n",tmp);
		exit(1);
	}
	else {
		
		close(fd);
	}
	free(file_read_temp); // 동적 할당 값 반환
	return 0;
}

int ans_file_list_load(char *tmp) // 정답 파일을 불러오는 함수
{
	char *sys;
	char *fname = "ssu_ans_list.txt";

	int len;
	int fd;
	int stdoutcopy = dup(1);
	len = strlen("ls ") + strlen(tmp) + strlen("/ -v1 | grep -v .csv | grep -v .exe | grep -v .stdout | grep -v .txt") ;
	sys = (char *) malloc(len + 1);
	sprintf(sys,"ls %s/ -v1 | grep -v .csv | grep -v .exe | grep -v .stdout | grep -v .txt",tmp);

	if ((fd = open(fname, O_WRONLY | O_CREAT,  0755)) < 0) {
		fprintf(stderr, "File Open Error %s\n", fname);
		exit(1);
	}
	else {
		dup2(fd,1);
		close(fd);
		system(sys);
		dup2(stdoutcopy,1);
		close(stdoutcopy);
	}
	free(sys); // 동적 메모리 반환.
	return 0;
}

int pro_file_list_load(void) // 문제 리스트를 불러오는 함수이다.
{

	char *fname = "ssu_pro_list.txt";
	int fd;
	off_t size;
	if ((fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0755)) < 0) { // 기존 문제 리스트를 삭제(새로 받아와야함)
		fprintf(stderr, "File Open Error : %s in pro_file_list_load function\n",fname);
		exit(1);
	}
	close(fd);

	for(int i=0; i<problem_count; i++) {
		if ((fd = open(fname, O_WRONLY | O_CREAT, 0755)) < 0) { // 문제 파일을 쓰기 시작.
			fprintf(stderr, "File Open Error : %s\n",fname);
			exit(1);
		}
		size = lseek(fd, 0,SEEK_END); // 파일의 길이를 받아옴.
		lseek(fd,size,SEEK_SET); // 가장 끝으로 이동함. (문제 리스트를 쓸때 파일의 끝부터 쓰게 하기 위함)
		int stdoutcopy = dup(1);
		dup2(fd,1);
		close(fd);
		char *sys;
		int len;
		len = strlen("ls ") + strlen(trueset_file_dir) + strlen("/") + strlen(ans_table[i].p_name) + strlen(" | grep -v .stdout") + strlen(" | grep -v .exe"); // ex) ls ANS_DIR/1 | grep -v .stdout | grep -v .exe에 대한 길이 받음.
		
		sys = (char *) malloc(len + 1); // 길이 만큼 동적 할당.
		sprintf(sys,"ls %s/%s | grep -v .stdout | grep -v .exe",trueset_file_dir,ans_table[i].p_name);
		system(sys);
		dup2(stdoutcopy,1);
		close(stdoutcopy);
		free(sys); // 동적 할당 메모리 반환.
	}
	return 0;

}

int stu_folder_list_load(char *tmp) // 학생 폴더 리스트를 불러오는 함수이다.
{
	char *sys;
	char *fname = "ssu_stu_folder_list.txt";

	int len;
	int fd;
	int stdoutcopy = dup(1);

	len = strlen("ls ") + strlen(tmp) + strlen("/ -v1 | grep -v .txt"); // ex) ls STD_DIR/ -v1 을 한 것과 같은 길이 할당.
	sys = (char *) malloc(len) + 1;
	sprintf(sys,"ls %s/ -v1 | grep -v .txt",tmp);

	if ((fd = open(fname, O_WRONLY | O_CREAT,  0755)) < 0) {
		fprintf(stderr, "File Open Error %s\n", fname);
		exit(1);
	}
	else {
		dup2(fd,1);
		close(fd);
		system(sys);
		dup2(stdoutcopy,1);
		close(stdoutcopy);
//		free(sys);
	}
	return 0;
}

int pro_type_set(void) // 해당 문제는 빈칸 관련 문제인가? 아니면 프로그램 채점 문제인가를 확인한다.
{
	int len;
	char *fname;
	for(int i = 0; i < problem_count; i++) {
		len = strlen(ans_table[i].p_name) + strlen(".txt"); // 문제폴더.txt에 대한 길이 받음
		fname = malloc(len + 1); // 동적 메모리 할당.
		sprintf(fname,"%s.txt",ans_table[i].p_name);

		if( strcmp(fname, pro_table[i].p_name) == 0 ) { // 문제폴더.txt와 문제이름 비교한 것이 같은가? (같다면 이는 빈칸 문제이다)
			ans_table[i].type = 1;
			pro_table[i].type = 1;
		}
		free(fname); // 동적 메모리 반환.

		len = strlen(ans_table[i].p_name) + strlen(".c"); // 문제폴더.c에 대한 길이를 받음
		fname = malloc(len + 1); // 동적 메모리 할당
		sprintf(fname,"%s.c",ans_table[i].p_name); 
		
		if( strcmp(fname, pro_table[i].p_name) == 0 ) { // 문제폴더.c와 문제이름 비교한 것이 같은가? (같다면 이는 프로그램 채점 문제)
			ans_table[i].type = 2;
			pro_table[i].type = 2;
		}
		free(fname);

	}
	return 0;
}
int score_csv_read(void) // score.csv를 읽어오는 함수이다.
{
	int fd;
	int c_count=0;
	
	char *temp = "./score.csv";
	if((fd = open(temp,O_RDONLY,0755)) < 0) {
		fprintf(stderr,"File Open Error for : %s\n",temp);
		exit(1);	
	}
	while(1) {
		char ch;
		if((read(fd,&ch,1)) > 0) { // 콤마개수를 센다. (이는 추후 총합을 찾을때 사용)
			if(ch == ',')
				c_count++;
			if(ch == '\n')
				break;
		}
		else
			break;
	}
	if(parameter_state[2] > 5) // parameter가 5 이상인 경우 5로 만들어 버림. (그 이상은 수행되서는 안된다.)  
		parameter_state[2] = 5;
	for(int i = 0; i < parameter_state[2]; i++) {
		lseek(fd,0,SEEK_SET);
		int flag = 0;
		int new_c_count = 0;
		int search_temp_len = 0;
		int search_temp2_len = 0;
		char *search_temp = (char *) malloc(1);
		char *search_temp2 = (char *) malloc(1);
		while(1) {
			char ch;
			if((read(fd,&ch,1)) > 0) { // 1칸씩 읽어들이면서 원하는 정보가 들어오는지 확인하는 과정이다.
				if(flag == 4 && ch != 'n') {
					search_temp2[search_temp2_len] = ch;
					search_temp2_len++;
					search_temp2 = (char *) realloc(search_temp2,search_temp2_len+1);
				}
				if(flag == 4 && ch == '\n')  { // 원하는 학생에 대한 점수를 찾은 경우 총합 출력.
					search_temp2[search_temp2_len] = 0;
					printf("%s",search_temp2);
					free(search_temp2);
					break;
				}
				if(ch == ',')
					new_c_count++;
				if(flag == 0 && ch == '\n') {
					flag = 1;
					new_c_count = 0;
				}
				else if(flag == 1 && ch != ',') { //  학생에 대한 정보를 찾기 못한 경우
					search_temp[search_temp_len] = ch;
					search_temp_len++;
					search_temp = (char *) realloc(search_temp,search_temp_len+1);
				}
				else if(flag == 1 && ch == ',') { // 학생에 대한 정보를 찾은 경우
					search_temp[search_temp_len] = 0;
					if(strcmp(search_temp,option_list[2][i]) == 0) {
						printf("%s's score : ",option_list[2][i]);
						flag = 3;
					}
					else {
						free(search_temp);
						search_temp = (char *) malloc(1);
						search_temp_len = 0;
						flag = 0;
					}
				}
				else if(flag == 3 && new_c_count == c_count) { // ,기존에 샜던 , 개수와 지금 현재 , 개수가 같은 경우
					flag = 4;
				} 	
			}	
			else
				break;
		}	
	}
	return 0;
}
int score_table_csv_remove(char *tmp) // score_table.csv를 제거하는 함수다. 정상 성공시 0을 리턴한다.
{
	char *sys;
	int len;

	len = strlen("rm ") + strlen(tmp) + strlen("/score_table.csv");
	sys = (char *) malloc(len+1);
	sprintf(sys,"rm %s/score_table.csv",tmp);
	
	system(sys);
	free(sys);	
	return 0;
}
