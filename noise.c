/*目的: 時系列データに白色ノイズを加えるプログラム*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-09-15*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* 文字列関数を扱えるようにする */
#include <math.h>
#include <time.h>
#include "MT.h"

#define num 1024

void whitenoise(double *A, int n, double p);//入力されたデータに白色ノイズを加える
double max(double *A,int n);//埼大値を求める
double min(double *A,int n);//最小値を求める
double average(double *A, int n);//平均値を求める
double variance(double *A, int n);//分散を求める

double Uniform(void);//一様乱数を返す
double rand_normal(double mu, double sigma);//(mu,sigma)の正規分布を返す

void option(int argc, char *argv[], double *p, double *seed);//オプション
void count_data(int *n, int *l);//データ数を数える
void read_comment(char **comment);//コメント文の読み込み
void read_data(double *data);//データの読み込み
void *e_calloc(int n, int size, char* p);//メモリ確保
void **matrix_calloc(int n, int m, char* p);//2次元配列のメモリ確保
void output_comment(int n, int l, char **comment, double p, double seed);//コメント文出力
void today(void);

int main(int argc, char *argv[]){
  //入力データ
  int i;
  int n, l;//データ数 //コメント文の行数
  double *A=NULL;
  char *str=NULL;//1行ずつ読みこむ
  char **comment=NULL;//コメント文を保存する
  double p=0;//ノイズ量を決める
  double seed=0;

  option(argc,argv,&p,&seed);
  init_genrand(seed);//乱数の初期化　シード値入れる
  count_data(&n, &l);  //データ数のカウント //コメント文の行数のカウント
  A=(double*)e_calloc(n,sizeof(double),"A");//メモリ確保
  str=(char*)e_calloc(num,sizeof(char),"str");//メモリ確保
  comment=(char **)matrix_calloc(l,num,"comment");
  read_comment(comment);//コメントアウト文の読み込み
  read_data(A);//データの読み込み
  output_comment(n, l, comment, p, seed);

  //for(i=0;i<n;i++){fprintf(stdout, "%lf \n",A[i] );}
  whitenoise(A,n,p);
  for(i=0;i<n;i++){fprintf(stdout, "%lf \n",A[i] );}

  free(A);//メモリ解放
  free(str);
  for(i=0;i<l;i++){free(comment[i]);}
  free(comment);

}

//入力されたデータにノイズを加える
void whitenoise(double *A, int n, double p){
  int i;
  double var_A;//入力されたデータの分散
  double mu_noise=0;
  double var_noise, sigma_noise;
  double noise;
  double tmp;

  //まず，ノイズの分散を求める
  // p = 10*log_10 ( sigma_A^2 / sigma_noise^2 );
  var_A=variance(A,n);
  //fprintf(stdout, "var_A= %g , p= %g \n", var_A, p );
  var_noise=pow( 10, (log10(var_A)-p/10) );
  //fprintf(stdout, "計算　var_noise= %g \n", var_noise );
  //tmp=10*log10( var_A/var_noise );
  //fprintf(stdout, "検算　var_noise= %g , p= %lf \n", var_noise, tmp );

  //ノイズを加える
  sigma_noise=sqrt(var_noise);
  for(i=0;i<n;i++){
    noise=rand_normal(mu_noise,sigma_noise);
    A[i]=A[i]+noise;
    //fprintf(stdout, "%g %g \n", A[i], noise );
  }
}

//標準化を行う
void normalize(double *A, int n){
  int i;
  double minA=min(A,n);
  double maxA=max(A,n);
  for(i=0;i<n;i++){
    A[i]=(A[i]-minA)/(maxA-minA);
  }
}

//最大値を求める
double max(double *A,int n){//埼大値を求める
  int i;
  double ans=0;
  for(i=0;i<n;i++){
    if(ans<=A[i]){ans=A[i];}
  }
  return ans;
}
//最小値を求める
double min(double *A,int n){//最小値を求める
  int i;
  double ans=1000000;
  for(i=0;i<n;i++){
    if(ans>=A[i]){ans=A[i];}
  }
  return ans;
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

//一様乱数を返す
double Uniform(void){
  return genrand_real3();
}

//(mu,sigma)の正規分布を返す
double rand_normal(double mu, double sigma){
  double z = sqrt( -2.0*log(Uniform()) )*sin( 2.0*M_PI*Uniform() );
  return mu + sigma*z;
}

//オプション
void option(int argc, char *argv[], double *p, double *seed){
  int i;
  for(i=1;i<argc;i++){
    if(strcmp(argv[i], "-p")==0){ *p=atof(argv[i+1]); }
    if(strcmp(argv[i], "-seed")==0){ *seed=atof(argv[i+1]); }
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
void output_comment(int n, int l, char **comment, double p, double seed){
  int i;
  //元々入力されていたコメントアウト文の出力
  for(i=0;i<l;i++){
    fprintf(stdout,"%s",comment[i]);
  }
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# noise.c (ノイズを加えるプログラム)\n");
  fprintf(stdout, "# ./noise < --.dat\n");
  fprintf(stdout, "# n= %d , p= %g , seed= %g \n", n, p, seed);
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
