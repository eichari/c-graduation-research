/*目的: RPを求めるプログラム (Euclid距離のソートを行い，正確にプロット割合を定める) RP.c*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-09-12*/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#define num 1024
 
void print_data(double **data, int n, int m);//動作確認用
void cal_range(double **data, double *L, double *R, double theta, double *theta_range, double n, double m, double *minl, double *maxl);
void RP(double *L, int n, int m, double minl, double maxl, double theta_range);
//ユークリッド距離のソートで使用する
void quicksort(double *A, int p, int r); /* 関数quicksort の宣言*/
int partition(double *A, int p, int r); /* 関数partition の宣言*/
 
void option(int argc, char *argv[], double *theta);
void count_data(int *n, int *l, int *m);//データ数を数える
void read_comment(char **comment);//コメント文の読み込み
void read_data(double **data, int m);//2次元データ，n行m列の読み込み
void *e_calloc(int n, int size, char* p);//メモリ確保
void **matrix_calloc(int n, int m, char* p);//2次元配列のメモリ確保
void output_comment(int argc, char *argv[], int l, int m, double theta, double theta_range, char **comment);
void today(void);//日付の表示
 
int main(int argc, char *argv[]){
  int i;
  //入力データ
  int n, l, m;//データ数 //コメント文の行数 //データの行数
  char **comment=NULL;//コメント文を保存する
  double **data=NULL;//入力m次元データ
  double *L=NULL;//Euclid距離の保存　2次元情報を1次元にした
  double *R=NULL;//theta_rangeを求めるための使い捨ての配列
  //RPのパラメータ
  double theta=0.1;//閾値
  double theta_range=0;// (n^2-n)*theta 個のプロットを行うための theta%点
  double minl=-1, maxl=-1;//最小値，最大値
   
  option(argc, argv, &theta);//オプション
  count_data(&n, &l, &m);  //データ数のカウント //コメント文の行数のカウント
  fprintf(stdout, "# n=%d m=%d l=%d \n",n,m,l );
  comment=(char **)matrix_calloc(l, num, "comment");
  data=(double **)matrix_calloc(n, m, "data");
  L=(double*)e_calloc(( (int)(n*(n-1)*0.5) ),sizeof(double),"L");
  R=(double*)e_calloc(( (int)(n*(n-1)*0.5) ),sizeof(double),"R");
 
  read_comment(comment);//コメントアウト文の読み込み
  read_data(data,m);//2次元データ，n行m列の読み込み
  cal_range(data,L,R,theta,&theta_range,n,m,&minl,&maxl);
  output_comment(argc, argv, l, m, theta, theta_range, comment);//コメント文の出力
  //print_data(data,n,m);//data 動作確認用
  RP(L,n,m,minl,maxl,theta_range);
 
  for(i=0;i<l;i++){free(comment[i]);}
  free(comment);
  for(i=0;i<n;i++){free(data[i]);}
  free(data);
  free(L);
  free(R);
 
  return 0;
}
 
