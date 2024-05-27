# プログラミング課題用レポジトリ

まずこのレポジトリをクローンします．
```
% git clone https://github.com/kanait/loopsub.git
```

コンパイル方法はPCの種類によって異なります．

[Windows]

コンパイルには Visual Studio Community が必要です．
https://visualstudio.microsoft.com/ja/vs/community/

VC のフォルダの中の loopsub.sln をダブルクリックして Release x64 でコンパイルしてください．
コンパイルが終了したらターミナルを開いて展開フォルダに行き
```
% .\VC\x64\Release\loopsub.exe bunnynh_sub500.obj
```
を実行してみてください．何も表示されないウインドウが出てきたら正常に実行できています．

[macOS]

ソースコードのコンパイルには g++, make, GLFW, GLEW, cmake が必要です．
g++, make は xcode command line tools が必要となります．
GLFW, GLEW, cmake は，brew を使ってあらかじめインストールしてください．
```
% brew install glfw glew cmake
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
% ./loopsub/loopsub ../bunnynh_sub500.obj
```
何も表示されないウインドウが出てきたら正常に実行できています．

[linux(ubuntu)]

GLFW, GLEW, cmake をapt 等を使ってインストールする必要があります．
あとは macOS と一緒です．

[data]

bunnynh_sub500.obj, venus_sub1000.obj ... Loop 細分割用データ
41.obj, oloid64_quad.obj, spot_quadrangualted.obj ... Catmull-Clark 細分割用データ
