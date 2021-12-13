/*目的: 時系列の極値を抽出するプログラム*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-09-15*/
/*データの1行目を参照する*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* 文字列関数を扱えるようにする */
#include <math.h>
#include <time.h>
#include "MT.h"

#define num 1024

void extremum(long double *A, int n, int skip);//時系列データの極値を抽出する

void option(int argc, char *argv[], int *skip);//オプション
void count_data(int *n, int *l);//データ数を数える
void read_comment(char **comment);//コメント文の読み込み
void read_data(long double *data);//データの読み込み
void *e_calloc(int n, int size, char* p);//メモリ確保
void **matrix_calloc(int n, int m, char* p);//2次元配列のメモリ確保
void output_comment(int n, int l, char **comment, int skip);//コメント文出力
void today(void);

int main(int argc, char *argv[]){
  //入力データ
  int i;
  int n, l, m;//データ数 //コメント文の行数
  long double *A=NULL;
  char **comment=NULL;//コメント文を保存する
  //double **data=NULL;
  int skip=0;//時系列データの 0<= t <=skip は出力しない

  option(argc,argv,&skip);
  count_data(&n, &l);  //データ数のカウント //コメント文の行数のカウント
  A=(long double*)e_calloc(n,sizeof(long double),"A");//メモリ確保
  str=(char*)e_calloc(num,sizeof(char),"str");//メモリ確保
  comment=(char **)matrix_calloc(l,num,"comment");
  //data=(double **)matrix_calloc(n, m, "data");
  read_comment(comment);//コメントアウト文の読み込み
  read_data(A);//データの読み込み
  output_comment(n, l, comment, skip);


  //for(i=0;i<n;i++){fprintf(stdout, "%lf %lf \n", data[i][0], data[i][1] );}
  extremum(A,n,skip);

  free(A);//メモリ解放
  for(i=0;i<l;i++){free(comment[i]);}
  free(comment);
  //for(i=0;i<n;i++){free(data[i]);}
  //free(data);
}

//入力されたデータの極値を出力する はじめのskip分のデータを省く
void extremum(long double *A, int n, int skip){
  int i;
  int count=0;
  //極値の抽出
  for(i=1;i<n-1;i++){
    if(i>skip){//はじめのデータを省く
      if( ( A[i-1]<A[i] ) && ( A[i+1]<A[i] ) ){//極大値
        fprintf(stdout, "%Lf \n", A[i]);
        count++;
      }
      if( A[i-1]>A[i] && A[i+1]>A[i] ){//極小値
        fprintf(stdout, "%Lf \n", A[i]);
        count++;
      }
    }
  }
  //fprintf(stdout, "# count= %d \n", count);
}


//オプション
void option(int argc, char *argv[], int *skip){
  int i;
  for(i=1;i<argc;i++){
    if(strcmp(argv[i], "-skip")==0){ *skip=atoi(argv[i+1]); }
  }
}


//データ数を数える
void count_data(int *n, int *l){
  int data_n=0;//データ数
  int comment_l=0;//コメントアウト文の行数
  char line[num];
  while(fgets(line,num,stdin)!=NULL){
    if(line[0]!='#'){
      data_n++;
    }else{
      comment_l++;
    }
  }
  *n=data_n;
  *l=comment_l;
  fseek(stdin, 0, SEEK_SET);
}

//データの読み込み
void read_data(long double *data){
  int i=0;
  char line[num];
  while(fgets(line,num,stdin)!=NULL){
    if(line[0]!='#'){
      sscanf(line,"%Lf", &data[i]);
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

    if(data==NULL)
    {
        fprintf(stderr,"Calloc failed: %s.\n",p);
        exit(1);
    }
    return (void **)data;
}

//コメントアウト文の出力
void output_comment(int n, int l, char **comment, int skip){
  int i;
  //元々入力されていたコメントアウト文の出力
  for(i=0;i<l;i++){
    fprintf(stdout,"%s",comment[i]);
  }
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# extremum.c (時系列データの極値を抽出を行うプログラム)\n");
  fprintf(stdout, "# ./extremum < --.dat\n");
  fprintf(stdout, "# n= %d , l= %d , skip= %d \n", n, l, skip);
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
