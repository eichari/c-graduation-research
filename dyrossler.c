//ダイナミカルノイズがある場合のrossler方程式をルンゲクッタ法で解くプログラム
//ノイズは wiener過程
/*作成者: Ei Miura*/
/*最終更新日: 2020-11-05*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "MT.h"


long double f(long double y, long double z);
long double g(long double x, long double y, long double a);
long double h(long double x, long double z, long double b, long double c);
void runge_kutta(long double transient, long double time, long double dt, long double a, long double b, long double c, long double x0, long double y0, long double z0, int xyz, long double adjust, int sampling, long double epsilon);//ルンゲクッタ法

double Uniform(void);//一様乱数を返す
double rand_normal(double mu, double sigma);//(mu,sigma)の正規分布を返す

void option(int argc, char *argv[], long double *transient, long double *time, long double *dt, long double *a, long double *b, long double *c, long double *x0, long double *y0, long double *z0, int *xyz, long double *adjust, int *sampling, long double *epsilon, int *seed);//オプション
void output_comment(int argc, char *argv[], long double transient, long double time, long double dt, long double a, long double b, long double c, long double x0, long double y0, long double z0, int xyz, long double adjust, int sampling, long double epsilon, int seed);//コメント文出力
void *e_calloc(int n, int size, char* p);//メモリ確保
void today(void);//日付を表示

int main(int argc, char *argv[]){
  int xyz=-2;//オプションで変数の出力を指定 x=0, y=1, z=2
  long double adjust=0;//LIFにいれるときなどの調整項
  int sampling=1;//sampling回に1回出力

  //rossler方程式のパラメータ
  long double a = 0.36;
  long double b = 0.4;
  long double c = 4.5;

  //ルンゲクッタ法のパラメータ
  long double x0 = 1.0, y0 = 0, z0 = 0;
  long double dt = 0.01, time = 1000, transient = 1000;//過渡状態を省く //100/0.01=1万プロット

  //ノイズのパラメータ
  long double epsilon=0;
  int seed=0;

  option(argc, argv, &transient, &time, &dt, &a, &b, &c, &x0, &y0, &z0, &xyz, &adjust, &sampling, &epsilon, &seed);//オプション
  init_genrand(seed);//乱数の初期化　シード値入れる
  output_comment(argc, argv, transient, time, dt, a, b, c, x0, y0, z0, xyz, adjust, sampling, epsilon, seed);//コメント文の出力
  runge_kutta(transient, time, dt, a, b, c, x0, y0, z0, xyz, adjust, sampling, epsilon);//ルンゲクッタ法

  return 0;
}


//rossler方程式
long double f(long double y, long double z){
	return (-y-z);
}
long double g(long double x, long double y, long double a){
	return (x+a*y);
}
long double h(long double x, long double z, long double b, long double c){
	return (b+z*(x-c));
}

//ルンゲクッタ法
void runge_kutta(long double transient, long double time, long double dt, long double a, long double b, long double c, long double x0, long double y0, long double z0, int xyz, long double adjust, int sampling, long double epsilon){
    int i=1;  
    long double t=0;;
    long double xn, yn, zn;
    long double wx=0, wy=0, wz=0; //標準wiener過程  //Dynamical Noise
    long double *kf, *kg, *kh;
    
    kf = (long double*)e_calloc(4,sizeof(long double),"kf");//メモリ確保
    kg = (long double*)e_calloc(4,sizeof(long double),"kg");//メモリ確保
    kh = (long double*)e_calloc(4,sizeof(long double),"kh");//メモリ確保

    for(t = 0 ; t <= (transient+time) ; t=t+dt){
      //wiener
      wx = epsilon * rand_normal(0.0,1.0) * sqrtl(dt);
      wy = epsilon * rand_normal(0.0,1.0) * sqrtl(dt);
      wz = epsilon * rand_normal(0.0,1.0) * sqrtl(dt);
      //k1
      kf[0] = dt * f(y0, z0);
      kg[0] = dt * g(x0, y0, a);
      kh[0] = dt * h(x0, z0, b, c);
      //k2
      kf[1] = dt * f(y0+kg[0]/2.0, z0+kh[0]/2.0);
      kg[1] = dt * g(x0+kf[0]/2.0, y0+kg[0]/2.0, a);
      kh[1] = dt * h(x0+kf[0]/2.0, z0+kh[0]/2.0, b, c);
      //k3
      kf[2] = dt * f(y0+kg[1]/2.0, z0+kh[1]/2.0);
      kg[2] = dt * g(x0+kf[1]/2.0, y0+kg[1]/2.0, a);
      kh[2] = dt * h(x0+kf[1]/2.0, z0+kh[1]/2.0, b, c);
      //k4
      kf[3] = dt * f(y0+kg[2], z0+kh[2]);
      kg[3] = dt * g(x0+kf[2], y0+kg[2], a);
      kh[3] = dt * h(x0+kf[2], z0+kh[2], b, c);

      xn = x0 + (kf[0] + 2.0*(kf[1]+kf[2]) + kf[3])/6.0 + wx;
      yn = y0 + (kg[0] + 2.0*(kg[1]+kg[2]) + kg[3])/6.0 + wy;
      zn = z0 + (kh[0] + 2.0*(kh[1]+kh[2]) + kh[3])/6.0 + wz;

      if(t>=transient-dt){//過渡状態省く
        if(i==sampling){
          if(xyz==3){
            fprintf(stdout, "%Lf %Lf \n", t-transient, xn);
          }else if(xyz==0){
            fprintf(stdout, "%Lf \n", xn);
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
void option(int argc, char *argv[], long double *transient, long double *time, long double *dt, long double *a, long double *b, long double *c, long double *x0, long double *y0, long double *z0, int *xyz, long double *adjust, int *sampling, long double *epsilon, int *seed){
  int i;
  for(i=1;i<argc;i++){
    if(strcmp(argv[i], "-transient")==0){ *transient=atof(argv[i+1]); }
    if(strcmp(argv[i], "-time")==0){ *time=atof(argv[i+1]); }
    if(strcmp(argv[i], "-dt")==0){ *dt=atof(argv[i+1]); }
    
    if(strcmp(argv[i], "-a")==0){ *a=atof(argv[i+1]); }
    if(strcmp(argv[i], "-b")==0){ *b=atof(argv[i+1]); }
    if(strcmp(argv[i], "-c")==0){ *c=atof(argv[i+1]); }
    
    if(strcmp(argv[i], "-x0")==0){ *x0=atof(argv[i+1]); }
    if(strcmp(argv[i], "-y0")==0){ *y0=atof(argv[i+1]); }
    if(strcmp(argv[i], "-z0")==0){ *z0=atof(argv[i+1]); }
    
    if(strcmp(argv[i], "-xyz")==0){ *xyz=atoi(argv[i+1]); }
    if(strcmp(argv[i], "-adjust")==0){ *adjust=atof(argv[i+1]); }
    if(strcmp(argv[i], "-sampling")==0){ *sampling=atoi(argv[i+1]); }
    
    if(strcmp(argv[i], "-epsilon")==0){ *epsilon=atof(argv[i+1]); }
    if(strcmp(argv[i], "-seed")==0){ *seed=atoi(argv[i+1]); }
    
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
void output_comment(int argc, char *argv[], long double transient, long double time, long double dt, long double a, long double b, long double c, long double x0, long double y0, long double z0, int xyz, long double adjust, int sampling, long double epsilon, int seed){
  int i;
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# dyrossler.c (ダイナミカルノイズを持つレスラー方程式を解くプログラム)\n");
  fprintf(stdout, "# ./dyrossler ");
  for(i=1;i<argc;i++){
    fprintf(stdout, "%s ", argv[i]);
  }
  fprintf(stdout, "\n");
  fprintf(stdout, "# time= %Lg (時系列長), dt= %Lg (刻み幅), transient= %Lg (過渡状態)\n",time, dt, transient);
  fprintf(stdout, "# a= %Lg , b= %Lg , c= %Lg\n", a, b, c);
  fprintf(stdout, "# x0= %Lg , y0= %Lg , z0= %Lg , adjust= %Lg , sampling= %d \n", x0, y0, z0, adjust, sampling);
  fprintf(stdout, "# epsilon= %Lg , seed= %d \n", epsilon, seed);
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
