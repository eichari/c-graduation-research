/*目的: RPの定量化指標を求めるプログラム*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-09-08*/
/*RPの座標情報からDETを計算する*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#define num 1024


void print_data(double **data, int n, int m);//動作確認用
void cal_DET(double **data, int n, int m, int index_max, double *DET);
int index_max_search(double **data, int n, int m);//入力データからRPのインデックスの最大値を求める

//コメント文出力しない　1 する 0
void option(int argc, char *argv[], int *memo, double *theta);//オプション
//オプションなし
void count_data(int *n, int *l, int *m);//データ数を数える
void read_comment(char **comment);//コメント文の読み込み
void read_data(double **data, int m);//2次元データ，n行m列の読み込み
void *e_calloc(int n, int size, char* p);//メモリ確保
void **matrix_calloc(int n, int m, char* p);//2次元配列のメモリ確保
void output_comment(int argc, char *argv[], int l, int n, int m, char **comment);
void today(void);//日付の表示

int main(int argc, char *argv[]){
  int i;
  //入力データ
  int n, l, m;//データ数 //コメント文の行数 //データの行数
  char *str=NULL;//1行ずつ読みこむ
  char **comment=NULL;//コメント文を保存する
  double **data=NULL;//入力m次元データ
  int memo=0;
  double theta=-1;

  //DETのパラメータ
  double DET;
  int index_max=0;//入力データからRPのインデックスの最大値を求める
  option(argc,argv,&memo,&theta);
  count_data(&n, &l, &m);  //データ数のカウント //コメント文の行数のカウント
  str=(char*)e_calloc(num,sizeof(char),"str");
  comment=(char **)matrix_calloc(l, num, "comment");
  data=(double **)matrix_calloc(n, m, "data");

  read_comment(comment);//コメントアウト文の読み込み
  read_data(data,m);//2次元データ，n行m列の読み込み
  if(memo==1){
    output_comment(argc, argv, l, n, m, comment);//コメント文の出力
  }

  index_max=index_max_search(data,n,m);//入力データからRPのインデックスの最大値を求める

  cal_DET(data, n, m, index_max, &DET);
  //print_data(data,n,m);
  if(memo==1){
    fprintf(stdout, "# index_max= %d (%d) \n", index_max, index_max+1 );
    fprintf(stdout, "# DET= %lf \n",DET );
  }
  if(theta>0){
    fprintf(stdout, "%lf %lf \n", theta, DET );
  }else{
    fprintf(stdout, "%lf \n",DET );
  }


  free(str);
  for(i=0;i<l;i++){free(comment[i]);}
  free(comment);
  for(i=0;i<n;i++){free(data[i]);}
  free(data);

  return 0;
}
//オプション
void option(int argc, char *argv[], int *memo, double *theta){
  int i;
  for(i=1;i<argc;i++){
    if(strcmp(argv[i], "-memo")==0){ *memo=atoi(argv[i+1]); }
    if(strcmp(argv[i], "-theta")==0){ *theta=atof(argv[i+1]); }
  }
}

//入力データからRPのインデックスの最大値を求める
int index_max_search(double **data, int n, int m){  
  int i,j;
  int index_max=0;
  for(i=0;i<n;i++){
    for(j=0;j<m;j++){
      if(index_max<(int)data[i][j]){
        index_max=(int)data[i][j];
      }
    }
  }
  return index_max;
}

//DETの計算
void cal_DET(double **data, int n, int m, int index_max, double *DET){
  //R(i,j)の周囲1マスは余白にする　データは 1-n
  int i,j,k,l;
  int d1,d2;//DETを計算するときの分子と分母
  int R_n=index_max+1;
  int l_max=0;
  int **R=NULL;
  int **T=NULL;//T[][] (i,j)の長さを記録 重複は許さない
  int **B=NULL;//D(l)を求める過程でR(i,j)が探索済みかどうか調べる
  int *D=NULL;//集合D(l)の数を表す |D(l)|
  R=(int **)matrix_calloc(R_n+2, R_n+2, "R");
  T=(int **)matrix_calloc(R_n, R_n, "T");
  B=(int **)matrix_calloc(R_n, R_n, "B");

  D=(int *)e_calloc(R_n,sizeof(int),"D");

  //R[][]の初期化 //外周も
  for(i=0;i<R_n+2;i++){
    for(j=0;j<R_n+2;j++){
      R[i][j]=0;
    }
  }

  //R[][]作成
  for(i=0;i<n;i++){//nはdataのインデックス
    R[ (int)data[i][0]+1 ][ (int)data[i][1]+1 ]=1;
  }

  //外周も出力
  /*
  if(R_n<15){//表示が多いときは出力しない
    fprintf(stdout, "# 入力RPの出力(下三角形のみ，外周あり)\n");
    for(i=R_n+1;i>=0;i--){
      fprintf(stdout, "# ");
      for(j=0;j<R_n+2;j++){
        fprintf(stdout, "%2d ",R[i][j] );
      }
      fprintf(stdout, "\n" );
    }
  }
  */


  //B[][] T[][]の初期化
  for(i=0;i<R_n-1;i++){
    for(j=i+1;j<R_n;j++){
      B[i][j]=0;//B[][]はn*n行列 //1は探索済み 0は未探索
      T[i][j]=0;//(i,j)からの長さ
    }
  }
  
  //T[][] (i,j)からの長さの計算 重複は許さない
  l_max=0;
  for(i=1;i<R_n;i++){
    for(j=i+1;j<R_n+1;j++){
      k=0;
      while(1){
        //printf("%d %d %d\n",i,j,k );
        if(R[i+k][j+k]==1 && B[i-1+k][j-1+k]==0){
          T[i-1][j-1]++;
          B[i-1+k][j-1+k]=1;
          k++;
        }else{//R[i][j]==0
          break;
        }
      }
      //lの最大長を求める
      if(l_max<T[i-1][j-1]){
        l_max=T[i-1][j-1];
      }
    }
  }
  /*
  //T[][]の出力  
  for(i=R_n-1;i>=0;i--){
    for(j=0;j<R_n;j++){
      if(T[i][j]>0){
        fprintf(stdout, "%2d ",T[i][j] );
      }else{
        fprintf(stdout,"  ");
      }
    }
    fprintf(stdout,"\n");
  }
  fprintf(stdout,"\n");
  */
  //|D[l]|を求める
  for(l=1;l<=l_max;l++){
    D[l]=0;
    for(i=0;i<R_n-1;i++){
      for(j=i+1;j<R_n;j++){
        if(l==T[i][j]){
          D[l]++;
        }
      }
    }
    if(D[l]!=0){
      //fprintf(stdout, "D[%d]= %d\n", l, D[l]);
    }
  }

  //DET
  d1=0;d2=0;
  for(l=1;l<=l_max;l++){
    d1=d1+l*D[l];
    if(l>=2){
      d2=d2+l*D[l];
    }
  }
  *DET=(double)d2/(double)d1;



  //メモリ解放
  for(i=0;i<R_n+2;i++){free(R[i]);} free(R);
  for(i=0;i<R_n;i++){free(T[i]);} free(T);
  for(i=0;i<R_n;i++){free(B[i]);} free(B);
  free(D);
}

