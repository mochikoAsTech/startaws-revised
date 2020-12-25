日本時間@<fn>{jst}になるようタイムゾーンの変更を行いましょう。

//footnote[jst][日本標準時（JST）は、協定世界時（UTC）から9時間進めた時間、つまりUTC+9ですね。]

//cmd{
# vi /etc/sysconfig/clock
//}

vi（ブイアイ）@<fn>{remoteSsh}はテキストファイルを編集するためのコマンドです。viコマンドでファイルを開くと、最初は次のような「閲覧モード」の画面（@<img>{startaws31}）が表示されます。閲覧モードは「見るだけ」なので編集ができません。

//footnote[remoteSsh][本書ではこの先もずっとターミナルからviコマンドでファイルを編集していきますが、「viつらい…」という人のために他の方法もご紹介しておきます。もしあなたがパソコンでVisual Studio Codeというエディタをお使いでしたら、公式（Microsoft）が提供している「Remote - SSH」という拡張機能を入れることで、Visual Studio Code上でサーバにログインして、サーバの中にあるテキストファイルを直接Visual Studio Codeで編集したり保存したりできる機能があります。コードがシンタックスハイライトされて、可読性も一気に上がるのでお勧めです。詳しくは公式ドキュメントの「Remote development over SSH」を参照してください。 @<href>{https://code.visualstudio.com/docs/remote/ssh-tutorial}]

//image[startaws31][viコマンドでファイルを開いた][scale=0.8]{
//}

この状態でi（アイ）を押すと「編集モード」@<fn>{insertMode}に変わります。（@<img>{startaws32}）左下に「-- INSERT --」と表示されていたら「編集モード」です。

//footnote[insertMode][ここでは初心者の方でも直感的に分かるよう「閲覧モード」「編集モード」と呼んでいますが、正しくは「ノーマルモード」「インサートモード」です。]


//image[startaws32][i（アイ）を押すと「-- INSERT --」と表示される「編集モード」になった][scale=0.8]{
//}

「編集モード」になるとファイルが編集できるようになります。それでは「ZONE="UTC"」を「ZONE="Asia/Tokyo"」（@<img>{startaws33}）に書き換えてください。

//image[startaws33][「ZONE="UTC"」を「ZONE="Asia/Tokyo"」に書き換える][scale=0.8]{
//}

「編集モード」のままだと保存ができないので書き終わったらESCキーを押します。すると左下の「-- INSERT --」が消えて再び「閲覧モード」になります。（@<img>{startaws34}）

//image[startaws34][ESCを押すと左下の「-- INSERT --」が消えて再び「閲覧モード」になる][scale=0.8]{
//}

「閲覧モード」に戻ったら「:wq」@<fn>{wq}と入力してEnterキーを押せば変更が保存されます。（@<img>{startaws35}）

//image[startaws35][「:wq」と入力してEnterキーを押せば保存される][scale=0.8]{
//}

//footnote[wq][書き込んで（write）、抜ける（quit）という命令なのでwqです。]

色々やっているうちになんだか訳が分からなくなってしまって「今の全部なかったことにしたい！取り合えずviからいったん抜けたい！」と思ったときは、ESCキーを押して「:q!」@<fn>{q}と入力してEnterキーを押すと変更を保存せずに抜けることができます。

//footnote[q][保存せずに強制終了（quit!）という命令なのでq!です。]

編集できたらcat（キャット）コマンド@<fn>{cat}でファイルの中身を確認してみましょう。次のように表示されたら、ちゃんと編集できています。

//cmd{
# cat /etc/sysconfig/clock
ZONE="Asia/Tokyo"
UTC=true
//}

//footnote[cat][catは猫ではなく「conCATenate files and print on the standard output」の略だそうです。筆者はいつも「猫がファイルの中身を全部出して見せてくれてるんだ」と考えることでちょっぴり幸せになっています。]

さらにlnコマンド@<fn>{ln}でシンボリックリンクを作ります。シンボリックリンクはWindowsでいうところのショートカットみたいなものです。成功した場合は、画面に何も表示されませんので安心してください。

//footnote[ln][lnはlinkの略です。]

//cmd{
# ln -sf /usr/share/zoneinfo/Asia/Tokyo /etc/localtime

代わりにこれでよさそう
# timedatectl set-timezone Asia/Tokyo
/etc/sysconfig/clock は変化ない
//}

ちなみに入力しているパス（path）は入力途中でタブを押すと自動的に補完されるので、全部手打ちしなくても大丈夫です。たとえば

//cmd{
# ln -sf /usr/sh
//}

まで打ってからTabキーを押すと次のように自動補完されます

//cmd{
# ln -sf /usr/share/
//}

シンボリックリンクが生成されたかlsコマンド@<fn>{ls}で確認してみましょう。（@<img>{startaws36}）

//cmd{
# ls -l /etc/localtime
//}

//footnote[ls][lsはlistの略で、名前のとおりファイルを一覧表示してくれるコマンドです。-lはlongの略で「詳細を表示」というオプションです。]

//image[startaws36][シンボリックリンクが生成された][scale=0.8]{
//}

「->」の矢印が実体であるファイルを指しているので、シンボリックリンクができていることがわかります。
