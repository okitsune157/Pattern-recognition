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

struct Image EnhanceContrast(struct Image data,int max_contrast,int min_contrast){
    for(int i = 0; i < data.height;i++){
        for(int j = 0; j < data.width; j++){
            data.image[i][j] = ((double)(data.image[i][j] - min_contrast)/(max_contrast - min_contrast)) * (double)data.max;
        }
    }
    return data;
}

void search_max_min(struct Image data,int *max_contrast,int *min_contrast){
    for(int i = 0; i < data.height;i++){
        for(int j = 0; j < data.width; j++){
            if(i == 0 && j == 0){
                *max_contrast = data.image[i][j];
                *min_contrast = data.image[i][j];
            }
            if(data.image[i][j] > *max_contrast){
                *max_contrast = data.image[i][j];
            }else if(data.image[i][j] < *min_contrast){
                *min_contrast = data.image[i][j];
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("usage: %s [input filename] [output filename]\n", argv[0]);
        exit(-1);
    }
    struct Image Image_data = file_read(argv[1]);
    //コントラストの最大値と最小値
    int max_contrast;
    int min_contrast;
    search_max_min(Image_data,&max_contrast,&min_contrast);
    //コントラスト強調
    Image_data = EnhanceContrast(Image_data,max_contrast,min_contrast);
    file_write(Image_data,argv[2]);
    return 0;        
} 
