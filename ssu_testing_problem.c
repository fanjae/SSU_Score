#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "ssu_extern.h"
#define WARNING_SCORE 0.1
#define ERROR_SCORE 0.0
#define TIME_LIMIT 5
#define TIME_LIMIT_SCORE 0.0
struct answer_data *ans_table;
struct problem_data *pro_table;
struct student_data *stu_table;

int option_state[5];
int parameter_state[5];
int start_index[5];
int problem_count;

int *ans_file_length;

char *student_file_dir;
char *trueset_file_dir;
char ***option_list;

double auto_problem_marking(int i,int j)
{
	int fd,fd2,fd3;
	int flag=0;
	int stdoutcopy=dup(1); // 표준 출력을 복사하는 변수
	int stderrcopy=dup(2); // 표준 에러를 복사하는 변수
	int ans_stdout_sys_temp_len; // 정답 파일 표준 출력 경로를 담을 임시변수
	int ans_stderr_sys_temp_len; // 정답 파일 표준 에러 경로를 담을 임시변수
	
	int compare_len; // 
	int stu_ans_temp_len; // 학생 정답 파일 경로 길이 담을 변수
	int stu_gcc_temp_len; // 학생 컴파일 파일 경로 길이 담을 변수
	int stu_stdexe_temp_len; // 학생 stdexe 파일 경로 길이 담을 변수
	int stu_error_e_temp_len; // 학생 에러(e옵션) 폴더 경로 길이 담을 변수
	int stu_rm_error_e_temp_len; // 학생 에러(e옵션) 제거 폴더 경로 길이 담을 변수
	int stu_stdout_temp_len; // 학생 표준출력 경로 길이 담을 변수
	int stu_mkdir_temp_len; // 학생 mkdir 경로 길이 담을 변수
	int stu_sys_temp_len; //  학생 백그라운드 실행 길이 담을 변수
	int stu_prokill_temp_len; // 학생 프로세스 죽일때 길이 담을 변수

	char *ans_stdout_sys_temp; // 정답 파일 표준 출력 경로
	char *ans_stderr_sys_temp; // 정답 파일 표준 에러 경로
	char *ans_problem_length; // 정답 문제 길이

	char *stu_problem_length; // 학생 문제 길이
	char *stu_prokill_temp; // 학생 프로세스 경로
	char *stu_ans_temp; // 학생 정답 경로
	char *stu_gcc_temp; // 학생 컴파일 경로
	char *stu_mkdir_temp; // 학생 mkdir 경로
	char *stu_stdexe_temp; // 학생 stdexe 경로
	char *stu_error_e_temp; // 학생 에러(e옵션) 경로
	char *stu_rm_error_e_temp; // 학생 에러(e옵션) 제거 경로
	char *stu_stdout_temp; // 학생 표준출력 경로
	char *stu_sys_temp; // 학생 백그라운드 실행 경로
	char *stdoutpath = "ssu_pro_open_test.txt";
	char *stderrpath = "ssu_pro_open_err_test.txt";
	
	if(option_state[4] == 1) { // -e 옵션이 존재하는 경우
		if((fd = open(stdoutpath,O_WRONLY | O_CREAT,0755)) < 0) { // 터미널 표준출력을  임시로 담을 ssu_pro_open_test
			fprintf(stderr,"File Open error : %s in auto_problem_marking()",stdoutpath);
			exit(1);
		}
		if((fd2 = open(stderrpath,O_WRONLY | O_CREAT,0755)) < 0) { // 터미널 표준에러를 임시로 담을 ssu_pro_open_err_test
			fprintf(stderr,"File Open error : %s in auto_problem_marking()",stderrpath);
			exit(1);
		}

		dup2(fd,1); // fd을 1로 복사
		close(fd);
		dup2(fd2,2); // fd을 2로 복사
		close(fd2);
	
		
		stu_mkdir_temp_len = strlen("mkdir ") + strlen(option_list[4][0]) + strlen("/") + strlen(stu_table[i].f_name);
		stu_mkdir_temp = (char *) malloc(stu_mkdir_temp_len + 1);

		sprintf(stu_mkdir_temp,"mkdir %s/%s",option_list[4][0],stu_table[i].f_name); // 에러 디렉토리 문자 할당
		system(stu_mkdir_temp); // 에러 디렉 토리 생성

		
		dup2(stdoutcopy,1); // 복사했던 표준 출력을 다시 되돌린다.
		dup2(stderrcopy,2); // 복사했던 표준 에러를 다시 되돌린다.
		close(stdoutcopy);
		close(stderrcopy);
	
		stu_error_e_temp_len = strlen(option_list[4][0]) + strlen("/") + strlen(stu_table[i].f_name) + strlen("/") + strlen(ans_table[j].p_name) + strlen("_error.txt");
		stu_error_e_temp = (char *) malloc(stu_error_e_temp_len + 1);
		sprintf(stu_error_e_temp,"%s/%s/%s_error.txt",option_list[4][0],stu_table[i].f_name,ans_table[j].p_name); // 에러파일/학번/문제번호_error.txt를 문자열에 할당.

	}
	else {
		stu_error_e_temp_len = strlen(student_file_dir) + strlen("/") + strlen(stu_table[i].f_name) + strlen("/") + strlen(ans_table[j].p_name) + strlen("_error.txt"); // STD_DIR/20190001/13_error.txt 임시 에러 할당.
		stu_error_e_temp = (char *) malloc(stu_error_e_temp_len + 1);
		sprintf(stu_error_e_temp,"%s/%s/%s_error.txt",student_file_dir,stu_table[i].f_name,ans_table[j].p_name);
		
	}
	if((fd = open(stdoutpath,O_WRONLY | O_CREAT, 0755)) < 0) {
		fprintf(stderr,"TT!! File Open Error : %s in auto_problem_marking()",stdoutpath);
		exit(1);
	}
	if((fd2 = open(stu_error_e_temp,O_WRONLY | O_CREAT, 0755)) < 0) {
		fprintf(stderr,"Test_set : File Open Error : %s in auto_problem_marking()",stu_error_e_temp);
		exit(1);
	}
	stu_ans_temp_len = strlen(student_file_dir) + strlen("/") + strlen(stu_table[i].f_name) + strlen("/") + strlen(ans_table[j].p_name);
	stu_ans_temp = (char *) malloc(stu_ans_temp_len + 1);
	sprintf(stu_ans_temp,"%s/%s/%s",student_file_dir,stu_table[i].f_name,ans_table[j].p_name);

	stdoutcopy = dup(1); // 표준 출력 복사
	stderrcopy = dup(2); // 표준 에러 복사
	dup2(fd,1); // fd를 1로 복사
	close(fd);  
	dup2(fd2,2); // fd2를 2로 복사 
	close(fd2);

	if(option_state[3] == 1) { // -t 옵션에 대한 처리 과정이다.
		if(parameter_state[3] >= 5)
			parameter_state[3] = 5;
		for(int k = 0; k < parameter_state[3]; k++)
			if(strcmp(option_list[3][k],ans_table[j].p_name) == 0) {
				flag = 1;
				stu_gcc_temp_len = strlen("gcc -o ") + strlen(stu_ans_temp) + strlen(".stdexe ") + strlen(stu_ans_temp) + strlen(".c -lpthread");
				stu_gcc_temp = (char *) malloc(stu_gcc_temp_len + 1);
				sprintf(stu_gcc_temp,"gcc -o %s.stdexe %s.c -lpthread",stu_ans_temp,stu_ans_temp);
				printf("%s\n",stu_gcc_temp);
				system(stu_gcc_temp); // t 옵션을 system을 통해서 실행시킨다. .c 파일을 stdexe 파일로 컴파일 시킨다.
			}
	}

	if(flag == 0) { // -p 없거나 -p 옵션이 있지만, 해당이 없는 폴더인 경우

		stu_gcc_temp_len = strlen("gcc -o ") + strlen(stu_ans_temp) + strlen(".stdexe ") + strlen(stu_ans_temp) + strlen(".c");
		stu_gcc_temp = (char *) malloc(stu_gcc_temp_len + 1);
		sprintf(stu_gcc_temp,"gcc -o %s.stdexe %s.c",stu_ans_temp,stu_ans_temp);
		system(stu_gcc_temp); // .c 파일을 stdexe 파일로 컴파일 시킨다.

	}
	dup2(stdoutcopy,1); // 복사한 표준 출력 다시 반환
	dup2(stderrcopy,2); // 복사한 표준 에러 다시 반환
	close(stdoutcopy); 
	close(stderrcopy);

	stdoutcopy = dup(1); // 표준 출력 복사
	stderrcopy = dup(2); // 표준 에러 복사
	if((fd = open(stdoutpath,O_WRONLY | O_CREAT,0755)) < 0) { 
		fprintf(stderr,"File Open Error : %s\n",stdoutpath);
		exit(1);
	}
	if((fd2 = open(stderrpath,O_WRONLY | O_CREAT,0755)) < 0) {
		fprintf(stderr,"File Open Error : %s\n",stderrpath);
		exit(1);
	}

	dup2(fd,1); // 표준 출력을 파일 디스크립터로 전환한다.
	close(fd);
	dup2(fd2,2); // 표준 에러를 파일 디스크립터로 전환한다.
	close(fd2);	
	if((fd3 = open(stu_error_e_temp,O_RDONLY,0755)) < 0) {
		fprintf(stderr,"File Open Error : %s\n",stu_error_e_temp);
		exit(1);
	}
	else {
		stu_rm_error_e_temp_len = strlen("rm ") + strlen(stu_error_e_temp);
		stu_rm_error_e_temp = (char *) malloc(stu_rm_error_e_temp_len + 1);
		sprintf(stu_rm_error_e_temp,"rm %s",stu_error_e_temp); // rm 에러 담은 경로를 문자열에 담는다.
		int length;
		char ch=0;
		
		while(1) { // 하나라도 오류가 있으면 지우지 말것.
			if((read(fd3,&ch,1) > 0)) {
				ch++;
				break;
			}
			else
				break;
		}
		if(ch == 0)
		{
			system(stu_rm_error_e_temp); // 오류가 없으면 파일을 생성해둘 필요가 없다.
		}
		close(fd3);
	}
	dup2(stdoutcopy,1); // 표준 출력을 다시 원래대로 되돌린다.
	dup2(stderrcopy,2); // 표준 에러를 다시 원래대로 되돌린다.
	close(stdoutcopy);
	close(stderrcopy);
	stdoutcopy = dup(1);
	stdoutcopy = dup(2);

	stu_stdexe_temp_len = strlen("./") + strlen(stu_ans_temp) + strlen(".stdexe"); // stdexe 파일 만큼 길이를 잰다.
	stu_stdout_temp_len = strlen("./") + strlen(stu_ans_temp) + strlen(".stdout"); // stdout 파일 만큼 길이를 잰다.
	stu_stdexe_temp = (char *) malloc(stu_stdexe_temp_len + 1);
	stu_stdout_temp = (char *) malloc(stu_stdout_temp_len + 1);
	sprintf(stu_stdexe_temp, "./%s.stdexe",stu_ans_temp); // stdexe 문자열 복사
	sprintf(stu_stdout_temp, "./%s.stdout",stu_ans_temp); // stdout 문자열 복사

	stu_prokill_temp_len = strlen("ps -ef --sort +pid | grep ") + strlen(stu_stdexe_temp);
	stu_prokill_temp = (char *) malloc(stu_prokill_temp_len + 1);
	sprintf(stu_prokill_temp,"ps -ef --sort +pid | grep %s",stu_stdexe_temp); // 현재 실행중인 프로세스 확인할 것을 문자열에 담음

	stu_sys_temp_len = strlen(stu_stdexe_temp) + strlen(" &");
	stu_sys_temp = (char *) malloc(stu_sys_temp_len + 1);
	sprintf(stu_sys_temp,"%s &",stu_stdexe_temp); 	

	int temp_process_num_len;
	int temp_process_len;
	char ch;
	char *temp_process;
	char *temp_process_num;

	if((fd = open(stu_stdout_temp,O_WRONLY | O_CREAT | O_TRUNC, 0755)) < 0) {
			fprintf(stderr,"File Open Error : %s\n",stu_stdout_temp);
			exit(1);
	}
	if((fd2 = open(stderrpath, O_WRONLY | O_CREAT | O_TRUNC, 0755)) < 0) {
			fprintf(stderr,"File Open Error : %s\n",stderrpath);
			exit(1);
	}
	dup2(fd,1); // 표준 출력을 파일 디스크립터로 전환한다.
	close(fd);
	dup2(fd2,2); // 표준 에러를 파일 디스크립터로 전환한다.
	close(fd2);
			
	system(stu_sys_temp); // 백그라운드 프로세스로 채점할 문제를 실행한다.
		
	dup2(stdoutcopy,1); // 돌려놨던 표준 출력 다시 전환
	dup2(stderrcopy,2); // 돌려놨던 표준 에러 다시 전환
	close(stdoutcopy);
	close(stderrcopy);
	stdoutcopy = dup(1); // 표준 출력 복사
	stderrcopy = dup(2); // 표준 에러 복사

	int timeout = 0; // 시간초과(on)
	int count = 0;  
	int hour; // 시간
	int minute; // 분
	int second; // 초
	char pid[300]; // 프로세스 id
	char time[100]; // 프로세스 실행 시간
	char directory[100]; // 파일이

	while(1)
	{
		stdoutcopy = dup(1);
		stderrcopy = dup(2);		
		int time_set;
		int line_length = 0;
		if((fd = open(stdoutpath, O_RDWR | O_CREAT | O_TRUNC, 0755)) < 0) {
			fprintf(stderr, "File Open Error : %s\n",stdoutpath);
			exit(1);
		} 
		if((fd2 = open(stderrpath, O_WRONLY | O_CREAT | O_TRUNC, 0755)) < 0) {
			fprintf(stderr, "File Open Error : %s\n",stderrpath);
			exit(1);
		}

		dup2(fd,1);
		close(fd);
		dup2(fd2,2);
		close(fd2);
		system(stu_prokill_temp); // 현재 실행중인 프로세스를 확인한다.

		dup2(stdoutcopy,1);
		close(stdoutcopy);

		stdoutcopy = dup(1);
		if((fd = open(stdoutpath, O_RDONLY,0755)) < 0) {
			fprintf(stderr,"File Open Error : %s\n",stdoutpath);
			exit(1);
		}
		while(1) { // 줄 수를 센다.
			if(read(fd,&ch,1) > 0) {
				if(ch == '\n') 
					line_length++;
			}
				else
					break;
		}
		lseek(fd,0,SEEK_SET);
		if(line_length >= 3) { // 줄수가 3개 이상이면 프로세스가 현재 실행중이라고 판단한다.

			int temp_save_length=0;
			while(1) {
				if(read(fd,&ch,1) > 0) {
			//		printf("%c",ch);
				}
				else {
					dup2(stderrcopy,2);
					close(stderrcopy);
					stderrcopy = dup(2);
					break;
				}	
			}
			lseek(fd,0,SEEK_SET);
			char buffer_size[1024];
			char *ptr;
 			int index = 0;
			int ch_len = 0;
			while(1) {
				if(read(fd,&ch,1) > 0) {
					buffer_size[ch_len] = ch;
					ch_len++;
				}
				else if(ch == '\n') {
					buffer_size[ch_len-1] = 0;
					break;
				}
			}
			ptr = strtok(buffer_size," ");
			char tempsize[300][500];
			sprintf(tempsize[index++],"%s",ptr);
			while(strcmp(ptr,stu_stdexe_temp) != 0) {
				ptr = strtok(NULL," ");
				sprintf(tempsize[index++],"%s",ptr);
			}
			sprintf(pid,"%s",tempsize[1]); // 프로세스를 읽어온다.
			sprintf(time,"%s",tempsize[6]); // 시간을 읽어온다
			sprintf(directory,"%s",tempsize[7]); // 경로를 읽어온다.
			ptr = strtok(tempsize[6],":");
			hour = atoi(ptr);
			ptr = strtok(NULL,":");
			minute = atoi(ptr);
			ptr = strtok(NULL,":");
			second = atoi(ptr);			
			time_set = (hour * 3600) + (minute * 60) + second;
		} // if(line_length >= 3에 대한 중괄호)
		else {
			dup2(stderrcopy,2);
			close(stderrcopy);
			stderrcopy = dup(2);
			break;	
		}
		if(time_set >= TIME_LIMIT || count >= TIME_LIMIT) // 시간 초과 한경우 프로세스를 죽인다.
		{
			int pkill_len;
			char *pkill;

			pkill_len = strlen("kill -9 ") + strlen(pid); 
			pkill = (char *) malloc(pkill_len + 1);
			sprintf(pkill,"kill -9 %s",pid); 
					
			system(pkill); // 프로세스 id를 죽인다. (해당 하는 경우)
			timeout = 1;
			
			dup2(stderrcopy,2);
			close(stderrcopy);
			stderrcopy = dup(2);
			break;
		}	
		sleep(1);
		close(fd);
		count++;
		if(timeout == 1) // 시간 초과 문제는 TIME_LIMIT_SCORE(기본 값은 0점)를 부여한다.
			return TIME_LIMIT_SCORE;
	}
	
	ans_stdout_sys_temp_len = strlen("./") + strlen(trueset_file_dir) + strlen("/") + strlen(ans_table[j].p_name) + strlen("/") + strlen(ans_table[j].p_name) + strlen(".stdout");
	ans_stdout_sys_temp = (char *) malloc(ans_stdout_sys_temp_len + 1);
	sprintf(ans_stdout_sys_temp,"./%s/%s/%s.stdout",trueset_file_dir,ans_table[j].p_name,ans_table[j].p_name);

	
	if((fd = open(ans_stdout_sys_temp,O_RDONLY,0755)) < 0) {
		fprintf(stderr,"File Open Error : %s\n",ans_stdout_sys_temp);
		exit(1);
	}
	int ans_problem_length_len = 0; 
	while(1) {
		if(read(fd,&ch,1) > 0) 
			ans_problem_length_len++; // 정답 문제 길이를 잼.
		else
			break;
	}

	lseek(fd,0,SEEK_SET);
	ans_problem_length = (char *) malloc(ans_problem_length_len + 1);	
	compare_len = 0;	
	
	
	while(1) {
		if(read(fd,&ch,1) > 0) {
			if(ch == ' ') // 공백은 무시한다.
				continue;	
			if(ch >= 'A' && ch <= 'Z') // 대소문자 구분을 하지 않는 프로그램 채점 문제의 특징을 따라 대문자는 항상 소문자로 받음.
				ch = ch + 32;
			ans_problem_length[compare_len] = ch;
			compare_len++;	
		}
			else {
				ans_problem_length[compare_len-1] = 0;
				break;		
		}
	}			
	close(fd);
	if((fd = open(stu_stdout_temp,O_RDONLY,0755)) < 0) {
		fprintf(stderr,"File Open Error : %s\n",stu_stdout_temp);
		exit(1);
	}
	lseek(fd,0,SEEK_SET);
	int stu_problem_length_len = 0;
	while(1) {
		if(read(fd,&ch,1) > 0)
			stu_problem_length_len++; // 채점 할 프로그램 문제 길이를 잼.
		else
			break;
	}
	stu_problem_length = (char *) malloc(stu_problem_length_len + 1);
	compare_len = 0;
	lseek(fd,0,SEEK_SET);
	while(1) {
		if(read(fd,&ch,1) > 0) {
			if(ch == ' ')
				continue;
			if(ch >= 'A' && ch <= 'Z') // 대소문자 구분을 하지 않는 프로그램 채점 문제의 특징을 따라 대문자는 항상 소문자로 받는다.
				ch = ch + 32;
			stu_problem_length[compare_len] = ch;
			compare_len++;
		}
		else {
				stu_problem_length[compare_len-1] = 0;
				break;
		}
	}
	close(fd);
	int warning=0;
	int error=0;
	
	if((fd = open(stu_error_e_temp,O_RDONLY)) < 0) {
//		printf("%s is not exist\n",stu_error_e_temp);
	}
	else {
		compare_len = 0;
		int text_length_temp=0; 
		int error_line=0;
		char *text_length;
		char *error_result=0;
		char *warning_result=0;
		while(1) {
			if(read(fd,&ch,1) > 0) {
				text_length_temp++;		
				if(ch == '\n')
					error_line++;
			}
			else
				break;
		}
		lseek(fd,0,SEEK_SET);
		text_length = (char *) malloc(text_length_temp + 1);

		while(1) {
			if(read(fd,&ch,1) > 0)
				text_length[compare_len++] = ch;
			else
				break;
		}

		error_result = strstr(text_length,"error:"); // 에러가 존재하는지 확인한다.
		if(error_result != NULL)
		{
			error_result = strstr(error_result+1,"error:"); // 에러 개수 만큼 에러를 샌다.
			error++;
		}

		warning_result = strstr(text_length,"warning:"); // warning이 존재하는지 확인한다.
		if(warning_result != NULL)
		{
			warning_result = strstr(warning_result+1,"warning:"); // warning 개수 만큼 warning을 샌다.
			warning++;
		}	
		if(option_state[4] == 0) {
			int sys_rm_temp_len;
			char *sys_rm_temp;
			sys_rm_temp_len = strlen("rm ") + strlen(stu_error_e_temp);
			sys_rm_temp = (char *) malloc(sys_rm_temp_len+1);
			sprintf(sys_rm_temp,"rm %s",stu_error_e_temp); // e 옵션이 꺼져있으면 error는 모두 제거한다. (STD_DIR에 임시 저장했었음)
			system(sys_rm_temp);
		}
	}
	if(error > 0) // 에러가 1개이상이라도 존재하는 경우
		return ERROR_SCORE; // ERROR_SCORE를 준다. (기본 점수는 0점)
	if(strcmp(stu_problem_length,ans_problem_length) == 0) { // 프로그램 채점 결과 동일할 경우
		if(error > 0)
			return ERROR_SCORE;
		if((ans_table[j].score - (WARNING_SCORE * warning)) < 0) {
			stu_table[i].score[j] = 0.0;
			return stu_table[i].score[j];
		}
		else {
			stu_table[i].score[j] = ans_table[j].score - (WARNING_SCORE * warning);
			return stu_table[i].score[j];
		}
	}
	return 0.0;
}
int ans_checking(char *tmp)
{
	for(int i=0; i<problem_count; i++) {
		char *stdoutpath = "ssu_ans_open_test.txt";
		char *stderrpath = "ssu_ans_error_test.txt";
		int stdoutcopy = dup(1); // stdoutcopy 복사
		int stderrcopy = dup(2); // stderrcopy 복사
		if(ans_table[i].type == 2) {

			int fd,fd2; // File Descriptor
			int flag = 0; // -p 옵션이 없는 경우이거나, -p 옵션은 있는데 폴더가 해당사항이 없는 경우
			int gcc_ans_temp_len; // gcc 정답이름에 대한 길이 담을 변수
			int gcc_sys_temp_len; // gcc 명령어에 대한 길이 담을 변수 
			int exe_sys_temp_len; // exe 실행에 대한 명령에 대한 길이를 담을 변수
			int stdout_sys_temp_len; // .stdout 저장에 대한 명령어를 담을 변수

			char *gcc_ans_temp; // gcc 정답이름에 공통 부분 변수 
			char *gcc_sys_temp; // gcc 명령어에 대한 변수
			char *exe_sys_temp; // exe 실행에 대한 명령어 담을 변수
			char *stdout_sys_temp; // .stdout 저장에 대한 명령어 담을 변수
			if((fd = open(stdoutpath, O_WRONLY | O_CREAT, 0755)) < 0) { // gcc 과정을 터미널에 보여주지 않게 하기 위해 ans_open_test를 불러옴.
				fprintf(stderr, "File Open Error in ans_checking : %s\n",stdoutpath);
				exit(1);
			}
			if((fd2 = open(stderrpath, O_WRONLY | O_CREAT, 0755)) < 0) { // gcc 에러 과정을 터미널에 보여주지 않게 하기 위해 error_test를 불러옴.
				fprintf(stderr, "File Open Error in ans_checking : %s\n",stderrpath);
				exit(1);
			}
			dup2(fd,1);
			close(fd);

			dup2(fd2,2);
			close(fd2);
			// gcc_ans에 대한 공통 부분을 담아둔다. /ANS/문제번호/문제번호 까지는 일치함.
			gcc_ans_temp_len = strlen(trueset_file_dir) + strlen("/") + strlen(ans_table[i].p_name) + strlen("/") + strlen(ans_table[i].p_name);
			gcc_ans_temp = (char *) malloc(gcc_ans_temp_len + 1);
			sprintf(gcc_ans_temp,"%s/%s/%s",trueset_file_dir,ans_table[i].p_name,ans_table[i].p_name);			

			if(option_state[3] == 1) { // p 옵션이 on 상태인 경우
				if(parameter_state[3] >= 5) // parameter 개수가 5개가 넘어가면 5개까지만 수행 시키기.
					parameter_state[3] = 5;
				for(int j = 0; j < parameter_state[3]; j++)
					if(strcmp(option_list[3][j],ans_table[i].p_name) == 0)	{ // -p 옵션이 적용되는 대상의 경우
						flag = 1;
						gcc_sys_temp_len = strlen("gcc -o ") + strlen(gcc_ans_temp) + strlen(".exe ") + strlen(gcc_ans_temp) + strlen(".c -lpthread");
						gcc_sys_temp = (char *) malloc(gcc_sys_temp_len + 1);
						sprintf(gcc_sys_temp,"gcc -o %s.exe %s.c -lpthread",gcc_ans_temp,gcc_ans_temp);
						system(gcc_sys_temp);
						dup2(stdoutcopy,1); // gcc에 대한 명령이 끝나므로 다시 표준 출력을 열어줌.
						close(stdoutcopy); // 복사본은 닫음.
					}
			}
			if(flag == 0) { // -p가 없거나 -p 옵션이 있지만, -p와 해당이 없는 폴더인 경우  
				gcc_sys_temp_len = strlen("gcc -o ") + strlen(gcc_ans_temp) + strlen(" .exe ") + strlen(gcc_ans_temp) + strlen(".c");
				gcc_sys_temp = (char *) malloc(gcc_sys_temp_len + 1);
				sprintf(gcc_sys_temp,"gcc -o %s.exe %s.c",gcc_ans_temp,gcc_ans_temp);
				system(gcc_sys_temp);
				dup2(stdoutcopy,1); // gcc에 대한 명령이 끝났으므로 다시 표준 출력을 열어줌.
				close(stdoutcopy); // 복사본은 닫음.
			}

			stdoutcopy = dup(1); // 표준 출력을 다시 복사.
			exe_sys_temp_len = strlen(gcc_ans_temp) + strlen(".exe");
			exe_sys_temp = malloc(exe_sys_temp_len + 1);
			sprintf(exe_sys_temp,"%s.exe",gcc_ans_temp); // exe 명령어 복사 완료.

			stdout_sys_temp_len = strlen(gcc_ans_temp) + strlen(".stdout");
			stdout_sys_temp = malloc(exe_sys_temp_len + 1);
			sprintf(stdout_sys_temp,"%s.stdout",gcc_ans_temp);	// stdout 명령어 복사 완료.		

			if((fd = open(stdout_sys_temp,O_WRONLY | O_CREAT,0755)) < 0) {
				fprintf(stderr,"File Open Error in ans_checking : %s\n",stdout_sys_temp);
				exit(1);
			}
			dup2(fd,1);
			close(fd);
			system(exe_sys_temp);
			dup2(stdoutcopy,1);
			close(stdoutcopy);

			free(gcc_ans_temp); // 동적 메모리 반환
			free(gcc_sys_temp); // 동적 메모리 반환
			free(exe_sys_temp); // 동적 메모리 반환
			free(stdout_sys_temp); // 동적 메모리 반환

		} // if(ans_table[i].type == 2)에 대한 중괄호
		dup2(stderrcopy,2); // 에러에 대한 명령이 끝났으므로 표준 에러를 열어줌.
		close(stderrcopy); // 에러에 대한 명령이 끝났으므로 표준 에러를 열어줌.

	} // for(i = 0; i < problem_count; i++)에 대한 중괄호
	system("rm ssu_ans_open_test.txt");
	system("rm ssu_ans_error_test.txt");
	return 0;	
}	
