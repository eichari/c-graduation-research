/*目的: ロジスティック写像の値をn個生成*/
/*作成者: Ei Miura*/
/*最終更新日: 2020-06-15*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* 文字列関数を扱えるようにする */
#include <math.h>
#include <unistd.h>
#include <time.h>


int main(int argc, char *argv[]){
  char opt;//オプションの変数
  int n;//最終的な頂点数
  FILE *output;

  int i;
  double t;
  double y;
  double x;
  double a;

  n=100;//デフォルトは100

  /*コマンドライン引数、オプションの設定*/
  //printf("argc=%d\n",argc);//文字列の個数を表示
  for(i=1;i<argc;i++){//i=0は clt.exe
    if(*argv[i] == '-'){
      opt = *(argv[i]+1);
      if(opt == 'n'){//最終的な頂点数
        n=atoi(argv[i+1]);
      }
    }
  }

  //output=fopen("roji.dat","w");

  x=0.1;
  a=4;
  for(t=0;t<n;t=t+1){
    y=a*x*(1-x);
    //fprintf(output,"%f \n",y);
    fprintf(stdout,"%f \n",y);
    x=y;
  }
  //fclose(output);

}
