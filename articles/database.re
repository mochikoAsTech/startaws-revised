= データベースサーバを立てよう

WordPressを使うにはMySQLというデータベースが必要です。
この章ではRDSでデータベースサーバを立てましょう。

//pagebreak

== WordPressにはデータベースが必要

=== CMSとは？

こちらをご覧ください！このおしゃれなブログはWordPressというCMSで動いています。（@<img>{startaws124}）

//image[startaws124][WordPressで作ったおしゃれなブログ][scale=0.8]{
//}

CMSとはContents Management Systemの略で、@<ttb>{ウェブサイトのコンテンツを管理したり、ウェブページを更新したりするためのソフト}のことです。CMSがあればHTMLやCSSなどを自分で直接編集しなくても、Wordのように文章を書いてレイアウトを整え、「更新」ボタンをクリックするだけでサイトが更新できます。

日本国内で人気のCMSはWordPress（ワードプレス）やMovable Type（ムーバブル・タイプ）ですが、どちらも個人利用であれば無料で使うことができます。今回はCMS界でトップシェアを誇るWordPressを使えるように環境を構築していきましょう。

WordPressの管理画面でブログ記事を書いて保存すると、記事のデータはデータベースに保存されます。そしてページを表示するときはデータベースからデータを取ってきて画面が構成されます。そのためWordPressを使うにはMySQLというデータベースが必須となっています。

===[column] 【コラム】MySQLとMariaDB

データベースには、Oracle DatabaseやPostgreSQLなど色々な種類があるのですが、その中でもMySQLは非常に普及率の高いデータベースです。

MySQLは、元々MySQL ABという会社が開発してオープンソースで公開していましたが、2008年にMySQL ABがSun Microsystemsに買収され、さらに2009年にSunがOracleに買収されたため、現在はOracleのものになっています。Oracleに買収された後も、ありがたいことにMySQLは引き続きオープンソースで公開されており、MySQL Community Editionは2020年12月現在も無償で使えます。しかしOracleはもともとOracle Databaseという有償のデータベースも販売しているため、MySQLはその競合にあたり、今は平気でもいつかはサポートが終了したりオープンソースで公開されなくなるのでは？という懸念もあります。

またMySQLを最初に作った開発者はすでにOracleを離れており、MariaDBというMySQLから派生した別のデータベースを開発しています。このことからLinuxのディストリビューションであるCentOS 6やAmazon LinuxではMySQLが採用されていましたが、CentOS 7及びAmazon Linux 2からはMariaDBに切り替わっています。しかもyumでMySQLをインストールしようとすると、何も言わずにそーっとMariaDBがインストールされる落とし穴仕様です。サーバの仕様を「OSはCentOS 7でDBはMySQLです」と決めていたのに、環境構築後によくよく確認したら入っていたのはMariaDBだった、というようなことにならないようご注意ください。

上記のような背景から、引き続きMySQLを使うべきか、今後はMariaDBに切り替えるべきかという懸念はここ数年ずっとある気がしますが、個人的にはOracleが「MySQLの開発とサポートやめた！」と言い出すまでは積極的にMariaDBへ切り替える理由もないかな、という気持ちです。

ちなみにWordPressはMySQLでもMariaDBでも動きますので、今回は深く考えずにMySQL@<fn>{sakila}を使いましょう。

===[/column]

//footnote[sakila][ちなみにMySQLのイベントで登壇するともらえる（ことがある）、Sakila（MySQLのロゴにいるイルカ）のぬいぐるみはとてもかわいいです。オラクルのMySQLに対する愛を感じるかわいさなのです。]

=== EC2にインストールするか？RDSを使うか？

AWSでMySQLを使いたいときには、ざっくり言うと次の2つの方法があります。

 1. EC2でインスタンスを作成してyumでMySQLをインストールする@<fn>{apache}方法
 2. Amazon RDSというサービスを使う方法

//footnote[apache][先ほどサーバにApacheをインストールしてウェブサーバを作ったのと同じように、MySQLをインストールしてデータベースサーバを作る、ということです。既にMySQLがインストールされたAMIからインスタンスを作ることもできます。]

Amazon RDSはAmazon Relational Database Serviceの略で、ざっくり言うとデータベースを提供してくれるサービスです。データベースに接続することはできますが、MySQLが動いているサーバに直接SSHでログインすることはできません。RDSではデータベースエンジンとしてMySQLだけでなくAmazon Aurora@<fn>{aurora}、Microsoft SQL Server、Oracle、PostgreSQLなども選択可能です。

