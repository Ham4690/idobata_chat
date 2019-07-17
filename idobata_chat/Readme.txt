担当:室田：idobata_chat_server.c
    東口: idobata_chat.c,idobata_chat_client.c

特徴:

苦労した点:
    お互いのコードでかみ合わない部分やコンパイルエラーが生じた際にどちらの問題か検証する作業が大変だった。
    計画通りにプログラムを作成できず、テストを十分に行えなかった。
    ソースコードの書き方や作成したいプログラムの情報共有が十分に行えなかった。
    また相手に自分が作りたいもの、相手に直して欲しい部分などを説明するのが難しいと感じた。


使用方法
    ./idobata_chat <username> (<port_number>)

username:クライアントとして起動したい場合に自身の名前となるもの
(port_number):任意で接続したいポート番号があれば指定できる
            　なお、指定しなかった場合50001番に接続するようになっている。

起動時
<server>
Wait_count:1
Wait_count:2
Wait_count:3
S
You are Server

<client>:taroが接続する場合
Wait_count:1
C
Hello. Welcome idobata_chat [tetuo].

----------画面説明-----------
Wait_count：クライアント、サーバーの判別時に送ったパケットの回数を表示
            ３回送信終了した場合はサーバとなる
S:サーバとして起動したという表記
C:クライアントとして起動したという表記
"You are Server":サーバとして接続したことをユーザに伝えるメッセージ
"Hello. Welcome idobata_chat [tetuo].":クライアントとして接続したことをユーザに伝えるメッセージ

特徴・工夫した点
    新しいクライアントが接続するたびに他のクライアントに"--login--"と出力するようになっている。
    なるべく関数にまとめてソースコードがみやすいように心がけた。

起動画面（例）:taroが接続した時

$ ./idobata taro
Wait_count:1
C
Hello. Welcome idobata_chat [taro].
                                        [hanako] --login--
                                        [yasuo] --login--
                                        [tetuo] --login--
                                        [kanako] --login--
Hi
                                        [kanako] Hello
                                        [server] [yasuo]が退出しました.
                                        [Takeshi] --login--
