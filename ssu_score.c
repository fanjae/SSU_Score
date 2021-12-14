#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include "ssu_help.h"
#include "ssu_file_load.h"
#include "ssu_table_load.h"
#include "ssu_extern.h"

#define BUFFER_SIZE 1024
#define SECOND_TO_MICRO 1000000

int option_state[5]={0};
int parameter_state[5]={0};
int start_index[5]={0};

char *option_set[5]={"-h","-p","-c","-t","-e"};
char *student_file_dir;
char *trueset_file_dir;

char ***option_list;

void ssu_runtime(struct timeval* begin_t, struct timeval* end_t)
{
	end_t -> tv_sec -= begin_t -> tv_sec;

	if (end_t -> tv_usec < begin_t -> tv_usec) {
		end_t -> tv_sec--;
		end_t -> tv_usec += SECOND_TO_MICRO;
	}

	end_t -> tv_usec -= begin_t -> tv_usec;
	printf("Runtime: %ld:%06ld(sec:usec)\n" , end_t -> tv_sec, end_t -> tv_usec);
}

int main(int argc, int *argv[])
{
	struct timeval begin_t, end_t;
	char *non_option_set[21]={"-a","-b","-d","-f","-g","-i","-j","-k","-l","-m","-n","-o","-q","-r","-s","-u","-v","-w","-x","-y","-z"};

	gettimeofday(&begin_t,NULL); // 시간 측정 시작

	if( argc == 1) { // argument가 아예 없는 경우 처리
		fprintf(stderr,"Usage a File : %s\n",(char *) argv[0]);
		fprintf(stderr,"ex) %s -h\n",(char *) argv[0]);
		exit(1);
	}

	if( (argc == 2) && (strcmp((char *) argv[1], "-h") != 0) && (strcmp((char *) argv[1], "-c") != 0)) { // argument가 STD_DIR과 ANS_DIR 계열이 없어도 사용 가능한 것 외에는 argument는 도움말을 입력하도록 유도함.
		fprintf(stderr,"Usage a File : %s\n",(char *) argv[0]);
		fprintf(stderr,"ex) %s -h\n",(char *) argv[0]);
		exit(1);
	}

	for (int i=0; i<argc; i++) { // 존재하지 않는 옵션에 대한 처리.
		for (int j=0; j<21; j++) {
				if (strcmp((char *) argv[i], non_option_set[j]) == 0) {
					fprintf(stderr,"Error! It's exist not option\n");
					gettimeofday(&end_t,NULL);
					ssu_runtime(&begin_t,&end_t);
					exit(1);
				}
		}
	}
	
	for (int i=0; i<argc; i++) { // 각 argument를 보면서 옵션 상태를 파악.
		if (strcmp((char *) argv[i], "-h") == 0) {
			option_state[0]++; // -h Option Open.
			start_index[0] = i;			
		}
		if (strcmp((char *) argv[i], "-p") == 0) {
			option_state[1]++; // -p Option Open.
			start_index[1] = i;
		}
		if ((strcmp((char *) argv[i], "-c") == 0) || (strcmp((char *) argv[i], "-t") == 0) || (strcmp((char *) argv[i], "-e") == 0)) { // argument 개수가 여러개가 들어올 수 있는 것들에 대한 처리.
			int index;

		if (strcmp((char *) argv[i], "-c") == 0) {
			index = 2;
			option_state[index]++; // -c Option Open.
			start_index[index] = i;
		}
		if (strcmp((char *) argv[i], "-t") == 0) {
			index = 3;
			option_state[index]++; // -t Option Open.
			start_index[index] = i;
		}
		if (strcmp((char *) argv[i], "-e") == 0) {
			index = 4;
			option_state[index]++; // -e Option Open.
			start_index[index] = i; 
		}
	
		for (int j = i; ; j++) {
			int flag = 0;
			if (j == argc) {
 				parameter_state[index] = --j - i;
				i = j;
				break;
			}
				for (int k = 0; k < 5; k++) {
					if ((strcmp((char *)argv[i], (char *)option_set[k]) != 0) && (strcmp((char *)argv[j], (char *)option_set[k]) == 0))
						flag = 1;
				}
				if (flag) {
					parameter_state[index] = --j - i;
					i = j;
					break;
				}
			}
		}
	}
	for(int i = 0; i < 5; i++) {
		if(option_state[i] > 1) {
			fprintf(stderr,"Error ! Some option it's duplication\n");
			exit(1);
		}
	}
	option_list = (char ***) malloc(sizeof(char **) * 5); // option_list를 받을 수 있는 5칸을 할당.
	
	for(int i = 0; i < 5; i++) {
		option_list[i] = (char **) malloc(sizeof(char *) * parameter_state[i]); // option_list는 parameter별 개수가 다름.
	}

	
	if (option_state[0] == 1)
	{
		ssu_score_help(); // 도움말 호출.
		
		gettimeofday(&end_t, NULL);
		ssu_runtime(&begin_t, &end_t);

		exit(1);
	}


	if ((option_state[2] == 0) || ((strcmp((char *)argv[1],"-c") != 0) && (strcmp((char *)argv[2],"-c") != 0))) {
		int len = strlen( (char *)argv[1]); // argv[1] (학생 파일에 대한 길이를 잰다.)
		student_file_dir = (char *) malloc(len + 1);  // 학생 파일에 대한 길이를 잰 것을 동적 할당
		strcat(student_file_dir, (char *) argv[1]); // 학생 파일 디렉토리 argument에서 불러온다.

		len = strlen( (char *)argv[2]); // argv[2] (정답 파일에 대한 길이를 잰다.)
		trueset_file_dir = (char *) malloc(len + 1); // 정답 파일에 대한 길이를 잰 것을 동적 할당
		strcat(trueset_file_dir, (char *) argv[2]); // 정답 파일 디렉토리 argument에서 불러온다.
	}
	
	if (option_state[2] == 1) // -c를 입력한 경우
	{
		if (parameter_state[2] == 0) // -c에 대한 Argument를 입력하지 않은 경우
		{
			fprintf(stderr,"Error -c is have to Arguments!\n");
	
			gettimeofday(&end_t, NULL);
			ssu_runtime(&begin_t, &end_t);

			exit(1);
		}
		else // -c의 argument 개수를 처리한다. 
		{
			int count = 0; 
			for (int j=start_index[2]+1; j<=start_index[2] + parameter_state[2]; j++) {
				option_list[2][count] = (char *) malloc(strlen( (char *)argv[j])); // option 개수 처리
				strcat(option_list[2][count], (char *)argv[j]); // option_list에 추가
				count++;
			}
		}	
	}

	if (option_state[3] == 1) // -t를 입력한 경우
	{
		if (parameter_state[3] == 0)
		{
			fprintf(stderr, "Error -t is have to Arguments!\n");
			exit(1);
		}
		else // -t의 개수를 처리한다.
		{
			int count = 0;
			for (int j=start_index[3]+1; j<=start_index[3] + parameter_state[3]; j++) {
				ans_directory_not_exist((char *)argv[j]); // 해당 파일이 정답 파일에 존재하는가 확인한다. (in ssu_file_load.c)
				option_list[3][count] = (char *) malloc( strlen( (char *)argv[j])); // option 개수 처리 
				strcat(option_list[3][count], (char *)argv[j]); // option_list에 추가
				count++;
			}
		}
	}

	if (option_state[4] == 1) // -e를 입력한 경우
	{
		if (parameter_state[4] == 0)
		{
			fprintf(stderr, "Error -e is have to Arguments!\n");
			exit(1);
		}
		else // -e의 개수를 처리한다.
		{
			int count = 0;
			for (int j=start_index[4]+1; j<=start_index[4]+1; j++) {
				err_directory_not_exist((char *)argv[j]); // 에러 디렉토리가 존재하는가 확인한다.
				option_list[4][count] = (char *) malloc( strlen( (char *)argv[j]));
				strcat(option_list[4][count], (char *)argv[j]);
				count++;
			}
			error_reset();
		}
	}
// 해당 줄은 argument의 개수가 5개를 넘어가는 상황에 대한 처리를 하였음. (단, -e 옵션은 2개를 넘어가면 안됨.)
	for(int i=0; i<argc; i++) { 
		if ((strcmp((char *)argv[i],"-c") == 0) && parameter_state[2] >= 6) // -c 개수가 5개를 넘어가는 경우
			for (int j=start_index[2]+6; j<=start_index[2] + parameter_state[2]; j++) {
				printf("Maximum Number of Argument Exceeded.  :: %s\n",(char *)argv[j]);
				i = j;
			}	
		if ((strcmp((char *)argv[i],"-t") == 0) && parameter_state[3] >= 6) // -t 개수가 5개를 넘어가는 경우
			for (int j=start_index[3]+6; j<=start_index[3] + parameter_state[3]; j++) {
				printf("Maximum Number of Argument Exceeded.  :: %s\n",(char *)argv[j]);
				i = j;
			}
		if ((strcmp((char *)argv[i],"-e") == 0) && parameter_state[4] >= 2) // -e 개수가 2개를 넘어간 경우
			for (int j=start_index[4]+2; j<=start_index[4] + parameter_state[4]; j++) {
				printf("Maximum Number of Argument Exceeded.  :: %s\n",(char *)argv[j]);
				i = j;
			}
	}
	if(option_state[2] == 1 && ((strcmp((char *)argv[1],"-c") == 0) || (strcmp((char *)argv[2],"-c") == 0))) // -c만 들어온 경우
	{
		score_csv_read(); // csv를 읽어온다.
		gettimeofday(&end_t,NULL);
		ssu_runtime(&begin_t, &end_t);
		exit(1);
	}

	// directory_not_exist는 현재 위치의 디렉토리가 제대로 존재하는 디렉토리인지 확인해준다. (in file_load.c)
		if ((directory_not_exist(student_file_dir) == 1) && (directory_not_exist(trueset_file_dir) == 1)) {
			ans_file_list_load(trueset_file_dir); // 정답 파일 리스트를 먼저 읽어온다.
			problem_setting(trueset_file_dir);
		}
	gettimeofday(&end_t, NULL); // 시간 측정 종료
	ssu_runtime(&begin_t, &end_t);
	
}
