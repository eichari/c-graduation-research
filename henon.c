/*目的: henon写像を解くプログラム*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-10-21*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* 文字列関数を扱えるようにする */
#include <math.h>
#include <unistd.h>
#include <time.h>

void option(int argc, char *argv[], int *time, long double *x0, long double *y0, long double *a, long double *b, int *skip, int *ch);
void henon(int time, long double x0, long double y0, long double a, long double b, int skip, int ch);
void output_comment(int argc, char *argv[], int time, long double x0, long double y0, long double a, long double b, int skip);
void today(void);

int main(int argc, char *argv[]){

  // x(n+1) = 1 - ax(n)^2 + y(n)
  // y(n+1) = bx(n) 
  int time=100;
  long double a=1.4, b=0.3;//パラメータ
  long double x0=1.2, y0=0.8;//周期 位相
  int skip=0;
  int ch=0;// 出力
  
  //デフォルトは　y=sin(t) を 100個生成
  option(argc, argv, &time, &x0, &y0, &a, &b, &skip, &ch);
  output_comment(argc, argv, time, x0, y0, a, b, skip);
  henon(time, x0, y0, a, b, skip, ch);
  
}


//オプション
void option(int argc, char *argv[], int *time, long double *x0, long double *y0, long double *a, long double *b, int *skip, int *ch){
  int i;
  for(i=1;i<argc;i++){
    if(strcmp(argv[i], "-time")==0){ *time=atoi(argv[i+1]); }
    if(strcmp(argv[i], "-x0")==0){ *x0=atof(argv[i+1]); }
    if(strcmp(argv[i], "-y0")==0){ *y0=atof(argv[i+1]); }
    if(strcmp(argv[i], "-a")==0){ *a=atof(argv[i+1]); }
    if(strcmp(argv[i], "-b")==0){ *b=atof(argv[i+1]); }
    if(strcmp(argv[i], "-skip")==0){ *skip=atoi(argv[i+1]); }
    if(strcmp(argv[i], "-ch")==0){ *ch=atoi(argv[i+1]); }
  }
}

//henon写像を解く
void henon(int time, long double x0, long double y0, long double a, long double b, int skip, int ch){
  int i;
  long double x, y;
  long double xn, yn;

  // x(n+1) = 1 - ax(n)^2 + y(n)
  // y(n+1) = bx(n) 
  x=x0;
  y=y0;
  for(i=0;i<=(time+skip);i++){
    if(i>=skip){
      if(ch==0){
        fprintf(stdout, "%Lf \n", y);
      }else if(ch==1){
        fprintf(stdout, "%d %Lf \n", i, y);
      }else if(ch==2){
        fprintf(stdout, "%d %Lf \n", i, x);
      }else if(ch==3){
        fprintf(stdout, "%Lf %Lf \n", x, y);
      }
    }
    xn=1-a*x*x+b*y;
    yn=x;
    x=xn;
    y=yn;
  }
}


//コメントアウト文の出力
void output_comment(int argc, char *argv[], int time, long double x0, long double y0, long double a, long double b, int skip){
  int i;
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# henon.c (henon写像を解くプログラム)\n");
  fprintf(stdout, "# ./henon ");
  for(i=1;i<argc;i++){
    fprintf(stdout, "%s ", argv[i]);
  }
  fprintf(stdout, "\n");
  //  y=Asin( 2Πt/p+q )+adjust 
  fprintf(stdout, "# time= %d , x0= %Lf , y0= %Lf , a= %Lf , b= %Lf , skip= %d \n", time, x0, y0, a, b, skip);
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
