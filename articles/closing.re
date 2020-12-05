= AWSをやめたくなったらすること

この章では1年間の無料利用期間が終わりに近づいて、AWSの利用をやめたくなったらしておくべき手続きを紹介します。

//pagebreak

== 無料の1年が終わる前にすべきこと

@<chapref>{doTheFirst}で書いたとおり、AWSアカウントを作成してから1年間は無料利用枠の範囲内であれば利用料が無料となります。

たとえ全然使わない月があったとしてもその分の無料利用枠が翌月以降に繰り越されることはありません。1年経過後、無料利用枠の有効期限が切れると以降は通常の従量課金できっちり請求が来ますので、不要になったインスタンスやサービスは忘れずに削除してください。

何を使ったか忘れてしまって全部消せるか心配・・・という場合は、AWSのアカウントごと停止するという方法もあります。

=== AWSアカウントを停止する

ルートユーザーでサインインしたら、マネジメントコンソールの右上にあるルートユーザー名（@<img>{startaws79}）から「アカウント」をクリックしてください。

//image[startaws79][ルートユーザ名＞アカウント][scale=0.8]{
//}

アカウントのページを一番下までスクロールして「アカウントの解約」（@<img>{startaws80}）の記載内容を確認してください。

いくつか注意点があります。たとえばEC2やRDSのインスタンスを削除せずにアカウントを解約した場合、そのインスタンスは解約後すぐに消えるわけではありません。インスタンスが消えるタイミング@<fn>{delete}はAWSにゆだねられます。サイト自体をすぐにクローズしたいのであれば先にインスタンスを削除してからアカウントを解約するようにしましょう。

//footnote[delete][検証していないので推測ですが「AWSアカウントに残されたコンテンツは、閉鎖後期間が過ぎると削除されます。」という記載がありますので、アカウント解約後も90日間はそのままなのかも知れません。]

また月の途中で解約した場合、その日までの利用料は請求されます。たとえば2018年9月4日@<fn>{tds}にアカウントを解約した場合、9月1日～4日までの利用料は10月の初めに請求されます。解約後、90日間はアカウントを再開（再有効化）できるようですが、その期間を過ぎると再開はできず、同じメールアドレスで新しいアカウントを作ることもできなくなります。

//footnote[tds][冒険とイマジネーションの海よ、17周年おめでとう！]

内容を確認し、同意して解約する場合はチェックボックスにチェックを入れて「アカウントの解約」をクリックします。

//image[startaws80][チェックボックスにチェックを入れて「アカウントの解約」をクリック][scale=0.8]{
//}

「本当にアカウントを解約してもよろしいですか?」と表示されるので、解約してよければ「アカウントの解約」をクリックします。（@<img>{startaws81}）

//image[startaws81][解約してよければ「アカウントの解約」をクリック][scale=0.8]{
//}

アカウント解約を知らせるメールが届き、以降はルートユーザーでもIAMユーザーでもマネジメントコンソールにはサインインできなくなります。