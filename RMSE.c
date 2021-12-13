/*目的: 2つの時系列データの平均平方二乗誤差を求めるプログラム*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-09-14*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* 文字列関数を扱えるようにする */
#include <math.h>
#include <time.h>

#define num 1024

double RMSE(double *A, double *B, int n);//標準化を行う
double average(double *A, int n);//平均値を求める
double variance(double *A, int n);//分散を求める

void option(int argc, char *argv[], char *fname1, char *fname2);
void count_data(int *n, int *l, FILE *fp);//データ数を数える
void read_comment(char **comment, FILE *fp);//コメント文の読み込み
void read_data(double *data, FILE *fp);//データの読み込み
void *e_calloc(int n, int size, char* p);//メモリ確保
void **matrix_calloc(int n, int m, char* p);//2次元配列のメモリ確保
void output_comment(int n, int l, char **comment);//コメント文出力
void today(void);

int main(int argc, char *argv[]){
  //入力データ
  int i;
  int n1, l1, n2, l2;//データ数 //コメント文の行数
  int n;//小さい方の時系列長を入れる
  double ans=0;

  double *A=NULL;
  double *B=NULL;
  char **comment1=NULL;//コメント文を保存する
  char **comment2=NULL;//コメント文を保存する
  FILE *fp1=NULL;
  FILE *fp2=NULL;
  char *fname1=argv[1];
	char *fname2=argv[2];
  
  //fprintf(stdout, "input two filename. -file1 -file2 \n");
  //option(argc, argv, fname1, fname2);
  //fprintf(stdout, "fname1= %s\n",fname1 );
  //fprintf(stdout, "fname2= %s\n",fname2 );
  
  fp1=fopen(fname1,"r");
  count_data(&n1, &l1, fp1);//データ数のカウント //コメント文の行数のカウント
  A=(double*)e_calloc(n1,sizeof(double),"A");//メモリ確保
  comment1=(char **)matrix_calloc(l1,num,"comment1");
  read_comment(comment1,fp1);//コメントアウト文の読み込み
  read_data(A,fp1);//データの読み込み
  output_comment(n1, l1, comment1);

  fp2=fopen(fname2,"r");
  count_data(&n2, &l2, fp2);//データ数のカウント //コメント文の行数のカウント
  B=(double*)e_calloc(n2,sizeof(double),"B");//メモリ確保
  comment2=(char **)matrix_calloc(l2,num,"comment2");
  read_comment(comment2,fp2);//コメントアウト文の読み込み
  read_data(B,fp2);//データの読み込み
  output_comment(n2, l2, comment2);

  //for(i=0;i<n1;i++){fprintf(stdout, "%lf \n",A[i] );}
  //for(i=0;i<n2;i++){fprintf(stdout, "%lf \n",B[i] );}
  
  if(n1>n2){n=n2;}else{n=n1;}//時系列長の小さいほうに合わせる
  ans=RMSE(A,B,n);//平均二乗誤差平方根
  fprintf(stdout, "%lf \n",ans );

  //終了処理
  free(A);//メモリ解放
  for(i=0;i<l1;i++){free(comment1[i]);}
  free(comment1);
  fclose(fp1);

  free(B);
  for(i=0;i<l2;i++){free(comment2[i]);}
  free(comment2);
  fclose(fp2);

}

//相関係数を求める
double RMSE(double *A, double *B, int n){
  int i;
  double ans=0;
  for(i=0;i<n;i++){
    ans+=(A[i]-B[i])*(A[i]-B[i]);
  }
  return sqrt(ans/n);
}

//平均値を求める
double average(double *A, int n){
  int i;
  double ans=0;
  double kijun;
  kijun=A[0];
  for(i=0;i<n;i++){
    ans=ans+(A[i]-kijun);
  }
  ans=ans/n+kijun;
  //fprintf(stdout, "%lf \n",ans );
  return ans;
}

//分散を求める
double variance(double *A, int n){
  int i;
  double ans=0;
  double ave=average(A,n);
  for(i=0;i<n;i++){
    ans=ans+(A[i]-ave)*(A[i]-ave);
  }
  ans=ans/n;
  //fprintf(stdout, "%lf \n",ans );
  return ans;
}

//オプション
/*
void option(int argc, char *argv[], char *fname1, char *fname2){
  int i;
  for(i=1;i<argc;i++){
    //if(strcmp(argv[i], "-fname1")==0){strcpy(fname1,argv[i+1]); }
    if(strcmp(argv[i], "-fname1")==0){
      *fname1=argv[i+1]; 
    }
    if(strcmp(argv[i], "-fname2")==0){
      *fname2=argv[i+1];
    }
  }
}
*/

//データ数を数える
void count_data(int *n, int *l, FILE *fp){
  int data_n=0;//データ数
  int comment_l=0;//コメントアウト文の行数
  char line[num];
  while(fgets(line,num,fp)!=NULL){
    if(line[0]!='#'){
      data_n++;
    }else{
      comment_l++;
    }
  }
  *n=data_n;
  *l=comment_l;
  fseek(fp, 0, SEEK_SET);
}

//データの読み込み
void read_data(double *data, FILE *fp){
  int i=0;
  char line[num];
  while(fgets(line,num,fp)!=NULL){
    if(line[0]!='#'){
      sscanf(line,"%lf", &data[i]);
      i++;
    }
  }
  fseek(fp, 0, SEEK_SET);
}

//コメントアウト文の読み込み
void read_comment(char **comment, FILE *fp){
  int i=0;
  char line[num];
  while(fgets(line,num,fp)!=NULL){
    if(line[0]=='#'){//コメントアウト文
      strcpy(comment[i],line);//コメントアウト文の読み込み
      i++;
    }else{
      break;
    }
  }
  fseek(fp, 0, SEEK_SET);
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

    if(data==NULL)
    {
        fprintf(stderr,"Calloc failed: %s.\n",p);
        exit(1);
    }
    return (void **)data;
}

//コメントアウト文の出力
void output_comment(int n, int l, char **comment){
  int i;
  //元々入力されていたコメントアウト文の出力
  for(i=0;i<l;i++){
    fprintf(stdout,"%s",comment[i]);
  }
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# RMSE.c (平均平方二乗誤差を求めるプログラム)\n");
  fprintf(stdout, "# ./RMSE -test.dat -test2.dat \n");
  fprintf(stdout, "# n= %d \n", n);
  today();
}

//日付表示
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
