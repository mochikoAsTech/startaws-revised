= ELBとAuto Scalingで複数台のサーバを運用しよう

この章ではロードバランサーの設定や、サーバが停止してしまったときに自動復旧するAuto Scalingの設定をします。

//pagebreak

== ELBはなんのためにある？

ウェブサーバやデータベースサーバなどのインフラ環境を構築するときには@<ttb>{「ここが壊れたり止まったりしたらサービス全体が停止する」というSPOF}@<fn>{spof}をつぶしておくことが大切です。皆さんが本書で構築した環境は「EC2のウェブサーバ1台＋RDSのデータベースサーバ1台」という低コストのお手軽構成ですので、ウェブサーバもデータベースサーバもどちらもSPOFになっています。たとえばEC2のウェブサーバが壊れたらその瞬間にWordPressのサイトは見られなくなります。@<fn>{break}

//footnote[spof][Single Point of Failureの略。単一障害点のこと。]
//footnote[break][サーバが壊れると言われてもピンと来ないかも知れませんが、ホストサーバのハードウェアが壊れてしまえば、当然その上で動いていたゲストサーバ（EC2のインスタンス）も影響を受けて「突然うんともすんとも言わなくなった！」状態になったりします。ただその場合もOSやデータが入ったEBSボリュームは生きているので、EC2ダッシュボードを開いて手動でインスタンスを起動させれば別のホストサーバ上でまた元気に働きだすはずです。]

このSPOFをなくすための手段の1つとしてELB@<fn>{elb}、いわゆる負荷分散のためのロードバランサーのサービスがあります。もしEC2のウェブサーバが2台あって、その手前のロードバランサーがアクセスを2台に振り分けていたら、1台が停止している間ももう1台が応答できるのでサイトは停止しません。

//footnote[elb][Elastic Load Balancingの略。]

「予算がないのでウェブサーバを2台用意するのは無理です」とか「小規模なサイトなのでそこまでの冗長性は求めていません」という場合でもELBは役に立ちます。ELBを使って1台のウェブサーバにアクセスを流していた場合、Auto Scaling（オートスケーリング）というサービスを使うことで、その1台が死んでしまったときに自動で代替のインスタンスを立ててサイトを自動復旧させることができます。

他にもELBを使うと、無料でSSL証明書を取得してサイトを楽にHTTPS化できるなど色々なメリットがあります。ELBは無料利用枠にも含まれていますので、ここではELBからEC2のインスタンスへアクセスを流すロードバランシングの設定をしてみましょう。

== ロードバランサーを作ろう

それではマネジメントコンソールの左上にある「サービス」から、「コンピューティング」の下にある「EC2」（@<img>{startaws129}）をクリックしてください。

//image[startaws129][サービス＞コンピューティング＞EC2][scale=0.8]{
//}

EC2のダッシュボードで左メニューの「ロードバランサー」を開いたら、「ロードバランサーの作成」をクリック（@<img>{startaws131}）します。

//image[startaws131][「ロードバランサー」で「ロードバランサーの作成」をクリック][scale=0.8]{
//}

=== ロードバランサーの種類の選択

ロードバランサーにもいくつか種類があるのですが、今回はいちばん左の「Application Load Balancer」を使います。「作成」をクリック（@<img>{startaws132}）してください。

//image[startaws132][「Application Load Balancer」の「作成」をクリック][scale=0.8]{
//}

ここからは6つの手順でロードバランサーを作成していきます。

=== 手順 1: ロードバランサーの設定

名前に「start-aws-elb」と入力（@<img>{startaws133}）したらそのまま下の方へスクロールします。

//image[startaws133][名前に「start-aws-elb」と入力][scale=0.8]{
//}

アベイラビリティーゾーンはすべてにチェック（@<img>{startaws134}）を入れて、「次の手順: セキュリティ設定の構成」をクリックします。

//image[startaws134][AZはすべてにチェックを入れて「次の手順: セキュリティ設定の構成」をクリック][scale=0.8]{
//}

=== 手順 2: セキュリティ設定の構成

いきなり黄色で「ロードバランサーのセキュリティを向上させましょう。ロードバランサーは、いずれのセキュアリスナーも使用していません。」と表示（@<img>{startaws135}）されましたが、これは「HTTPじゃなくてもっとセキュアなHTTPS@<fn>{startSSL}にした方がいいよ」とアドバイスをしてくれているだけですので、今はそのままスルーで構いません。「次の設定: セキュリティグループの設定」をクリックします。

