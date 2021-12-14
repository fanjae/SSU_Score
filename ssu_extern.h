#ifndef __SSU_SCORE_EXTERN_H
#define __SSU_SCORE_EXTERN_H

extern int option_state[5]; // Option 활성화 여부를 확인하는 변수.
extern int parameter_state[5]; // Option의 Parameter 개수를 확인하는 변수
extern int start_index[5]; // Option의 argument가 처음 발생한 위치를 확인하는 변수

extern char *option_set[5]; // option의 정보를 받는 외부 변수
extern char *student_file_dir; // 학생 파일의 경로를 받아오는 변수
extern char *trueset_file_dir; // 정답 파일의 경로를 받아오는 변수

extern char ***option_list; // argument 별 Option list를 받아오는 변수

#endif

#ifndef __SSU_TABLE_LOAD_EXTERN_H
#define __SSU_TABLE_LOAD_EXTERN_H

struct answer_data // 정답 정보를 담는 구조체
{
	char *p_name; // 폴더의 이름
	double score; // 점수
	int type; // 문제 타입(1. 빈칸, 2. 프로그램 채점)
};

struct problem_data // 문제 정보를 담는 구조체
{
	char *p_name; // 폴더의 이름
	int type; // 점수
};

struct student_data // 학생 정보를 담는 구조체
{
	char *f_name; // 학생 폴더의 이름 
	double *score; // 학생 득점 점수
	double sum; // 학생 점수 총합
};

extern struct answer_data *ans_table; // 정답 정보를 담는 구조체
extern struct problem_data *pro_table; // 문제 정보를 담는 구조체
extern struct student_data *stu_table; // 학생 정보를 담는 구조체

extern int *stu_folder_length; // 폴더의 길이를 담을 변수
extern int *pro_file_length; // 프로그램 파일 길이
extern int *ans_file_length; // 정답 파일 길이
extern int student_count; //  학생의 수를 담는 변수
extern int problem_count; // 문제의 개수를 담는 변수

extern double sum; // 총 합계
extern double all_blank_score; // 모든 빈칸 문제에 대한 점수(채점 테이블을 만들때 1번을 선택한 경우)
extern double all_program_score; // 모든 프로그램 채점 문제에 대한 점수(채점 테이블을 만들때 1번을 선택한 경우)

#endif
