# プログラミング課題用レポジトリ

まずこのレポジトリをクローンします．
```
% git clone https://github.com/kanait/loopsub.git
```

コンパイル方法はPCの種類によって異なります．

## Windows

コンパイルには Visual Studio Community が必要です．
https://visualstudio.microsoft.com/ja/vs/community/
(インストールの際，必ず「C++ によるデスクトップ開発」を選択すること)

VC のフォルダの中の loopsub.sln をダブルクリックして Release x64 でビルドしてください．
コンパイルが終了したらターミナルを開いて展開フォルダに行き
```
% .\VC\x64\Release\loopsub.exe .\data\bunnynh_sub500.obj
```
を実行してみてください．何も表示されないウインドウが出てきたら正常に実行できています．

## macOS

ソースコードのコンパイルには g++, make, GLFW, GLEW, cmake, eigen が必要です．
g++, make は xcode command line tools が必要となります．
GLFW, GLEW, cmake, eigen は，brew を使ってあらかじめインストールしてください．
(eigen はもともと入っていますが，brew で入れたものを使う方がコンパイルできる可能性が高いです)
```
% brew install glfw glew cmake eigen
```
展開フォルダの中で以下を実行してください．
```
% mkdir build
% cd build
% cmake -DCMAKE_BUILD_TYPE=Release ..
% make
```
（コンパイルの際 warning が出ますが気にしないでください）
コンパイルが終了したら build フォルダで以下を実行してください．
```
% ./loopsub/loopsub ../data/bunnynh_sub500.obj
```
何も表示されないウインドウが出てきたら正常に実行できています．

## linux(ubuntu)

GLFW, GLEW, cmake, eigen をapt 等を使ってインストールする必要があります．
```
% sudo apt install libglfw3-dev libglew-dev cmake libeigen3-dev
```
あとは macOS と一緒です．

## data

bunnynh_sub500.obj, venus_sub1000.obj ... Loop 細分割用データ
41.obj, oloid64_quad.obj, spot_quadrangualted.obj ... Catmull-Clark 細分割用データ
