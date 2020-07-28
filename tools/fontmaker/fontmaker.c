#include<stdio.h>
#include<stdlib.h>

int main(int argc, char *argv[]){
	int i, j, k;
	int num = 0;
	char s[255];

	FILE *Ifp;
	if((Ifp = fopen(argv[1], "r")) == NULL){ //hankaku.txt
		fprintf(stderr, "Input Fileのオープンに失敗しました.\n");
        	exit(EXIT_FAILURE);
	}
	FILE *Ofp;
	if((Ofp = fopen(argv[2], "w")) == NULL){ //hankaku.c
		fprintf(stderr, "Output fileのオープンに失敗しました.\n");
        	exit(EXIT_FAILURE);
	}

	//まずはじめの二行を読み飛ばす
	fgets(s, sizeof(s), Ifp);
	fgets(s, sizeof(s), Ifp);
	fprintf(Ofp, "char hankaku[4096] = {\n");

	for(i = 0;i < 256;i++){
		for(j = 0;j < 18;j++){
			fgets(s, sizeof(s), Ifp);
			if(j == 0 || j == 17){ //char 0xXXの行か、改行のみの行は読み飛ばす
				continue;
			}
			
			num = 0;
			for(k = 0;k < 8;k++){
				if(s[k] == '*'){
					num |= 1<<(7-k);
				}
			}
			
			if(j == 1){
				fprintf(Ofp, "\t0x%02x, ", num);
			}
			else if(j == 16){
				fprintf(Ofp, "0x%02x,\n", num);
			}
			else{
				fprintf(Ofp, "0x%02x, ", num);
			}
		}
	}
	fprintf(Ofp, "};");
	fclose(Ifp);
	fclose(Ofp);
	return 0;
}

