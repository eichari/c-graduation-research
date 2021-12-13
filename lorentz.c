//lorentz方程式をルンゲクッタ法で解くプログラム
/*作成者: Ei Miura*/
/*最終更新日: 2020-12-07*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

long double f(long double x, long double y, long double sigma);
long double g(long double x, long double y, long double z, long double r);
long double h(long double x, long double y, long double z, long double b);
void runge_kutta(long double transient, long double time, long double dt, long double sigma, long double r, long double b, long double x0, long double y0, long double z0, int xyz, long double adjust, int sampling);//ルンゲクッタ法
void option(int argc, char *argv[], long double *transient, long double *time, long double *dt, long double *sigma, long double *r, long double *b, long double *x0, long double *y0, long double *z0, int *xyz, long double *adjust, int *sampling);//オプション
void output_comment(int argc, char *argv[], long double transient, long double time, long double dt, long double sigma, long double r, long double b, long double x0, long double y0, long double z0, int xyz, long double adjust, int sampling);//コメント文出力
void *e_calloc(int n, int size, char* p);//メモリ確保
void today(void);//日付を表示

int main(int argc, char *argv[]){
  int xyz=-2;//オプションで変数の出力を指定 x=0, y=1, z=2
  long double adjust=0;//LIFにいれるときなどの調整項
  int sampling=1;//sampling回に1回出力

  //rossler方程式のパラメータ
  long double sigma = 10;
  long double r = 28;
  long double b = 8.0/3.0;

  //ルンゲクッタ法のパラメータ
  long double x0 = 1.0, y0 = 0, z0 = 0;
  long double dt = 0.01, time = 1000, transient = 1000;//過渡状態を省く //100/0.01=1万プロット

  option(argc, argv, &transient, &time, &dt, &sigma, &r, &b, &x0, &y0, &z0, &xyz, &adjust, &sampling);//オプション
  output_comment(argc, argv, transient, time, dt, sigma, r, b, x0, y0, z0, xyz, adjust, sampling);//コメント文の出力
  runge_kutta(transient, time, dt, sigma, r, b, x0, y0, z0, xyz, adjust, sampling);//ルンゲクッタ法

  return 0;
}


//lorentz方程式
long double f(long double x, long double y, long double sigma){
  return (sigma*(-x+y));
}
long double g(long double x, long double y, long double z, long double r){
  return (-x*z+r*x-y);
}
long double h(long double x, long double y, long double z, long double b){
  return (x*y-b*z);
}

//ルンゲクッタ法
void runge_kutta(long double transient, long double time, long double dt, long double sigma, long double r, long double b, long double x0, long double y0, long double z0, int xyz, long double adjust, int sampling){
    long double t=0;;
    long double xn, yn, zn;
    int i=1;
    long double *kf, *kg, *kh;
    kf=(long double*)e_calloc(4,sizeof(long double),"kf");//メモリ確保
    kg=(long double*)e_calloc(4,sizeof(long double),"kg");//メモリ確保
    kh=(long double*)e_calloc(4,sizeof(long double),"kh");//メモリ確保


    for(t = 0 ; t <= (transient+time) ; t=t+dt){
      //k1
      kf[0] = dt * f(x0, y0, sigma);
      kg[0] = dt * g(x0, y0, z0, r);
      kh[0] = dt * h(x0, y0, z0, b);
      //k2
      kf[1] = dt * f(x0+kf[0]/2.0, y0+kg[0]/2.0, sigma);
      kg[1] = dt * g(x0+kf[0]/2.0, y0+kg[0]/2.0, z0+kh[0]/2.0, r);
      kh[1] = dt * h(x0+kf[0]/2.0, y0+kg[0]/2.0, z0+kh[0]/2.0, b);
      //k3
      kf[2] = dt * f(x0+kf[1]/2.0, y0+kg[1]/2.0, sigma);
      kg[2] = dt * g(x0+kf[1]/2.0, y0+kg[1]/2.0, z0+kh[1]/2.0, r);
      kh[2] = dt * h(x0+kf[1]/2.0, y0+kg[1]/2.0, z0+kh[1]/2.0, b);
      //k4
      kf[3] = dt * f(x0+kf[2], y0+kg[2], sigma);
      kg[3] = dt * g(x0+kf[2], y0+kg[2], z0+kh[2], r);
      kh[3] = dt * h(x0+kf[2], y0+kg[2], z0+kh[2], b);

      xn = x0 + (kf[0] + 2.0*(kf[1]+kf[2]) + kf[3])/6.0;
      yn = y0 + (kg[0] + 2.0*(kg[1]+kg[2]) + kg[3])/6.0;
      zn = z0 + (kh[0] + 2.0*(kh[1]+kh[2]) + kh[3])/6.0;

      if(t>=transient-dt){//過渡状態省く
        if(i==sampling){
          if(xyz==3){
//            fprintf(stdout, "%Lf %Lf \n", t-transient, xn);
            fprintf(stdout, "%Lf %Lf \n", t-transient, (xn+2)*(xn+2));
          }else if(xyz==0){
            fprintf(stdout, "%Lf \n", xn);
//            fprintf(stdout, "%Lf \n", (xn+2)*(xn+2) );
          }else if(xyz==1){
            fprintf(stdout, "%Lf \n", yn);
          }else if(xyz==2){
            fprintf(stdout, "%Lf \n", zn);
          }else if(xyz==4){
            fprintf(stdout, "%Lf \n", xn + adjust);
          }else if(xyz==5){
            fprintf(stdout, "%Lf %Lf \n", xn + adjust, t-transient);
          }else if(xyz==6){
            fprintf(stdout, "%Lf %Lf %Lf \n", xn, yn, zn);
          }else{
            fprintf(stdout, "%Lf %Lf %Lf %Lf \n",t-transient, xn, yn, zn);
          }
          i=0;
        }
        i++;  
      }

      x0 = xn;
      y0 = yn;
      z0 = zn;
    }
    
    free(kf);
    free(kg);
    free(kh);
}


//オプション
void option(int argc, char *argv[], long double *transient, long double *time, long double *dt, long double *sigma, long double *r, long double *b, long double *x0, long double *y0, long double *z0, int *xyz, long double *adjust, int *sampling){
  int i;
  for(i=1;i<argc;i++){
    if(strcmp(argv[i], "-transient")==0){ *transient=atof(argv[i+1]); }
    if(strcmp(argv[i], "-time")==0){ *time=atof(argv[i+1]); }
    if(strcmp(argv[i], "-dt")==0){ *dt=atof(argv[i+1]); }
    
    if(strcmp(argv[i], "-sigma")==0){ *sigma=atof(argv[i+1]); }
    if(strcmp(argv[i], "-r")==0){ *r=atof(argv[i+1]); }
    if(strcmp(argv[i], "-b")==0){ *b=atof(argv[i+1]); }
    
    if(strcmp(argv[i], "-x0")==0){ *x0=atof(argv[i+1]); }
    if(strcmp(argv[i], "-y0")==0){ *y0=atof(argv[i+1]); }
    if(strcmp(argv[i], "-z0")==0){ *z0=atof(argv[i+1]); }
    
    if(strcmp(argv[i], "-xyz")==0){ *xyz=atoi(argv[i+1]); }
    if(strcmp(argv[i], "-adjust")==0){ *adjust=atof(argv[i+1]); }
    if(strcmp(argv[i], "-sampling")==0){ *sampling=atoi(argv[i+1]); }
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

//コメントアウト文の出力
void output_comment(int argc, char *argv[], long double transient, long double time, long double dt, long double sigma, long double r, long double b, long double x0, long double y0, long double z0, int xyz, long double adjust, int sampling){
  int i;
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# lorentz.c (ローレンツ方程式を解くプログラム)\n");
  fprintf(stdout, "# ./lorentz ");
  for(i=1;i<argc;i++){
    fprintf(stdout, "%s ", argv[i]);
  }
  fprintf(stdout, "\n");
  fprintf(stdout, "# time= %Lg (時系列長), dt= %Lg (刻み幅), transient= %Lg (過渡状態)\n",time, dt, transient);
  fprintf(stdout, "# sigma= %Lg , r= %Lg , b= %Lg\n", sigma, r, b);
  fprintf(stdout, "# x0= %Lg , y0= %Lg , z0= %Lg , adjust= %Lg , sampling= %d \n", x0, y0, z0, adjust, sampling);
  fprintf(stdout, "# n(データ数)= %d\n", (int)(time/dt) );
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