//footnote[aurora][本当は「MySQLの最大5倍のパフォーマンス」といわれるMySQL互換のデータベース、Amazon Auroraを使いたかったのですがAWSの無料利用枠に含まれないため本書では断念しました。WordPressはAuroraでも動くようです。]

自力でインストールするのではなくRDSを使うことで、データのバックアップや脆弱性パッチの適用といった色々な面倒ごとをRDSにお任せできます。一方でRDSというサービス上で用意されていない機能は、たとえMySQLで実装されていても使えないため自由度は若干下がります。

旅行代理店のパック旅行を申し込めば、飛行機のチケットを取ってホテルの予約してレンタカーを借りて…といったこまごましたことを自分でしなくて済むから楽だけれど、代わりにできることや行けるところは限られていて自由度が下がる、というのと同じですね。今回はWordPressと繋いで使うだけで、自由度の高さは求めていないため、運用コストの低いRDSを選択します。

== インスタンスを立てる事前準備

マネジメントコンソールの左上にある「サービス」から、「データベース」の下にある「RDS」（@<img>{startaws82}）をクリックしてください。

//image[startaws82][サービス＞データベース＞RDS][scale=0.8]{
//}

RDSダッシュボードを開いたら左メニューの「パラメータグループ」をクリック（@<img>{startaws83}）します。

//image[startaws83][左メニューの「パラメータグループ」をクリック][scale=0.8]{
//}

=== パラメータグループを作成

RDSでのインスタンス作成に先駆けて、パラメータグループを作成します。パラメータグループが何なのかについては追って説明しますので、とりあえず右上にある「パラメータグループの作成」をクリック（@<img>{startaws84}）してください。

//image[startaws84][「パラメータグループの作成」をクリック][scale=0.8]{
//}

パラメータグループファミリーのプルダウンから「mysql8.0」を選択します。グループ名に「start-aws-parameter-group」、説明に「Supports Japanese」と入力したら「作成」をクリック（@<img>{startaws85}）します。

//image[startaws85][「mysql8.0」を選択してグループ名と説明を入力したら「作成」をクリック][scale=0.8]{
//}

パラメータグループの一覧に、今作成した「start-aws-parameter-group」が表示されました。「start-aws-parameter-group」をクリック（@<img>{startaws86}）してください。

//image[startaws86][「start-aws-parameter-group」をクリック][scale=0.8]{
//}

すると「パラメータ」がたくさん表示（@<img>{startaws89}）されました。パラメータというのはMySQLにおけるさまざまな設定値のことです。Apacheにhttpd.confという設定ファイルがあったように、MySQLにはmy.cnfという設定ファイルがあるのですが、RDSではmy.cnfを直接書き換える代わりにこのパラメータを変更します。

//image[startaws89][「パラメータ」がたくさん表示された][scale=0.8]{
//}

先ほど作ったパラメータグループというのは、このパラメータをまとめたグループのことなのですが、この後RDSでデータベースのインスタンスを作るときに「どのパラメータグループを使用するか？」という選択が出てくるため、インスタンスを作る前にパラメータグループを作っておく必要があったのです。

=== パラメータの設定

ここでは2つのパラメータの値を設定します。@<fn>{mysql57}

//footnote[mysql57][日本語や絵文字を文字化けさせないため、MySQL5.7のときは値を「utf8mb4」に変更する必要があったcharacter_set_client、character_set_connection、character_set_database、character_set_results、character_set_serverという5つのパラメータですが、MySQL8ではいずれもデフォルト値が「utf8mb4」になったため、設定不要となりました。詳しくはMySQL 8.0 Reference Manualの「5.1.8 Server System Variables」を参照してください。 @<href>{https://dev.mysql.com/doc/refman/8.0/en/server-system-variables.html}]

先ずは「フィルタ パラメータ」と表示されているところに「collation_server」と入力して検索（@<img>{startaws90}）します。検索結果が表示されたら右上にある「パラメータの編集」ボタンをクリックしてください。

//image[startaws90][「collation_server」でパラメータを検索][scale=0.8]{
//}

「collation_server」というパラメータの値を、プルダウンで「utf8mb4_bin」（@<img>{startaws92}）にします。@<fn>{collationServer}

