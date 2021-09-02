#include<stdio.h>
#include<stdlib.h>
//#include <math.h>

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

struct Image LinearInterpolation(struct Image data){
    int height_mag,width_mag;
    int tmp_height = data.height;
    int tmp_width = data.width;
    double x0,y0;
    //入力を受け取る
    printf("縦倍率:");
    scanf("%d",&height_mag);
    printf("横倍率:");
    scanf("%d",&width_mag);
    data.height = data.height*height_mag;
    data.width = data.width*width_mag;
    int imageout[data.height][data.width];
    int u,v;//元画像の座標
    double a,b;//内分比

    for(int y = 0;y < data.height;y++){ //(ⅱ)
        for(int x = 0;x < data.width;x++){
            u = x/width_mag;
            v = y/height_mag;
            x0 = (double)x/width_mag;
            y0 = (double)y/height_mag;
            if(0 <= x0 && x0 < tmp_width-1 && 0 <= y0 && y0 < tmp_height-1){
                //0で割る場合
                // if(!(u == 0 || v == 0)){
                //     a = fmod(x0, u);
                //     b = fmod(y0, v);
                // }
                a = x0-u;
                b = y0-v;
                //線形補間
                imageout[y][x] = (data.image[v][u]*(1-a)*(1-b)) + (data.image[v][u+1]*a*(1-b)) + (data.image[v+1][u]*(1-a)*b) + (data.image[v+1][u+1]*a*b);

            }else{
                imageout[y][x] = data.image[v][u];//端の行・列には元画像のf(u,v)を代入
            }

        }
    }
    
        for(int i = 0; i < tmp_height;i++) { //data.heightじゃあ変更後のサイズだからオーバーフローだよ！
        free(data.image[i]);
	}
	free(data.image);

    //メモリ確保
    data.image = malloc(sizeof(int *)*data.height); //(ⅰ)
    for(int i = 0;i < data.height;i++){
        data.image[i] = malloc(sizeof(int)*data.width);
    }
    for(int i = 0;i < data.height;i++){ //(ⅲ)
        for(int j = 0;j < data.width;j++){
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
    struct Image Image_data = file_read(argv[1]);
    //線形補完法
    Image_data = LinearInterpolation(Image_data);

    file_write(Image_data,argv[2]);
    return 0;        
} 
