#include "pcap_transport.h"
#include <netinet/ip_icmp.h>
using namespace std;

/* コンストラクタ :メンバ変数にデータ格納 */
ICMPPacket::ICMPPacket (const u_char *raw) {
	struct basic_icmp *icmphdr = (struct basic_icmp *)raw;
	this->type = icmphdr->type;
	this->code = icmphdr->code;
	this->check = ntohs (icmphdr->check);
}

/* getPacketDetail()関数 :パケットの備考を返す関数 */
string ICMPPacket::getPacketDetail (const u_char *raw) {
	stringstream ss;
	ss.str("");
	switch (this->type) {
		case ICMP_ECHOREPLY : {
                     /* Echo Reply */
                     ss << "エコー応答, ";
			/* ICMPデータは，識別子とシーケンス番号 */
			u_int16_t *data = (u_int16_t *)raw;
			ss << "ID:" << dec << ntohs (data[0])
			   << "(0x" << hex << ntohs (data[0]) << ")" << ", ";
			ss << "シーケンス番号:" << dec << ntohs (data[1])
			   << "(0x" << hex << ntohs (data[1]) << ")" << dec;
              } break;
              case ICMP_DEST_UNREACH :
                     /* Destination Unreachable */
                     ss << "宛先到達不能, ";
                     switch (this->code) {
                            case ICMP_NET_UNREACH :
                                   ss << "ネットワークに到達できない"; break;
                            case ICMP_HOST_UNREACH :
                                   ss << "ホストに到達できない"; break;
                            case ICMP_PROT_UNREACH :
                                   ss << "プロトコルに到達できない"; break;
                            case ICMP_PORT_UNREACH :
                                   ss << "ポートに到達できない"; break;
                            case ICMP_FRAG_NEEDED :
                                   ss << "フラグメント化が必要だが，DFフラグが設定されている"; break;
                            case ICMP_SR_FAILED :
                                   ss << "ソースルートが使用できない"; break;
                            case ICMP_NET_UNKNOWN :
                                   ss << "宛先ネットワークが不明"; break;
                            case ICMP_HOST_UNKNOWN :
                                   ss << "宛先ホストが不明"; break;
                            case ICMP_HOST_ISOLATED :
                                   ss << "送信元ホストが分離されている"; break;
                            case ICMP_NET_ANO :
                                   ss << "宛先ネットワークとの通信が管理上禁止されている"; break;
                            case ICMP_HOST_ANO :
                                   ss << "宛先ホストとの通信が管理上禁止されている"; break;
                            case ICMP_NET_UNR_TOS :
                                   ss << "指定のサービスタイプではネットワークに到達できない"; break;
                            case ICMP_HOST_UNR_TOS :
                                   ss << "指定のサービスタイプではホストに到達できない"; break;
                     }
			break;
              case ICMP_SOURCE_QUENCH :
                     /* Source Quench */
                     ss << "発信抑制";
			break;
              case ICMP_REDIRECT : {
                     /* Redirect */
                     ss << "ルート変更, ";
                     switch (this->code) {
                            case ICMP_REDIR_NET:
                                   ss << "ネットワークに関するルート変更"; break;
                            case ICMP_REDIR_HOST :
                                   ss << "ホストに関するルート変更"; break;
                            case ICMP_REDIR_NETTOS :
                                   ss << "このサービスタイプとネットワークに関するルート変更"; break;
                            case ICMP_REDIR_HOSTTOS :
                                   ss << "このサービスタイプとネットワークに関するルート変更"; break;
                     }
                     /* ICMPデータはIPアドレス */
			struct in_addr *data = (struct in_addr *)raw;
			ss << ", IPアドレス:" << inet_ntoa (*data);
              } break;
              case ICMP_ECHO : {
                     /* Echo Request */
                     ss << "エコー要求, ";
			u_int16_t *data = (u_int16_t *)raw;
			ss << "ID:" << dec << ntohs (data[0])
			   << "(0x" << hex << ntohs (data[0]) << ")" << ", ";
			ss << "シーケンス番号:" << dec << ntohs (data[1])
			   << "(0x" << hex << ntohs (data[1]) << ")" << dec;
              } break;
              case ICMP_TIME_EXCEEDED :
                     /* Time Exceeded for Datagram */
                     ss << "時間超過, ";
                     switch (this->code) {
                            case ICMP_EXC_TTL :
                                   ss << "生存時間終了"; break;
                            case ICMP_EXC_FRAGTIME :
                                   ss << "フラグメント再構成タイマー終了"; break;
                     }
			break;
              case ICMP_PARAMETERPROB : {
                     /* Parameter Problem on Datagram */
                     ss << "パラメータ異常, ";
			u_int8_t *data = (u_int8_t *)raw; /* ポインタ */
			if (*data == 0)
				ss << "IPバージョン，またはIPヘッダ長フィールドに問題あり";
			else if (*data == 1)
				ss << "IPサービスタイプフィールドに問題あり";
			else if (*data == 2 || *data == 3)
				ss << "IPパケット長フィールドに問題あり";
			else if (*data == 4 || *data == 5)
				ss << "IP識別子フィールドに問題あり";
			else if (*data == 6 || *data == 7)
				ss << "IPフラグフィールドに問題あり";
			else if (*data == 8)
				ss << "IP生存時間フィールドに問題あり";
			else if (*data == 9)
				ss << "IPプロトコルフィールドに問題あり";
			else if (*data == 10 || *data == 11)
				ss << "IPヘッダチェックサムフィールドに問題あり";
			else if (*data > 11 || *data < 16)
				ss << "送信元IPアドレスフィールドに問題あり";
			else if (*data > 15 || *data < 20)
				ss << "宛先IPアドレスフィールドに問題あり";
			else
				ss << "IPオプションフィールドに問題あり";
		} break;
              case ICMP_TIMESTAMP : {
                     /* Timestamp Request */
                     ss << "タイムスタンプ要求, ";
			struct icmp_timestamp_data *data = (struct icmp_timestamp_data *)raw;
			ss << "ID:" << dec << ntohs (data->id) << "(0x"
			   << hex << ntohs (data->id) << "), ";
			ss << "シーケンス番号:" << dec << ntohs (data->seq)
			   << "(0x" << hex << ntohs (data->seq) << "), ";
			ss << "開始時刻" << dec << ntohs (data->startts)
			   << "(0x" << hex << ntohs (data->startts) << ")";
              } break;
              case ICMP_TIMESTAMPREPLY : {
                     /* Timestamp Reply */
                     ss << "タイムスタンプ応答, ";
			struct icmp_timestamp_data *data = (struct icmp_timestamp_data *)raw;
			ss << "識別子:" << dec << ntohs (data->id) << "(0x"
			   << hex << ntohs (data->id) << "), ";
			ss << "シーケンス番号:" << dec << ntohs (data->seq)
			   << "(0x" << hex << ntohs (data->seq) << "), ";
			ss << "開始時刻:" << dec << ntohs (data->startts)
			   << "(0x" << hex << ntohs (data->startts) << "), ";
			ss << "受信時刻:" << dec << ntohs (data->receivets)
			   << "(0x" << hex << ntohs (data->receivets) << "), ";
			ss << "送出時刻:" << dec << ntohs (data->sendts)
			   << "(0x" << hex << ntohs (data->sendts) << ")";
              } break;
              case ICMP_ADDRESS : {
                     /* Address Mask Request */
                     ss << "アドレスマスク要求, ";
			struct icmp_addressmask_data *data = (struct icmp_addressmask_data *)raw;
			ss << "識別子:" << dec << ntohs (data->id) << "(0x"
			   << hex << ntohs (data->id) << "), ";
			ss << "シーケンス番号:" << dec << ntohs (data->seq)
			   << "(0x" << hex << ntohs (data->seq) << "), ";
			ss << "サブネットマスク:" << inet_ntoa (data->mask);
              } break;
		case ICMP_ADDRESSREPLY : {
                     /* Address Mask Reply */
                     ss << "アドレスマスク応答, ";
			struct icmp_addressmask_data *data = (struct icmp_addressmask_data *)raw;
			ss << "識別子:" << dec << ntohs (data->id) << "(0x"
			   << hex << ntohs (data->id) << "), ";
			ss << "シーケンス番号:" << dec << ntohs (data->seq)
			   << "(0x" << hex << ntohs (data->seq) << "), ";
			ss << "サブネットマスク:" << inet_ntoa (data->mask);
              } break;
	}
	return ss.str();
}

