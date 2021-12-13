/*目的: //閾値を決め，重畳RPを作成するプログラム*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-09-23*/
//入力はRP[i][j]
//  ./RPth -theta 5 < RPs.dat

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* 文字列関数を扱えるようにする */
#include <math.h>
#include <time.h>

#define num 5000000

void tyoujou_th(int **RP, int n, int m, int theta);//閾値から重畳RPを作成する

void option(int argc, char *argv[], int *theta);
void print_data(int **data, int n, int m);//行列の出力　動作確認用
void count_data(int *n, int *l, int *m);//データ数を数える
void read_data(int **data, int m);//2次元データ，n行m列の読み込み
void *e_calloc(int n, int size, char* p);//メモリ確保
void **matrix_calloc(int n, int m, char* p);//2次元配列のメモリ確保
void output_comment(int argc, char *argv[]);//コメント文出力
void today(void);

int main(int argc, char *argv[]){
  //入力データ
  int i,j;
  int n1, m1, l1;

  int **RP=NULL;
  //FILE *fp1=NULL;//ファイル名1
  //char *rp1;//=argv[1];
  int theta=0;
  
  //  ./RPth rps.dat -theta 5
  //for(i=0;i<argc;i++){fprintf(stdout, "argv[%d]= %s\n", i, argv[i] );}
  option(argc, argv, &theta);//オプション
  //fp1=fopen(argv[1],"r");  
  count_data(&n1, &l1, &m1);//データ数のカウント //コメント文の行数のカウント
  //fprintf(stdout, "n1= %d , m1= %d \n", n1, m1 );//行列数の確認
  RP=(int **)matrix_calloc(n1, m1, "RP");
  read_data(RP,m1);//データの読み込み
  //print_data(RP,n1,m1);//入力データの確認
  output_comment(argc, argv);
  tyoujou_th(RP,n1,m1,theta);//閾値により重畳RPを出力する

  //終了処理
  for(i=0;i<n1;i++){free(RP[i]);}
  free(RP);
  //fclose(fp1);

}

//RPをRPの座標データに変換する
void tyoujou_th(int **RP, int n, int m, int theta){
  int i,j;
  for(i=0;i<n;i++){
    for(j=0;j<m;j++){
      if(RP[i][j]>=theta){
        fprintf(stdout, "1 " );
      }else{
        fprintf(stdout, "0 " );
      }
    }
    fprintf(stdout, "\n" );
  }
  //fprintf(stdout, "\n" );
}


//行列の出力　動作確認用
void print_data(int **data, int n, int m){
  int i,j;
  for(i=0;i<n;i++){
    for(j=0;j<m;j++){
      fprintf(stdout, "%d ", data[i][j]);
    }
    fprintf(stdout, "\n");
  }
}

//オプション
void option(int argc, char *argv[], int *theta){
  int i;
  for(i=1;i<argc;i++){
    if(strcmp(argv[i], "-theta")==0){ *theta=atoi(argv[i+1]); }
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
void read_data(int **data, int m){
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
void output_comment(int argc, char *argv[]){
  int i;
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# RPth.c (閾値を決め，重畳RPを作成するプログラム)\n");
  fprintf(stdout, "# ./RPth ");
  for(i=1;i<argc;i++){
    fprintf(stdout, "%s ", argv[i]);
  }
  fprintf(stdout, "\n");
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
