/*目的: Izhikevichニューロンモデルの微分方程式をルンゲクッタ法で解くプログラム*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-09-02*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define num 1024

//Izhikevichニューロンモデル
double f(double v, double u, double I_static);
double g(double v, double u, double a, double b);

//入力信号が定数の場合
void runge_kutta(double transient, double time, double dt, double I_static, double Theta, double a, double b, double c, double d, double u0, double v0, double *kf, double *kg, int uv, int *spike_count);
//入力信号が時間変化する場合
void runge_kutta_2(double transient, double time, double dt, double Theta, double a, double b, double c, double d, double *I, double u0, double v0, double *kf, double *kg, int uv, int *spike_count);

void option(int argc, char *argv[], double *transient, double *time, double *dt, double *I_static, double *Theta, double *a, double *b, double *c, double *d, double *u0, double *v0, int *uv);
void count_data(int *n, int *l);//データ数を数える
void read_comment(char **comment);//コメント文の読み込み
void read_data(double *data);//データの読み込み
void *e_calloc(int n, int size, char* p);//メモリ確保
void **matrix_calloc(int n, int m, char* p);//2次元配列のメモリ確保
void output_comment(int argc, char *argv[], double transient, double time, double dt, double I_static, double Theta, double a, double b, double c, double d, double u0, double v0, int uv, int spike_count);
void today(void);//日付の表示

int main(int argc, char *argv[]){
  int j;
  int uv=-1;
  int spike_count=0;

  //入力データ
  int n=0, l=0;//データ数 //コメント文の行数
  double *I=NULL;
  char *str=NULL;//1行ずつ読みこむ
  char **comment=NULL;//コメント文を保存する

  //Izhikevichニューロンモデルのパラメータ
  double I_static=-1;
  double a,b,c,d;
  double Theta=30;

  //ルンゲクッタ法のパラメータ
  double v0 = -65, u0 = -10;
  double *kf, *kg;
  double dt = 0.01, time = 100, transient = 0;//過渡状態を省く //100/0.01=1万プロット

  option(argc, argv, &transient, &time, &dt,&I_static, &Theta, &a, &b, &c, &d, &u0, &v0, &uv);//オプション
  if(I_static>0){//入力信号が定数の場合
    kf=(double*)e_calloc(4,sizeof(double),"kf");
    kg=(double*)e_calloc(4,sizeof(double),"kg");

    output_comment(argc, argv, transient, time, dt, I_static, Theta, a, b, c, d, u0, v0, uv, spike_count);//コメント文の出力

    runge_kutta(transient, time, dt, I_static, Theta, a, b, c, d, u0, v0, kf, kg, uv, &spike_count);
    free(kf);
    free(kg);
  }else{
    count_data(&n, &l);  //データ数のカウント //コメント文の行数のカウント
    I=(double*)e_calloc(n,sizeof(double),"I");
    str=(char*)e_calloc(num,sizeof(char),"str");//メモリ確保
    comment=(char **)matrix_calloc(l,num,"comment");
    kf=(double*)e_calloc(4,sizeof(double),"kf");
    kg=(double*)e_calloc(4,sizeof(double),"kg");
    read_comment(comment);//コメントアウト文の読み込み
    read_data(I);//データの読み込み
    output_comment(argc, argv, transient, time, dt, I_static, Theta, a, b, c, d, u0, v0, uv, spike_count);//コメント文の出力

    runge_kutta_2(transient, time, dt, Theta, a, b, c, d, I, u0, v0, kf, kg, uv, &spike_count);
  
    free(I);  //メモリ解放
    free(str);
    free(comment);
    free(kf);
    free(kg);
  }

  return 0;
}

//Izhikevichニューロンモデル
double f(double v, double u, double I_static){
	return 0.04*v*v+5*v+140-u+I_static;
}
double g(double v, double u, double a, double b){
	return a*(b*v-u);
}

//オプション
void option(int argc, char *argv[], double *transient, double *time, double *dt, double *I_static, double *Theta, double *a, double *b, double *c, double *d, double *u0, double *v0, int *uv){
  int i;
  for(i=1;i<argc;i++){
    if(strcmp(argv[i], "-transient")==0){ *transient=atof(argv[i+1]); }
    if(strcmp(argv[i], "-time")==0){ *time=atof(argv[i+1]); }
    if(strcmp(argv[i], "-dt")==0){ *dt=atof(argv[i+1]); }
    if(strcmp(argv[i], "-I_static")==0){ *I_static=atof(argv[i+1]); }
    if(strcmp(argv[i], "-Theta")==0){ *Theta=atof(argv[i+1]); }

    if(strcmp(argv[i], "-a")==0){ *a=atof(argv[i+1]); }
    if(strcmp(argv[i], "-b")==0){ *b=atof(argv[i+1]); }
    if(strcmp(argv[i], "-c")==0){ *c=atof(argv[i+1]); }
    if(strcmp(argv[i], "-d")==0){ *d=atof(argv[i+1]); }

    if(strcmp(argv[i], "-u0")==0){ *u0=atof(argv[i+1]); }
    if(strcmp(argv[i], "-v0")==0){ *v0=atof(argv[i+1]); }
    if(strcmp(argv[i], "-uv")==0){ *uv=atoi(argv[i+1]); }

    if(strcmp(argv[i], "-RS")==0){ *a=0.02; *b=0.2; *c=-65; *d=8; }//RS
    if(strcmp(argv[i], "-FS")==0){ *a=0.1; *b=0.2; *c=-65; *d=2; }//FS
    if(strcmp(argv[i], "-IB")==0){ *a=0.02; *b=0.2; *c=-55; *d=4; }//IB
    if(strcmp(argv[i], "-CH")==0){ *a=0.02; *b=0.2; *c=-50; *d=2; }//CH
    if(strcmp(argv[i], "-Chaos")==0){ *a=0.0511; *b=0.2; *c=-55; *d=2; }//Chaos
  }
}

//ルンゲクッタ法
void runge_kutta(double transient, double time, double dt, double I_static, double Theta, double a, double b, double c, double d, double u0, double v0, double *kf, double *kg, int uv, int *spike_count){
    double t;
    double un, vn;

    for(t = 0 ; t <= (transient+time); t=t+dt){
      //k1
      kf[0] = dt * f(v0, u0, I_static);
      kg[0] = dt * g(v0, u0, a, b);
      //k2
      kf[1] = dt * f(v0+kf[0]/2.0, u0+kg[0]/2.0, I_static);
      kg[1] = dt * g(v0+kf[0]/2.0, u0+kg[0]/2.0, a, b);
      //k3
      kf[2] = dt * f(v0+kf[1]/2.0, u0+kg[1]/2.0, I_static);
      kg[2] = dt * g(v0+kf[1]/2.0, u0+kg[1]/2.0, a, b);
      //k4
      kf[3] = dt * f(v0+kf[2], u0+kg[2], I_static);
      kg[3] = dt * g(v0+kf[2], u0+kg[2], a, b);

      vn = v0 + (kf[0] + 2.0*(kf[1]+kf[2]) + kf[3])/6.0;
      un = u0 + (kg[0] + 2.0*(kg[1]+kg[2]) + kg[3])/6.0;

      //閾値 \Theta を超えたらリセットする
      if(vn>=Theta){
        vn=c;
        un=un+d;
        if(uv==1){
          fprintf(stdout, "%lf \n", t-transient);
          spike_count++;
        }
      }

      if(t>transient){//過渡状態省く
        if(uv==2){
          fprintf(stdout, "%lf \n", un);
        }else if(uv==3){
          fprintf(stdout, "%lf \n", vn);
        }else if(uv==4){
          fprintf(stdout, "%lf %lf \n", t-transient, vn);
        }else{
          fprintf(stdout, "%lf %lf %lf \n",t-transient, un, vn);
        }
      }

      v0 = vn;
      u0 = un;
    }
}

//ルンゲクッタ法
void runge_kutta_2(double transient, double time, double dt, double Theta, double a, double b, double c, double d, double *I, double u0, double v0, double *kf, double *kg, int uv, int *spike_count){
    double t;
    double un, vn;
    double td=1/dt;
    int i;
    //transient=0;
    spike_count=0;

    for(t = 0 ; t <= (transient+time) ; t=t+dt){
      i=(int)(t*td);

      //k1
      kf[0] = dt * f(v0, u0, I[i]);
      kg[0] = dt * g(v0, u0, a, b);
      //k2
      kf[1] = dt * f(v0+kf[0]/2.0, u0+kg[0]/2.0, I[i]);
      kg[1] = dt * g(v0+kf[0]/2.0, u0+kg[0]/2.0, a, b);
      //k3
      kf[2] = dt * f(v0+kf[1]/2.0, u0+kg[1]/2.0, I[i]);
      kg[2] = dt * g(v0+kf[1]/2.0, u0+kg[1]/2.0, a, b);
      //k4
      kf[3] = dt * f(v0+kf[2], u0+kg[2], I[i]);
      kg[3] = dt * g(v0+kf[2], u0+kg[2], a, b);

      vn = v0 + (kf[0] + 2.0*(kf[1]+kf[2]) + kf[3])/6.0;
      un = u0 + (kg[0] + 2.0*(kg[1]+kg[2]) + kg[3])/6.0;

      //閾値 \Theta を超えたらリセットする
      if(vn>=Theta){
        vn=c;
        un=un+d;
        if(uv==1){
          fprintf(stdout, "%lf \n", t-transient);
          spike_count++;
        }else if(uv==2){
          fprintf(stdout, "%lf %lf \n", t-transient, vn);
        }
        
        if(t>transient){//
          if(uv==-1){
            fprintf(stdout, "%lf \n", t-transient);
          }
          if(uv==-2){
            fprintf(stdout, "%lf %lf \n", t-transient, vn);
          }
        }
        
      }

      if(t>transient){//過渡状態省く
        if(uv==3){
          fprintf(stdout, "%lf \n", un);
        }else if(uv==4){
          fprintf(stdout, "%lf \n", vn);
        }else if(uv==5){
          fprintf(stdout, "%lf %lf \n", t-transient, vn);
        }else if(uv==6){
          fprintf(stdout, "%lf %lf %lf \n",t-transient, un, vn);
        }
      }

      v0 = vn;
      u0 = un;
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

    if(data==NULL){
        fprintf(stderr,"Calloc failed: %s.\n",p);
        exit(1);
    }
    return (void **)data;
}

//コメントアウト文の出力
void output_comment(int argc, char *argv[], double transient, double time, double dt, double I_static, double Theta, double a, double b, double c, double d, double u0, double v0, int uv, int spike_count){
  int i;
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# izh.c (Izhikevichニューロンモデル)\n");
  fprintf(stdout, "# ./izh ");
  for(i=1;i<argc;i++){
    fprintf(stdout, "%s ", argv[i]);
  }
  fprintf(stdout, "\n");
  if(I_static>0){
    fprintf(stdout, "# Static Input Signals.\n");
    fprintf(stdout, "# I_static= %g\n", I_static);
  }else{
    fprintf(stdout, "# Dynamical Input Signals.\n");
  }
  fprintf(stdout, "# time= %g (時系列長), dt= %g (刻み幅), transient= %g (過渡状態)\n",time, dt, transient);
  fprintf(stdout, "# a= %g , b= %g , c= %g , d= %g\n", a, b, c, d);
  fprintf(stdout, "# u0= %g , v0= %g , Theta= %g \n", u0, v0, Theta);
  fprintf(stdout, "# n(データ数)= %d\n", spike_count );
  today();
}

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