//thetaの計算
void cal_range(double **data, double *L, double *R, double theta, double *theta_range, double n, double m, double *minl, double *maxl){
  int k,l;
  int p,q;
  double Euclid;
  int triangle=0;
  //ユークリッド距離の計算
  l=0;
  for(p=0;p<n-1;p++){//i 1
    for(q=p+1;q<n;q++){//i 2
      Euclid=0;
      for(k=0;k<m;k++){//i1ベクトルとi2ベクトルのEuclid距離の計算
        Euclid+=pow( (data[p][k]-data[q][k]) ,2.0);
      }
      L[l]=sqrt(Euclid);
      R[l]=L[l];
      //fprintf(stdout,"# L[%d]=%lf \n", l, L[l]);
      l++;
    }
  }
  triangle=(int)(n*(n-1)*0.5);
  quicksort(R,0,triangle-1);//昇順にソート

  /*
  l=0;
  for(p=0;p<n-1;p++){
    for(q=p+1;q<n;q++){
      fprintf(stdout,"# R[%d]=%lf \n", l, R[l]);
      l++;
    }
  }
  */
  *minl=R[0];
  *maxl=R[triangle-1];
  if(theta==0){
    *theta_range=0;//RPは斜めと端しか出力されない
  }else if(theta==1){
    *theta_range=1000000;//RPはすべて出力されるため
  }else{
    *theta_range=R[ (int)( (triangle-1)*theta ) ];//配列は0スタートなので，-1
  }  
  //出力情報の確認 便宜的に.datファイルの一番上に出力される
  fprintf(stdout, "# ///// RP information /////\n");
  fprintf(stdout, "# triangle= %d \n", triangle );
  fprintf(stdout, "# triangle*theta= %d \n", (int)(triangle*theta) );//キャストは切り捨て
  fprintf(stdout, "# max= %g , min= %g \n", *maxl, *minl );
  fprintf(stdout, "# range= %g \n", (*maxl- *minl) );
  fprintf(stdout, "# theta = %g , theta_range= %g \n", theta, *theta_range );
}
 
//RPの出力
void RP(double *L, int n, int m, double minl, double maxl, double theta_range){
  int p,q,l;
  int plot_count;
  double plot_per=-1;
  l=0; plot_count=0;
  for(p=0;p<n;p++){
    fprintf(stdout, "%d %d \n", p, p );
  }
  for(p=0;p<n-1;p++){//i 1
    for(q=p+1;q<n;q++){//i 2
      if( ( L[l]<=theta_range) ){
        fprintf(stdout, "%d %d \n", p, q );
        fprintf(stdout, "%d %d \n", q, p );
        plot_count++;
      }
      l++;
    }
  }
  plot_count=plot_count*2;
  plot_per=plot_count/(double)(n*(n-1));//n>10万だと計算できない
  //fprintf(stdout, "# plot_count=%d plot_per=%g \n",plot_count, plot_per );
}
 
 
void print_data(double **data, int n, int m){
  int i,j;
  for(i=0;i<n;i++){
    for(j=0;j<m;j++){
      fprintf(stdout, "%g ", data[i][j]);
    }
    fprintf(stdout, "\n");
  }
}
 
//オプション
void option(int argc, char *argv[], double *theta){
  int i;
  for(i=1;i<argc;i++){
    if(strcmp(argv[i], "-theta")==0){ *theta=atof(argv[i+1]); }
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
      d--;
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
void read_data(double **data, int m){
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
void output_comment(int argc, char *argv[], int l, int m, double theta, double theta_range, char **comment){
  int i;
  //元々入力されていたコメントアウト文の出力
  for(i=0;i<l;i++){
    fprintf(stdout,"%s",comment[i]);
  }
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# RP.c (リカレンスプロット)\n");
  fprintf(stdout, "# ./RP ");
  for(i=1;i<argc;i++){
    fprintf(stdout, "%s ", argv[i]);
  }
  fprintf(stdout, "\n");
  fprintf(stdout, "# m= %d , theta= %g , theta_range= %g \n", m, theta, theta_range);
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

//ユークリッド距離のソートを行う　//quicksort
void quicksort(double *A, int p, int r){ /* 関数quicksort の宣言*/
    int q,i;
    if(p<r){
    	q=partition(A,p,r);
    	quicksort(A,p,q-1);
    	quicksort(A,q+1,r);
    }
}
//quicksort
int partition(double *A, int p, int r){ /* 関数partition の宣言*/
    int i,j;
    double x;
    double tmp;

    x=A[r];//一番後ろをpivotに設定
    i=p-1;//p-1<= i <=r-1

    for(j=p;j<r;j++){
      if(A[j]<=x){
        i++;
        tmp=A[i];
        A[i]=A[j];
        A[j]=tmp;
      }
    }
    //pivot=r と 
    tmp=A[i+1];
    A[i+1]=A[r];
    A[r]=tmp;

    return i+1;
}
