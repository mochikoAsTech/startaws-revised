= ウェブサーバの設定をしよう

この章ではウェブサーバの設定を行います。
Apacheでバーチャルホストを作ってブラウザでページを表示してみましょう。

//pagebreak

== ウェルカムページを見てみよう

@<chapref>{serverBuilding}でサーバにApacheをインストールしました。インストールしただけでまだ何の設定もしていませんが、ウェルカムページと呼ばれるデフォルトのページが見られるはずです。しかしブラウザで「@<href>{http://login.自分のドメイン名/}」を開いてウェルカムページを見ようとしたところ、「ページ読み込みエラー 接続がタイムアウトしました」「このサイトにアクセスできません。login.自分のドメイン名 からの応答時間が長すぎます。」といったエラーメッセージが表示（@<img>{startaws57}）されてページを見ることはできませんでした。

//image[startaws57][@<href>{http://login.自分のドメイン名/}が見られなかった][scale=0.8]{
//}

これはインスタンスの手前に居るセキュリティグループ@<fn>{securityGroup}が「SSH（ポート番号22番）の通信しか通さない」という設定になっていて、HTTP（ポート番号80番）のリクエストをすべて遮断していることが原因です。@<ttb>{ポート番号とはサーバという家や、その手前にあるセキュリティグループという壁についているドア}のようなものだと思ってください。同じサーバを訪問するときでもSSHは22番のドアを、HTTPは80番のドアを、HTTPSは443番のドアを通ります。

//footnote[securityGroup][セキュリティグループはいわゆる「ファイアウォール」のことです。セキュリティグループってどんなものだったっけ？という方は@<chapref>{serverBuilding}でEC2のインスタンスを作るとき「ステップ6」で設定したことを思い出してください。]

=== セキュリティグループで80番ポートの穴あけをしよう

ウェルカムページが見られるように、セキュリティグループで「HTTP（ポート番号80番）」の穴あけをしましょう。マネジメントコンソール@<fn>{consoleUrl}の左上にある「サービス」から、「コンピューティング」の下にある「EC2」（@<img>{startaws58}）をクリックしてください。

//footnote[consoleUrl][@<href>{https://console.aws.amazon.com/}]

//image[startaws58][サービス＞コンピューティング＞EC2][scale=0.8]{
//}

「EC2」をクリックすると、EC2ダッシュボード（@<img>{startaws59}）が表示されます。左メニューの「セキュリティグループ」をクリックしてください。

//image[startaws59][現状はSSH（ポート番号22番）しか通れないので「インバウンドルール」タブの「インバウンドルールを編集」をクリック][scale=0.8]{
//}

「ec2-security-group」をクリックして「インバウンドルール」タブを見ると、現状は通信を許可する対象に「SSH（ポート番号22番）」しか含まれていません。「インバウンドルール」タブの「インバウンドルールを編集」をクリックします。

「ルールを追加」をクリックしたらタイプは「HTTP」を選択（@<img>{startaws60}）します。ソースのプルダウンは「カスタム」のままで、「0.0.0.0/0」を選択してください。入力できたら「ルールを保存」をクリックします。

//image[startaws60][タイプは「HTTP」、ソースは「0.0.0.0/0」にして「ルールを保存」をクリック][scale=0.8]{
//}

「インバウンドセキュリティグループのルールが、セキュリティグループで正常に変更されました」と表示され、「インバウンドルール」タブでHTTPが追加（@<img>{startaws61}）されていたら穴あけ作業は完了です。

//image[startaws61][「インバウンドルール」タブでHTTPが追加されていたら穴あけ完了][scale=0.8]{
//}

もう一度ブラウザで「@<href>{http://login.自分のドメイン名/}」を開いてみましょう。「Test Page」と書かれたウェルカムページが表示（@<img>{startaws62}）されるはずです。

//image[startaws62][ApacheのTest Pageが表示された][scale=0.8]{
//}

== 自分のサイト用にバーチャルホストを作ろう

ここからは自分のサイトの「バーチャルホスト」を作ります。

=== バーチャルホストとは？

「バーチャルホスト」という言葉を聞いたことはありますか？

バーチャルホストというのは@<ttb>{1台のサーバ上で2つ以上のウェブサイトを扱う運用方法のこと}です。たとえば筆者の趣味が「スイーツブッフェめぐり」と「読書」だったとして、次のような2つのバーチャルホストを作ってやれば、1台のウェブサーバ上で2つのサイト同時に公開できます。

 * sweets.startdns.fun（スイーツブッフェめぐりのサイト）
 * book.startdns.fun（読んだ本を紹介するサイト）

マンションの建物（ホストサーバ）の中に101号室や102号室などの各戸（仮想サーバ）があり、その中にはそれぞれ子供部屋や書斎や居間などの部屋（ウェブサイト）があって、その部屋を訪れるお客さんたち（サイトを見る人）がいる、と例えると分かりやすいでしょうか。

これからApacheで自分のサイト用にこのバーチャルホストを作ってみます。

=== バーチャルホストを設定しよう

それでは早速、自分のドメイン名（筆者ならstartdns.fun）のサイト用にバーチャルホストを作ってみましょう。@<fn>{subDomain}

//footnote[subDomain][サーバ1台にウェブサイト1つだけであればバーチャルホストにする必要はあまりないのですが、バーチャルホストの作り方を覚えておけば、今後「サブドメイン名で別のサイトを作ってみよう」と思ったときに役立つはずです。]

Windowsの方はRLoginを起動して「start-aws-instance」に接続してください。Macの方はターミナルで次のコマンドを実行してください。

//cmd{
ssh ec2-user@login.自分のドメイン名 -i ~/Desktop/start-aws-keypair.pem
//}

「Amazon Linux 2 AMI」と表示されたら「sudo su -」@<fn>{sudoSu}でrootになりましょう。（@<img>{startaws63}）

//cmd{
$ sudo su -
//}

//image[startaws63][Amazon Linux 2 AMIと表示されたらrootになろう][scale=0.8]{
//}

//footnote[sudoSu][sudoは「他のユーザとしてコマンドを実行する」ためのコマンドで、 suは「他のユーザになる」ためのコマンドです。「他のユーザ＝root」の場合はユーザ名を書かなくてもいいので省略していますが、省略せずに書くと「sudo -u root su - root」（rootとして「rootになる」というコマンドを実行する）ということです。ちなみに勘違いされることが多いですがsuは「Super User」ではなく「Substitute User（ユーザーを代用する）」の略です。]

Apacheの大本となる設定ファイルは「/etc/httpd/conf/httpd.conf」です。350行以上あるのでtailコマンドを使って最後の5行だけ確認してみましょう。

//cmd{
# tail -5 /etc/httpd/conf/httpd.conf
//}

//image[startaws143][tailコマンドでhttpd.confの最後の5行を見てみよう][scale=0.8]{
//}

すると最後の行に、次のように書かれているため、大本の設定ファイル（httpd.conf）の中で、さらに「conf.d」というディレクトリ内の「*.conf」というファイルをインクルード（取り込み）@<fn>{includeOptional}していることが分かります。

//cmd{
IncludeOptional conf.d/*.conf
//}

でもいきなり「conf.d/*.conf」と書かれても「3丁目」とだけ書かれた住所のようなもので、どこの「conf.d/*.conf」を指しているのかよく分かりませんよね。「conf.d」より上のディレクトリはどこで指定しているのか、grepという検索のコマンドで探してみましょう。

//footnote[includeOptional][IncludeディレクティブやIncludeOptionalディレクティブについては@<href>{https://httpd.apache.org/docs/2.4/ja/mod/core.html#include}を参照。]

//cmd{
# grep "^ServerRoot" /etc/httpd/conf/httpd.conf
ServerRoot "/etc/httpd"
//}

ServerRoot@<fn>{serverRoot}ではベースとなるディレクトリを指定しています。これで「conf.d/*.conf」が実際は「/etc/httpd/conf.d/*.conf」であることが分かりました。

//footnote[serverRoot][ServerRootディレクティブについては@<href>{https://httpd.apache.org/docs/2.4/ja/mod/core.html#serverroot}を参照。]

では自分のドメイン名のサイト用にバーチャルホストを「/etc/httpd/conf.d」の下で作成してみましょう。viコマンドで新しい設定ファイルを作ります。

//cmd{
# vi /etc/httpd/conf.d/start-aws-virtualhost.conf
//}

こんな画面（@<img>{startaws64}）で「"/etc/httpd/conf.d/start-aws-virtualhost.conf" [New File]」と表示されましたか？

//image[startaws64][viでバーチャルホストの設定ファイルを作ろう][scale=0.8]{
//}

この状態でi（アイ）を押して（@<img>{startaws65}）左下に「-- INSERT --」と表示されたら「編集モード」になっています。

//image[startaws65][i（アイ）を押すと「-- INSERT --」と表示される「編集モード」になった][scale=0.8]{
//}

「編集モード」になったら次のようにバーチャルホストの設定を書いていってください。@<ttb>{「自分のドメイン名」のところはそのまま日本語で書かず、自分のドメイン名に置き換えてください}。

//cmd{
<VirtualHost *:80>
    DocumentRoot "/var/www/start-aws-documentroot"
    ServerName www.自分のドメイン名

    ErrorLog "logs/start-aws-error.log"
    CustomLog "logs/start-aws-access.log" combined

    <Directory "/var/www/start-aws-documentroot">
        AllowOverride All
    </Directory>
</VirtualHost>
//}

書き終わったらESCキーを押す（@<img>{startaws66}）と左下の「-- INSERT --」が消えて再び「閲覧モード」になります。

//image[startaws66][ESCを押すと左下の「-- INSERT --」が消えて再び「閲覧モード」になる][scale=0.8]{
//}

「閲覧モード」に戻ったら「:wq」と入力してEnterキーを押せば保存されます。（@<img>{startaws67}）

//image[startaws67][「閲覧モード」に戻ったら「:wq」と入力してEnterキーを押せば保存される][scale=0.8]{
//}

では確認のため、次のコマンドを叩いてみてください。（@<img>{startaws67-2}）

//cmd{
# cat /etc/httpd/conf.d/start-aws-virtualhost.conf
//}

//image[startaws67-2][catコマンドでstart-aws-virtualhost.confの中身を確認しよう][scale=0.8]{
//}

特に「ServerName」のところが日本語の「www.自分のドメイン名」ではなく、ちゃんと自分のドメイン名に置き換わっているか、を確認してください。たとえば筆者なら「startdns.fun」というドメイン名なので次のようになっています。

//cmd{
ServerName www.startdns.fun
//}

もしcatコマンドを叩いたときに「そのようなファイルやディレクトリはありません」と表示されたら、設定ファイル（start-aws-virtualhost.conf）が作成できていません。作り直しましょう。

=== 設定ファイルの構文チェック

バーチャルホストの設定ファイルが書けたのでapachectlコマンドで構文チェックをしてみましょう。apachectlは、名前のとおりApacheを操作するためのコントローラのようなもので、引数にconfigtestとつけてやると設定ファイルの構文チェックができます。

//cmd{
# apachectl configtest
AH00112: Warning: DocumentRoot [/var/www/start-aws-documentroot] does not exist
Syntax OK
//}

早速、警告のメッセージが表示されています。落ち着いて読んでみましょう。「@<ttb>{DocumentRoot [/var/www/start-aws-documentroot] does not exist}」と書いてあります。これは「@<ttb>{ドキュメントルートに[/var/www/start-aws-documentroot]というディレクトリを指定しているけど、そんなディレクトリは存在しないよ}」と言われているのです。

=== ドキュメントルートを作成

ドキュメントルート@<fn>{docRoot}とは「@<ttb>{サイトにアクセスしたらここに置いたファイルが表示されるよ}」というディレクトリのことです。つまりバーチャルホストの設定で

//footnote[docRoot][@<href>{https://httpd.apache.org/docs/2.4/ja/mod/core.html#documentroot}]

//cmd{
DocumentRoot "/var/www/start-aws-documentroot"
ServerName www.startdns.fun
//}

と書いてあったら、

//cmd{
/var/www/start-aws-documentroot/
//}

に置いた「index.html」が@<href>{http://www.startdns.fun/index.html} で見られるということです。

先ほどバーチャルホストの設定で次のように書いたのですが、この「/var/www/start-aws-documentroot」というディレクトリがまだ存在していないため、警告が出てしまっているようです。ですのでこのディレクトリを作成しましょう。

//cmd{
DocumentRoot "/var/www/start-aws-documentroot"
//}

ディレクトリを作るにはmkdir@<fn>{mkdir}というコマンドを使います。mkdirコマンドでディレクトリを作ったら、ちゃんと作成できたかlsコマンドで確認してみましょう。

//footnote[mkdir][mkdirはMaKe DIRectoryの略。]

//cmd{
# mkdir /var/www/start-aws-documentroot

# ls -l /var/www/
合計 0
drwxr-xr-x 2 root root 6  8月 25 03:55 cgi-bin
drwxr-xr-x 2 root root 6  8月 25 03:55 html
drwxr-xr-x 2 root root 6 12月 25 13:49 start-aws-documentroot
//}

これでドキュメントルートができました。再びapachectlコマンドで構文チェックをしてみましょう。今度は「Syntax OK」とだけ表示されるはずです。

//cmd{
# apachectl configtest
Syntax OK
//}

=== index.htmlを置いてみよう

ドキュメントルートを用意したのですが、何かしらコンテンツを置いておかないとアクセスしたときに「404 Not Found」になってしまうので、viコマンドでドキュメントルートの下にindex.htmlのファイルを用意しておきましょう。

//cmd{
# vi /var/www/start-aws-documentroot/index.html
//}

i（アイ）を押して「編集モード」に変わったら「AWSをはじめよう」と書いてみましょう。（@<img>{startaws68}）

//image[startaws68][i（アイ）を押して「-- INSERT --」と表示されたら「AWSをはじめよう」と書いてみよう][scale=0.8]{
//}

書き終わったらESCキーを押して「閲覧モード」に戻り、「:wq」と入力してEnterキーを押して保存しましょう。（@<img>{startaws69}）

//image[startaws69][ESCを押して「閲覧モード」に戻ったら「:wq」で保存][scale=0.8]{
//}

それではバーチャルホストの設定を有効にするため、apachectlコマンドでApacheを再起動しましょう。

//cmd{
# apachectl restart
//}

何のメッセージも表示されなければ、問題なくApacheが再起動できています。

=== curlでページを確認しよう

これでバーチャルホストの設定は完了です。ウェブサーバに「自分のウェブサイトのページ見せて」と頼んだら、ちゃんとウェブページを返してくれるのか確認してみましょう。次のようなcurlコマンドを叩いてみてください。@<ttb>{「www.自分のドメイン名」の部分は自分のドメイン名に置き換えます}。たとえば筆者なら「startdns.fun」というドメイン名なので「www.startdns.fun」にします。

//cmd{
# curl -H "Host:www.自分のドメイン名" http://localhost/index.html
AWSをはじめよう
//}

これはcurl（カール）という「ターミナルにおけるブラウザ」のようなコマンドを使って、localhost（自分自身）の「www.自分のドメイン名」というホストに対して「ページを見せて」というリクエストを投げています。ちゃんと自分のバーチャルホストが応答して、ドキュメントルートにあるindex.htmlの「AWSをはじめよう」が表示されましたね。おめでとうございます！

== Route53で自分のサイトのドメイン名を設定しよう

続いてブラウザでも「@<href>{http://www.自分のドメイン名}」を開いてサイトを確認してみましょう。（@<img>{startaws70}）するとなんと「アクセスしようとしているサイトを見つけられません。」と表示されてしまいました。

//image[startaws70][ブラウザで「www.自分のドメイン名」を開いたらエラーになってしまった][scale=0.8]{
//}

これはまだ「www.自分のドメイン名」というドメイン名と、ウェブサーバのIPアドレスをつなぐAレコードが存在していないからです。Route53でAレコードを作りましょう。

=== 自分のサイトのドメイン名を作ろう

マネジメントコンソールの左上にある「サービス」から、「ネットワーキングとコンテンツ配信」の下にある「Route53」（@<img>{startaws71}）をクリックしてください。

//image[startaws71][サービス＞ネットワーキングとコンテンツ配信＞Route53][scale=0.8]{
//}

Route53ダッシュボードを開いたらDNS管理の「ホストゾーン」をクリック（@<img>{startaws72}）します。

//image[startaws72][「ホストゾーン」をクリック][scale=0.8]{
//}

「ドメイン名」の列にある自分のドメイン名（筆者の場合はstartdns.fun）をクリック（@<img>{startaws73}）します。

//image[startaws73][自分のドメイン名をクリック][scale=0.8]{
//}

「レコードを作成」をクリック（@<img>{startaws74}）してください。

//image[startaws74][「レコードを作成」をクリック][scale=0.8]{
//}

「レコードのクイック作成」が表示されたら、レコード名には「www」、値にはElastic IPを入力（@<img>{startaws75}）@<fn>{elasticIpIs}してください。それ以外の箇所は何も変更せずそのままで構いません。入力できたら「レコードを作成」をクリックします。

//footnote[elasticIpIs][Elastic IPはメモしてあると思いますが、もしぱっと出てこなかったら、1つ前の画面の「login.自分のドメイン名」のところにも書いてあるのでそれを見ても構いません。]

//image[startaws75][レコード名には「www」、値にはElastic IPを入力して「レコードを作成」][scale=0.8]{
//}

「startdns.fun のレコードが正常に作成されました。」と表示されます。これで「www.自分のドメイン名」（@<img>{startaws76}）というAレコードが作成できました。

//image[startaws76][「www.自分のドメイン名」というAレコードができた][scale=0.8]{
//}

=== 【ドリル】/（スラッシュ）で終わるURLを開いたときにindex.html以外を返したい

==== 問題

@<href>{http://www.startdns.fun/}を開くとファイル名は指定していないのにindex.htmlが表示されます。このようにファイル名を省略して「/（スラッシュ）」で終わるURLを開いたとき、index.htmlではなくneko.htmlを表示させたかったらApacheの設定ファイル（httpd.conf）でどの設定を書き換えればよいでしょうか？

 * A. DirectorySlashディレクティブ
 * B. DirectoryIndexディレクティブ
 * C. Directoryディレクティブ

//raw[|latex|\begin{reviewimage}\begin{flushright}\includegraphics[width=0.5\maxwidth\]{./images/answerColumnShort.png}\end{flushright}\end{reviewimage}]

==== 解答

正解はBです。grepコマンドを使ってApacheの設定ファイル（httpd.conf）で「index.html」を検索してみると、次のような設定が見つかります。

//cmd{
# grep index.html /etc/httpd/conf/httpd.conf 
    DirectoryIndex index.html
//}

DirectoryIndexディレクティブ@<fn>{directoryIndex}ではファイル名を省略して「/（スラッシュ）」で終わるURLを開いたときに返すファイルの名前を指定できます。次のように複数指定した場合は「index.html」「index.php」「index.txt」の順に探して、最初に見つかったものを返します。

//cmd{
DirectoryIndex index.html index.php index.txt
//}

//footnote[directoryIndex][@<href>{https://httpd.apache.org/docs/2.4/ja/mod/mod_dir.html#directoryindex}]

=== ブラウザでページを見てみよう

Aレコードの追加が終わったら再びブラウザで「@<href>{http://www.自分のドメイン名}」を開いてみましょう。（@<img>{startaws77}）今度こそindex.htmlの「AWSをはじめよう」が表示されました。おめでとうございます！

//image[startaws77][index.htmlの「AWSをはじめよう」が表示された][scale=0.8]{
//}

=== アクセスログとエラーログの大切さ

ブラウザ上ではサイトが表示されましたが、サーバ側でもアクセスログを確認してみましょう。

//cmd{
# tail -f /etc/httpd/logs/start-aws-access.log
//}

tailコマンドに-fオプションを付けると、ファイルの追記を監視できるので、今来ているアクセスのログをタイムリーに確認できます。何回かEnterキーを叩いて改行したら、この状態でブラウザの再読込ボタンをクリックしてみましょう。ブラウザでページを再読込するたびに、そのアクセスログがターミナルで表示されると思います。（@<img>{startaws78}）tailコマンドに-fオプションを付けた場合、Ctrl+cで抜けるまでずっとログの追記が表示され続けます。

//image[startaws78][tail -fでログを確認しながらブラウザでサイトの表示を更新してみよう][scale=0.8]{
//}

念のためエラーログも確認してみましょう。アクセスログ（2.9K）に対してエラーログはファイルサイズが0です。エラーログは1行も出ていないようですので問題ありません。

//cmd{
# ls -lh /etc/httpd/logs/start-aws-*
-rw-r--r-- 1 root root 2.9K 12月 25 15:06 /etc/httpd/logs/start-aws-access.log
-rw-r--r-- 1 root root    0 12月 25 14:54 /etc/httpd/logs/start-aws-error.log
//}

たとえばサイトが上手く表示されないとき、アクセスログやエラーログを確認すれば「サーバまでたどり着いていない」のか「サーバにはたどり着いているけれど、ドキュメントルートに置いたPHPファイルのエラーでちゃんとページが出ない」のか、といった問題の切り分けができます。上手くいかないときはログを見るようにしましょう。