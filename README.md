# Pcap-qt とは
Pcap-qt は，Linux 上で動作する，ネットワークパケットをリアルタイムで可視化できるネットワークアナライザアプリケーションです．Pcap-qt を使うことによって，ユーザーはネットワーク機器から取得したネットワークパケットのデータをウィンドウ上で閲覧できるだけでなく，Qt を使って実装されたアニメーション機能によって可視化されたネットワークパケットの流れを見ることができます．

<img src="https://github.com/Gitomoking/pcap-qt/blob/images/IMG_9628.png" alt="pcap-qt logo" title="pcap-qt">

Pcap-qt の主な機能は以下の通りです：

- ネットワーク機器からのパケットキャプチャ
- リアルタイムでのパケットの可視化
- ネットワークの統計データ（トラフィック量，プロトコルの割合，通信量の多い組み合わせ）の表示
- データベースを使ったパケットデータの記録と管理

パケットキャプチャを行うためのライブラリには　C 言語の Libpcap を用いており，Libpcap を使って取得したパケットをウィンドウ上で表示する機能は Qt (C++) を使って実装しました．

# 機能紹介（スクリーンショット）
## パケットキャプチャ
pcap-qt で解析可能なプロトコルは以下の通りです：

- ARP（Address Resolution Protocol）
- IPv4（Internet Protocol v4）
- TCP（Transmission Control Protocol）
- UDP（User Datagram Protocol）
- ICMP（Internet Control Message Protocol）
- DNS（Domain Name System）

また，以下のプロトコルは解析はできませんが識別可能なプロトコルです：

- FTP（File Transfer Protocol）
- Telnet
- SSH（Secure Shell）
- HTTP（HyperText Transfer Protocol）
- HTTPS
- DHCP（Dynamic Host Configuration Protocol）
- NTP（Network Time Protocol）

以下，pcap-qt のスクリーンショットです．

メインフレーム（パケット表示）

<img src="https://github.com/Gitomoking/pcap-qt/blob/images/packet-list.png" alt="packet-list">

メインフレーム（パケット詳細）

<img src="https://github.com/Gitomoking/pcap-qt/blob/images/packet-detail.png" alt="packet-detail">

<img src="https://github.com/Gitomoking/pcap-qt/blob/images/%E3%83%8F%E3%82%9A%E3%82%B1%E3%83%83%E3%83%88%E3%83%86%E3%82%99%E3%83%BC%E3%82%BF.png" alt="packet-raw">

パケットの可視化

<img src="https://github.com/Gitomoking/pcap-qt/blob/images/可視化.png" alt="packet-visualization">

パケットの検索

<img src="https://github.com/Gitomoking/pcap-qt/blob/images/packet-search.png" alt="packet-search">

パケットのフィルタリング

<img src="https://github.com/Gitomoking/pcap-qt/blob/images/フィルタ式確認画面.png" alt="filtering-1">

<img src="https://github.com/Gitomoking/pcap-qt/blob/images/フィルタ設定画面プロトコルタブ.png" alt="filtering-2">

<img src="https://github.com/Gitomoking/pcap-qt/blob/images/フィルタ設定画面ポート番号タブ.png" alt="filtering-3">

<img src="https://github.com/Gitomoking/pcap-qt/blob/images/フィルタ設定画面ホストタブ.png" alt="filtering-4">

ネットワーク統計

<img src="https://github.com/Gitomoking/pcap-qt/blob/images/packet-traffic.png" alt="traffic">

<img src="https://github.com/Gitomoking/pcap-qt/blob/images/node-statistics.png" alt="node">

<img src="https://github.com/Gitomoking/pcap-qt/blob/images/プロトコル統計画面.png" alt="protocol">

ログ

<img src="https://github.com/Gitomoking/pcap-qt/blob/images/log.png" alt="log">

# 各ディレクトについて
本リポジトリは下記ディレクトリからなります：

- pcap-qt : GUI 表示のためのプログラム
- pcap : パケットキャプチャのためのプログラム
- pseudo-pcap : パケットキャプチャのデモンストレーションのためのプログラム

本ソフトウェアを利用する際，pcap ディレクトリと pcap-qt ディレクトリは，同じ親ディレクトリを持つようにしてください．例えば，次のようなディレクトリ構成になります．

親ディレクトリ<br />
 |---pcap-qt---pcap-qt mainwindow.cpp etc.<br />
 |---pcap---pcap-device pcap-capture device_main.cpp pcap_main.cpp etc. <br />
 |---pseudo-pcap---pseudo_capture pseudo_main.cpp etc. 

# インストール
## 準備
アプリケーションをインストールするためには，先に以下のライブラリをインストールしてください．

- libpcap および libpcap-devel　（v1.5.3で動作確認済み）． 
- Qt : QtCharts, QtSQL含む　（v5.7/5.8で動作確認済み）． 
- MariaDB-10.1.3 以降 

ただし，MariaDB は，ログ機能を利用する場合（ネットワーク統計を保存する場合）のみ必要となります．

## データベースのセットアップ（ログ機能を利用する場合）
データベースの暗号化については，付録「データベースのセットアップ.pdf」を参照してください．
データベース pcapdb に次のテーブルを作成します．

- LogData
- NodeSetData
- Traffic
- Protocol

それぞれのテーブルの作成コマンドを以下に示します．

(LogDataテーブルの作成)

```sql
CREATE TABLE LogData(
	LogID INT PRIMARY KEY AUTO_INCREMENT,
	DateTime DATETIME,
	FavFlag TINYINT(1) DEFAULT 0,
	Memo VARCHAR(50)
) ENGINE=InnoDB ENCRYPTED=YES ENCRYPTION_KEY_ID=1;
```

(NodeSetテーブルの作成)

```sql
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

(Trafficテーブルの作成)

```sql
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

(Protocolテーブルの作成)

```sql
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

```cpp
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

```cpp
process = new QProcess(this);   // プロセスの生成
process->start(pcap-capture, arguments); // 外部プログラムの実行
```

を以下のように変更してください．process->start() に渡す第1引数を変更します．

```cpp
process = new QProcess(this);   // プロセスの生成
process->start(pseudo_capture, arguments); // 外部プログラムの実行
```

再び pcap-qt をビルドし，アプリを実行すると擬似的なパケットキャプチャを行えます．

また，本来のパケットキャプチャを行いたい場合は，process->start() の第1引数を元の引数 pcap-capture に戻してください．

以上でインストール手順を終了します．
