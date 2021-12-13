/*目的: LIFモデルの微分方程式をルンゲクッタ法で解くプログラム*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-09-03*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define num 1024

double f(double v, double k, double input);
//Leaky Integrated-and-Fireモデル //入力信号が定数の場合
void runge_kutta(double transient, double time, double dt, double I_static, double Theta, double k, double v0, double vs, double *kf, int v, int *spike_count);
//入力信号が時間変化する場合
void runge_kutta_2(double transient, double time, double dt, double Theta, double k, double *I, double v0, double vs, double *kf, int v, int *spike_count);

void option(int argc, char *argv[], double *transient, double *time, double *dt, double *I_static, double *Theta, double *k, double *v0, double *vs, int *v);
void count_data(int *n, int *l);//データ数を数える
void read_comment(char **comment);//コメント文の読み込み
void read_data(double *data);//データの読み込み
void *e_calloc(int n, int size, char* p);//メモリ確保
void **matrix_calloc(int n, int m, char* p);//2次元配列のメモリ確保
void output_comment(int argc, char *argv[], double transient, double time, double dt, double I_static, double Theta, double k, double v0, double vs, int v, int l, char **comment, int spike_count);
void today(void);//日付の表示

int main(int argc, char *argv[]){
  int j;
  int v=-1;
  int spike_count=0;

  //入力データ
  int n=0, l=0;//データ数 //コメント文の行数
  double *I=NULL;
  char **comment=NULL;//コメント文を保存する

  //LIFモデルのパラメータ
  double I_static=-1;//入力信号が定数の場合
  double k=0;
  double Theta=20;

  //ルンゲクッタ法のパラメータ
  double v0 = 0, vs = 0;
  double *kf;
  double dt = 0.01, time = 2.5, transient = 0;//過渡状態を省く //100/0.01=1万プロット

  option(argc, argv, &transient, &time, &dt, &I_static, &Theta, &k, &v0, &vs, &v);//オプション

  if(I_static>0){//入力信号が定数の場合
    kf=(double*)e_calloc(4,sizeof(double),"kf");
    output_comment(argc, argv, transient, time, dt, I_static, Theta, k, v0, vs, v, l, comment, spike_count);//コメント文の出力
    today();
    runge_kutta(transient, time, dt, I_static, Theta, k, v0, vs, kf, v, &spike_count);
    free(kf);

  }else{//入力信号が時間変化する場合
    count_data(&n, &l);  //データ数のカウント //コメント文の行数のカウント
    I=(double*)e_calloc(n,sizeof(double),"I");//メモリ確保
    comment=(char **)matrix_calloc(l,num,"comment");
    kf=(double*)e_calloc(4,sizeof(double),"kf");
    read_comment(comment);//コメントアウト文の読み込み
    read_data(I);//データの読み込み
    output_comment(argc, argv, transient, time, dt, I_static, Theta, k, v0, vs, v, l, comment, spike_count);//コメント文の出力

    runge_kutta_2(transient, time, dt, Theta, k, I, v0, vs, kf, v, &spike_count);

    free(I);  //メモリ解放
    for(j=0;j<l;j++){free(comment[j]);}
    free(comment);
    free(kf);
  }


  return 0;
}

//LIFモデル //入力信号が定数の場合
double f(double v, double k, double input){
  return (input-k*v);
}

//オプション
void option(int argc, char *argv[], double *transient, double *time, double *dt, double *I_static, double *Theta, double *k, double *v0, double *vs, int *v){
  int i;
  for(i=1;i<argc;i++){
    if(strcmp(argv[i], "-transient")==0){ *transient=atof(argv[i+1]); }
    if(strcmp(argv[i], "-time")==0){ *time=atof(argv[i+1]); }
    if(strcmp(argv[i], "-dt")==0){ *dt=atof(argv[i+1]); }

    if(strcmp(argv[i], "-I_static")==0){ *I_static=atof(argv[i+1]); }
    if(strcmp(argv[i], "-Theta")==0){ *Theta=atof(argv[i+1]); }
    if(strcmp(argv[i], "-k")==0){ *k=atof(argv[i+1]); }

    if(strcmp(argv[i], "-v0")==0){ *v0=atof(argv[i+1]); }
    if(strcmp(argv[i], "-vs")==0){ *vs=atof(argv[i+1]); }
    if(strcmp(argv[i], "-v")==0){ *v=atoi(argv[i+1]); }
  }
}


//ルンゲクッタ法 //入力信号が定数の場合
void runge_kutta(double transient, double time, double dt, double I_static, double Theta, double k, double v0, double vs, double *kf, int v, int *spike_count){
    double t;
    double vn;
    transient=0;
    spike_count=0;

    for(t = 0 ; t <= (transient+time) ; t=t+dt){
      //k1
      kf[0] = dt * f(v0, k, I_static);
  		//k2
      kf[1] = dt * f(v0+kf[0]/2.0, k, I_static);
  		//k3
      kf[2] = dt * f(v0+kf[1]/2.0, k, I_static);
  		//k4
      kf[3] = dt * f(v0+kf[2], k, I_static);

      vn = v0 + (kf[0] + 2.0*(kf[1]+kf[2]) + kf[3])/6.0;

      //閾値 \Theta を超えたらリセットする
      if(vn>=Theta){
        vn=vs;
        if(t>transient){
          if(v!=0 && v!=1 && v!=2 ){//発火時刻の出力
            fprintf(stdout, "%lf \n", t-transient);
            //fprintf(stdout, "%lf %lf \n", t-transient, vn);
            spike_count++;
          }
        }
      }

      if(t>transient){//過渡状態省く
        if(v==0){
          fprintf(stdout, "%lf \n", vn);
        }else if(v==1){
          fprintf(stdout, "%lf %lf \n", t-transient, vn);
        }
      }

      v0 = vn;
    }
}

//ルンゲクッタ法 //入力信号が時間変化する場合
void runge_kutta_2(double transient, double time, double dt, double Theta, double k, double *I, double v0, double vs, double *kf, int v, int *spike_count){
  double t;
  double vn=0;
  double td=1/dt;
  int i;
  transient=0;
  spike_count=0;

  for(t = 0 ; t <= (transient+time) ; t=t+dt){
    i=(int)(t*td);
    //k1
    kf[0] = dt * f(v0, k, I[i]);
    //k2
    kf[1] = dt * f(v0+kf[0]/2.0, k, I[i]);
    //k3
    kf[2] = dt * f(v0+kf[1]/2.0, k, I[i]);
    //k4
    kf[3] = dt * f(v0+kf[2], k, I[i]);

    vn = v0 + (kf[0] + 2.0*(kf[1]+kf[2]) + kf[3])/6.0;

    /*if(t==transient){
      //fprintf(stdout,"%lf %lf \n", t-transient, vn);
      fprintf(stdout,"%lf 0 \n", t-transient);
    }*/

    if(v==2){
      if(vn>=Theta){
        fprintf(stdout, "%lf 20 \n", t-transient);
      }else{
        fprintf(stdout, "%lf 0 \n", t-transient);
      }
    }

    
    //閾値 \Theta を超えたらリセットする
    if(vn>=Theta){
      vn=vs;
      if(t>transient){
        if(v!=0 && v!=1 && v!=2){
          fprintf(stdout, "%lf \n", t-transient); //defaoult
          spike_count++;
        }
      }
    }

    if(t>transient){//過渡状態省く
      if(v==0){
        fprintf(stdout, "%lf \n", vn);
      }else if(v==1){
        fprintf(stdout, "%lf %lf \n",t-transient, vn);
      }
    }
    v0 = vn;
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
void output_comment(int argc, char *argv[], double transient, double time, double dt, double I_static, double Theta, double k, double v0, double vs, int v, int l, char **comment, int spike_count){
  int i;
  //元々入力されていたコメントアウト文の出力
  for(i=0;i<l;i++){
    fprintf(stdout,"%s",comment[i]);
  }
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# lif.c (LIFモデル)\n");
  fprintf(stdout, "# ./lif ");
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

  fprintf(stdout, "# k= %g , v0= %g , vs= %g , Theta= %g\n", k, v0, vs, Theta);
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