//image[startaws135][「次の設定: セキュリティグループの設定」をクリック][scale=0.8]{
//}

//footnote[startSSL][HTTPSについては、本書の続編「SSLをはじめよう」で説明しています。]

=== 手順 3: セキュリティグループの設定

次にセキュリティグループ（ファイアウォール）の設定をします。「セキュリティグループの割り当て」で「新しいセキュリティグループを作成する」を選択（@<img>{startaws136}）します。「セキュリティグループ名」と「説明」は次のように入力してください。（@<table>{elbSecurityGroup}）

//table[elbSecurityGroup][セキュリティグループの設定]{
セキュリティグループ名	elb-security-group
説明	HTTP Allow from anywhere
//}

続いて「ここからのアクセスのみを通す」というルールを設定します。ルールはタイプが「HTTP」で、ソースを「カスタム」の「0.0.0.0/0」にします。

//image[startaws136][セキュリティグループを設定したら「次の手順: ルーティングの設定」をクリック][scale=0.8]{
//}

これはロードバランサーの手前に立ちはだかって、ロードバランサーを守ってくれるセキュリティグループを、「HTTP（ポート番号80番）ならどこからのリクエストでも通す。それ以外は一切通さない。」という設定にしています。ルールを設定したら「次の手順: ルーティングの設定」をクリックします。

=== 手順 4: ルーティングの設定

ターゲットグループというのはロードバランサーの分散先となるサーバのグループのことです。つまり「このグループに入っているインスタンスにアクセスを割り振ります」ということですね。

名前に「elb-target-group」と入力（@<img>{startaws137}）します。

//image[startaws137][名前に「elb-target-group」と入力][scale=0.8]{
//}

下にスクロールして、ヘルスチェック@<fn>{healthcheck}の「パス」に「/healthcheck」と入力（@<img>{startaws137-2}）します。それ以外の設定はすべてのそのままで構いません。「次の手順: ターゲットの登録」をクリックします。

//image[startaws137-2][「パス」に「/healthcheck」と入力したら「次の手順: ターゲットの登録」をクリック][scale=0.8]{
//}

//footnote[healthcheck][ここで設定している「ヘルスチェック」とは、名前のとおりロードバランサーがターゲットグループのインスタンスに対して行う「ねぇ、ウェブサーバ生きてる？ねぇねぇ生きてる？」という死活チェックのことです。このチェックに対してウェブサーバが応答をしなくなると「あ、死んでるからこのウェブサーバにアクセスを流すのやめよう」となって負荷分散対象から除外されるのです。@<chapref>{wordPress}で、ドキュメントルートの下にWordPressのファイルを展開した後、echoコマンドを叩いて「healthcheck」というファイルを作ったのを覚えていますか？@<href>{http://www.自分のドメイン名/healthcheck}に対してヘルスチェックをして欲しいので、「パス」に「/healthcheck」と入力しました。]

=== 手順 5: ターゲットの登録

ターゲットとは、実際の分散先となるサーバのことです。下部の「インスタンス」にある「start-aws-instance」にチェック（@<img>{startaws138}）を入れたら、「登録済みに追加」をクリックします。

//image[startaws138][「start-aws-instance」にチェックを入れたら「登録済みに追加」をクリック][scale=0.8]{
//}

上部の「登録済みターゲット」に「start-aws-instance」が表示（@<img>{startaws139}）されたら「次の手順: 確認」をクリックします。

//image[startaws139][「登録済みターゲット」に「start-aws-instance」が表示されたら「次の手順: 確認」をクリック][scale=0.8]{
//}

=== 手順 6: 確認

設定内容を確認したら「作成」をクリック（@<img>{startaws140}）します。

//image[startaws140][設定内容を確認したら「作成」をクリック][scale=0.8]{
//}

「ロードバランサーを正常に作成しました」と表示（@<img>{startaws141}）されたら「閉じる」をクリックします。

//image[startaws141][「閉じる」をクリック][scale=0.8]{
//}

これでロードバランサーの作成は完了です。（@<img>{startaws142}）

//image[startaws142][ロードバランサーの作成は完了][scale=0.8]{
//}

=== ヘルスチェックの確認をしよう

