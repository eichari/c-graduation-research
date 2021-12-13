/*目的: 時系列データの長さをあわせるプログラム*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-09-22*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* 文字列関数を扱えるようにする */
#include <math.h>
#include <time.h>

#define num 1024

void option(int argc, char *argv[], double *dt, double *start, double *end);
void match(long double *A, int n, double dt, double start, double end);//データの幅を合わせる

void count_data(int *n, int *l);//データ数を数える
void read_comment(char **comment);//コメント文の読み込み
void read_data(long double *data);//データの読み込み
void *e_calloc(int n, int size, char* p);//メモリ確保
void **matrix_calloc(int n, int m, char* p);//2次元配列のメモリ確保
void output_comment(int argc, char *argv[], int n, int l, char **comment, double dt);//コメント文出力
void today(void);

int main(int argc, char *argv[]){
  //入力データ
  int i;
  int n, l;//データ数 //コメント文の行数
  long double *A=NULL;
  char *str=NULL;//1行ずつ読みこむ
  char **comment=NULL;//コメント文を保存する
  double dt=-1;
  double start=0, end=-1;
  

  option(argc,argv,&dt,&start,&end);
  count_data(&n, &l);  //データ数のカウント //コメント文の行数のカウント
  A=(long double*)e_calloc(n,sizeof(long double),"A");//メモリ確保
  str=(char*)e_calloc(num,sizeof(char),"str");//メモリ確保
  comment=(char **)matrix_calloc(l,num,"comment");
  read_comment(comment);//コメントアウト文の読み込み
  read_data(A);//データの読み込み
  output_comment(argc,argv,n, l, comment,dt);
  
  //fprintf(stdout, "average= %lf \n",average(A,n) );
  //fprintf(stdout, "variance= %lf \n",variance(A,n) );
  //for(i=0;i<n;i++){fprintf(stdout, "%lf \n",A[i] );}
  match(A,n,dt,start,end);
  //fprintf(stdout, "standardize.\n" );
  //for(i=0;i<n;i++){fprintf(stdout, "%Lf \n",A[i] );}

  free(A);//メモリ解放
  free(str);
  for(i=0;i<l;i++){free(comment[i]);}
  free(comment);

}

//オプション
void option(int argc, char *argv[], double *dt, double *start, double *end){
  int i;
  for(i=1;i<argc;i++){
    if(strcmp(argv[i], "-dt")==0){ *dt=atof(argv[i+1]); }
    if(strcmp(argv[i], "-start")==0){ *start=atof(argv[i+1]); }
    if(strcmp(argv[i], "-end")==0){ *end=atof(argv[i+1]); }
  }
}

//時系列データの長さを合わせる
void match(long double *A, int n, double dt, double start, double end){
  int i;
  double t=0;
  if(end<0){//endが入力されなかったら
    if(start>0){
      t=start;
    }
    for(i=0;i<n;i++){
      fprintf(stdout, "%lf %Lf \n", t, A[i]);
      t=t+dt;
    }

  }else{//endが入力されたら
    dt=(double)(end-start)/(double)(n-1);
    if(start>0){
      t=start;
    }
    for(i=0;i<n-1;i++){
      fprintf(stdout, "%lf %Lf \n", t, A[i]);
      t=t+dt;
    }
    fprintf(stdout, "%lf %Lf \n", end, A[n-1]);
    
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
void output_comment(int argc, char *argv[], int n, int l, char **comment, double dt){
  int i;
  //元々入力されていたコメントアウト文の出力
  for(i=0;i<l;i++){
    fprintf(stdout,"%s",comment[i]);
  }
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# match.c (時系列データの長さを合わせるプログラム)\n");
  fprintf(stdout, "# ./match ");
  for(i=1;i<argc;i++){
    fprintf(stdout, "%s ",argv[i] );
  }
  fprintf(stdout, "\n" );
  fprintf(stdout, "# n= %d , dt= %lf \n", n, dt);
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
