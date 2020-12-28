= WordPressでサイトを作ろう

この章ではWordPressをインストールしてウェブサイトを作ります。
大変な道のりでしたがいよいよおしゃれなサイトがお目見えです！

//pagebreak

== WordPressのインストール

これからEC2のインスタンスでWordPressのインストールを行います。Windowsの方はRLoginを起動して「start-aws-instance」に接続してください。Macの方はターミナルで次のコマンドを実行してください。

//cmd{
ssh ec2-user@login.自分のドメイン名 -i ~/Desktop/start-aws-keypair.pem
//}

「Amazon Linux 2 AMI」と表示されたらログイン完了です。ここからの作業はrootにならずに@<ttb>{ec2-userのまま}で行ってください。

=== WordPressをダウンロード

先ずはcurlコマンドを使って最新のWordPress（@<href>{https://wordpress.org/latest.tar.gz}）をダウンロードしましょう。curlコマンドは何もオプションをつけないと結果をそのまま画面に出力しますが、-Oオプションを付けるとファイルとして保存してくれます。

//cmd{
$ curl -O https://wordpress.org/latest.tar.gz
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
100 14.7M  100 14.7M    0     0  5072k      0  0:00:02  0:00:02 --:--:-- 5071k
//}

ダウンロードしたファイルをlsコマンドで確認してみましょう。

//cmd{
$ ls -lh /home/ec2-user/
合計 15M
-rw-rw-r-- 1 ec2-user ec2-user 15M 12月 26 00:37 latest.tar.gz
//}

圧縮された最新版のWordPress（latest.tar.gz）をダウンロードできました。

=== 展開してドキュメントルートに設置

実はWordPressのインストールと言っても、今ダウンロードした圧縮ファイルを展開してドキュメントルートに置き、後はブラウザでぽちぽちと進めていくだけなので簡単です。

それでは圧縮・解凍ソフトのようなtarコマンドを使ってWordPressの圧縮ファイル（latest.tar.gz）を展開します。lsコマンドで確認すると、latest.tar.gzを展開したことによって、wordpressというディレクトリができていることがわかります。

//cmd{
$ tar -xzf /home/ec2-user/latest.tar.gz

$ ls -lh /home/ec2-user/
合計 15M
-rw-rw-r-- 1 ec2-user ec2-user  15M 12月 26 00:37 latest.tar.gz
drwxr-xr-x 5 ec2-user ec2-user 4.0K 12月  9 07:13 wordpress
//}

展開できたらmvコマンド@<fn>{mv}を使ってwordpressディレクトリの中身をすべてドキュメントルートに移動させましょう。特にいちばん後ろのスラッシュは書き忘れないように注意してください。

//footnote[mv][mvはmoveの略。ファイルを移動させたりファイル名を変更したりできるコマンドです。]

//cmd{
$ sudo mv /home/ec2-user/wordpress/* /var/www/start-aws-documentroot/
//}

今後、WordPressの管理画面から画像ファイルをアップしたりプラグイン@<fn>{plugin}をインストールしたりするためには、ドキュメントルート以下のファイルやディレクトリに対して、ec2-userではなくapacheユーザが権限を持っておく必要があります。chownコマンド@<fn>{chown}でドキュメントルート以下のオーナー（持ち主）をapacheユーザに変更しておきましょう。

//footnote[plugin][WordPressの拡張機能のこと。]
//footnote[chown][chownはchange ownershipの略。]

//cmd{
$ sudo chown apache:apache -R /var/www/start-aws-documentroot
//}

chownコマンドを実行した後、lsコマンドで確認して次のように表示されていれば問題ありません。

//cmd{
$ ls -ld /var/www/start-aws-documentroot
drwxr-xr-x 5 apache apache 4096 12月 26 00:40 /var/www/start-aws-documentroot
//}

それから@<chapref>{apacheSetting}で動作確認用に作った「AWSをはじめよう」と書いてあるindex.htmlファイルは削除しておきましょう。

//cmd{
$ sudo rm -f /var/www/start-aws-documentroot/index.html
//}

これでドキュメントルート以下にあるのは展開したWordPressのファイルだけになりました。

//cmd{
$ ls -lh /var/www/start-aws-documentroot/
合計 212K
-rw-r--r--  1 apache apache  405  2月  6  2020 index.php
-rw-r--r--  1 apache apache  20K  2月 12  2020 license.txt
-rw-r--r--  1 apache apache 7.2K  6月 26 22:58 readme.html
-rw-r--r--  1 apache apache 7.0K  7月 29 02:20 wp-activate.php
drwxr-xr-x  9 apache apache 4.0K 12月  9 07:13 wp-admin
-rw-r--r--  1 apache apache  351  2月  6  2020 wp-blog-header.php
-rw-r--r--  1 apache apache 2.3K 10月  9 06:15 wp-comments-post.php
-rw-r--r--  1 apache apache 2.9K  2月  6  2020 wp-config-sample.php
drwxr-xr-x  4 apache apache   52 12月  9 07:13 wp-content
-rw-r--r--  1 apache apache 3.9K  7月 31 04:14 wp-cron.php
drwxr-xr-x 25 apache apache 8.0K 12月  9 07:13 wp-includes
-rw-r--r--  1 apache apache 2.5K  2月  6  2020 wp-links-opml.php
-rw-r--r--  1 apache apache 3.3K  2月  6  2020 wp-load.php
-rw-r--r--  1 apache apache  49K 11月  9 19:53 wp-login.php
-rw-r--r--  1 apache apache 8.4K  4月 14  2020 wp-mail.php
-rw-r--r--  1 apache apache  21K 11月 12 23:43 wp-settings.php
-rw-r--r--  1 apache apache  31K 10月  1 06:54 wp-signup.php
-rw-r--r--  1 apache apache 4.7K 10月  9 06:15 wp-trackback.php
-rw-r--r--  1 apache apache 3.2K  6月  9  2020 xmlrpc.php
//}

=== サイトにアクセスしてインストール実行

それではブラウザで@<href>{http://www.自分のドメイン名/}を開いてください。筆者なら自分のドメイン名は「startdns.fun」なので@<href>{http://www.startdns.fun/}を開きます。

するとWordPressの言語選択画面（@<img>{startaws112}）が表示されるので「日本語」を選択して「次へ」をクリックします。

//image[startaws112][「日本語」を選択して「次へ」をクリック][scale=0.8]{
//}

続いて「WordPressへようこそ。」という画面（@<img>{startaws113}）が表示されたら「さあ、始めましょう！」をクリックします。

//image[startaws113][「さあ、始めましょう！」をクリック][scale=0.8]{
//}

データベースの接続情報を入力する画面（@<img>{startaws114}）が表示されたら、@<chapref>{database}で設定した内容を思い出しながら次のように入力します。（@<table>{dbAuth}）テーブル接頭辞は変更せずそのままで構いません。

//table[dbAuth][データベースの接続情報]{
データベース名	start_aws_wordpress_dbname（最初のデータベース名）
ユーザー名	start_aws_dbuser（マスターユーザ名）
パスワード	start_aws_db_password（マスターパスワード）
データベースのホスト名	RDSのエンドポイント
//}

RDSのエンドポイントは「start-aws-db-instance.cesouf5kakle.ap-northeast-1.rds.amazonaws.com」のような長いドメイン名です。パソコンのメモ帳にメモしてあると思いますので、そこからコピーペーストしましょう。すべて入力したら「送信」をクリックします。

//image[startaws114][データベースの接続情報を入力して「送信」をクリック][scale=0.8]{
//}

データベースの接続情報に誤りがなく、WordPressがデータベースと繋がったら「この部分のインストールは無事完了しました。WordPress は現在データベースと通信できる状態にあります。準備ができているなら…」と表示（@<img>{startaws115}）されます。「インストール実行」をクリックしましょう。

//image[startaws115][「インストール実行」をクリック][scale=0.8]{
//}

「WordPressの有名な5分間インストールプロセスへようこそ!」と表示（@<img>{startaws116}）されたら、サイトのタイトルなどを入力します。サイトのタイトルは好きなものにしてください。ユーザー名やパスワードはWordPressの管理画面にログインするときに使用しますのであなた自身で決めてメモ（@<table>{wpAdminMemo}）しておいてください。「検索エンジンでの表示」はどちらでも構いませんが、作ったサイトをGoogleなどの検索結果に表示したくなければチェックを入れておきましょう。もしパスワードを忘れてしまった場合、ここに書いたメールアドレス宛てにパスワードリセットのメールが届きますのでメールアドレスは正確に入力してください。すべて入力したら「WordPressをインストール」をクリックします。

//table[wpAdminMemo][WordPress管理画面に入るための情報をメモ]{
------------------------------------
ユーザー名	　　　　　　　　　　　　　　　　　　　　
パスワード	　　　　　　　　　　　　　　　　　　　　
メールアドレス	　　　　　　　　　　　　　　　　　　　　
//}

//image[startaws116][「WordPressをインストール」をクリック][scale=0.8]{
//}

「成功しました!」と表示（@<img>{startaws117}）されたらWordPressのインストールは完了です。「ログイン」をクリックしてWordPressの管理画面に入ってみましょう。

//image[startaws117][「ログイン」をクリックしてWordPressの管理画面に入ってみよう][scale=0.8]{
//}

== 管理画面にログイン

WordPressの管理画面に入るため、先ほど設定したユーザー名とパスワードを入力（@<img>{startaws118}）したら「ログイン状態を保存する」にチェックを入れて「ログイン」をクリックします。

//image[startaws118][ユーザー名とパスワードを入力して「ログイン」をクリック][scale=0.8]{
//}

もし左メニューで「更新」に赤いマーク（@<img>{startaws119}）がついていたら、クリックしてWordPress本体やプラグインの更新をしておきましょう。

//image[startaws119][「更新」に赤いマークがついていたら「更新」をクリック][scale=0.8]{
//}

WordPress本体やプラグイン、テーマなどで新しいバージョンが出ていたら、きちんと更新（@<img>{startaws120}）しておきましょう。@<fn>{security}

//footnote[security][WordPressでは定期的に脆弱性が発見され、対策済みのバージョンが公開されます。脆弱性（ぜいじゃくせい）というのは悪用が可能なバグや設定不備のことです。もしWordPressに脆弱性が見つかって対策済みの新しいバージョンのWordPressが公開されたとしても、あなたが更新をしないとウェブサイトは脆弱性がある危険な状態のままで放置されていることになります。古いバージョンのWordPressを使っていると、サイトを改ざんしてウイルスをばらまかれたり、データベースの個人情報を盗まれたりする可能性もありますので、新しいバージョンが出たらきちんと更新しましょう。]

//image[startaws120][新しいバージョンが出ていたら更新しよう][scale=0.8]{
//}

更新すると進捗が表示されます。「すべての更新が完了しました。」と表示（@<img>{startaws121}）されたら、できあがったサイトを見てみましょう。左上の家のマークをクリックしてください。

//image[startaws121][更新完了したら家のマークをクリック][scale=0.8]{
//}

そしてこちらが完成したWordPressのおしゃれなサイトです！（@<img>{startaws122}）ここからは見た目をカスタマイズするもよし、技術的なことを調べたブログ記事を日々綴っていくもよし、自分のサイトを自由に楽しんでください。

//image[startaws122][できあがった「自分のサイト」をここからは自由にカスタマイズしよう][scale=0.8]{
//}

たとえばテーマを「Twenty Twenty-One」から「Twenty Seventeen」に変えただけで、こんなにがらっと見た目（@<img>{startaws123}）が変わります。

//image[startaws123][テーマを変えると見た目もがらっと変わる][scale=0.8]{
//}

=== 【ドリル】WordPressからのパスワードリセットメールが届かない

==== 問題

「AWSをはじめよう」を読みながら構築した環境で、WordPressの管理画面に入るパスワードを忘れてしまったためパスワードのリセットを行いました。ところがいくら待ってもパスワードリセットのメールがGmailに届きません。どうすればメールが届くのでしょう？

 * A. WordPressの管理画面でメール送信元の設定をすれば届くようになる
 * B. マネジメントコンソールからEメール送信制限解除申請をすれば届くようになる
 * C. 実は迷惑メールのトレイに届いている

//raw[|latex|\begin{reviewimage}\begin{flushright}\includegraphics[width=0.5\maxwidth\]{./images/answerColumnShort.png}\end{flushright}\end{reviewimage}]

==== 解答

正解はBです。EC2のインスタンスからメールを送信しようとしても、実は初期状態では25番ポート@<fn>{port25}に対する通信の制限がかけられているので、メールの送信ができません@<fn>{limitAmount}。つまりサーバからスパムメールをむやみやたらと送れないよう、初期制限がかかっているのです。メールを送信したい場合は、マネジメントコンソールで「Eメール送信制限解除申請@<fn>{email}」を行いましょう。@<fn>{spf}

//footnote[port25][メール送信で使用するポート番号。]
//footnote[limitAmount][「Eメール送信制限解除申請」のページには"We enforce default limits on the amount of email that can be sent from EC2 accounts."（EC2アカウントから送信できるメールの量にデフォルトの制限をかけています）とあるので、あくまで「量に制限をかけている」ように読めるのですが、筆者が2020年12月に試したときは、そもそも外部のメールサーバに対して25番で通信しようとすると「Connection timed out」になってしまい、最初からまったく送れない状態だったため、無料利用枠で立てたインスタンスの場合は、送れる量の上限がそもそも0になっていて一切送れないのでは？と想像しています。この制限について詳しくは、「EC2インスタンスからポート25の制限を削除するにはどうすればよいですか？」を参照してください。 @<href>{https://aws.amazon.com/jp/premiumsupport/knowledge-center/ec2-port-25-throttle/}]
//footnote[email][@<href>{https://aws-portal.amazon.com/gp/aws/html-forms-controller/contactus/ec2-email-limit-rdns-request}]
//footnote[spf][メールが迷惑メールと判定されずにきちんと宛先へ届くためには、この制限解除とはまた別に、SPFレコードやPTRレコード（逆引き）の設定も必要です。その辺りは「DNSをはじめよう」で説明しているので、本書では省略します。]

ここでは詳しく説明しませんが、いっそインスタンスからは直接メールを送信せず、代わりにAmazon SES@<fn>{ses}というサービスを使ってメールを送信する、という方法もあります。

//footnote[ses][Amazon Simple Email Serviceの略。メールを送信したり、受信したりするためのサービスです。]

=== 管理画面にダイジェスト認証をかけよう

ところでWordPressは利用者が多いため、その管理画面を乗っ取ろうと狙ってくる攻撃も多いです。「こんな作ったばかりの小さなブログに攻撃なんか来ないのでは？」と思われるかもしれませんが、攻撃者はIPアドレスを端から順番に試していくだけなので、@<ttb>{サイトの開設時期や規模にかかわらず、どんなサーバでも攻撃はされる}と思って間違いありません。

管理画面にログインするにはユーザー名とパスワードの認証が必要ですが、安全のためその手前にもうひとつ「ダイジェスト認証」という認証をかけておきましょう。EC2のインスタンスでコマンドを叩きますのでRLoginやターミナルに戻ってrootになってください。

//cmd{
$ sudo su -
//}

rootになったら先ずはhtdigestコマンドを使ってダイジェスト認証のパスワードファイルを作成します。ここではユーザー名を「start-aws-digest-user」、パスワードを「start-aws-digest-password」としますが、もし自分で考えたユーザー名やパスワードにした場合は忘れないように必ずメモしておきましょう。（@<table>{digestUserData}）

//table[digestUserData][ダイジェスト認証情報]{
ダイジェスト認証情報	例	自分のダイジェスト認証情報
------------------------------------
ユーザー名	start-aws-digest-user	
パスワード	start-aws-digest-password	
//}

//cmd{
# htdigest -c /etc/httpd/conf/htdigest wp-admin-area start-aws-digest-user
//}

「New password:」と表示されたら、パスワードの「start-aws-digest-password」を入力してEnterキーを押してください。再度「Re-type new password:」とパスワードを求められますので、もう一度「start-aws-digest-password」と入力してEnterキーを押します。

//cmd{
New password:
Re-type new password:
//}

これでダイジェスト認証のパスワードファイルが生成できました。catコマンドでパスワードファイルを見てみましょう。

//cmd{
# cat /etc/httpd/conf/htdigest
start-aws-digest-user:wp-admin-area:10b74c85e2a0273d00eee83755e329b8
//}

続いてviコマンドでバーチャルホストにダイジェスト認証の設定を書き足します。

//cmd{
# vi /etc/httpd/conf.d/start-aws-virtualhost.conf 
//}

i（アイ）を押して「編集モード」になったら、次のようなダイジェスト認証の設定をVirtualHostディレクティブの中に追記します。

//cmd{
<Directory "/var/www/start-aws-documentroot/wp-admin">
    AuthType Digest
    AuthName wp-admin-area
    AuthUserFile /etc/httpd/conf/htdigest
    Require valid-user
</Directory>
//}

書き終わったらESCキーを押して「閲覧モード」に戻り、「:wq」と入力してEnterキーを押せば保存されます。保存できたら、確認のためcatコマンドを叩いて、ダイジェスト認証の設定が追加されているか確認してください。

//cmd{
# cat /etc/httpd/conf.d/start-aws-virtualhost.conf
<VirtualHost *:80>
    DocumentRoot "/var/www/start-aws-documentroot"
    ServerName www.startdns.fun

    ErrorLog "logs/start-aws-error.log"
    CustomLog "logs/start-aws-access.log" combined

    <Directory "/var/www/start-aws-documentroot">
        AllowOverride All
    </Directory>

    <Directory "/var/www/start-aws-documentroot/wp-admin">
        AuthType Digest
        AuthName wp-admin-area
        AuthUserFile /etc/httpd/conf/htdigest
        Require valid-user
    </Directory>
</VirtualHost>
//}

apachectlで構文チェックをしてみましょう。「Syntax OK」とだけ表示されれば問題ありません。

//cmd{
# apachectl configtest
Syntax OK
//}

それではダイジェスト認証の設定を有効にするため、apachectlでApacheを再起動しましょう。

//cmd{
# apachectl restart
//}

何のメッセージも表示されなければ問題なくApacheが再起動できています。それではブラウザで確認してみましょう。

先ずは管理画面ではなくブログの方を表示してみましょう。ブラウザで@<href>{http://www.自分のドメイン名/}を開いてください。こちらは先ほどまでと代わらず普通に表示（@<img>{startaws126}）されるはずです。

//image[startaws126][ブログは認証なしで普通に表示される][scale=0.8]{
//}

続いてブラウザでWordPressの管理画面（@<href>{http://www.自分のドメイン名/wp-admin/}）を開いてみましょう。すると「ユーザー名とパスワードを入力してください」というダイジェスト認証のポップアップが表示（@<img>{startaws125}）されますので、先ほど設定したダイジェスト認証のユーザー名とパスワードを入力して「OK」をクリックしてください。（@<table>{digestAuth}）

//table[digestAuth][ダイジェスト認証の例]{
------------------------------------
ユーザー名	start-aws-digest-user
パスワード	start-aws-digest-password
//}

//image[startaws125][管理画面では「ユーザー名とパスワードを入力してください」というダイジェスト認証のポップアップが表示された][scale=0.8]{
//}

正しいユーザー名とパスワードを入れれば管理画面に入るためのログイン画面（もしくは管理画面）が表示（@<img>{startaws127}）されますが、間違ったものを入力すると「Unauthorized」と表示（@<img>{startaws128}）されて管理画面には入れません。これで管理画面は「ダイジェスト認証」と「WordPressの認証」の2段階@<fn>{3rdAuth}で守られるようになりました。

//image[startaws128][正しいユーザー名とパスワードを入れないと「Unauthorized」と表示されて管理画面に入れない][scale=0.8]{
//}

//image[startaws127][管理画面が「ダイジェスト認証」と「WordPressの認証」の2段階で守られるようになった][scale=0.8]{
//}

//footnote[3rdAuth][自宅のIPアドレスが固定の人はIP認証もかけておくとさらに安心です。]

== 画像をS3に保存する

ところでWordPressで画像をアップすると、画像ファイルはドキュメントルート以下にある「wp-content/uploads」というディレクトリに保存されます。

//cmd{
# ls -l /var/www/start-aws-documentroot/wp-content/uploads/
合計 0
drwxr-xr-x 3 apache apache 16 12月 26 01:04 2020
//}

ですが画像をEC2インスタンス内に保存することによって、次のようなデメリットが発生します。

 1. アクセス数が増えてきて、負荷分散のためウェブサーバの台数を増やしたときに、画像が片方のサーバにしか保存されず、もう1台のサーバでは記事内の画像が表示されなくなってしまう
 2. ウェブサーバが壊れてAMI@<fn>{ami}から復元したとき、画像のフォルダがAMIを作った時点まで先祖返りしてしまい、記事内の画像が表示されなくなってしまう

//footnote[ami][AMIについては@<chapref>{backup}で解説します。]

そこで台数を増やしたりサーバを復元したりしたときでも、記事内の画像が問題なく表示されるよう、WordPressでアップした画像はインスタンス内に保存するのではなくAmazon S3に保存するようにしておきましょう。

=== Amazon S3とは？

Amazon S3というのは「Amazon Simple Storage Service」の略で、頭文字のSが3つなのでS3です。シンプルストレージという名前のとおり、S3はDropboxやGoogleクラウドのようにファイルを保存しておけるサービスです。S3は99.999999999%@<fn>{nine}の耐久性をもち、格納できるデータの容量も無制限ですので「ハードディスクが壊れて保存してた画像が全部消えちゃった…」「容量がいっぱいになっちゃってこれ以上保存できない！」といったトラブルとも無縁です。

//footnote[nine][9が11並んでいるのでイレブンナインと読みます。格好いい…！]

S3に保存した画像ファイルにはすべてURLが付与され、HTTPSでどこからでもアクセスができます。

=== S3アップ用のIAMロールを作ろう

皆さんがマネジメントコンソールへログインするときに使っている「IAMユーザー」@<fn>{iamUser}とはまた別に、AWSで作ったサービスやインスタンスに対して権限を付与できる「IAMロール」という仕組みがあります。

//footnote[iamUser][本書ではIAMユーザーのstart-aws-user、またはルートユーザーでマネジメントコンソールへログインしています。]

WordPressでアップしたファイルをS3に保存するため、IAMロールを作って、EC2のインスタンスにそのIAMロールをアタッチ（紐付け）してあげましょう。

==== ルートユーザーでログインしてIAMダッシュボードを開く

IAMに関する操作は、現状のIAMユーザー（start-aws-user）ではできないため、先ずはマネジメントコンソールにルートユーザでログイン（@<img>{startaws147-1}）してください。

//image[startaws147-1][マネジメントコンソールにルートユーザでログイン][scale=0.8]{
//}

ログインできたら、マネジメントコンソールの左上にある「サービス」を開いて、「セキュリティ、ID、およびコンプライアンス」の下にある「IAM」（@<img>{selectIam}）をクリックしてください。

//image[selectIam][サービス＞セキュリティ、ID、およびコンプライアンス＞IAM][scale=0.8]{
//}

「IAM」をクリックすると、IAMのダッシュボード（@<img>{iamDashboardAgain}）が表示されます。

//image[iamDashboardAgain][IAMダッシュボード][scale=0.8]{
//}

==== IAMロールを作成

左メニューの「ロール」を開いて、IAMロールの一覧が表示されたら「ロールの作成」をクリック（@<img>{startaws147-2}）します。ここからは4つのステップでIAMロールを作成していきます。

//image[startaws147-2][「ロール」を開いて「ロールの作成」をクリック][scale=0.8]{
//}

===== ステップ1. 信頼関係

今回はEC2のインスタンスに対して、「S3に画像ファイルを保存したり、S3から画像ファイルを読み込んだりする」という権限を付与してあげたいので、「信頼されたエンティティの種類」は「AWSサービス」、「ユースケースの選択」は「EC2」を選択（@<img>{startaws147-3}）します。「次のステップ: アクセス権限」をクリックしてください。

//image[startaws147-3][「AWSサービス」の「EC2」を選択して「次のステップ: アクセス権限」をクリック][scale=0.8]{
//}

===== ステップ2. アクセス権限

作成するIAMロールに対して、どんな権限を付与するかを設定します。「AmazonS3FullAccess」で検索（@<img>{startaws147-4}）して、チェックを入れたら「次のステップ: タグ」をクリックします。

//image[startaws147-4][「AWSサービス」の「EC2」を選択して「次のステップ: アクセス権限」をクリック][scale=0.8]{
//}

===== ステップ3. タグ

今回は特にタグは設定しませんので、そのまま「次のステップ: 確認」をクリック（@<img>{startaws147-5}）してください。

//image[startaws147-5][何も変更せず「次のステップ: 確認」をクリック][scale=0.8]{
//}

===== ステップ4. 確認

「ロール名」に「s3-upload-role」と入力（@<img>{startaws147-6}）します。「ロールの説明」はそのままで構いません。「ポリシー」が「AmazonS3FullAccess」になっていることを確認したら「ロールの作成」をクリックします。

//image[startaws147-6][何も変更せず「次のステップ: 確認」をクリック][scale=0.8]{
//}

「ロール s3-upload-role が作成されました。」と表示（@<img>{startaws147-7}）されたら、IAMロールの作成は完了です。

//image[startaws147-7][何も変更せず「次のステップ: 確認」をクリック][scale=0.8]{
//}

==== EC2インスタンスにIAMロールをアタッチ

次は、今作ったIAMロールを、EC2のインスタンスに紐付ける作業を行います。マネジメントコンソールの左上にある「サービス」から、「コンピューティング」の下にある「EC2」（@<img>{iamAttach1}）をクリックしてください。

//image[iamAttach1][サービス＞コンピューティング＞EC2][scale=0.8]{
//}

左メニューの「インスタンス」を開いて、「start-aws-instance」にチェックを入れたら、「アクション＞セキュリティ」から「IAMロールを変更」をクリック（@<img>{iamAttach2}）します。

//image[iamAttach2][「アクション＞セキュリティ」から「IAMロールを変更」をクリック][scale=0.8]{
//}

IAMロールで「s3-upload-role」を選択（@<img>{iamAttach3}）して、「保存」をクリックします。

//image[iamAttach3][「s3-upload-role」を選択して「保存」をクリック][scale=0.8]{
//}

「s3-upload-role をインスタンス i-0b96448d17e7dadee に正常にアタッチしました」と表示（@<img>{iamAttach3}）されたら、IAMロールをEC2インスタンスに紐付ける作業は完了です。

//image[iamAttach4][「s3-upload-role」を選択して「保存」をクリック][scale=0.8]{
//}

これでEC2インスタンスが、ひいてはその上で動くWordPressがS3に画像を保存したり、S3から画像を読み込んだりできるようになりました。WordPressプラグインのインストールに進みましょう。

=== WordPressにS3を使うためのプラグインを入れよう

WordPressのプラグイン（拡張機能）を使って、記事の画像をS3に保存する設定を行います。使用するプラグインは次のとおりです。

 * WP Offload Media Lite

==== WP Offload Media Liteプラグイン

ブラウザでWordPressの管理画面（@<href>{http://www.自分のドメイン名/wp-admin/}）を開き、ダイジェスト認証と管理画面へのログインを行います。管理画面にログインできたら、「プラグイン」の「新規追加」をクリック（@<img>{startaws149}）して、「プラグインの検索...」と書かれたところに「WP Offload Media Lite」と入力します。「WP Offload Media Lite for Amazon S3, DigitalOcean Spaces, and Google Cloud Storage」が表示されたら「今すぐインストール」をクリックします。

//image[startaws149][「WP Offload Media Lite」を検索して「今すぐインストール」をクリック][scale=0.8]{
//}

ボタンの表示が「インストール中」の後に「有効化」になったらクリック（@<img>{startaws150}）します。

//image[startaws150][「有効化」になったらクリック][scale=0.8]{
//}

WP Offload Media Liteプラグインがインストールできたので「Settings」をクリック（@<img>{startaws151}）します。

//image[startaws151][WP Offload Media Liteプラグインの「Settings」をクリック][scale=0.8]{
//}

まずはProviderの設定をします。「Amazon S3」の「My server is on Amazon Web Services and I'd like to use IAM Roles」を選択して、「Next」をクリックします。

//image[startaws151-2][「Amazon S3」の「My server（中略）use IAM Roles」を選択して「Next」をクリック][scale=0.8]{
//}

続いて「Create new bucket」をクリック（@<img>{startaws152}）します。WordPressでアップした画像を保存しておくための「バケット」と呼ばれる入れ物が、S3にまだないので作ります。

//image[startaws152][バケットがまだないので「Create new bucket」をクリック][scale=0.8]{
//}

「Region」は「Asia Pacific (Tokyo)」を選択し、「Bucket Name」に「start-aws-wordpress-bucket.自分のドメイン名」と入力（@<img>{startaws153}）したら「Create new bucket」をクリックします。筆者の場合は「Bucket Name」は「start-aws-wordpress-bucket.startdns.fun」です。

//image[startaws153][バケットの名前とリージョンを入れて「Create new bucket」をクリック][scale=0.8]{
//}

「設定を保存しました。」と表示（@<img>{startaws154}）されました。これでバケットの作成が完了して、WordPressでアップした画像はS3へ保存されるようになりました。

//image[startaws154][バケットの作成が完了して画像がS3へ保存されるようになった][scale=0.8]{
//}

「WP Offload Media Lite」の設定画面に戻って、「Force HTTPS」をオン（@<img>{startaws155}）にしたら「Save Changes」をクリックします。@<fn>{https}

//footnote[https][デフォルトだと「ページをHTTPで開いたら画像もHTTPで表示、ページをHTTPSで開いたら画像もHTTPSで表示」という設定なのですが、「Force HTTPS」をオンにしておくと「画像は常にHTTPSで表示」になります。今後サイトをHTTPS化したときに、画像のURLをHTTPからHTTPSへ書き換えなくて済むよう、最初からオンにしておくことをお勧めします。]

//image[startaws155][「Force HTTPS」をオンにして「Save Changes」をクリック][scale=0.8]{
//}

「設定を保存しました。」と表示されたらWP Offload Media Liteプラグインの設定は完了@<fn>{cloudFront}です。

//footnote[cloudFront][ちなみにWP Offload Media LiteプラグインではS3ではなくCloudFront（クラウドフロント）を使う設定にもできるようです。本書では扱いませんがCloudFrontはCDN（Content Delivery Network）のサービスです。CloudFrontを使うと画像や動画などのコンテンツを世界中のCDNサーバを使って配信できるため、ウェブサイトが高速で表示されるようになります。]

=== 投稿を試してみよう

画像をS3にアップする設定がされたか確認するため、試しに記事を投稿してみましょう。WordPressで左メニューの「投稿」から「新規追加」をクリック（@<img>{startaws156}）したら、タイトルに「画像はS3にアップされるようになりました」と入力します。タイトルが入力できたら、「＋」をクリックして「画像」を選択します。

//image[startaws156][タイトルを入力したら「＋」をクリックして「画像」を選択][scale=0.8]{
//}

「アップロード」をクリック（@<img>{startaws157}）して適当な画像を選択します。

//image[startaws157][適当な画像を選択したら「投稿に挿入」をクリック][scale=0.8]{
//}

これで投稿に画像が挿入されました。「公開」をクリック（@<img>{startaws158}）します。「公開してもよいですか？公開する前に、設定を再確認しましょう。」と表示されたら、再度「公開」をクリックします。

//image[startaws158][「公開」をクリック][scale=0.8]{
//}

投稿が公開されたら「投稿を表示」をクリック（@<img>{startaws159}）してください。

//image[startaws159][「投稿を表示」をクリック][scale=0.8]{
//}

記事を確認したら画像を右クリックして「画像だけを表示」をクリック（@<img>{startaws160}）します。

//image[startaws160][画像を右クリックして「画像だけを表示」をクリック][scale=0.8]{
//}

すると画像のURLが「@<href>{https://s3.ap-northeast-1.amazonaws.com/start-aws-wordpress-bucket.startdns.fun/wp-content/uploads/2020/12/28161755/twitter_cover-1-1024x342.png}」のようになっている（@<img>{startaws161}）ので、WordPressでアップした画像がS3に保存されて記事に挿入されていることが分かります。

//image[startaws161][画像のURLがS3になっている][scale=0.8]{
//}

以上でWordPressのインストールと設定は完了です。お疲れさまでした！