//行列の出力 //動作確認用
void print_data(double **data, int n, int m){
  int i,j;
  for(i=0;i<n;i++){
    for(j=0;j<m;j++){
      fprintf(stdout, "%g ", data[i][j]);
    }
    fprintf(stdout, "\n");
  }
}

//データ情報を取得
void count_data(int *n, int *l, int *m){
  int data_n=0;//データ数
  int data_m=0, d=0;
  int comment_l=0;//コメントアウト文の行数
  char line[num];
  char *tok,*space=" ";

  while(fgets(line,num,stdin)!=NULL){
    d=0;
    if(line[0]!='#'){//データの行，列数のカウント
      data_n++;
      tok = strtok(line,space);
      while(tok!=NULL){
        d++;
        tok=strtok(NULL,space);
      }//spcae" "の数を数えてるのでデータの右端，改行する前にスペース" "があるか確認
      //また，エディタによっては行末スペース削除される
      d--;
      data_m+=d;
    }else{//コメント文の行数のカウント
      comment_l++;
    }
  }
  *n=data_n;
  *m=data_m/data_n;
  *l=comment_l;
  fseek(stdin, 0, SEEK_SET);
}

//データの読み込み
void read_data(double **data, int m){
  int i=0,j=0;
  char line[num];
  char *tok,*space=" ";
  while(fgets(line,num,stdin)!=NULL){
    j=0;
    if(line[0]!='#'){//データの行，列数のカウント
      tok=strtok(line,space);
      data[i][j]=atof(tok);
      while(tok!=NULL){
        j++;
        tok=strtok(NULL,space);
        if(j<m){
          data[i][j]=atof(tok);
        }
      }
      i++;
    }
  }
  fseek(stdin, 0, SEEK_SET);
}

//コメントアウト文の読み込み
void read_comment(char **comment){
  int i=0;
  char line[num];
  while(fgets(line,num,stdin)!=NULL){
    if(line[0]=='#'){//コメントアウト文
      strcpy(comment[i],line);//コメントアウト文の読み込み
      i++;
    }else{
      break;
    }
  }
  fseek(stdin, 0, SEEK_SET);
}

//メモリ確保
void *e_calloc(int n, int size, char* p){
    void *o;
    if((o = (void*)calloc(n, size))==NULL){
        fprintf(stderr, "Calloc failed: %s.\n", p);
        exit(1);
    }
    return (void*)o;
}

//2次元配列のメモリ確保
void **matrix_calloc(int n, int m, char* p){
    int i;
    double **data;

    data=(double**)calloc(n,sizeof(double*));
    for(i=0;i<n;i++){
      data[i]=(double*)calloc(m,sizeof(double));
    }

    if(data==NULL){
        fprintf(stderr,"Calloc failed: %s.\n",p);
        exit(1);
    }
    return (void **)data;
}

//コメントアウト文の出力
void output_comment(int argc, char *argv[], int l, int n, int m, char **comment){
  int i;
  //元々入力されていたコメントアウト文の出力
  for(i=0;i<l;i++){
    fprintf(stdout,"%s",comment[i]);
  }
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# DET.c (RPの定量化指標DETを求める)\n");
  fprintf(stdout, "# ./DET （オプションなし）");
  for(i=1;i<argc;i++){
    fprintf(stdout, "%s ", argv[i]);
  }
  fprintf(stdout, "\n");
  fprintf(stdout, "# n= %d , m= %d \n", n, m);
  today();
}

void today(void){
  time_t timer;        /* 時刻を取り出すための型（実際はunsigned long型） */
  struct tm *local;    /* tm構造体（時刻を扱う */
  int year, month, day;  /* 年月日保存用 */

  timer = time(NULL);        /* 現在時刻を取得 */
  local = localtime(&timer);    /* 地方時に変換 */

  /* 年月日をtm構造体の各パラメタから変数に代入 */
  year = local->tm_year + 1900;        /* 1900年からの年数が取得されるため */
  month = local->tm_mon + 1;        /* 0を1月としているため */
  day = local->tm_mday;

  /* 現在の日時を表示 */
  fprintf(stdout,"# プログラム実行日 %d年%d月%d日\n", year, month, day);
}
