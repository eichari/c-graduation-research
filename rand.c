/*目的: sin関数の値を time * dt 個生成*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-10-13*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* 文字列関数を扱えるようにする */
#include <math.h>
#include <unistd.h>
#include <time.h>
#include "MT.h"


void option(int argc, char *argv[], long double *time, long double *dt, long double *mu, long double *sigma, int *ch);
void rand_make(long double time, long double dt, long double mu, long double sigma, int ch);
void output_comment(int argc, char *argv[], long double time, long double dt, long double mu, long double sigma);
void today(void);

double Uniform(void);//一様乱数を返す
double rand_normal(double mu, double sigma);//(mu,sigma)の正規分布を返す


int main(int argc, char *argv[]){

  long double time=100, dt=1;
  long double mu=0, sigma=1; // 正規分布の平均と分散
  int ch=0;// 出力 デフォルトは一様乱数
  
  //デフォルトは　一様乱数 を 100個生成
  option(argc, argv, &time, &dt, &mu, &sigma, &ch);
  output_comment(argc, argv, time, dt, mu, sigma);
  rand_make(time, dt, mu, sigma, ch);
  
}


//オプション
void option(int argc, char *argv[], long double *time, long double *dt, long double *mu, long double *sigma, int *ch){
  int i;
  //  y=Asin( 2Πt/p+q )+adjust //pは周期 
  for(i=1;i<argc;i++){
    if(strcmp(argv[i], "-time")==0){ *time=atof(argv[i+1]); }
    if(strcmp(argv[i], "-dt")==0){ *dt=atof(argv[i+1]); }
    if(strcmp(argv[i], "-mu")==0){ *mu=atof(argv[i+1]); }
    if(strcmp(argv[i], "-sigma")==0){ *sigma=atof(argv[i+1]); }
    if(strcmp(argv[i], "-ch")==0){ *ch=atoi(argv[i+1]); }
  }
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


//乱数の値を生成
void rand_make(long double time, long double dt, long double mu, long double sigma, int ch){
  long double t=0;
  long double y;

  //一様乱数列を生成
  if(ch==0){
    for(t=0;t<=time;t=t+dt){
      y=Uniform();
      fprintf(stdout, "%Lf \n", y);      
      //fprintf(stdout, "%Lf %Lf \n", t, y);
    }
  }

  //正規分布列を生成
  if(ch==1){
    for(t=0;t<=time;t=t+dt){
      y=rand_normal(mu, sigma);
      fprintf(stdout, "%Lf %Lf \n", t, y);
    }
  }
}


//コメントアウト文の出力
void output_comment(int argc, char *argv[], long double time, long double dt, long double mu, long double sigma){
  int i;
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# rand.c (乱数の値を生成するプログラム)\n");
  fprintf(stdout, "# ./rand ");
  for(i=1;i<argc;i++){
    fprintf(stdout, "%s ", argv[i]);
  }
  fprintf(stdout, "\n");
  fprintf(stdout, "# time= %Lf , dt= %Lf , mu= %Lf , sigma= %Lf \n", time, dt, mu, sigma);
  today();
}

//日付の出力
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
