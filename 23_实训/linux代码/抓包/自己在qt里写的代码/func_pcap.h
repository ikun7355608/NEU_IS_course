#ifndef FUNC_PCAP_H
#define FUNC_PCAP_H
#include<pcap.h>
#include<string>
#include<arpa/inet.h>
#include<QString>
using namespace std;
static int packet_number = 1;
string str;
struct ether_header
{
    u_int8_t ether_dhost[6];
    u_int8_t ether_shost[6];
    u_int8_t ether_type;
};
typedef u_int32_t in_addr_t;
//IP协议
struct ip_header
{
#ifdef WORDS_BIGENDIAN
    u_int8_t ip_version : 4, ip_header_length : 4;
#else
    u_int8_t ip_header_length:4,ip_version:4;
#endif
    u_int8_t ip_tos;                                        //TPS服务质量
    u_int16_t ip_length;                                    //总长度
    u_int16_t ip_id;                                        //标识
    u_int16_t ip_off;                                       //偏移
    u_int8_t ip_ttl;                                        //生存时间
    u_int8_t ip_protocol;                                   //协议类型
    u_int16_t ip_checksum;                                  //校验和
    struct in_addr ip_source_address;                       //源IP
    struct in_addr ip_destination_address;                  //目的ip
};
//ARP协议
struct arp_header
{
    u_int16_t arp_hardware_type;                        //硬件地址类型
    u_int16_t arp_protocol_type;                        //协议地址类型
    u_int8_t arp_hardwware_length;                      //硬件地址长度
    u_int8_t arp_protocol_length;                       //协议地址长度
    u_int16_t arp_operation_code;                       //操作类型
    u_int8_t arp_source_ethernet_address[6];            //源以太网地址
    u_int8_t arp_source_ip_address[4];                  //源IP地址
    u_int8_t arp_destination_ethernet_address[6];       //目的以太网地址
    u_int8_t arp_destination_ip_address[4];             //目的ip地址
};
//TCP协议
struct tcp_header
{
    u_int16_t tcp_source_port;                              //TCP源端口
    u_int16_t tcp_destinatin_port;                          //TCP目的端口
    u_int32_t tcp_acknowledgement;                          //序列号
    u_int32_t tcp_ack;                                      //确认号
#ifdef WORDS_BIGENDIAN
    u_int8_t tcp_offset : 4, tcp_reserved : 4;
    #else
    u_int8_t tcp_reserved : 4, tcp_offset : 4;
    #endif
    u_int8_t tcp_flags;                                     //标识
    u_int16_t tcp_windows;                                  //窗口大小
    u_int16_t tcp_checksum;                                 //校验和
    u_int16_t tcp_urgent_pointer;                           //紧急指针
};
//UDP协议
struct udp_header
{
    u_int16_t udp_source_port;                              //UDP源端口
    u_int16_t udp_destinatin_port;                          //UDP目的端口
    u_int16_t udp_length;                                   //长度
    u_int16_t udp_checksum;                                 //校验和
};
//ICMP协议
struct icmp_header
{
    u_int8_t icmp_type;                                     //ICMP类型
    u_int8_t icmp_code;                                     //ICMP代码
    u_int16_t icmp_checksum;                                //校验和
    u_int16_t icmp_id;                                      //标识符
    u_int16_t icmp_sequence;                                //序列号
};
//分析TCP协议
void tcp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct tcp_header *tcp_protocol;
    u_char flags;
    int header_length;
    u_short source_port;
    u_short destinatin_port;
    u_short windows;
    u_int sequence;
    u_int acknowledgement;
    u_int16_t checksum;
    tcp_protocol = (struct tcp_header *)(packet_content + 14 + 20);
    //14为以太网头，20为IP头
    source_port = ntohs(tcp_protocol->tcp_source_port);
    destinatin_port = ntohs(tcp_protocol->tcp_destinatin_port);
    header_length = tcp_protocol->tcp_offset * 4;
    sequence = ntohl(tcp_protocol->tcp_acknowledgement);
    acknowledgement = tcp_protocol->tcp_ack;
    windows = ntohs(tcp_protocol->tcp_windows);
    flags = tcp_protocol->tcp_flags;
    checksum = ntohs(tcp_protocol->tcp_checksum);
    str+="源端口号:"+to_string(source_port)+"\t";
    str+="目的端口号:"+to_string(destinatin_port)+"\n";
    printf("源端口号:%d\t", source_port);
    printf("目的端口号:%d\t", destinatin_port);
    switch(destinatin_port)
    {
        case 80:
            printf("上层协议为HTTP\n");
            break;
        case 21:
            printf("上层协议为FTP\n");
            break;
        case 23:
            printf("上层协议为Telnet\n");
            break;
        case 25:
            printf("上层协议为SMTP\n");
            break;
        case 110:
            printf("上层协议为POP3\n");
            break;
        default:
            break;
    }
    printf("序列号:%u\t", sequence);
    printf("确认号:%u\t", acknowledgement);
    printf("首部长度:%d\t", header_length);
    printf("保留位:%d\t", tcp_protocol->tcp_reserved);
    printf("窗口大小:%d\t", windows);
    printf("校验和:%d\n", checksum);
    printf("Flags:");
    if(flags & 0x08)
        printf("PSH ");
    if(flags & 0x10)
        printf("ACK ");
    if(flags & 0x02)
        printf("SYN ");
    if(flags & 0x20)
        printf("URG ");
    if(flags & 0x01)
        printf("FIN ");
    if(flags & 0x04)
        printf("RST ");
    printf("\n");

}
//分析UDP协议
void udp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct udp_header *udp_protocol;
    u_short source_port;
    u_short destinatin_port;
    u_short length;
    u_int16_t checksum;
    udp_protocol = (struct udp_header *)(packet_content + 14 + 20);
    //14为以太网头，20为IP头
    source_port = ntohs(udp_protocol->udp_source_port);
    destinatin_port = ntohs(udp_protocol->udp_destinatin_port);
    length = ntohs(udp_protocol->udp_length);
    checksum = ntohs(udp_protocol->udp_checksum);
    printf("源端口号:%d\t", source_port);
    printf("目的端口号:%d\t", destinatin_port);
    str+="源端口号:"+to_string(source_port)+"\t";
    str+="目的端口号:"+to_string(destinatin_port)+"\n";
    printf("长度:%d\t", length);
    printf("校验和:%d\n", checksum);
    switch(destinatin_port)
    {
    case 138:
        printf("上层协议为NetBIOS数据报服务\n");
        break;
    case 137:
        printf("上层协议为NetBIOS名字服务\n");
        break;
    case 139:
        printf("上层协议为NetBIOS会话服务\n");
        break;
    case 53:
        printf("上层协议为域名服务\n");
        break;
    default:
        break;
    }

}
//分析ICMP协议
void icmp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct icmp_header *icmp_protocol;
    u_int8_t type;
    u_int16_t checksum;
    u_int16_t id;
    u_int16_t sequence;
    icmp_protocol = (struct icmp_header *)(packet_content + 14 + 20);
    type = icmp_protocol->icmp_type;
    checksum = ntohs(icmp_protocol->icmp_checksum);
    id = icmp_protocol->icmp_id;
    sequence = icmp_protocol->icmp_sequence;
    switch(type)
    {
    case 8:
        printf("该报文为ICMP请求报文\n");
        break;
    case 0:
        printf("该报文为ICMP应答报文\n");
        break;
    default:
        break;
    }
    printf("标识符:%d\t", id);
    printf("序列号:%d\t", sequence);
    printf("校验和:%d\n", checksum);
}
//分析ARP协议
void arp_protocol_packet_callback(u_char *argument,const struct pcap_pkthdr* packet_header,const u_char*packet_content)
{
    struct arp_header *arp_protocol;                     //ARP协议变量
    u_short protocol_type;                              //协议类型
    u_short hardware_type;                              //硬件类型
    u_short operation_code;                             //操作类型
    u_char *mac_string;                                 //以太网地址
    struct in_addr source_ip_address;                   //源ip地址
    struct in_addr destination_ip_address;              //目的IP地址
    u_char hardware_length;                             //硬件地址长度
    u_char protocol_length;                             //协议地址长度
    arp_protocol = (struct arp_header *)(packet_content + 14);
    //获取ARP协议数据，+14是为了跳过以太网数据部分
    hardware_type = ntohs(arp_protocol->arp_hardware_type);
    protocol_type = ntohs(arp_protocol->arp_protocol_type);
    operation_code = ntohs(arp_protocol->arp_operation_code);
    hardware_length = arp_protocol->arp_hardwware_length;
    protocol_length = arp_protocol->arp_protocol_length;
    printf("ARP硬件类型:%d\t", hardware_type);
    printf("ARP协议类型:%d\t", protocol_type);
    printf("ARP硬件长度:%d\t", hardware_length);
    printf("ARP协议长度:%d\t", protocol_length);
    printf("ARP操作类型:%d\t", operation_code);
    switch(operation_code)
    {
        case 1:
            printf("ARP查询协议\n");
            break;
        case 2:
            printf("ARP应答协议\n");
            break;
        case 3:
            printf("RARP查询协议\n");
            break;
        case 4:
            printf("RARP应答协议\n");
            break;
        default:
            break;
    }
    mac_string = arp_protocol->arp_source_ethernet_address;
    printf("以太网源地址为：%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    memcpy((void *)&source_ip_address, (void *)&arp_protocol->arp_source_ip_address,sizeof(struct in_addr));
    printf("源IP地址:%s\n", inet_ntoa(source_ip_address));
    mac_string = arp_protocol->arp_destination_ethernet_address;
    printf("以太网目的地址为：%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    memcpy((void *)&destination_ip_address, (void *)&arp_protocol->arp_destination_ip_address,sizeof(struct in_addr));
    printf("目的IP地址:%s\n", inet_ntoa(destination_ip_address));
}
// 分析IP协议
void ip_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    struct ip_header *ip_protocol;
    u_int header_length;
    u_int offset;
    u_char tos;
    u_int16_t checksum;
    ip_protocol = (struct ip_header *)(packet_content + 14);
    checksum = ntohs(ip_protocol->ip_checksum);
    tos = ip_protocol->ip_tos;
    offset = ntohs(ip_protocol->ip_off);
    printf("IP版本：IPV%d\t", ip_protocol->ip_version);
    printf("TOS:%d\t", tos);
    printf("总长度:%d\t", ntohs(ip_protocol->ip_length));
    str+="报文长度:";
    str+=to_string(ntohs(ip_protocol->ip_length));
    str+="\n";
    printf("标识:%d\t", (offset & 0x1fff) * 8);
    printf("TTL：%d\n", ip_protocol->ip_ttl);
    switch(ip_protocol->ip_protocol)
    {
        case 6:
            printf("上层协议为TCP协议\n");
            str+="上层协议为TCP协议\n";
            break;
        case 17:
            printf("上层协议为UDP协议\n");
            str+="上层协议为UDP协议\n";
            break;
        case 1:
            printf("上层协议为ICMP协议\n");
            str+="上层协议为ICMP协议\n";
            break;
        default:
            break;
    }
    printf("头部校验和:%d\t", checksum);
    printf("源IP:%s\t", inet_ntoa(ip_protocol->ip_source_address));
    str+="源IP:";
    str+=inet_ntoa(ip_protocol->ip_source_address);
    str+="\t";
    printf("目的IP:%s\n", inet_ntoa(ip_protocol->ip_destination_address));
    str+="目的IP:";
    str+=inet_ntoa(ip_protocol->ip_destination_address);
    str+="\n";
    switch(ip_protocol->ip_protocol)
    {
        case 6:
            tcp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
        case 1:
            icmp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
        case 17:
            udp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
        default:
            break;
    }
}
// 分析以太网协议
void ethernet_protocol_packet_callback(u_char *argument,const struct pcap_pkthdr* packet_header,const u_char*packet_content)
{
    u_short ethernet_type;
    struct ether_header *ethernet_protocol;
    u_char *mac_string;
    printf("---------------------------\n");
    str+="第"+to_string(packet_number)+"个数据包正在捕获\n";
//    printf("第%d个数据包正在捕获\n", packet_number);
    ethernet_protocol = (struct ether_header *)packet_content;
    ethernet_type = ntohs(ethernet_protocol->ether_type);
    printf("以太网类型%04x\t", ethernet_type);
    switch(ethernet_type)
    {
        case 0x0800:
            printf("使用的是IP协议\n");
//            str+="使用的是IP协议\n";
            break;
        case 0x0806:
            printf("使用的是ARP协议\n");
//            str+="使用的是ARP协议\n";
            break;
        case 0x8035:
            printf("使用的是RARP协议");
//            str+="使用的是RARP协议\n";
            break;
        default:
            break;
    }
    mac_string = ethernet_protocol->ether_shost;
    char s[100];
    sprintf(s, "%02x:%02x:%02x:%02x:%02x:%02x",*mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    string temp = s;
    str += "源MAC地址："+temp + '\t';
//    printf("源地址为：%02x:%02x:%02x:%02x:%02x:%02x\t", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    mac_string = ethernet_protocol->ether_dhost;
    sprintf(s, "%02x:%02x:%02x:%02x:%02x:%02x",*mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    temp=s;
    str+="目的MAC地址："+temp+"\n";
//    printf("目的地址为：%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    switch(ethernet_type)
    {
        case 0x0800:
            ip_protocol_packet_callback(argument, packet_header, packet_content);
            break;
        case 0x0806:
            arp_protocol_packet_callback(argument, packet_header, packet_content);
            break;
        default:
            break;
    }
    packet_number++;
}
#endif // FUNC_PCAP_H
