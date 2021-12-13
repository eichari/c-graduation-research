/*目的: 時系列データのサンプリングを行うプログラム*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-12-14*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* 文字列関数を扱えるようにする */
#include <math.h>
#include <time.h>

#define num 5000000

void sampling(long double **A, int n, int m, int sample);//サンプリングを行う

void option(int argc, char *argv[], int *sample);
void read_comment(char **comment);//コメント文の読み込み
void print_data(long double **data, int n, int m);//行列の出力　動作確認用
void count_data(int *n, int *l, int *m);//データ数を数える
void read_data(long double **data, int m);//2次元データ，n行m列の読み込み

void *e_calloc(int n, int size, char* p);//メモリ確保
void **matrix_calloc(int n, int m, char* p);//2次元配列のメモリ確保
void output_comment(int argc, char *argv[], int n, int l, int sample, char **comment);//コメント文出力
void today(void);

int main(int argc, char *argv[]){
  //入力データ
  int i;
  int n, l, m;//データ数 //コメント文の行数
  long double **A=NULL;
  char **comment=NULL;//コメント文を保存する
  int sample=0;

  option(argc, argv, &sample);

  count_data(&n, &l, &m);  //データ数のカウント //コメント文の行数のカウント
  //fprintf(stdout, "n= %d , l= %d , m= %d \n", n, l, m );
  A=(long double **)matrix_calloc(n,m,"A");
  comment=(char **)matrix_calloc(l,num,"comment");
  read_comment(comment);//コメントアウト文の読み込み
  read_data(A,m);//データの読み込み
  output_comment(argc, argv, n, l, sample, comment);
  
  //print_data(A,n,m);
  //fprintf(stdout, "\n");
  sampling(A,n,m,sample);

  for(i=0;i<n;i++){free(A[i]);}
  free(A);//メモリ解放
  for(i=0;i<l;i++){free(comment[i]);}
  free(comment);

}

//オプション
void option(int argc, char *argv[], int *sample){
  int i;
  for(i=1;i<argc;i++){
    if(strcmp(argv[i], "-sample")==0){ *sample=atoi(argv[i+1]); }
  }
}

//サンプリングを行う
void sampling(long double **A, int n, int m, int sample){
  int i,j;
  for(i=0;i<n;i++){
    if((i%sample)==0){
      for(j=0;j<m;j++){
        fprintf(stdout, "%Lf ", A[i][j]);      
      }
      fprintf(stdout, "\n");      
    }
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
      d--;//データの下に行がある場合は必要　データファイルは最後に空白の一行が追加されるだいたい
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
void read_data(long double **data, int m){
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

void print_data(long double **data, int n, int m){
  int i,j;
  for(i=0;i<n;i++){
    for(j=0;j<m;j++){
      fprintf(stdout, "%Lf ", data[i][j]);
    }
    fprintf(stdout, "\n");
  }
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
    long double **data;

    data=(long double**)calloc(n,sizeof(long double*));
    for(i=0;i<n;i++){
      data[i]=(long double*)calloc(m,sizeof(long double));
    }

    if(data==NULL)
    {
        fprintf(stderr,"Calloc failed: %s.\n",p);
        exit(1);
    }
    return (void **)data;
}

//コメントアウト文の出力
void output_comment(int argc, char *argv[], int n, int l, int sample, char **comment){
  int i;
  //元々入力されていたコメントアウト文の出力
  for(i=0;i<l;i++){
    fprintf(stdout,"%s",comment[i]);
  }
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# sampling.c (サンプリングを行うプログラム)\n");
  fprintf(stdout, "# ./sampling ");
  for(i=1;i<argc;i++){
    fprintf(stdout, "%s ", argv[i]);
  }
  fprintf(stdout, "< --.dat\n");
  fprintf(stdout, "# sample= %d \n", sample);
  fprintf(stdout, "# n= %d\n", n);
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
