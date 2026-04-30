#ifndef LWIPOPTS_H
#define LWIPOPTS_H


#define LWIP_SNTP 1
// ---------- Core ----------
#define NO_SYS                          1
#define LWIP_SOCKET                     0
#define LWIP_NETCONN                    0

// ---------- Memory ----------
#define MEM_ALIGNMENT                   4
#define MEM_SIZE                        4000

#define MEMP_NUM_PBUF                   16
#define MEMP_NUM_UDP_PCB                4
#define MEMP_NUM_TCP_PCB                0
#define MEMP_NUM_TCP_SEG                0
#define MEMP_NUM_SYS_TIMEOUT            10

#define PBUF_POOL_SIZE                  16
#define PBUF_POOL_BUFSIZE               256

// ---------- ARP / IP ----------
#define LWIP_ARP                        1
#define ARP_TABLE_SIZE                  10

#define LWIP_IPV4                       1
#define LWIP_IPV6                       0

#define IP_FORWARD                      0

// ---------- ICMP ----------
#define LWIP_ICMP                       1

// ---------- UDP ----------
#define LWIP_UDP                        1
#define LWIP_TCP                        0
#define LWIP_RAW                        0

// ---------- DHCP ----------
#define LWIP_DHCP                       1

// ---------- DNS ----------
#define LWIP_DNS                        1
#define DNS_TABLE_SIZE                  4
#define DNS_MAX_SERVERS                 2

// ---------- Timeouts ----------
#define LWIP_TIMERS                     1

// ---------- Debug ----------
#define LWIP_DEBUG                      0

#endif /* LWIPOPTS_H */