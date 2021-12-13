/*目的: RPから重み付きネットワークを作成するプログラム*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-09-17*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* 文字列関数を扱えるようにする */
#include <math.h>
#include <time.h>

#define num 5000000

void weight(int **RP, int n, int m);//RPから重み付きネットワークを作成する

void print_data(int **data, int n, int m);//行列の出力　動作確認用
void count_data(int *n, int *l, int *m, FILE *fp);//データ数を数える
void read_data(int **data, int m, FILE *fp);//2次元データ，n行m列の読み込み
void read_comment(char **comment, FILE *fp);//コメント文の読み込み
void *e_calloc(int n, int size, char* p);//メモリ確保
void **matrix_calloc(int n, int m, char* p);//2次元配列のメモリ確保
void output_comment(int argc, char *argv[], int n, int m, int l, char **comment);//コメント文出力
void today(void);

int main(int argc, char *argv[]){
  //入力データ
  int i,j;
  int n1, m1, l1;
  int index_max=0;

  int **RP=NULL;
  char *str1=NULL;//1行ずつ読みこむ
  char **comment1=NULL;//コメント文を保存する
  FILE *fp1=NULL;//ファイル名1
  char *rp1;//=argv[1];
  
  //オプションなし (1<i<argc :: argv[i])
  //for(i=0;i<argc;i++){fprintf(stdout, "argv[%d]= %s\n", i, argv[i] );}
  
  fp1=fopen(argv[1],"r");  
  count_data(&n1, &l1, &m1, fp1);//データ数のカウント //コメント文の行数のカウント
  //fprintf(stdout, "n1= %d , m1= %d \n", n1, m1 );//行列数の確認
  RP=(int **)matrix_calloc(n1, m1, "RP");
  str1=(char*)e_calloc(num,sizeof(char),"str1");//メモリ確保
  comment1=(char **)matrix_calloc(l1,num,"comment1");
  read_comment(comment1,fp1);//コメントアウト文の読み込み
  read_data(RP,m1,fp1);//データの読み込み
  //print_data(data1,n1,m1);//data1の確認
  //output_comment(argc, argv, n1, m1, l1, comment1);
  weight(RP,n1,m1);//座標データからRPへの変換

  //終了処理
  for(i=0;i<n1;i++){free(RP[i]);}
  free(RP);
  free(str1);
  for(i=0;i<l1;i++){free(comment1[i]);}
  free(comment1);
  fclose(fp1);
}

//RPから重み付きネットワークを作成する
void weight(int **RP, int n, int m){
  int i,j,k;
  int and_count=0;
  int or_count=0;
  for(i=0;i<n;i++){
    for(j=0;j<m;j++){
      if(i==j){//重み付きネットワークにおいて対角線は0（頂点の自己ループなし）
        fprintf(stdout, "0 " );
      }else if(RP[i][j]==1){//枝の重みを求める
        and_count=0;
        or_count=0;
        for(k=0;k<n;k++){
          if( RP[i][k]==1 && RP[j][k]==1 ){
            and_count++;
          }
          if( RP[i][k]==1 || RP[j][k]==1 ){
            or_count++;
          }
        }
        fprintf(stdout, "%g ", 1-( (double)and_count/(double)or_count ) );
      }else{// i,j がつながっていない
        fprintf(stdout, "-1 " );
      }
    }
    fprintf(stdout, "\n" );
  }
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

//データ情報を取得
void count_data(int *n, int *l, int *m, FILE *fp){
  int data_n=0;//データ数
  int data_m=0, d=0;
  int comment_l=0;//コメントアウト文の行数
  char line[num];
  char *tok,*space=" ";
 
  while(fgets(line,num,fp)!=NULL){
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
  fseek(fp, 0, SEEK_SET);
}

//データの読み込み
void read_data(int **data, int m, FILE *fp){
  int i=0,j=0;
  char line[num];
  char *tok,*space=" ";
  while(fgets(line,num,fp)!=NULL){
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
  fseek(fp, 0, SEEK_SET);
}

//コメントアウト文の読み込み
void read_comment(char **comment, FILE *fp){
  int i=0;
  char line[num];
  while(fgets(line,num,fp)!=NULL){
    if(line[0]=='#'){//コメントアウト文
      strcpy(comment[i],line);//コメントアウト文の読み込み
      i++;
    }else{
      break;
    }
  }
  fseek(fp, 0, SEEK_SET);
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
void output_comment(int argc, char *argv[], int n, int m, int l, char **comment){
  int i;
  //元々入力されていたコメントアウト文の出力
  for(i=0;i<l;i++){
    fprintf(stdout,"%s",comment[i]);
  }
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# weight.c (RPから重み付きネットワークを作成するプログラム)\n");
  fprintf(stdout, "# ./weight ");
  for(i=1;i<argc;i++){
    fprintf(stdout, "%s ", argv[i]);
  }
  fprintf(stdout, "\n");
  //fprintf(stdout, "# n= %d m= %d \n", n, m);
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
