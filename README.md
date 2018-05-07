# Pcap-qt とは
Pcap-qt は，Linux 上で動作する，ネットワークパケットをリアルタイムで可視化できるネットワークアナライザアプリケーションです．Pcap-qt を使うことによって，ユーザーはネットワーク機器から取得したネットワークパケットのデータをウィンドウ上で閲覧できるだけでなく，Qt を使って実装されたアニメーション機能によって可視化されたネットワークパケットの流れを見ることができます．

<img src="https://github.com/Gitomoking/pcap-qt/blob/images/IMG_9628.png" alt="pcap-qt logo" title="pcap-qt">

Pcap-qt の主な機能は以下の通りです：
- ネットワーク機器からのパケットキャプチャ
- リアルタイムでのパケットの可視化
- ネットワークの統計データ（トラフィック量，プロトコルの割合，通信量の多い組み合わせ）の表示
- データベースを使ったパケットデータの記録と管理

パケットキャプチャを行うためのライブラリには　C 言語の Libpcap を用いており，Libpcap を使って取得したパケットをウィンドウ上で表示する機能は Qt (C++) を使って実装しました．

# 各ディレクトについて
本リポジトリは下記ディレクトリからなります：

- pcap-qt : GUI 表示のためのプログラム
- pcap : パケットキャプチャのためのプログラム
- pseudo-pcap : パケットキャプチャのデモンストレーションのためのプログラム

本ソフトウェアを利用する際，pcap ディレクトリと pcap-qt ディレクトリは，同じ親ディレクトリを持つようにしてください．例えば，次のようなディレクトリ構成になります．
親ディレクトリ
 |---pcap-qt---pcap-qt mainwindow.cpp etc.
 |---pcap---pcap-device pcap-capture device_main.cpp pcap_main.cpp etc.
 |---pseudo-pcap---pseudo_capture pseudo_main.cpp etc.

# インストール
## 準備
アプリケーションをインストールするためには，先に以下のライブラリをインストールしてください．

a) libpcap および libpcap-devel　（v1.5.3で動作確認済み）．
b) Qt : QtCharts, QtSQL含む　（v5.7/5.8で動作確認済み）．
c) MariaDB-10.1.3 以降

ただし，c)は，ログ機能を利用する（ネットワーク統計を保存する）場合のみです．

## データベースのセットアップ（ログ機能を利用する場合）
データベースの暗号化については，付録「データベースのセットアップ.pdf」を参照してください．
データベース pcapdb に次のテーブルを作成します．

- LogData
- NodeSetData
- Traffic
- Protocol

それぞれのテーブルの作成コマンドを以下に示します．

```sql:LogDataテーブルの作成
CREATE TABLE LogData(
	LogID INT PRIMARY KEY AUTO_INCREMENT,
	DateTime DATETIME,
	FavFlag TINYINT(1) DEFAULT 0,
	Memo VARCHAR(50)
) ENGINE=InnoDB ENCRYPTED=YES ENCRYPTION_KEY_ID=1;
```

```sql:NodeSetテーブルの作成
CREATE TABLE NodeSet(
	LogID INT,
	NodeSetID INT PRIMARY KEY AUTO_INCREMENT,
	SrcNode VARCHAR(50),
	DstNode VARCHAR(50),
	TrafficVolStat FLOAT DEFAULT 0,
	FOREIGN KEY (LogID)
	REFERENCES LogData(LogID)
	ON DELETE CASCADE
) ENGINE=InnoDB ENCRYPTED=YES ENCRYPTION_KEY_ID=1;
```

```sql:Trafficテーブルの作成
CREATE TABLE Traffic(
	LogID INT,
	NodeSetID INT,
	DataID INT PRIMARY KEY AUTO_INCREMENT,
	TrafficVol FLOAT DEFAULT 0,
	FOREIGN KEY (LogID, NodeSetID)
	REFERENCES NodeSet(LogID, NodeSetID)
	ON DELETE CASCADE
) ENGINE=InnoDB ENCRYPTED=YES ENCRYPTION_KEY_ID=1;
```