ロードバランサーの作成が完了したら、EC2ダッシュボードの左メニューで「ターゲットグループ」を開いて、「elb-target-group」をクリック（@<img>{startaws162}）します。

//image[startaws162][「ターゲットグループ」を開いて「elb-target-group」をクリック][scale=0.8]{
//}

「Targets」のタブ（@<img>{startaws162-2}）で、ターゲットである「start-aws-instance」のStatusがhealthyになっていれば、ロードバランサーからの「ねぇ生きてる？」に対して、EC2で作ったウェブサーバが「生きてる！」と元気に応答しているということです。

//image[startaws162-2][「start-aws-instance」のStatusを確認してみよう][scale=0.8]{
//}

もしStatusが「unhealthy」になっていたら「ねぇ生きてる？」にちゃんと応答できていませんので、次の確認をしましょう。

 * EC2のインスタンス（start-aws-instance）が起動しているか？
 * EC2のインスタンスでApacheが起動しているか？
 * EC2の手前にいるセキュリティグループ（ec2-security-group）でHTTP（ポート番号80番）のアクセスを許可しているか？
 * ヘルスチェックに応答するためのファイル（/var/www/start-aws-documentroot/healthcheck）が存在しているか？
 * ブラウザで@<href>{http://www.自分のドメイン名/healthcheck}を開くとhealthcheckと表示されるか？

== WordPressのサイトを表示する経路を変更しよう

=== 「www.自分のドメイン名」と紐づくIPアドレスを変更しよう

EC2ダッシュボードの左メニューで「ロードバランサー」をクリック（@<img>{startaws180}）したら「start-aws-elb」の「説明」タブにある「DNS名」をコピーしてください。右側の四角いマークをクリックすると、クリップボードにコピーできますので、パソコンの中のメモ帳にメモしておいてください。

//image[startaws180][「start-aws-elb」の「説明」タブにある「DNS名」をコピー][scale=0.8]{
//}

それではRoute53でDNSの変更を行いましょう。

=== Route53でAレコードをAliasに変更しよう

マネジメントコンソールの左上にある「サービス」から、「ネットワーキングとコンテンツ配信」の下にある「Route53」（@<img>{startaws182}）をクリックしてください。

//image[startaws182][サービス＞ネットワーキングとコンテンツ配信＞Route53][scale=0.8]{
//}

Route53ダッシュボードを開いたらDNS管理の「ホストゾーン」をクリック（@<img>{startaws183}）します。

//image[startaws183][「ホストゾーン」をクリック][scale=0.8]{
//}

「ドメイン名」の列にある自分のドメイン名（筆者の場合はstartdns.fun）をクリック（@<img>{startaws184}）します。

//image[startaws184][自分のドメイン名をクリック][scale=0.8]{
//}

「www.自分のドメイン名」（筆者の場合は「www.startdns.fun」）にチェックを入れて、「編集」をクリック（@<img>{startaws185-1}）します。

//image[startaws185-1][「www.自分のドメイン名」にチェックを入れて「編集」をクリック][scale=0.8]{
//}

既存のAレコードが表示されました。現状は「値/トラフィックのルーティング先」にEC2インスタンスのElastic IPが設定（@<img>{startaws185-2}）されています。つまりこのままだと「@<href>{http://www.自分のドメイン名/}」を開いたときに、ロードバランサー経由ではなく、直接EC2インスタンスへ「ウェブページを見せて！」とリクエストが行ってしまうため、ロードバランサーを作成した意味がありません。「www.自分のドメイン名」の紐づく先を、Elastic IPからELBに変更しましょう。

//image[startaws185-2][「値/トラフィックのルーティング先」にはEC2インスタンスのElastic IPが設定されている][scale=0.8]{
//}

「レコードタイプに応じたIPアドレスまたは別の値」をクリック（@<img>{startaws186}）して、「Application Load BalancerとClassic Load Balancerへのエイリアス」に変更します。リージョンのプルダウンは「アジアパシフィック(東京) [ap-noattheast-1]」を選択して、「ロードバランサーを選択」のプルダウンは先ほど作ったばかりのロードバランサー@<fn>{dnsName}を選択します。3つとも選択したら「変更を保存」をクリックします。

//image[startaws186][「www.自分のドメイン名」をクリックすると右側に既存のAレコードが表示される][scale=0.8]{
//}

//footnote[dnsName][さっきメモした「start-aws-elb」のDNS名の頭に、「dualstack.」を付けたドメイン名が表示されていると思います。]

「www.自分のドメイン名 は正常に更新されました。」と表示されます。これで「www.自分のドメイン名」の紐づく先が、Elastic IPからELBに変わりました。

//image[startaws188][「www.自分のドメイン名」の紐づく先がElastic IPからELBに変わった][scale=0.8]{
//}

それではEC2のインスタンスでdigコマンド@<fn>{dig}を叩いてAレコードの設定が変更されたか確認してみましょう。Windowsの方はRLoginを起動して「start-aws-instance」に接続してください。Macの方はターミナルで次のコマンドを実行して接続します。

//footnote[dig][digコマンドについては「DNSをはじめよう」の第4章「digとwhoisを叩いて学ぶDNS」を参照してください。]

//cmd{
ssh ec2-user@login.自分のドメイン名 -i ~/Desktop/start-aws-keypair.pem
//}

「Amazon Linux 2 AMI」と表示されたらログイン完了です。次のコマンドを叩いてみてください。

//cmd{
$ dig www.自分のドメイン名 +short
//}

筆者ならドメイン名が「startdns.fun」なので次のようになります。

//cmd{
$ dig www.startdns.fun +short
13.115.238.62
54.168.2.57
//}

このようにIPアドレスが2つ返ってきたら、Route53でELBへのエイリアスがちゃんと設定できています。これで「@<href>{http://www.自分のドメイン名/}」を開いたときに、ロードバランサーを経由してWordPressのサイトが表示されるようになりました。

=== HTTPを通すのはELB経由のアクセスだけにしよう

DNSの設定を変更したことで、今後は「サイトを見たい人→ロードバランサー→EC2のインスタンス」という経路だけを使うことになるので、「サイトを見たい人→EC2のインスタンス（Elastic IP）」という不要な経路はふさいでおきましょう。

それではマネジメントコンソールの左上にある「サービス」から、「コンピューティング」の下にある「EC2」（@<img>{startaws189}）をクリックしてください。

//image[startaws189][サービス＞コンピューティング＞EC2][scale=0.8]{
//}

EC2のダッシュボードが表示（@<img>{startaws190}）されたら左メニューの「セキュリティグループ」をクリックします。「ec2-security-group」の「インバウンドルール」タブをクリックすると、現状は「0.0.0.0/0」、つまり「@<ttb>{HTTP（ポート番号80番）はどこからのリクエストでも通す}」という設定になっています。ここを「@<ttb>{HTTP（ポート番号80番）はELB経由のリクエストのみ通す}」という設定に変更したいので、「インバウンドルールを編集」をクリックしてください。

//image[startaws190][「ec2-security-group」の「インバウンドルールを編集」をクリック][scale=0.8]{
//}

HTTPの「ソース」に書いてある「0.0.0.0/0」を消して、代わりに「elb」と入力（@<img>{startaws191}）するとプルダウンで「elb-security-group」が表示されるので選択して、「ルールを保存」をクリックします。

//image[startaws191][「elb-security-group」を選択して「ルールを保存」をクリック][scale=0.8]{
//}

「インバウンドセキュリティグループのルールが、セキュリティグループで正常に変更されました」を表示されたら、セキュリティグループの設定変更は完了です。これでEC2インスタンスの手前にいるセキュリティグループは、「@<ttb>{HTTP（ポート番号80番）はELB経由のリクエストのみ通す}」という状態（@<img>{startaws193}）になりました。

//image[startaws193][「HTTP（ポート番号80番）はELB経由のリクエストのみ通す」という状態になった][scale=0.8]{
//}

ブラウザで、EC2インスタンスに紐づいているElastic IPを直接叩いても（@<img>{startaws193}）、セキュリティグループに阻まれてWordPressのサイトは表示されません。

//image[startaws195][Elastic IPを直接叩くとセキュリティグループに阻まれてサイトが表示されない][scale=0.6]{
//}

でもELBを経由する「@<href>{http://www.自分のドメイン名/}」を開いたときは、ちゃんとサイトが表示（@<img>{startaws194}）されます。

//image[startaws194][ELBを経由する「@<href>{http://www.自分のドメイン名/}」を開いたときはちゃんと表示される][scale=0.6]{
//}

=== 【ドリル】自宅以外からサイトが見られないようアクセス制限をしたい

==== 問題

「WordPressで作ったブログは自分の勉強ノートとして使いたいので、自宅からしか見えないようにIPアドレスで制限しよう！」と思ったAさんは、EC2のインスタンスでバーチャルホストの設定に

//cmd{
<Directory "/var/www/start-aws-documentroot">
    Require all denied
    Require ip 自宅のIPアドレス
</Directory>
//}

と追記してApacheを再起動しました。するとなぜか

 * 自宅からサイトが見られない
 * 自宅以外の場所からもサイトが見られない
 * ロードバランサーからのヘルスチェックも通さず、インスタンスが死んでいると判断されてしまう

という全拒否状態になってしまいました。慌てたAさんは、次のように修正してもう一度Apacheを再起動しました。

//cmd{
<Directory "/var/www/start-aws-documentroot">
    Require all denied
    Require ip 自宅のIPアドレス
    Require ip ロードバランサーのIPアドレス
</Directory>
//}

すると今度は

 * 自宅からはサイトが見られる
 * 自宅以外の場所からもサイトが見られる
 * ロードバランサーからのヘルスチェックも通す

という全許可状態になってしまいました。いったいどうしてでしょう？どう設定したらAさんの期待する、次のような状態になるのでしょうか？

 * 自宅からはサイトが見られる
 * 自宅以外の場所からはサイトが見られない
 * ロードバランサーからのヘルスチェックは通す

//raw[|latex|\begin{reviewimage}\begin{flushright}\includegraphics[width=0.5\maxwidth\]{./images/answerColumnLong.png}\end{flushright}\end{reviewimage}]

==== 解答

ELBを経由したアクセスの場合、EC2インスタンスから見た「アクセス元」はすべてロードバランサーになってしまうため、次のようにロードバランサーのIPアドレスを許可すると、結果としてすべてのクライアントからのアクセスを許可することになってしまいます。

//cmd{
<Directory "/var/www/start-aws-documentroot">
    Require all denied
    Require ip 自宅のIPアドレス
    Require ip ロードバランサーのIPアドレス
</Directory>
//}

そこで次のように「RemoteIPHeader X-Forwarded-For」@<fn>{remoteIpHeader}を書き加えると、経由したロードバランサーのIPアドレスではなく、本当のクライアントのIPアドレスが「アクセス元」として利用されるようになります。

//footnote[remoteIpHeader][@<href>{https://httpd.apache.org/docs/2.4/ja/mod/mod_remoteip.html#remoteipheader}]

//cmd{
RemoteIPHeader X-Forwarded-For

<Directory "/var/www/start-aws-documentroot">
    Require all denied
    Require ip 自宅のIPアドレス
    Require ip ロードバランサーのIPアドレス
</Directory>
//}

これでAさんの期待する、次のようなアクセス制限が実現できました。

 * 自宅からはサイトが見られる
 * 自宅以外の場所からはサイトが見られない
 * ロードバランサーからのヘルスチェックは通す

でも本当はApacheで制御するよりもELBの手前にいるSecurityGroupで遮断する方が、EC2インスタンスに無駄な働きをさせなくて済むのでお勧めです。

== Auto Scalingでサーバを自動復旧させよう

AWS Auto Scaling（オートスケーリング）はサーバの自動拡張・縮小をしてくれるサービスです。アクセスが増えてきてウェブサーバ1台ではさばききれなくなったら、Auto Scalingが自動的に追加のサーバを立ててくれますし、アクセス数が落ち着いてきて1台で十分な状態になったら自動的に不要なサーバを削除してくれます。

しかし本書では@<ttb>{Auto Scalingを拡張や縮小ではなく「インスタンス数の維持」のために利用}します。何か問題が起きてEC2のインスタンスが終了してしまっても、Auto Scalingによって新たにインスタンスが1台立てられて、ウェブサイトが自動復旧する状態を目指します。

=== 起動テンプレートを作ろう

EC2ダッシュボードの左メニューで「起動テンプレート」を開いて、「起動テンプレートを作成」をクリック（@<img>{launchTemplate1}）します。

//image[launchTemplate1][「起動テンプレート」で「起動テンプレートを作成」をクリック][scale=0.8]{
//}

起動テンプレート名に「start-aws-autoscaling-launch-template」を入力（@<img>{launchTemplate2}）します。テンプレートバージョンの説明には「start-aws-instanceの起動テンプレート」と入力します。Auto Scalingのガイダンスにもチェックを入れておきましょう。

//image[launchTemplate2][起動テンプレート名とテンプレートバージョンの説明を入力][scale=0.8]{
//}

Amazonマシンイメージ（AMI）AMIは「start-aws-ami」で検索（@<img>{launchTemplate3}）して、「自分のAMI」の下にある「start-aws-ami」を選択します。これは@<chapref>{backup}であなたが作成したAMIです。インスタンスタイプは、現在のインスタンスと同じ「t2.micro」を選択します。Auto Scalingで自動起動してくるインスタンスにも、同じキーペアでSSHログインしたいので、キーペア（ログイン）は、「start-aws-keypair」を選択してください。

//image[launchTemplate3][AMI、インスタンスタイプ、キーペアを選択][scale=0.8]{
//}

ネットワーク設定のセキュリティグループは、「ec2-security-group」を選択（@<img>{launchTemplate4}）します。ストレージ（ボリューム）とリソースタグ、ネットワークインターフェイスは、何も変更せずそのままで構いません。

//image[launchTemplate4][セキュリティグループは「ec2-security-group」を選択][scale=0.8]{
//}

「高度な詳細」を開いて、IAMインスタンスプロフィールで「s3-upload-role」を選択（@<img>{launchTemplate6}）します。

//image[launchTemplate6][IAMインスタンスプロフィールで「s3-upload-role」を選択][scale=0.8]{
//}

「正常にstart-aws-autoscaling-launch-template (lt-0f47b626b6cc0f742)を作成しました」と表示（@<img>{launchTemplate7}）されたら、起動テンプレートの作成は完了です。続いて、この起動テンプレートを使ってAuto Scalingグループを作成したいので「Auto Scalingグループを作成」をクリックしてください。

//image[launchTemplate7][「Auto Scalingグループを作成」をクリック][scale=0.8]{
//}

=== Auto Scalingグループを作成しよう

起動テンプレートができたら、続いて「Auto Scalingグループ」を作成します。Auto ScalingグループはEC2インスタンスのグループのことで、このグループで常に維持したいインスタンスの数などを設定します。インスタンスの数はここで設定した最小数と最大数の間で増減します。

ここからは7つのステップでAuto Scalingグループを作成していきます。

==== ステップ1. 起動テンプレートまたは起動設定を選択する

Auto Scalingグループ名に「start-aws-autoscaling-group」と入力（@<img>{startaws172}）します。起動テンプレート
が、さきほど作った「start-aws-autoscaling-launch-template」になっていることを確認してください。バージョンは「Latest (1)」にしておきます。「次へ」をクリックします。

//image[startaws172][Auto Scalingグループ名に「start-aws-autoscaling-group」と入力][scale=0.8]{
//}

==== ステップ2. 設定の構成

インスタンスの購入オプションは、変更せずそのまま進みます。サブネットは既存のEC2インスタンスと同じ「ap-northeast-1a」を選択して、「次へ」をクリック（@<img>{startaws173}）します。

//image[startaws173][「ap-northeast-1a」を選択して「次へ」をクリック][scale=0.8]{
//}

==== ステップ 3. 詳細オプションを設定

ロードバランシングは「Attach to an existing load balancer」（既存のロードバランサーにアタッチ）を選択（@<img>{startaws174-1}）します。

//image[startaws174-1][「Attach to an existing load balancer」を選択][scale=0.8]{
//}

ターゲットグループは「elb-target-group」を選択（@<img>{startaws174-2}）して、ヘルスチェックのタイプは「ELB」にチェックを入れます。これでELBからのヘルスチェックに対してインスタンスが応答しなくなったら、Auto Scalingによって自動的にインスタンスが立てられることになります。すべて設定したら「次へ」をクリックします。

//image[startaws174-2][「elb-target-group」と「ELB」を選択して「次へ」][scale=0.8]{
//}

==== ステップ 4. グループサイズとスケーリングポリシーを設定する

本書ではAuto Scalingを拡張や縮小ではなく「インスタンス数の維持」のために利用したいので、グループサイズはすべて1のままにしておきます。その他の設定もそのままで、「次へ」をクリック（@<img>{startaws175}）します。

//image[startaws175][グループサイズはすべて1のままで「次へ」をクリック][scale=0.8]{
//}

==== ステップ 5. 通知を追加

「通知の追加」をクリックします。「トピックを作成する」をクリックして、「次に通知を送信」に「start-aws-sns-topic」、「受信者」に自分のメールアドレス@<fn>{mailAddress}を記入したら「次へ」をクリック（@<img>{startaws176}）します。

//footnote[mailAddress][メールアドレスの確認のため「AWS Notification - Subscription Confirmation」というメールが届きます。メール本文中にある「Confirm subscription」というリンクを踏んでおいてください。「Subscription confirmed!」と書かれたページが表示されたら、メールアドレスの確認は完了です。]

//image[startaws176][メールアドレスを記入したら「次へ」をクリック][scale=0.8]{
//}

==== ステップ 6. タグを追加

何も変更せず「次へ」をクリック（@<img>{startaws177}）します。

//image[startaws177][何も変更せず「次へ」をクリック][scale=0.8]{
//}

==== ステップ 7. 確認

内容を確認したら「Auto Scalingグループを作成」をクリック（@<img>{startaws178}）してください。

//image[startaws178][内容を確認したら「Auto Scalingグループを作成」をクリック][scale=0.8]{
//}

「start-aws-autoscaling-group, 1トピック, 1サブスクリプション, 1通知が正常に作成されました」と表示（@<img>{startaws179}）されたら、Auto Scalingグループの作成は完了です。

//image[startaws179][Auto Scalingグループの作成は完了][scale=0.8]{
//}

Auto Scalingグループを作ると、その瞬間に「Auto Scalingグループ配下には常に1台のインスタンスが居るのが正しい！なのにいまは0台！よし、1台作るぞ！」とEC2のインスタンスが作られます。「え、でももともとのstart-aws-instanceがいるでしょ？」と思われると思いますが、あれはあなたが自力で立てたインスタンスであって、Auto Scalingによって管理されているインスタンスではないので、カウント外なのです。

=== もともとのインスタンスを停止させよう

EC2ダッシュボードでインスタンスを開く（@<img>{stopInstance1}）と、もともとあった「start-aws-instance」と、いまAuto Scalingによって作られたインスタンスが2つ並んでいます。

//image[stopInstance1][インスタンスが2つ並んでいる][scale=0.8]{
//}

Auto Scalingで管理されている1台だけが起動していれば十分なので、もともとあった「start-aws-instance」は停止します。「start-aws-instance」にチェックを入れて、「インスタンスの状態＞インスタンスを停止」をクリック（@<img>{stopInstance2}）してください。なお今後、ミドルウェアなどを追加でインストールするときは、この「start-aws-instance」をAMIの元として使うため、停止はしますが、終了（削除）はしないでおきます。

//image[stopInstance2][「start-aws-instance」にチェックを入れてインスタンスを停止][scale=0.8]{
//}

停止対象が「start-aws-instance」であることを確認したら、「停止」をクリック（@<img>{stopInstance3}）します。

//image[stopInstance3][停止対象のインスタンスを確認して「停止」をクリック][scale=0.8]{
//}

EC2ダッシュボードのターゲットグループで、「elb-target-group」のTargetsタブを確認（@<img>{stopInstance4}）すると、停止した「start-aws-instance」はStatusがunusedになっています。しかしAuto Scalingで作られたもう1台のインスタンスがhealthyになっているので、サイトには影響がありません。

//image[stopInstance4][「start-aws-instance」が停止しても、もう1台がhealthyなのでサイトには影響がない][scale=0.8]{
//}

ブラウザで「@<href>{http://www.自分のドメイン名/}」を開くと、サイトは問題なく表示（@<img>{startaws199}）されます。

//image[startaws199][「@<href>{http://www.自分のドメイン名/}」を開くとサイトが問題なく表示される][scale=0.8]{
//}

=== インスタンスを削除して自動復旧を試してみよう

それでは、Auto Scalingで作られたインスタンスを終了させても、サイトが自動復旧するかテストしてみましょう。

EC2ダッシュボードの左メニューからインスタンスを開いたら、先ほどAuto Scalingによって作られたインスタンス（つまり「start-aws-instance」ではない方）にチェックをいれて、「インスタンスの状態＞インスタンスの終了」（@<img>{startaws198}）をクリックしてみましょう。@<fn>{ami}

//footnote[ami][もしAuto Scalingの設定に失敗していて自動復旧しなくても、手動でAMIからインスタンスを作り直せば復旧できます。最悪、AMIの作成すら失敗していたとしても、停止中の「start-aws-instance」からAMIを作り直して、そこからインスタンスを作り直せば復旧できますので大丈夫です。]

//image[startaws198][「インスタンスの状態＞インスタンスの停止」をクリック][scale=0.8]{
//}

なにやら恐ろしげな警告が表示（@<img>{startaws200}）されますが、要は「このインスタンスはAuto Scalingグループで管理されているから、終了させても代替のインスタンスが自動起動してきますよ」と教えてくれているのです。ありがたいですね。代替のインスタンスが自動起動してくるのは望むところなので、元気よく「終了」をクリックしましょう。

//image[startaws200][「終了」をクリック][scale=0.8]{
//}

インスタンスを終了させると、インスタンスの状態がまず「シャットダウン中」に変わります。再びブラウザで「@<href>{http://www.自分のドメイン名/}」を見てみましょう。サイトがウェブサーバごといなくなってしまったので、「503 Service Temporarily Unavailable」と表示（@<img>{startaws201}）されています。

//image[startaws201][再び@<href>{http://www.自分のドメイン名/}」を開くと「503 Service Temporarily Unavailable」と表示される][scale=0.8]{
//}

停止から約1分後、「Auto Scaling: launch for group "start-aws-autoscaling-group"」という件名のメール（@<img>{startaws202}）が届きました。どうやらAuto ScalingによってAMIからインスタンスが自動生成されたようです。

//image[startaws202][Auto Scalingによるインスタンスの生成を知らせるメールが届いた][scale=0.6]{
//}

EC2ダッシュボードの左メニューでインスタンスを開くと、先ほど手動で終了させたインスタンスの状態は「終了済み」@<fn>{terminated}になり、その下に新たなインスタンスが表示（@<img>{startaws204}）されています。

//footnote[terminated][インスタンスの状態が「終了済み」になってから、一定時間が経過すると一覧に表示されなくなります。]

//image[startaws204][先ほど終了したインスタンスの下に新たなインスタンスが表示されている][scale=0.8]{
//}

停止から約3分後、もう一度ブラウザで「@<href>{http://www.自分のドメイン名/}」を開いてみましょう。サイトは記事も画像もちゃんと復旧しています。（@<img>{startaws203}）

//image[startaws203][もう一度@<href>{http://www.自分のドメイン名/}」を開くとサイトが復旧していた！][scale=0.8]{
//}

このようにAuto Scalingで管理されているインスタンスを終了させたことで、一時的にWordPressのサイトが見られなくなりましたが、すぐにAuto Scalingによってインスタンスが生成されサイトも自動復旧しました。このとき記事データはRDS（データベースサーバ）に、画像はS3にあるので何もかも元通りに表示されます。

但しAuto Scalingでインスタンスが自動生成された場合、SSHのログイン時に使っていたElastic IPだけは新しいインスタンスに自動で紐づきません。@<fn>{templateMiss}いまElastic IPは、停止中のインスタンス（start-aws-instance）に紐づいている状態@<fn>{freeEip}です。左メニューの「Elastic IP」をクリック（@<img>{startaws205}）して、Auto Scalingによって新しく生まれたインスタンスに「アドレスの関連付け」をしてやれば、再びSSHでログインできるようになります。

//footnote[templateMiss][起動テンプレートの「ネットワークインターフェイス」の設定で、Elastic IPのENIを付けてやれば、Elastic IPをアタッチした状態で新しいインスタンスが生まれてくると思ったのですが、どうしてもうまくいかずに諦めました。同じエラーがフォーラムに報告されていたので、次に本書を改訂するときは設定できることを祈っています。 @<href>{https://forums.aws.amazon.com/message.jspa?messageID=864259}]
//footnote[freeEip][停止中のインスタンスに紐づいている、あるいはどのインスタンスに紐付けせずに宙ぶらりんな状態になっていると、そのElastic IPは無料利用枠の対象外となってしまい、お金がかかりますので注意してください。]

//image[startaws205][新しいEC2インスタンスにElastic IPの「アドレスの関連付け」をしてあげよう][scale=0.8]{
//}

ウェブサーバを終了させても、ものの数分で何事もなかったかのように自動復旧するなんてすごくないですか？以上で、Auto ScalingでEC2インスタンスを自動復旧させる設定はおしまいです。