/*目的: 時系列データから再構成を行うプログラム*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-09-02*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* 文字列関数を扱えるようにする */
#include <math.h>
#include <time.h>

#define num 1024

void delay(double *A, int n, int tau, int m);//遅れ時間座標系の作成

void option(int argc, char *argv[], int *tau, int *m);//オプション
void count_data(int *n, int *l);//データ数を数える
void read_comment(char **comment);//コメント文の読み込み
void read_data(double *data);//データの読み込み
void *e_calloc(int n, int size, char* p);//メモリ確保
void **matrix_calloc(int n, int m, char* p);//2次元配列のメモリ確保
void output_comment(int argc, char *argv[], int n, int l, int tau, int m, char **comment);//コメント文出力
void today(void);

int main(int argc, char *argv[]){
  int i;
  //再構成
  int tau;//遅れ時間τ
  int m;//埋め込み次元数
  //入力データ
  int n, l;//データ数 //コメント文の行数
  double *A=NULL;
  char *str=NULL;//1行ずつ読みこむ
  char **comment=NULL;//コメント文を保存する

  option(argc, argv, &tau, &m);  //オプション
  count_data(&n, &l);  //データ数のカウント //コメント文の行数のカウント
  A=(double*)e_calloc(n,sizeof(double),"A");
  str=(char*)e_calloc(num,sizeof(char),"str");//メモリ確保
  comment=(char **)matrix_calloc(l,num,"comment");
  read_comment(comment);//コメントアウト文の読み込み
  read_data(A);//データの読み込み
  output_comment(argc, argv, n, l, tau, m, comment);

  delay(A, n, tau, m);  //遅れ時間座標系の作成
  free(A);  //メモリ解放
  free(str);
  for(i=0;i<l;i++){free(comment[i]);}
  free(comment);
}

//遅れ時間座標系の作成
void delay(double *A, int n, int tau, int m){
  int i, j;
  for(i=0;i<n-tau*(m-1);i++){
      for(j=0;j<m;j++){
        fprintf(stdout, "%g ",A[i+j*tau] );
      }
      fprintf(stdout,"\n");
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
void read_data(double *data){
  int i=0;
  char line[num];
  while(fgets(line,num,stdin)!=NULL){
    if(line[0]!='#'){
      sscanf(line,"%lf", &data[i]);
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

//オプション
void option(int argc, char *argv[], int *tau, int *m){
  int i;
  /*コマンドライン引数、オプションの設定*/
  for(i=1;i<argc;i++){
    if(strcmp(argv[i], "-tau")==0){ *tau=atoi(argv[i+1]); }//遅延時間
    if(strcmp(argv[i], "-m")==0){ *m=atoi(argv[i+1]); }//埋め込み次元数
  }
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
void output_comment(int argc, char *argv[], int n, int l, int tau, int m, char **comment){
  int i;
  //元々入力されていたコメントアウト文の出力
  for(i=0;i<l;i++){
    fprintf(stdout,"%s",comment[i]);
  }
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# reconstruct.c (時系列データから再構成を行うプログラム)\n");
  fprintf(stdout, "# ./reconstruct ");
  for(i=1;i<argc;i++){
    fprintf(stdout, "%s ", argv[i]);
  }
  fprintf(stdout, "< --.dat\n");
  fprintf(stdout, "# tau= %d , m= %d\n", tau, m);
  fprintf(stdout, "# n(データ数)= %d\n", (int)(n-tau*(m-1)) );
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
