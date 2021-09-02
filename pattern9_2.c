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
struct Image Image_Binarization(struct Image data){//2値化
    int threshold = -1;//閾値
    int N = data.height*data.width;//全画素数
    int n[data.max+1];//画素値の頻度
    for(int k = 0;k <= data.max;k++){
        n[k] = 0;//初期化
    }
    for(int i = 0;i < data.height;i++){//(i)
        for(int j = 0;j < data.width;j++){
            n[data.image[i][j]]++;//頻度
        }
    }
    double o = 0;//生起確率
    double m = 0;//閾値までの平均画素値
    double mt = 0;//全体の平均画素値
    double p = 0;//画素値iを持つ画素の確率
    double denominator = 0;//分母
    double numerator = 0;//分子
    double D = 0;//クラス間分散
    double tmp_D = 0;//クラス間分散一時保存

    //全体の平均画素値
    for(int i = 0;i < data.max;i++){//(ⅱ)
        p = (double)n[i]/N;
        mt = mt + (i*p);
    }

    for(int i = 0;i < data.max;i++){//(ⅲ)
        p = (double)n[i]/N;
        o = o + p;
        m = m + (i*p);
        numerator = mt*o - m;
        numerator = pow(numerator,2.0);
        denominator = o*(1-o);
        if(denominator != 0){//(ⅳ)
            tmp_D = numerator/denominator;
            if(tmp_D > D){
                D = tmp_D;
                threshold = i;
            }
        }
    }
    for(int i = 0;i < data.height;i++){ //(ⅴ)
        for(int j = 0;j < data.width;j++){
            if(data.image[i][j] > threshold){
                data.image[i][j] = data.max;
            }else{
                data.image[i][j] = 0;
            }
        }
    }
    return data;
}

int CalculationAC(int label,struct Image data){
    struct ImageFeatures region[label];
    int maxArea = -1;
    int maxNum = -1;
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
        if(i > 0){
            if(maxArea < region[i].area){
                maxArea = region[i].area;
                maxNum = i;
            }
        }
        
    }
    return maxNum;
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

struct Image Labeling(struct Image data,int *maxNum){
    int label = 1;//ラベル
    *maxNum = -1;
    for(int i = 0;i < data.height;i++){ 
        for(int j = 0;j < data.width;j++){
            if(data.image[i][j] == data.max){
                Concatenation8(i,j,label,data);
                label++;
            }
        }
    }
    *maxNum = CalculationAC(label,data);//面積が一番大きい領域のラベル番号

    return data;
}

struct Image FaceExtraction(struct Image data_tmp,struct Image data,int maxNum){
    for(int i = 0;i < data.height;i++){ 
        for(int j = 0;j < data.width;j++){
            
            if(!(data_tmp.image[i][j] == maxNum)){
                data.image[i][j] = 0;
            }
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
    struct Image Image_data_tmp = file_read(argv[1]);
    int maxNum = -1;
    Image_data_tmp = Image_Binarization(Image_data_tmp);//二値化
    Image_data_tmp = Labeling(Image_data_tmp,&maxNum);//2値化後のラベリング
    Image_data = FaceExtraction(Image_data_tmp,Image_data,maxNum);

    file_write(Image_data,argv[2]);
    return 0;        
} 