/* getTransportDetail()関数 :トランスポート層詳細を返す関数 */
string ICMPPacket::getTransportDetail (const u_char *raw) {
	stringstream ss;
	ss.str("");
	ss << "[ICMP]" << this->getPacketDetail(raw) << "\n";
	ss << " > ICMPタイプ : " << dec << (int)this->type;
	switch (this->type) {
		case ICMP_ECHOREPLY : {
                     /* Echo Reply */
                     ss << "(エコー応答)\n";
			ss << " > ICMPコード : " << (int)this->code << "\n";
			/* ICMPデータは，識別子とシーケンス番号 */
			u_int16_t *data = (u_int16_t *)raw;
			ss << " > 識別子 : " << dec << ntohs (data[0])
			   << "(0x" << hex << ntohs (data[0]) << ")" << ", ";
			ss << " > シーケンス番号 : " << dec << ntohs (data[1])
			   << "(0x" << hex << ntohs (data[1]) << ")" << dec;
              } break;
              case ICMP_DEST_UNREACH :
                     /* Destination Unreachable */
                     ss << "(宛先到達不能)\n";
                     ss << " > ICMPコード : " << (int)this->code << "(";
                     switch (this->code) {
                            case ICMP_NET_UNREACH :
                                   ss << "ネットワークに到達できない"; break;
                            case ICMP_HOST_UNREACH :
                                   ss << "ホストに到達できない"; break;
                            case ICMP_PROT_UNREACH :
                                   ss << "プロトコルに到達できない"; break;
                            case ICMP_PORT_UNREACH :
                                   ss << "ポートに到達できない"; break;
                            case ICMP_FRAG_NEEDED :
                                   ss << "フラグメント化が必要だが，DFフラグが設定されている"; break;
                            case ICMP_SR_FAILED :
                                   ss << "ソースルートが使用できない"; break;
                            case ICMP_NET_UNKNOWN :
                                   ss << "宛先ネットワークが不明"; break;
                            case ICMP_HOST_UNKNOWN :
                                   ss << "宛先ホストが不明"; break;
                            case ICMP_HOST_ISOLATED :
                                   ss << "送信元ホストが分離されている"; break;
                            case ICMP_NET_ANO :
                                   ss << "宛先ネットワークとの通信が管理上禁止されている"; break;
                            case ICMP_HOST_ANO :
                                   ss << "宛先ホストとの通信が管理上禁止されている"; break;
                            case ICMP_NET_UNR_TOS :
                                   ss << "指定のサービスタイプではネットワークに到達できない"; break;
                            case ICMP_HOST_UNR_TOS :
                                   ss << "指定のサービスタイプではホストに到達できない"; break;
                     }
			ss << ")\n";
                     break;
              case ICMP_SOURCE_QUENCH :
                     /* Source Quench */
                     ss << "(発信抑制)\n";
                     ss << " > ICMPコード : " << (int)this->code;
                     break;
              case ICMP_REDIRECT : {
                     /* Redirect */
                     ss << "(ルート変更)\n";
                     ss << " > ICMPコード : " << (int)this->code << "(";
                     switch (this->code) {
                            case ICMP_REDIR_NET:
                                   ss << "ネットワークに関するルート変更"; break;
                            case ICMP_REDIR_HOST :
                                   ss << "ホストに関するルート変更"; break;
                            case ICMP_REDIR_NETTOS :
                                   ss << "このサービスタイプとネットワークに関するルート変更"; break;
                            case ICMP_REDIR_HOSTTOS :
                                   ss << "このサービスタイプとネットワークに関するルート変更"; break;
                     }
                     ss << ")\n";
                     /* ICMPデータはIPアドレス */
			struct in_addr *data = (struct in_addr *)raw;
			ss << " > ルータのIPアドレス : " << inet_ntoa (*data);
              } break;
              case ICMP_ECHO : {
                     /* Echo Request */
                     ss << "(エコー要求)\n";
                     ss << " > ICMPコード : " << (int)this->code << "\n";
			/* ICMPデータは，識別子とシーケンス番号 */
			u_int16_t *data = (u_int16_t *)raw;
			ss << " > 識別子 : " << dec << ntohs (data[0])
			   << "(0x" << hex << ntohs (data[0]) << ")" << ", ";
			ss << " > シーケンス番号 : " << dec << ntohs (data[1])
			   << "(0x" << hex << ntohs (data[1]) << ")" << dec;
              } break;
              case ICMP_TIME_EXCEEDED :
                     /* Time Exceeded for Datagram */
                     ss << "(時間超過)\n";
                     ss << " > ICMPコード : " << (int)this->code << "\n";
			ss << " > ";
                     switch (this->code) {
                            case ICMP_EXC_TTL :
                                   ss << "生存時間終了"; break;
                            case ICMP_EXC_FRAGTIME :
                                   ss << "フラグメント再構成タイマー終了"; break;
                     }
			break;
              case ICMP_PARAMETERPROB : {
                     /* Parameter Problem on Datagram */
                     ss << "(パラメータ異常)\n";
                     ss << " > ICMPコード : " << (int)this->code << "\n";
			u_int8_t *data = (u_int8_t *)raw; /* ポインタ */
			ss << " > ";
			if (*data == 0)
				ss << "IPバージョン，またはIPヘッダ長フィールドに問題あり";
			else if (*data == 1)
				ss << "IPサービスタイプフィールドに問題あり";
			else if (*data == 2 || *data == 3)
				ss << "IPパケット長フィールドに問題あり";
			else if (*data == 4 || *data == 5)
				ss << "IP識別子フィールドに問題あり";
			else if (*data == 6 || *data == 7)
				ss << "IPフラグフィールドに問題あり";
			else if (*data == 8)
				ss << "IP生存時間フィールドに問題あり";
			else if (*data == 9)
				ss << "IPプロトコルフィールドに問題あり";
			else if (*data == 10 || *data == 11)
				ss << "IPヘッダチェックサムフィールドに問題あり";
			else if (*data > 11 || *data < 16)
				ss << "送信元IPアドレスフィールドに問題あり";
			else if (*data > 15 || *data < 20)
				ss << "宛先IPアドレスフィールドに問題あり";
			else
				ss << "IPオプションフィールドに問題あり";
		} break;
              case ICMP_TIMESTAMP : {
                     /* Timestamp Request */
                     ss << "(タイムスタンプ要求)\n";
                     ss << " > ICMPコード : " << (int)this->code << "\n";
			struct icmp_timestamp_data *data = (struct icmp_timestamp_data *)raw;
			ss << " > 識別子 : " << dec << ntohs (data->id) << "(0x"
			   << hex << ntohs (data->id) << ")\n";
			ss << " > シーケンス番号 : " << dec << ntohs (data->seq)
			   << "(0x" << hex << ntohs (data->seq) << ")\n";
			ss << " > 開始タイムスタンプ : " << dec << ntohs (data->startts)
			   << "(0x" << hex << ntohs (data->startts) << ")\n";
			ss << " > 受信タイムスタンプ : " << dec << ntohs (data->receivets)
			   << "(0x" << hex << ntohs (data->receivets) << ")\n";
			ss << " > 送出タイムスタンプ : " << dec << ntohs (data->sendts)
			   << "(0x" << hex << ntohs (data->sendts) << ")";
              } break;
              case ICMP_TIMESTAMPREPLY : {
                     /* Timestamp Reply */
                     ss << "(タイムスタンプ応答)\n";
                     ss << " > ICMPコード : " << (int)this->code << "\n";
			struct icmp_timestamp_data *data = (struct icmp_timestamp_data *)raw;
			ss << " > 識別子 : " << dec << ntohs (data->id) << "(0x"
			   << hex << ntohs (data->id) << ")\n";
			ss << " > シーケンス番号 : " << dec << ntohs (data->seq)
			   << "(0x" << hex << ntohs (data->seq) << ")\n";
			ss << " > 開始タイムスタンプ : " << dec << ntohs (data->startts)
			   << "(0x" << hex << ntohs (data->startts) << ")\n";
			ss << " > 受信タイムスタンプ : " << dec << ntohs (data->receivets)
			   << "(0x" << hex << ntohs (data->receivets) << ")\n";
			ss << " > 送出タイムスタンプ : " << dec << ntohs (data->sendts)
			   << "(0x" << hex << ntohs (data->sendts) << ")";
              } break;
              case ICMP_ADDRESS : {
                     /* Address Mask Request */
                     ss << "(アドレスマスク要求)\n";
                     ss << " > ICMPコード : " << (int)this->code << "\n";
			struct icmp_addressmask_data *data = (struct icmp_addressmask_data *)raw;
			ss << " > 識別子 : " << dec << ntohs (data->id) << "(0x"
			   << hex << ntohs (data->id) << ")\n";
			ss << " > シーケンス番号 : " << dec << ntohs (data->seq)
			   << "(0x" << hex << ntohs (data->seq) << ")\n";
			ss << " > サブネットマスク : " << inet_ntoa (data->mask);
              } break;
		case ICMP_ADDRESSREPLY : {
                     /* Address Mask Reply */
                     ss << "(アドレスマスク応答)\n";
                     ss << " > ICMPコード : " << (int)this->code << "\n";
			struct icmp_addressmask_data *data = (struct icmp_addressmask_data *)raw;
			ss << " > 識別子 : " << dec << ntohs (data->id) << "(0x"
			   << hex << ntohs (data->id) << ")\n";
			ss << " > シーケンス番号 : " << dec << ntohs (data->seq)
			   << "(0x" << hex << ntohs (data->seq) << ")\n";
			ss << " > サブネットマスク : " << inet_ntoa (data->mask);
              } break;
	}
	ss << "\n";
	ss << "チェックサム : 0x" << hex << ntohs (this->check) << "\n";
	ss << dec;
	return ss.str();
}

/* checkSum()関数 :チェックサムを計算する関数 */
bool ICMPPacket::checkSum (void) {
	return true;
}
