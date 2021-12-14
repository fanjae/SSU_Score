#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "ssu_extern.h"

struct answer_data *ans_table;
struct problem_data *pro_table;
struct student_data *stu_table;

char *trueset_file_dir;

int student_count;
int problem_count;

int score_result_create(void) {
	int fd;
	char temp_data[100];
	char ch=32;
	char *fname = "./score.csv";	

	if((fd = open(fname,O_WRONLY | O_CREAT | O_TRUNC,0755)) < 0) {
		fprintf(stderr,"File Open Error : %s\n",fname);
		exit(1);
	}
	for(int i=0; i<student_count+1; i++) {
		for(int j=0; j<problem_count+1; j++) {
			char temp_data[20];
			if(i == 0 && j == 0)
				write(fd,&ch,1);
			else if(i == 0 && j != problem_count) {
				write(fd,(char *)pro_table[j-1].p_name,strlen(pro_table[j-1].p_name));
			}
			else if(i > 0 && j == 0 && j != problem_count) {
				write(fd,(char *)stu_table[i-1].f_name,strlen(stu_table[i-1].f_name));
			}
			else if(i > 0 && j > 0 && j != problem_count) {
				sprintf(temp_data, "%.2lf", stu_table[i-1].score[j-1]);
				write(fd,(char *)temp_data,strlen(temp_data));
			}
			else if(i == 0 && j == problem_count) {
				write(fd,(char *)"sum",3);
				write(fd,"\n",1);
				break;
			}
			else if(i > 0 && j == problem_count) {
				sprintf(temp_data, "%.2lf", stu_table[i-1].sum);
				write(fd,(char *)temp_data,strlen(temp_data));
				write(fd,"\n",1);
				break;
			}
			write(fd,",",1);
		}
	}
}
