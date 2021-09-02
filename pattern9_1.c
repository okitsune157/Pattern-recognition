#include<stdio.h>
#include<stdlib.h>
#include <math.h>

struct Image {
    char magic[3];
    int width;
    int height;
    int max;
    int **image;
};

struct ImageFeatures {
    int area;//面積
    int xT;//xの合計
    int yT;
    int xwt;//xの重心
    int ywt;
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

// struct Image EnhanceContrast(struct Image data,int max_contrast,int min_contrast){
//     for(int i = 0; i < data.height;i++){
//         for(int j = 0; j < data.width; j++){
//             data.image[i][j] = ((double)(data.image[i][j] - min_contrast)/(max_contrast - min_contrast)) * (double)data.max;
//         }
//     }
//     return data;
// }

// void search_max_min(struct Image data,int *max_contrast,int *min_contrast){
//     for(int i = 0; i < data.height;i++){
//         for(int j = 0; j < data.width; j++){
//             if(i == 0 && j == 0){
//                 *max_contrast = data.image[i][j];
//                 *min_contrast = data.image[i][j];
//             }
//             if(data.image[i][j] > *max_contrast){
//                 *max_contrast = data.image[i][j];
//             }else if(data.image[i][j] < *min_contrast){
//                 *min_contrast = data.image[i][j];
//             }
//         }
//     }
// }

struct ImageFeatures CalculationAC(int label,struct Image data){
    struct ImageFeatures region[label];
    for(int i = 0;i < label;i++){
        region[i].area = 0;
        region[i].xT = 0;
        region[i].yT = 0;
    }
    for(int y = 0;y < data.height;y++){
        for(int x = 0;x < data.width;x++){
            region[data.image[y][x]].area++;
            region[data.image[y][x]].xT += x;
            region[data.image[y][x]].yT += y;
        }
    }
    for(int i = 0;i < label;i++){
        region[i].xwt = region[i].xT/region[i].area;
        region[i].ywt = region[i].yT/region[i].area;
    }
    printf("label number  area  xwt  ywt\n");
    for(int i = 0;i < label;i++){
        printf("    %d        %d  %d  %d\n",i,region[i].area,region[i].xwt,region[i].ywt);
    }
}

struct Image Concatenation8(int i, int j, int label, struct Image data){//ラベル付け関数
    data.image[i][j] = label;
    if(data.image[i-1][j-1] == data.max){
        Concatenation8(i-1,j-1,label,data);
    }
    if(data.image[i-1][j] == data.max){
        Concatenation8(i-1,j,label,data);
    }
    if(data.image[i-1][j+1] == data.max){
        Concatenation8(i-1,j+1,label,data);
    }
    if(data.image[i][j-1] == data.max){
        Concatenation8(i,j-1,label,data);
    }
    if(data.image[i][j+1] == data.max){
        Concatenation8(i,j+1,label,data);
    }
    if(data.image[i+1][j-1] == data.max){
        Concatenation8(i+1,j-1,label,data);
    }
    if(data.image[i+1][j] == data.max){
        Concatenation8(i+1,j,label,data);
    }
    if(data.image[i+1][j+1] == data.max){
        Concatenation8(i+1,j+1,label,data);
    }
}

struct Image Labeling(struct Image data){
    int label = 1;//ラベル
    for(int i = 0;i < data.height;i++){ 
        for(int j = 0;j < data.width;j++){
            if(data.image[i][j] == data.max){
                Concatenation8(i,j,label,data);
                label++;
            }
        }
    }
    CalculationAC(label,data);

    return data;
}


int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("usage: %s [input filename] [output filename]\n", argv[0]);
        exit(-1);
    }
    struct Image Image_data = file_read(argv[1]);
    //ラベリング
    Image_data = Labeling(Image_data);
    //コントラストの最大値と最小値
    // int max_contrast;
    // int min_contrast;
    //search_max_min(Image_data,&max_contrast,&min_contrast);
    //コントラスト強調
    //Image_data = EnhanceContrast(Image_data,max_contrast,min_contrast);

    file_write(Image_data,argv[2]);
    return 0;        
} 
