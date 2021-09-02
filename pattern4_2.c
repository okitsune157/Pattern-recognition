#include<stdio.h>
#include<stdlib.h>
#define SIZE 500

struct Image {
    char magic[3];
    int width;
    int height;
    int max;
    int image[SIZE][SIZE];
};

struct Image file_read(char* filename){
    struct Image data;
    FILE *infp;
    infp = fopen(filename,"r");
    if(infp == NULL){
        printf("%s のオープン失敗\n", filename);
        exit(EXIT_FAILURE);
    }
    fscanf(infp,"%s%d%d%d",data.magic,&data.width,&data.height,&data.max);
    for(int i = 0; i < data.height;i++){
        for(int j = 0; j < data.width; j++){
            fscanf(infp,"%d",&data.image[i][j]);
        }
    }
    fclose(infp);
    return data;
}

struct Image file_write(struct Image data, char* filename){
    FILE *outfp;
    outfp = fopen(filename,"w");
    //ファイルへのデータ書き込み
    fprintf(outfp,"%s\n%d %d\n%d\n",data.magic,data.width,data.height,data.max);
    for(int i = 0; i < data.height;i++){
        for(int j = 0; j < data.width; j++){
            fprintf(outfp,"%d ",data.image[i][j]);
        }
        fprintf(outfp,"\n");
    }
    fclose(outfp);
}

struct Image AveragingBlock(struct Image data){
    int bsize,tmp;
    printf("Block size: ");
    scanf("%d",&bsize);
    //int count_block = 0;

    for(int i = 0;i <= data.height-bsize;i += bsize){  //二重for文[1]
        for(int j = 0;j <= data.width-bsize;j += bsize){
            tmp = 0;
            for(int k = i;k < i + bsize;k++){   //二重for文[2]
                for(int l = j;l < j + bsize; l++){
                    tmp += data.image[k][l];
                }
            }
            tmp = tmp / (bsize*bsize);
            for(int k = i;k < i + bsize;k++){   //二重for文[3]
                for(int l = j;l < j + bsize; l++){
                    data.image[k][l] = tmp;
                }
            }
            //count_block++;            
            
        }
    }
    //printf("%d",count_block);
    return data;
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("usage: %s [input filename] [output filename]\n", argv[0]);
        exit(-1);
    }
    //ファイル読み込み
    struct Image Image_data = file_read(argv[1]);
    //ブロック内を平均化
    Image_data = AveragingBlock(Image_data);
    //ファイル書き込み
    file_write(Image_data,argv[2]);
    return 0;        
} 