//footnote[collationServer][このパラメータをデフォルト値の「utf8mb4_0900_ai_ci」のままにしておくと、濁音半濁音や、大文字小文字を区別しないため、たとえば「カート」で検索したときに「カード」も引っかかったり、正しいパスワードは「password」なのに「PASSWORD」と入力したときもログインできてしまったりします。]

//image[startaws92][「collation_server」の値を「utf8mb4_bin」にする][scale=0.8]{
//}

続いて「init_connect」で検索（@<img>{startaws93}）します。「init_connect」@<fn>{initConnect}というパラメータの値に、「SET NAMES utf8mb4;」と記入したら、右上にある「変更の保存」をクリックしてください。

//footnote[initConnect][「init_connect」というパラメータには、「データベースへ接続しにきた各クライアントに対してサーバが実行するSQL文」を指定します。]

//image[startaws93][「init_connect」の値に「SET NAMES utf8mb4;」と記入して「変更の保存」をクリック][scale=0.8]{
//}

2つのパラメータの値を変更したので、これでパラメータグループの作成とパラメータの設定は完了です。続いてオプショングループも作成します。

=== オプショングループを作成

左メニューの「オプショングループ」をクリック（@<img>{startaws95}）してから、右上にある「グループの作成」をクリックします。

//image[startaws95][「オプショングループ」で「グループの作成」をクリック][scale=0.8]{
//}

名前に「start-aws-option-group」、説明に「for WordPress」と記入します。エンジンは「mysql」、メジャーエンジンのバージョンは「8.0」を選択したら「作成」をクリック（@<img>{startaws96}）します。

//image[startaws96][オプショングループの詳細を設定したら「作成」をクリック][scale=0.8]{
//}

オプショングループの一覧に「start-aws-option-group」が表示（@<img>{startaws97}）されたらオプショングループの作成は完了です。いよいよRDSでインスタンスを作りましょう。

//image[startaws97][オプショングループの一覧に「start-aws-option-group」が表示された][scale=0.8]{
//}

== RDSでインスタンスを立てよう

左メニューの「データベース」をクリック（@<img>{startaws98}）してから、右上にある「データベースの作成」をクリックします。

//image[startaws98][「インスタンス」で「データベースの作成」をクリック][scale=0.8]{
//}

ここから3つのステップでRDSのインスタンスを作成していきます。

=== データベース作成方法を選択

データベース作成方法は「標準作成」（@<img>{startaws99-1}）のままで構いません。

//image[startaws99-1][データベース作成方法は「標準作成」のまま][scale=0.8]{
//}

=== エンジンのオプション

エンジンのタイプは「MySQL」、バージョンは「MySQL 8.0.21」を選択（@<img>{startaws99-2}）します。

//image[startaws99-2][エンジンのタイプは「MySQL」、バージョンは「MySQL 8.0.21」を選択][scale=0.8]{
//}

=== テンプレート

テンプレートは「無料利用枠」を選択（@<img>{startaws99-3}）します。

//image[startaws99-3][テンプレートは「無料利用枠」を選択][scale=0.8]{
//}

=== 設定

設定（@<img>{startaws100}）は次のようにします。（@<table>{db}）

//table[db][設定]{
DBインスタンス識別子	start-aws-db-instance
マスターユーザ名	start_aws_dbuser
マスターパスワード	start_aws_db_password
パスワードを確認	マスターパスワードと同じ
//}

ここで設定した「マスターユーザ名」と「マスターパスワード」は、この後でWordPressをデータベースに繋ぐときに必要となります。

//image[startaws100][「設定」を入力したら「次へ」をクリック][scale=0.8]{
//}

DBインスタンスサイズ、ストレージ、可用性と耐久性@<fn>{multiAZ}、接続、データベース認証は何も変更せずそのままで構いません。

//footnote[multiAZ][「可用性と耐久性」に「マルチAZ配置」という設定があります。マルチAZ配置とは、メインのRDSインスタンスとは別のアベイラビリティゾーンにあらかじめレプリカを待機させておいて、何らかの障害でメインのRDSインスタンスが停止してしまったら自動的に待機していたインスタンスに切り替わる仕組みです。マルチAZ配置はAWSの無料利用枠に含まれないため本書ではシングルAZ配置の構成です。]

