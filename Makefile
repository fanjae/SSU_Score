ssu_score: ssu_score.o ssu_help.o ssu_file_load.o ssu_table_load.o ssu_testing_problem.o ssu_create_result.o ssu_word.o
	gcc -g -o ssu_score ssu_score.o ssu_help.o ssu_file_load.o ssu_table_load.o ssu_testing_problem.o ssu_create_result.o ssu_word.o 

ssu_score.o: ssu_extern.h ssu_create_result.h ssu_help.h ssu_file_load.h ssu_table_load.h ssu_testing_problem.h ssu_score.c 
	gcc -c -o ssu_score.o ssu_score.c 

ssu_help.o: ssu_help.h ssu_help.c
	gcc -c -o ssu_help.o ssu_help.c

ssu_file_load.o: ssu_file_load.h ssu_file_load.c
	gcc -c -o ssu_file_load.o ssu_file_load.c

ssu_table_load.o: ssu_table_load.h ssu_table_load.c
	gcc -c -o ssu_table_load.o ssu_table_load.c

ssu_testing_problem.o : ssu_testing_problem.h ssu_testing_problem.c
	gcc -c -o ssu_testing_problem.o ssu_testing_problem.c

ssu_create_result.o : ssu_create_result.h ssu_create_result.c
	gcc -c -o ssu_create_result.o ssu_create_result.c

ssu_word.o : ssu_word.h ssu_word.c
	gcc -c -o ssu_word.o ssu_word.c
