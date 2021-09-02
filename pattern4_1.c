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

struct Image MedianFilterMethod(struct Image data){
    int imageout[SIZE][SIZE];
    int a[9];
    int tmp;
    for(int i = 1; i < data.height-1; i++){  //二重for文[1]
        for(int j = 1; j < data.width-1; j++){
            a[0] = data.image[i-1][j-1];
            a[1] = data.image[i-1][j];
            a[2] = data.image[i-1][j+1];
            a[3] = data.image[i][j-1];
            a[4] = data.image[i][j];
            a[5] = data.image[i][j+1];
            a[6] = data.image[i+1][j-1];
            a[7] = data.image[i+1][j];
            a[8] = data.image[i+1][j+1];
            // if(i == data.height-1 || j == data.width-1){
            //     printf("%d, ",data.image[i+1][j+1]);
            // }

            for(int n = 0; n < 9; n++){    //二重for文[2]
                for(int m = n+1; m < 9; m++){
                    if(a[n] > a[m]){
                        tmp = a[n];
                        a[n] = a[m];
                        a[m] = tmp;
                    }
                }
            }
            //中央値を代入            
            imageout[i][j] = a[4];
        }
    }
    for(int i = 1; i < data.height; i++){  //二重for文[3]
        for(int j = 1; j < data.width; j++){
            data.image[i][j] = imageout[i][j];
        }
    }
    return data;
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("usage: %s [input filename] [output filename]\n", argv[0]);
        exit(-1);
    }
    //ファイル読み込み
    struct Image Image_data = file_read(argv[1]);
    //メディアンフィルタ法
    Image_data  = MedianFilterMethod(Image_data);
    //ファイル書き込み
    file_write(Image_data,argv[2]);
    return 0;        
} 
