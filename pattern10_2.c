#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <math.h>

struct Image {
    char magic[3];
    int width;
    int height;
    int max;
    int **image;
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
    //メモリ確保
    data.image = malloc(sizeof(int *)*data.height); //(ⅰ)
    for(int i = 0;i < data.height;i++){
        data.image[i] = malloc(sizeof(int)*data.width);
    }
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

    //メモリ開放
    for(int i = 0; i < data.height;i++) {
		free(data.image[i]);
	}
	free(data.image);
}

struct Image template_matching_similarity(struct Image data,struct Image data_te){
    double maxS = DBL_MIN;
    int op_x = -1;
    int op_y = -1;
    int TT = 0;
    for(int m = 0;m <= data_te.height-1;m++){
        for(int n = 0;n <= data_te.width-1;n++){
            TT += data_te.image[m][n]*data_te.image[m][n];
        }
    }
    for(int y = 0;y <= data.height - data_te.height;y++){ 
        for(int x = 0;x <= data.width - data_te.width;x++){
            int IT = 0;
            int II = 0;
            double S = 0;
            for(int m = 0;m <= data_te.height-1;m++){
                for(int n = 0;n <= data_te.width-1;n++){
                    //int tmp = data.image[y+m][x+n];
                    IT += data.image[y+m][x+n]*data_te.image[m][n];
                    II += data.image[y+m][x+n]*data.image[y+m][x+n];
                }
            }
            S = (double)(IT/(sqrt(II)*sqrt(TT)));
            if(S > maxS){
                maxS = S;
                op_x = x;
                op_y = y;
            }
        }
    }
    for(int x = op_x;x < op_x + data_te.width;x++){
        data.image[op_y][x] = data.max;
        data.image[op_y + data_te.height][x] = data.max;
    }
    for(int y = op_y;y < op_y + data_te.height;y++){
        data.image[y][op_x] = data.max;
        data.image[y][op_x + data_te.width] = data.max;
    }
    printf("最適x: %d \n最適y: %d \n類似度の最大: %f",op_x,op_y,maxS);
    return data;
}

int main(int argc, char *argv[]) {
    if(argc != 4) {
        printf("usage: %s [input filename] [output filename]\n", argv[0]);
        exit(-1);
    }
    struct Image Image_data = file_read(argv[1]);
    struct Image Image_data_te = file_read(argv[2]);
    Image_data = template_matching_similarity(Image_data, Image_data_te);
    file_write(Image_data,argv[3]);
    return 0;        
} 
