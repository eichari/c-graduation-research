/*目的: sin関数の値を time * dt 個生成*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-10-13*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* 文字列関数を扱えるようにする */
#include <math.h>
#include <unistd.h>
#include <time.h>

void option(int argc, char *argv[], long double *time, long double *dt, long double *A, long double *p, long double *q, long double *adjust, int *ch);
void sin_make(long double time, long double dt, long double A, long double p, long double q, long double adjust, int ch);
void output_comment(int argc, char *argv[], long double time, long double dt, long double A, long double p, long double q, long double adjust);
void today(void);

int main(int argc, char *argv[]){

  long double time=100, dt=0.1;
  //  y=Asin( 2Πt/p+q )+adjust //pは周期 
  long double A=1;//振幅
  long double p=1, q=0;//周期 位相
  long double adjust=0;// y軸調整
  int ch=0;// 出力
  
  //デフォルトは　y=sin(t) を 100個生成
  option(argc, argv, &time, &dt, &A, &p, &q, &adjust, &ch);
  output_comment(argc, argv, time, dt, A, p, q, adjust);
  sin_make(time, dt, A, p, q, adjust, ch);
  
}


//オプション
void option(int argc, char *argv[], long double *time, long double *dt, long double *A, long double *p, long double *q, long double *adjust, int *ch){
  int i;
  //  y=Asin( 2Πt/p+q )+adjust //pは周期 
  for(i=1;i<argc;i++){
    if(strcmp(argv[i], "-time")==0){ *time=atof(argv[i+1]); }
    if(strcmp(argv[i], "-dt")==0){ *dt=atof(argv[i+1]); }
    if(strcmp(argv[i], "-A")==0){ *A=atof(argv[i+1]); }
    if(strcmp(argv[i], "-p")==0){ *p=atof(argv[i+1]); }
    if(strcmp(argv[i], "-q")==0){ *q=atof(argv[i+1]); }
    if(strcmp(argv[i], "-adjust")==0){ *adjust=atof(argv[i+1]); }
    if(strcmp(argv[i], "-ch")==0){ *ch=atoi(argv[i+1]); }
  }
}

//sin関数の値を生成
void sin_make(long double time, long double dt, long double A, long double p, long double q, long double adjust, int ch){
  long double t=0;
  long double y;
  //  y=Asin( 2Πt/p+q )+adjust
  for(t=0;t<=time;t=t+dt){
    y=A*sin(2*M_PI*t/p+q)+adjust;
    if(ch==0){
      fprintf(stdout, "%Lf \n", y);
    }else if(ch==1){
      fprintf(stdout, "%Lf %Lf \n", t, y);
    }
  }
}


//コメントアウト文の出力
void output_comment(int argc, char *argv[], long double time, long double dt, long double A, long double p, long double q, long double adjust){
  int i;
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# sin.c (sin関数の値を生成するプログラム)\n");
  fprintf(stdout, "# ./sin ");
  for(i=1;i<argc;i++){
    fprintf(stdout, "%s ", argv[i]);
  }
  fprintf(stdout, "\n");
  //  y=Asin( 2Πt/p+q )+adjust 
  fprintf(stdout, "# time= %Lf , dt= %Lf , y= %Lf sin( 2Πt/%Lf + %Lf ) + %Lf \n", time, dt, A, p, q, adjust);
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
