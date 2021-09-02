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

struct Cluster{
    int center;//中心
    int old_center;//旧中心
    int count_pattern;//パターン数
    int total_features;//特徴量合計
};

struct ImageFeatures {
    int area;//面積
    int cluster_num;//クラスタ番号
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

void Clustering(int pattern_num,struct Image data,struct ImageFeatures pattern[]){
    int cluster_num;
    int dif = 0;
    printf("クラスタ数:");
    scanf("%d",&cluster_num);
    struct Cluster cluster[cluster_num];
    for(int i = 0;i < cluster_num;i++){
        cluster[i].center = pattern[i].area; 
    }
    do{
        dif = 0;//クラスタ中心ずれ初期化
        for(int i = 0;i < cluster_num;i++){
            cluster[i].count_pattern = 0;//パターン数合計初期化
            cluster[i].total_features = 0;//特徴量合計初期化
        }
        for(int p = 0;p < pattern_num;p++){
            int minD = INT_MAX;//最短距離初期化
            for(int i = 0;i < cluster_num;i++){
                int D =abs(cluster[i].center - pattern[p].area);
                if(D < minD){
                    minD = D;
                    pattern[p].cluster_num = i;
                }
                cluster[pattern[p].cluster_num].count_pattern++;//パターン数合計
                cluster[pattern[p].cluster_num].total_features += pattern[p].area;//特徴量合計
            }
        }
        for(int i = 0;i < cluster_num;i++){
            cluster[i].old_center = cluster[i].center;
            cluster[i].center = cluster[i].total_features / cluster[i].count_pattern;
            dif += abs(cluster[i].center - cluster[i].old_center);//クラスタ中心ずれ合計
        }
    }while(dif > 0);
    printf("label number    area    cluster number\n");
    for(int i = 0;i < pattern_num;i++){
        printf("    %2d      %8d           %d\n",i,pattern[i].area,pattern[i].cluster_num);
    }
}

struct Image Inversion(struct Image data){
    for(int i = 0; i < data.height;i++){
        for(int j = 0; j < data.width; j++){
            data.image[i][j] = data.max - data.image[i][j];
        }
    }
    return data;
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

struct ImageFeatures Calculation(int label,struct Image data,struct ImageFeatures pattern[]){
    for(int i = 0;i < label;i++){
        pattern[i].area = 0;
    }
    for(int y = 0;y < data.height;y++){
        for(int x = 0;x < data.width;x++){
            pattern[data.image[y][x]].area++;
        }
    }
    Clustering(label,data,pattern);//クラスタリング
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
    struct ImageFeatures pattern[label];
    Calculation(label,data,pattern);
    return data;
}


int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("usage: %s [input filename] [output filename]\n", argv[0]);
        exit(-1);
    }
    struct Image Image_data = file_read(argv[1]);
    Image_data = Inversion(Image_data);//反転
    Image_data = Image_Binarization(Image_data);//二値化
    Image_data = Labeling(Image_data);//ラベリング

    file_write(Image_data,argv[2]);
    return 0;        
} 
