#ifndef __SSU_FILE_LOAD_H__
#define __SSU_FILE_LOAD_H__

int error_reset(void);
int directory_not_exist(char *);
int err_directory_not_exist(char *);
int ans_directory_not_exist(char *);
int stu_directory_not_exist(char *);
int ans_file_list_load(char *);
int stu_folder_list_load(char *);

int score_csv_read(void);
int score_table_csv_remove(char *);
int pro_file_list_load(void);
int pro_type_set(void);
#endif