```sql:Protocolテーブルの作成
CREATE TABLE Protocol(
	LogID INT PRIMARY KEY,
	ARP INT DEFAULT 0,
	TCP INT DEFAULT 0,
	UDP INT DEFAULT 0,
	ICMP INT DEFAULT 0,
	DNS INT DEFAULT 0,
	HTTP INT DEFAULT 0,
	HTTPS INT DEFAULT 0,
	FTP INT DEFAULT 0,
	NTP INT DEFAULT 0,
	DHCP INT DEFAULT 0,
	Telnet INT DEFAULT 0,
	SSH INT DEFAULT 0,
	Others INT DEFAULT 0,
	FOREIGN KEY (LogID)
	REFERENCES LogData(LogID)
	ON DELETE CASCADE
) ENGINE=InnoDB ENCRYPTED=YES ENCRYPTION_KEY_ID=1;
```

次に，アプリケーションからデータベースに接続できるようプログラムを編集します．
pcap-qt ディレクトリの dbaccess.cpp ソースファイルを編集してください．

DBAccess クラスのコンストラクタが次のようになっていると思います．

```cpp:dbaccess.cppのコンストラクタ
myDB = QSqlDatabase::addDatabase("QMYSQL"); // MySQLに接続
myDB.setHostName("127.0.0.1");  // ホスト名（ローカルホスト）
myDB.setDatabaseName("pcap"); // データベース名
myDB.setUserName("root");   // ユーザ名
myDB.setPassword("");   // ユーザのパスワード
```

これらの変数（データベース名，ユーザ名，ユーザのパスワード）に適切な値を代入してください．

これでデータベースのセットアップは終了です．

## ビルド
pcap ディレクトリと pcap-qt ディレクトリには，それぞれプログラムのソースコードが格納されています．pcap ディレクトリに Makefile があること，pcap-qt ディレクトリに pcap-qt.pro があることを確認してください．

まず，パケットキャプチャのプログラム pcap-device と pcap-capture をビルドします．次のコマンドを実行します．「ディレクトリ」には pcap の親ディレクトリを入力してください．

```
$ cd /ディレクトリ/pcap
$ make
```

次に，GUI アプリケーション pcap-qt をビルドします．次のコマンドを実行します．

```
$ cd /ディレクトリ/pcap-qt
$ qmake pcap-qt.pro
$ make
```

これでビルドは終了です．

## アプリケーションの実行
**pcap-qt は，必ず管理者権限で実行してください**．管理者権限がないと，pcap-device でネットワークデバイスを取得するとき，取得できるデバイスが制限されます．

次のコマンドを実行すると，pcap-qt が起動します．パケットキャプチャを行うデバイスを選択してください．

```アプリの実行
$ sudo /ディレクトリ/pcap-qt/pcap-qt
```

## パケットキャプチャのデモンストレーションを行いたい場合（pseudo-pcap を利用する）
pseudo_capture は，疑似的なパケットを生成し，標準出力に出力するプログラムです． pseudo_capture を利用することによって，ネットワークに接続していない場合でも，pcap-qt を動作させることができます． 
まず，ディレクトリ pseudo-pcap 内をビルドしてください．

```
$ cd /ディレクトリ/pseudo-pcap
$ make
```

次に，pcap-qt 内で使用するキャプチャプログラムを pseudo_capture に変更します．

pcap-qt の mainwindow.cpp を編集します．  
449行あたりの次のコード

```startPacketCapture()関数内変更前
process = new QProcess(this);   // プロセスの生成
process->start(pcap-capture, arguments); // 外部プログラムの実行
```

を以下のように変更してください．process->start() に渡す第1引数を変更します．

```startPacketCapture()関数内変更後
process = new QProcess(this);   // プロセスの生成
process->start(pseudo_capture, arguments); // 外部プログラムの実行
```

再び pcap-qt をビルドし，アプリを実行すると擬似的なパケットキャプチャを行えます．

また，本来のパケットキャプチャを行いたい場合は，process->start() の第1引数を元の引数 pcap-capture に戻してください．

以上でインストール手順を終了します．
