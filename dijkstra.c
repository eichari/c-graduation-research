/*目的: 重み付きネットワークから距離行列を求めるプログラム*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-09-18*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#define num 5000000
#define INF 99999

void dijkstra(double **data, int n, int m);//ダイクストラ法

void print_data(double **data, int n, int m);//行列の出力　動作確認用
void count_data(int *n, int *l, int *m);//データ数を数える
void read_data(double **data, int m);//2次元データ，n行m列の読み込み
void read_comment(char **comment);//コメント文の読み込み
void *e_calloc(int n, int size, char* p);//メモリ確保
void **matrix_malloc_double(int n,int m);
void output_comment(int argc, char *argv[], int n, int m, int l, char **comment);//コメント文出力
void today(void);
 
int main(int argc, char *argv[]){
     
  int i,j;
  int n, l, m;
  double **data = NULL;
  char **comment1=NULL;//コメント文を保存する
  char *rp1;//=argv[1];
  
 
  count_data(&n, &l, &m);//データ数のカウント //コメント文の行数のカウント
  //fprintf(stdout, "n1= %d , m1= %d \n", n1, m1 );//行列数の確認
  data = (double **)matrix_malloc_double(sizeof(double)*n,sizeof(double)*m);
  //comment1=(char **)matrix_calloc(l,num,"comment1");
  comment1 = (char **)matrix_malloc_double(sizeof(char)*l,sizeof(char)*num);
  read_comment(comment1);//コメントアウト文の読み込み
  read_data(data,m);//データの読み込み
  //print_data(data,n,m);
  output_comment(argc, argv, n, m, l, comment1);

  dijkstra(data,n,m);
    
  //終了処理
  for(i=0;i<n;i++){free(data[i]);}
  free(data);
  for(i=0;i<l;i++){free(comment1[i]);}
  free(comment1);
  return 0;
}

//ダイクストラ法
void dijkstra(double **data, int n, int m){
  int i,j;
  int k=0, l=0;
  double min;
  double *COST=NULL;//始点からの距離
  bool *USED=NULL;//頂点iが使用済みか判断　//未使用 0 //使用済み 1
  int tansaku;//
  double newDist;//距離の更新に使う
  
  int *sRoute;//経由した頂点の保存
  double *pRoute;//始点から各頂点までの
  
  COST=(double*)e_calloc(n,sizeof(double),"COST");//メモリ確保   
  USED=(bool*)e_calloc(n,sizeof(bool),"USED");//メモリ確保  
  sRoute=(int*)e_calloc(n,sizeof(int),"sRoute");//メモリ確保   
  pRoute=(double*)e_calloc(n,sizeof(double),"pRoute");//メモリ確保   


  //dijkstra法   
  for(k=0;k<n;k++){//始点を動かす
    for(l=0;l<n;l++){
      min=INF;
      for(i=0;i<n;i++){
        sRoute[i]=-1;
        COST[i]=INF;
        USED[i]=false;
      }
      COST[k]=0;//始点のコストは0
           
      //最短経路探索処理
      while(1){
        i=0;
        while(i<n){//未探索の頂点を探す
          if(USED[i]==0){break;}
          i++;
        }
        if(i==n){break;}//未探索の頂点がなければ終了
        for(j=i+1;j<n;j++){ //最短距離がより短い地点を探す
            if((USED[j]==0) && (COST[j]<COST[i])){
                i=j;
            }
        }
        //距離の更新
        tansaku=i;
        USED[tansaku]=true;//探索済み
        for(j=0;j<n;j++){
          if((data[tansaku][j]>=0) && (USED[j]==0)){
            newDist=COST[tansaku]+data[tansaku][j];
            if(newDist<COST[j]){
              COST[j]=newDist;//始点からの距離を更新
              pRoute[j]=tansaku;
            }
          }
        }
      }       
      min=COST[l];
      j=0;
      i=l;    
      while(i!=k){
        sRoute[j]=i;
        i=pRoute[i];
        j++;
      } 
      sRoute[j]=k;
      
      fprintf(stdout,"%g ",min);
    }
    fprintf(stdout,"\n");
  }
  
  //メモリ解放
  free(COST);
  free(USED);
  free(sRoute);
  free(pRoute);
  //fprintf(stdout,"# finish \n");


}

//行列の出力　動作確認用
void print_data(double **data, int n, int m){
  int i,j;
  for(i=0;i<n;i++){
    for(j=0;j<m;j++){
      fprintf(stdout, "%g ", data[i][j]);
    }
    fprintf(stdout, "\n");
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

//n*mのデータを入れるための2次元配列を動的に確保する．
void **matrix_malloc_double(int n,int m){ //引数はsizeof()*nの形で渡す
    int i;
    void **data;
     
    data = malloc(sizeof(double *) * n);
    for (i=0;i < n;i++){
        data[i] = malloc(sizeof(double) * m);
    }
     
    if(data == NULL){
        fprintf(stderr, "Malloc failed.\n");
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
  fprintf(stdout, "# dijkstra.c (重み付きネットワークから距離行列を求めるプログラム)\n");
  fprintf(stdout, "# ./dijkstra ");
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
