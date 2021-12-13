/*目的: NWが分裂しているか調べるプログラム*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-10-21*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define num 1024

void BFS(int **connect, int n, int x, int *length);
int index_max_search(int **data, int n, int m);//入力の座標データからRPのインデックスの最大値を求める
void read_comment(char **comment);//コメント文の読み込み
void print_data(int **data, int n, int m);//行列の出力　動作確認用
void count_data(int *n, int *l, int *m);//データ数を数える
void read_data(int **data, int m);//2次元データ，n行m列の読み込み

void *e_calloc(int n, int size, char* p);//メモリ確保
void **matrix_calloc(int n, int m, char* p);//2次元配列のメモリ確保
void output_comment(int argc, char *argv[], int n, int m, int l, int index_max, char **comment);//コメント文出力
void today(void);


int main(int argc,char *argv[]){

  //入力データ
  int i,j;
  int n, l, m;//データ数 //コメント文の行数
  int **A=NULL;
  char **comment=NULL;//コメント文を保存する
  int index_max=0;//頂点数
  
  int **connect=NULL;//頂点i,j間がつながっているかを表す配列
  int *length=NULL;

  count_data(&n, &l, &m);  //データ数のカウント //コメント文の行数のカウント
  A=(int **)matrix_calloc(n,m,"A");
  comment=(char **)matrix_calloc(l,num,"comment");
  
  read_comment(comment);//コメントアウト文の読み込み
  read_data(A,m);//データの読み込み
  index_max=index_max_search(A,n,m);//入力データからRPのインデックスの最大値を求める
  connect=(int **)matrix_calloc(index_max,index_max,"connect");
  length=(int*)e_calloc(index_max,sizeof(int),"length");
  output_comment(argc, argv, n, m, l, index_max, comment);
  //print_data(A, n, m);


  //初期化
  for(i=0;i<index_max;i++){
    for(j=0;j<index_max;j++){
        connect[i][j]=0;
      }
	   length[i]=0;
  }

  
  //入力座標データをconnectに変換
  for(i=0;i<n;i++){
    connect[ A[i][0] ][ A[i][1] ]=1;
    connect[ A[i][1] ][ A[i][0] ]=1;
  }


  //0から各頂点への距離
  BFS(connect,index_max,0,length);
  int a=1; //0:分裂 //1:連結

  for(i=1;i<index_max;i++){
	  //printf("%d %d\n",i,length[i]);
	  if(length[i]>index_max||length[i]==0){
		  a=0;
		  fprintf(stdout, "# %d %d \n",i,length[i]);
	  }
  }

  if(a==0){//0:分裂
    fprintf(stdout, "# Networks are divided. \n");
    fprintf(stdout, "0 \n" );
  }else{//1:連結
    fprintf(stdout, "# Network is one. \n");
    fprintf(stdout, "1 \n" );

  }
  
  
  //メモリ解放
  for(i=0;i<n;i++){free(A[i]);}
  free(A);
  for(i=0;i<l;i++){free(comment[i]);}
  free(comment);
  for(i=0;i<index_max;i++){free(connect[i]);}
  free(connect);
  free(length);
  
}


//幅優先探索する関数
//connectは頂点間がつながっているか表す配列
//nは頂点数
//xは探索の始点
void BFS(int **connect, int n, int x, int *length){
  int u;
  int i;
  //int distance=0;
  int color[n];//探索済み:1 //未探索:0
  int d[n];//始点からの距離
  int pi[n];//経路上の一つ前の頂点を入れる
  int Q[n];//キュー
  int head=0;//キューの先頭位置
  int tail=0;//キューの末尾位置

  for(i=0;i<n;i++){
    //初期化
    Q[i]=0;
    color[i]=0;
    //未探索の頂点は0,発見しているが未探索は1,探索済は2
    d[i]=0;//iまでの距離
    pi[i]=x;//iに到達する直前の頂点
  }

  Q[head]=x;
  color[x]=1;//始点をキューに入れる
  tail=tail+1;

  if(tail>n){
    tail=0;//例外処理
  }

  while(tail>head){
    u=Q[head];

    for(i=0;i<n;i++){
      if(connect[u][i]==1){
        if(color[i]==0){//iが未探索なら
          color[i]=1;//iを発見できるので1
          d[i]=d[u]+1;
          pi[i]=u;
          //キュー末尾にiを入れる
          Q[tail]=i;
          tail=tail+1;
          if(tail>n){
            tail=0;
          }
        }
      }
    }
    color[u]=2;
    head=head+1;
  }
  for(i=0;i<n;i++){
    //xから出る各枝の長さを加算
	  length[i]=d[i];
  }
}


///////////////////////////////////////
//入力データからRPのインデックスの最大値を求める
int index_max_search(int **data, int n, int m){  
  int i,j;
  int index_max=0;
  for(i=0;i<n;i++){
    for(j=0;j<m;j++){
      if(index_max<data[i][j]){
        index_max=data[i][j];
      }
    }
  }
  return index_max+1;
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
    int **data;

    data=(int**)calloc(n,sizeof(int*));
    for(i=0;i<n;i++){
      data[i]=(int*)calloc(m,sizeof(int));
    }

    if(data==NULL)
    {
        fprintf(stderr,"Calloc failed: %s.\n",p);
        exit(1);
    }
    return (void **)data;
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

//コメントアウト文の出力
void output_comment(int argc, char *argv[], int n, int m, int l, int index_max, char **comment){
  int i;
  //元々入力されていたコメントアウト文の出力
  for(i=0;i<l;i++){
    fprintf(stdout,"%s",comment[i]);
  }
  //このプログラムについてのコメントアウト文
  fprintf(stdout, "# /////////////////////////////\n");
  fprintf(stdout, "# bunretu.c (NWが分裂してるか調べるプログラム)\n");
  fprintf(stdout, "# ./bunretu ");
  for(i=1;i<argc;i++){
    fprintf(stdout, "%s ",argv[i] );
  }
  fprintf(stdout, "\n" );
  fprintf(stdout, "# n= %d , m=%d , l= %d , index_max= %d \n", n, m, l, index_max);
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

///////////////////////////////////////
