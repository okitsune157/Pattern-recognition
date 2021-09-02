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

struct Image Image_affine_transformation(struct Image data){
    double A[2][2];//入力受け取り用
    double iA[2][2];//計算用
    double det;
    double cx,cy;//アフィン変換係数
    double x0,y0;//回転前の座標
    int u,v;//元画像の座標
    double a,b;//内分比
    int imageout[data.height][data.width];
    //入力を受け取る
    printf("係数(a,b,c,d,e,f):");
    scanf("%lf%lf%lf%lf%lf%lf",&A[0][0],&A[0][1],&cx,&A[1][0],&A[1][1],&cy);
    //逆行列を求める
    det = (A[0][0]*A[1][1])-(A[0][1]*A[1][0]);
    if(det == 0){
        printf("error:ae-bd=0\n");
        exit(1);
    }
    iA[0][0] = A[1][1];//=e
    iA[0][1] = -A[0][1];//=-b
    iA[1][0] = -A[1][0];//=-d
    iA[1][1] = A[0][0];//=a
    for(int y = 0;y < data.height;y++){ //(i)
        for(int x = 0;x < data.width;x++){
            //逆変換を適用
            x0 = ((iA[0][0]*x+iA[0][1]*y)*(1/det)+(iA[0][0]*(-cx)+iA[0][1]*(-cy)))*(1/det);//((ex+(-b)y)+(e(-c)+(-b)(-f)))(1/ae-bd)
            y0 = ((iA[1][0]*x+iA[1][1]*y)*(1/det)+(iA[1][0]*(-cx)+iA[1][1]*(-cy)))*(1/det);//(((-d)x+ay)+((-d)(-c)+a(-f)))(1/ae-bd)
            if(0 <= x0 && x0 < data.width-1 && 0 <= y0 && y0 < data.height-1){
                u = ((iA[0][0]*x+iA[0][1]*y)*(1/det)+(iA[0][0]*(-cx)+iA[0][1]*(-cy)))*(1/det);
                v = ((iA[1][0]*x+iA[1][1]*y)*(1/det)+(iA[1][0]*(-cx)+iA[1][1]*(-cy)))*(1/det);
                a = x0-u;
                b = y0-v;
                //線形補間
                imageout[y][x] = (data.image[v][u]*(1-a)*(1-b)) + (data.image[v][u+1]*a*(1-b)) + (data.image[v+1][u]*(1-a)*b) + (data.image[v+1][u+1]*a*b);
            }else{
                imageout[y][x] = 0;
            }
        }
    }

    for(int i = 0;i < data.height;i++){ //(ⅱ)
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
    //回転
    Image_data = Image_affine_transformation(Image_data);

    file_write(Image_data,argv[2]);
    return 0;        
} 
