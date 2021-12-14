#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int ssu_score_help(void)
{
	printf("Usage : ssu_score <STUDENTDIR> <TRUEDIR> [OPTION]\n");
	printf("Option : \n");
	printf(" %-3s %-10s %-50s\n","-e","<DIRNAME>","print error on 'DIRNAME/ID/qname_error.txt' file");
	printf(" %-3s %-10s %-50s\n","-t","<QNAMES>","compile QNAME.C with -lpthread option");
	printf(" %-3s %-10s %-50s\n","-h"," ","print usage");
	printf(" %-3s %-10s %-50s\n","-p"," ","print student's score and total average");
	printf(" %-3s %-10s %-50s\n","-c","<IDS>","print ID's score\n");
	return 0;
}