=== 追加設定

「追加設定」を開いて、「データベースの選択肢」（@<img>{startaws101}）を次のようにします。（@<table>{dbSchema}）

//table[dbSchema][データベースの選択肢]{
最初のデータベース名	start_aws_wordpress_dbname
DBパラメータグループ	start-aws-parameter-group
オプショングループ	start-aws-option-group
//}

//image[startaws101][「データベースの選択肢」を入力][scale=0.8]{
//}

ここで設定した「最初のデータベース名」も、この後でWordPressをデータベースに繋ぐときに必要となります。「追加設定」のバックアップ、モニタリング、ログのエクスポート、メンテナンス@<fn>{maintenance}、削除保護はすべてそのままで構いません。

//footnote[maintenance][ちなみに「メンテナンス」で「マイナーバージョン自動アップグレードの有効化」にチェックを入れていると、新しいバージョンが出たときにMySQLを自動でアップグレードして自動で再起動してしまいます。今回は構いませんが、重要なウェブサイトやサービスで何の予告もなくデータベースに繋がらなくなったら大変ですので、こちらは基本的に無効化しておきます。]

「データベースの選択肢」を入力し終わったら「データベースの作成」をクリック（@<img>{startaws102}）します。

//image[startaws102][「データベースの作成」をクリック][scale=0.8]{
//}

=== セキュリティグループで3306番ポートの穴あけをしよう

「データベース start-aws-db-instance を作成しています。データベースが起動するまでに、数分かかることがあります。」と表示（@<img>{startaws103}）されたら「DB識別子」の列で「start-aws-db-instance」をクリックします。RDSのインスタンスができあがるまで少し時間がかかりますので、その間に「このデータベースにはどこからの接続を許可するのか？」というセキュリティグループ@<fn>{securityGroupAgain}の設定をしておきましょう。

//image[startaws103][「start-aws-db-instance」をクリック][scale=0.8]{
//}

//footnote[securityGroupAgain][何度か出てきていますが、セキュリティグループはいわゆる「ファイアウォール」のことです。セキュリティグループってどんなものだったっけ？という方は、@<chapref>{serverBuilding}でEC2のインスタンスを作るとき「ステップ6」で設定したことを思い出してください。]

今作った「start-aws-db-instance」というRDSインスタンスの詳細が表示（@<img>{startaws104}）されます。

//image[startaws104][「start-aws-db-instance」というRDSインスタンスの詳細][scale=0.8]{
//}

下の方にスクロールして「接続とセキュリティ」（@<img>{startaws105}）を見てください。RDSインスタンスの手前に立ちはだかっているのは、「default」という名前のセキュリティグループであることが分かります。「default」をクリックしてください。

//image[startaws105][セキュリティグループの「default」をクリック][scale=0.8]{
//}

「default」の「インバウンドルール」タブで「インバウンドルールを編集」をクリック（@<img>{startaws106}）します。

//image[startaws106][「インバウンドルール」タブで「インバウンドルールを編集」をクリック][scale=0.8]{
//}

データベースに記事データを保存したり、データベースへ記事データを取りに行ったりするのは、EC2のインスタンス上で動いているWordPressです。そのためこのセキュリティグループでは、EC2のインスタンスからRDSの「MySQL（ポート番号3306番）」へ通信できるように許可する設定をしてやらなければいけません。

タイプを「すべてのトラフィック」から「MySQL/Aurora」に変更します。ソースに「ec2-security-group」と入力（@<img>{startaws107}）すると、プルダウンに下に「ec2-security-group」が表示されますのでクリックしてください。

//image[startaws107][「ec2」と入力して「ec2-security-group」が表示されたらクリック][scale=0.8]{
//}

ソースに「sg-0f65b9b3ccbccd4ca」のようなEC2のセキュリティグループIDが表示（@<img>{startaws108}）されたら、「ルールを保存」をクリックします。

//image[startaws108][「ルールを保存」をクリック][scale=0.8]{
//}

「default」の「インバウンドルール」タブに、いま設定したルールが表示（@<img>{startaws108-2}）されています。これでEC2のインスタンスからRDSの「MySQL（ポート番号3306番）」へ接続できるようになりました。

//image[startaws108-2][「ルールを保存」をクリック][scale=0.8]{
//}

=== エンドポイントのドメイン名をメモしておこう

