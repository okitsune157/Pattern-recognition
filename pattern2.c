#include<stdio.h>

int main() {
    char infile[20],outfile[20];
    FILE *infp, *outfp;
    char magic[20];
    int width, height, max, ret;

    printf("input file name?\n");
    scanf("%s",infile);
    infp = fopen(infile,"r");
    
    //ファイルからのデータ読み込み
    fscanf(infp,"%s%d%d%d",magic,&width,&height,&max);
    
    //C99以降のコンパイラを想定
    int image[height][width];

    for(int i = 0; i < height;i++){
        for(int j = 0; j < width; j++){
            fscanf(infp,"%d",&image[i][j]);
        }
    }

    fclose(infp);

    printf("output file name?\n");
    scanf("%s",outfile);
    outfp = fopen(outfile,"w");

    //ファイルへのデータ書き込み
    fprintf(outfp,"%s\n%d %d\n%d\n",magic,width,height,max);

    for(int i = 0; i < height;i++){
        for(int j = 0; j < width; j++){
            fprintf(outfp,"%d ",image[i][j]);
        }
        fprintf(outfp,"\n");
    }

    fclose(outfp);
    return 0;        
}