そろそろRDSのインスタンスができあがっている頃です。マネジメントコンソールの左上にある「サービス」から、「データベース」の下にある「RDS」（@<img>{startaws109}）をクリックしてください。

//image[startaws109][サービス＞データベース＞RDS][scale=0.8]{
//}

RDSダッシュボードを開いたら左メニューの「データベース」をクリック（@<img>{startaws110}）します。DB識別子の列の「start-aws-db-instance」をクリックしてください。

//image[startaws110][左メニューの「データベース」＞「start-aws-db-instance」をクリック][scale=0.8]{
//}

下の方にスクロールすると「接続とセキュリティ」のタブに「エンドポイント」（@<img>{startaws111}）があります。この「start-aws-db-instance.cesouf5kakle.ap-northeast-1.rds.amazonaws.com」という「エンドポイント」は、後でWordPressからデータベースに接続するときに必要となります。忘れないように、コピーしてパソコンのメモ帳にしっかりメモしておいてください。

//image[startaws111][「エンドポイント」をしっかりメモしておこう][scale=0.8]{
//}

=== ウェブサーバから接続確認してみよう

それではEC2のインスタンスからRDSのMySQLに接続できるか、mysqlコマンドを使って試してみましょう。いわゆる「ウェブサーバからデータベースへの疎通確認」ですね。MySQLに接続するときは先ほどメモしておいた次の3つを使用します。

 * エンドポイント
 * マスターユーザの名前
 * マスターパスワード

Windowsの方はRLoginを起動して「start-aws-instance」に接続してください。Macの方はターミナルで次のコマンドを実行して、「start-aws-instance」に入ります。

//cmd{
ssh ec2-user@login.自分のドメイン名 -i ~/Desktop/start-aws-keypair.pem
//}

「Amazon Linux 2 AMI」と表示されたら次のコマンドを叩きます。

//cmd{
$ mysql -h エンドポイント -u マスターユーザの名前 -p
//}

-hオプションは「接続先ホスト」を指定しています。-uオプションは「このユーザで」、-pオプションは「パスワード認証で」という意味です。筆者だったらこんなコマンドになります。

//cmd{
$ mysql -h start-aws-db-instance.cesouf5kakle.ap-northeast-1.rds.amazonaws.com
        -u start_aws_dbuser -p　※実際は改行せずに1行で実行
//}

コマンドを叩くと次のように表示されます。

//cmd{
Enter password:
//}

パスワードを求められているので「マスターパスワード」の「start_aws_db_password」を入力してEnterキーを押します。なお@<ttb>{パスワードは入力しても画面上何の変化もありません}。カーソルの位置も変わらないし「***」のような表示も一切されません。入力できている手ごたえがまったくありませんがちゃんと入力できていますし、間違えたときはBackSpaceキーを押せば消せていますので大丈夫です。パスワードを入力したらEnterキーを押してください。パスワードを入力後、次のように一番下に「mysql> 」と表示されたらMySQLへのログインに成功しています。

//cmd{
$ mysql -h エンドポイント -u マスターユーザの名前 -p
Enter password: 
Welcome to the MySQL monitor.  Commands end with ; or \g.
Your MySQL connection id is 18
Server version: 8.0.21 Source distribution

Copyright (c) 2000, 2020, Oracle and/or its affiliates. All rights reserved.

Oracle is a registered trademark of Oracle Corporation and/or its
affiliates. Other names may be trademarks of their respective
owners.

Type 'help;' or '\h' for help. Type '\c' to clear the current input statement.

mysql> 
//}

試しに「show databases;」と入力してEnterキーを押すとデータベースの一覧が表示されます。ちゃんと「start_aws_wordpress_dbname」という名前のデータベースも作成されていることが確認できました。

//cmd{
mysql> show databases;
+----------------------------+
| Database                   |
+----------------------------+
| information_schema         |
| mysql                      |
| performance_schema         |
| start_aws_wordpress_dbname |
| sys                        |
+----------------------------+
5 rows in set (0.01 sec)
//}

それではMySQLを抜けてEC2のインスタンスに戻りたいので「exit」と入力してEnterキーを押してください。

//cmd{
mysql> exit
Bye
//}

Byeと表示されたらMySQLとの接続を切ってEC2のインスタンスへ戻ってこられています。これでデータベースサーバは用意できました。いよいよWordPressのインストールを行いましょう。