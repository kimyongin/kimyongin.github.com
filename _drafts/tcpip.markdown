---
layout: post
title: TCP/IP
date: 2015-09-16 12:53:25
tags: tcp/ip
---

# [Wiki : Circuit Switching](https://en.wikipedia.org/wiki/Circuit_switching)

* Circuit switching is a methodology of implementing a telecommunications network in which two network nodes establish a dedicated communications channel (circuit) through the network before the nodes may communicate. 

> 회선교환은 두개의 네트워크 노드가 통신을 시작하기 전에 전용 통신 채널을 설립하는 방식이다.

* The circuit guarantees the full bandwidth of the channel and remains connected for the duration of the communication session. The circuit functions as if the nodes were physically connected as with an electrical circuit.

> 회선은 채널의 모든 대역폭을 사용할수 있으며, 통신 세션이 살아있는 동안 연결이 유지된다.

* The defining example of a circuit-switched network is the early analog telephone network. When a call is made from one telephone to another, switches within the telephone exchanges create a continuous wire circuit between the two telephones, for as long as the call lasts.

> 회선교한의 예를들면, 초기의 아날로그 전화 네트워크 이다. 이곳에서 다른곳으로 전화를 걸면, 전화교환국에서는 두 전화기 사이에 전화가 끝날때까지 물리적인 회선을 연결해준다. 

* Circuit switching contrasts with packet switching which divides the data to be transmitted into packets transmitted through the network independently. In packet switching, instead of being dedicated to one communication session at a time, network links are shared by packets from multiple competing communication sessions, resulting in the loss of the quality of service guarantees that are provided by circuit switching.

> 회선교환과 패킷교환의 다른점은, 패킷교환은 데이터를 네트워크 독립적인 패킷들로 쪼개는 것이다. 패킷교환에서는 통신세션을 독점하지 않고, 여러 통신세션간에 경쟁적으로 네트워크 링크를 공유하여, 결과적으로 서비스품질에 손실이 생긴다.

* In circuit switching, the bit delay is constant during a connection, as opposed to packet switching, where packet queues may cause varying and potentially indefinitely long packet transfer delays. No circuit can be degraded by competing users because it is protected from use by other callers until the circuit is released and a new connection is set up. Even if no actual communication is taking place, the channel remains reserved and protected from competing users.

> 회선교환에서는 연결되있는 동안 짧은 지연이 계속된다. 반대로 패킷교환에서는 패킷큐로 인해 긴 패킷 전송지연이 가변적이고 잠재적으로 계속된다. 회선에서는 유저간의 경쟁으로 인해 저하될수가 없다. 왜냐하면 회선이 해제되거나 새로운 연결이 수립되기까지는 회선이 보호되기 때문이다. 심지어 실질적인 통신이 없더라도 채널은 보장되고 보호된다.

* Virtual circuit switching is a packet switching technology that emulates circuit switching, in the sense that the connection is established before any packets are transferred, and packets are delivered in order. 

> 가상회선교환은 회선교환을 모방한 패킷교환 기술이다. 그런의미에서 어떤 패킷이 전송되기 전에 연결이 수립된다. 그리고 패킷들은 순서대로 전송이 된다.

* While circuit switching is commonly used for connecting voice circuits, the concept of a dedicated path persisting between two communicating parties or nodes can be extended to signal content other than voice. Its advantage is that it provides for continuous transfer without the overhead associated with packets making maximal use of available bandwidth for that communication. Its disadvantage is that it can be relatively inefficient because unused capacity guaranteed to a connection cannot be used by other connections on the same network.

> 회선교환은 일반적으로 전화회선 연결에 사용 되지만, 두개의 노드 사이에 독점적 지속적으로 연결 시키는 방식은 다른 신호 정보로 확장 될 수 있다. 이것은 대역폭에 맞게 패킷을 만드는 오버헤드 없이 지속적으로 데이터를 전송할수 있게 해주는 장점이 있다. 반대로, 사용되지 않는 용량이 같은 네트워크의 다른 연결에서 사용할수 없다는 비효율적인 단점이 있다.

**The call**

* For call setup and control (and other administrative purposes), it is possible to use a separate dedicated signalling channel from the end node to the network. ISDN is one such service that uses a separate signalling channel while plain old telephone service (POTS) does not.

> 전화를 걸거나 제어하기 위해, 분리된 전용 신호채널을 사용 할 수 있다. ISDN이 이런 서비스의 한 종류이다.

* The method of establishing the connection and monitoring its progress and termination through the network may also utilize a separate control channel as in the case of links between telephone exchanges which use CCS7 packet-switched signalling protocol to communicate the call setup and control information and use TDM to transport the actual circuit data.

> 연결을 수립하거나 진행, 종료를 모니터링하는 방법은 분리된 제어 채널을 활용한다. .. blah-blah .. 실제 회선 데이터를 전송할때는 TDM을 사용한다.

* Early telephone exchanges are a suitable example of circuit switching. The subscriber would ask the operator to connect to another subscriber, whether on the same exchange or via an inter-exchange link and another operator. In any case, the end result was a physical electrical connection between the two subscribers telephones for the duration of the call. The copper wire used for the connection could not be used to carry other calls at the same time, even if the subscribers were in fact not talking and the line was silent.

> 초기의 전화교환국은 회선교환의 예로 적당했다. 사용자는 교환수에게 다른 사용자에게 연결해달라고 요청을 했다. 같은 교환소이거나, 다른 교환소간의 연결이더라도 말이다. .. blah-blah .. 연결에 사용된 구리선은 동시에 다른 전화를 위해 사용될수 없다. 심지어 대화를 하지 않더라도 말이다.

**Compared to datagram packet switching**

* Circuit switching contrasts with packet switching which divides the data to be transmitted into small units, called packets, transmitted through the network independently. Packet switching shares available network bandwidth between multiple communication sessions.

> 회선교환과 패킷교환의 차이점은, 패킷교환은 데이터를 패킷이라고 불리는 작은 단위로 쪼개는 것이다. 패킷교환은 여러 통신세션간에 네트워크 대역폭을 공유한다.

* Multiplexing multiple telecommunications connections over the same physical conductor has been possible for a long time, but nonetheless each channel on the multiplexed link was either dedicated to one call at a time, or it was idle between calls.

> 멀티플렉싱(ex. TDM)은 같은 물리장비에서 여러개의 연결이 가능하게 해왔다. 하지만 멀티플렉스된 각각의 채널은 한번에 하나의 연결에 독점된다.

* In circuit switching, and virtual circuit switching, a route and bandwidth is reserved from source to destination. Circuit switching can be relatively inefficient because capacity is guaranteed on connections which are set up but are not in continuous use, but rather momentarily. However, the connection is immediately available while established.

> 회선교환, 가상회선교환은  출발지에서 목적지까지 경로와 대역폭이 예약된다. 회선교환은 비효율적이다. 왜냐하면 지속적으로 사용하지 않더라도 설립된 연결에 대해서는 용량이 예약되기 때문이다. 그러나 그 연결은 곧바로 ???

* Packet switching is the process of segmenting a message/data to be transmitted into several smaller packets. Each packet is labeled with its destination and a sequence number for ordering related packets, precluding the need for a dedicated path to help the packet find its way to its destination. Each packet is dispatched independently and each may be routed via a different path. At the destination, the original message is reassembled in the correct order, based on the packet number. Datagram packet switching networks do not require a circuit to be established and allow many pairs of nodes to communicate concurrently over the same channel.

> 패킷교환은 메세지/데이터를 여러개의 작은 패킷들로 나누는 과정이다. 각 패킷은 목적지와 정렬을 위한 시퀀스번호가 붙어있다. 패킷이 목적지에 찾아가기 위해서 경로를 점유할 필요가 없다. 각 패킷은 독립적으로 보내지고 각각은 다른 경로로 전달될수도 있다. (경로가 다르기 때문에 목적지에 도착하는 순서도 다르기 때문에) 목적지에서는 시퀀스넘버를 이용해서 패킷들을 정렬 조립하여 원래 메세지가 만들어진다. 데이터그램 패킷교환은 연결수립을 위한 회선이 필요없으며, 같은 채널에서 동시에 여러개의 연결을 허용한다.

***
<br>

# [Packet switching](https://en.wikipedia.org/wiki/Packet_switching)

* Packet switching is a digital networking communications method that groups all transmitted data into suitably sized blocks, called packets, which are transmitted via a medium that may be shared by multiple simultaneous communication sessions. Packet switching increases network efficiency, robustness and enables technological convergence of many applications operating on the same network.

> 패킷교환은 패킷이라고 불리우는 적절한 사이즈의 블록들이, 중간경유지를 여러개의 통신세션들이 동시에 공유하며 전송하는 디지털 네트워킹 통신 방법이다. 패킷교환은 네트워크 효율성을 높여주고, 강건하게 해주며, 많은 어플리케이션이 같은 네트워크에서 통신가능하도록 한다.

* Packets are composed of a header and payload. Information in the header is used by networking hardware to direct the packet to its destination where the payload is extracted and used by application software.

> 패킷은 헤더와 페이로드를 포함한다. 헤더정보는 네트워크장비에서 페이로드가 어느 목적지로 전달될지 가리키는데 사용된다.

**Concept**

* The routing and transferring of data by means of addressed packets so that a channel is occupied during the transmission of the packet only, and upon completion of the transmission the channel is made available for the transfer of other traffic

> 주소가 적힌 패킷에 의해서 데이터를 라우팅하고 전송한다. 그래서 패킷이 전송되는 동안 채널은 점유되고, 전송이 끝나면 다른 트래픽이 사용할수 있게 된다.

* Packet switching features delivery of variable bit rate data streams, realized as sequences of packets, over a computer network which allocates transmission resources as needed using statistical multiplexing or dynamic bandwidth allocation techniques. When traversing network nodes, such as switches and routers, packets are buffered and queued, resulting in variable latency and throughput depending on the link capacity and the traffic load on the network.

> 패킷교환은 가변 데이터 스트림을 전송하고, 패킷 순서대로 만들어지고, 필요에 따라 컴퓨터 너머에 설치된 전송 장비는 통계적 멀티플렉싱 혹은 동적 대역폭 할당 기법을 사용한다. 스위치나 라우터등의 네트워크 노드를 횡단할때, 패킷은 쌓이고 대기하게 된다. 결과적으로 가변적인 지연이 발생하고 용량에 따라서 처리량이 달라지며, 네트워크에 트래픽 부하가 발생하게 된다.

* Packet switching contrasts with another principal networking paradigm, circuit switching, a method which pre-allocates dedicated network bandwidth specifically for each communication session, each having a constant bit rate and latency between nodes. In cases of billable services, such as cellular communication services, circuit switching is characterized by a fee per unit of connection time, even when no data is transferred, while packet switching may be characterized by a fee per unit of information transmitted, such as characters, packets, or messages.

> 패킷교환과 회선교환의 차이점은 회선교환은 각 세션에 독점적인 네트워크 대역폭을 미리 할당하며, 노드사이에 고정된 전송률과 지연을 가지고 있다는 것이다. 전화 통신같은 비용을 청구할수 있는 서비스의 경우 회선교환은 연결시간당 요금이 결정된다. 심지어 아무런 데이터가 전송되지 않더라도 말이다. 반면 패킷교환은 문자, 패킷, 메세지등의 정보의 전송량에 따라서 요금이 결정된다.

* Packet mode communication may be implemented with or without intermediate forwarding nodes (packet switches or routers). Packets are normally forwarded by intermediate network nodes asynchronously using first-in, first-out buffering, but may be forwarded according to some scheduling discipline for fair queuing, traffic shaping, or for differentiated or guaranteed quality of service, such as weighted fair queuing or leaky bucket. In case of a shared physical medium (such as radio or 10BASE5), the packets may be delivered according to a multiple access scheme.

> 패킷모드 통신은 중간 전달 노드가 있을수도 있고 없을수도 있다. 패킷들은 일반적으로 중간 네트워크 노드들로부터 비동기적으로 first-in, first-out buffering 방식으로 전달된다. 그러나 

***

# [SmallVoid : Configure TCPIP settings for better performance](http://smallvoid.com/article/windows-tcpip-settings.html)

**Receive Window** (DefaultRcvWindow/TcpWindowSize)

* specifies how much data it will accept without an acknowledgment. 
* See how to [find the optimal TCPIP RWIN (Receive WINdow) size](http://smallvoid.com/article/tcpip-rwin-size.html).
* Note to be able to use a Receive Window larger than 65535 bytes, one have to enable Receive Window Scaling according to [RFC 1323](http://www.faqs.org/rfcs/rfc1323.html).

**Max Transfer Unit (MTU)** 

* specifies how large packets of data it will accept to send and receive. 
* See how to [find the optimal Max Transfer Unit (MTU) size](http://smallvoid.com/article/tcpip-mtu-size.html).
* Note the MTU is usually set for the individual network adapter, so it will only affect networks which the adapter connects to.
* Note also that if the adapter is used for several networks like LAN and Internet then optimizing for one network might degrade the others.

**Time To Live (TTL) (DefaultTTL)** 

* specifies how many seconds/hobs a TCP/IP packet will live before dying. 
* If it is set too high it will take a long time to timeout.
* if it is set too low it will not be able to reach destination before dying.
* The recommended value is 64.

**Path MTU Black Hole Detection (PMTUBlackHoleDetect/EnablePMTUBHDetect)** 

* specifies if it should be able to detect routers, which doesn't respond back if they get packets larger than their Max Transmission Unit (MTU) size. 
* When enabled it will detect if too many re-transmissions, and will then clear the Don't Fragment (DF)-flag, and if transmission succeeds, then it will lower the MTU size for the connection and enable the DF-flag again.
* The recommended setting is disabled.

**Path MTU Discovery (PMTUDiscovery)** 

* enables detection of Max Transmission Unit(MTU). 
* This allows the TCPIP stack to discover the optimal MTU for the connection, making sure packets are not fragmented. 
* MTU discovery can cause some slow down when establishing the connection, according to [RFC 1191](http://www.faqs.org/rfcs/rfc1191.html).
* but if disabled then it will use a MTU size of 576.
* The recommended setting is enabled.

**Selective Acknowledgement (SACK)** 

* operation (SackOpts) allows TCP to recover from IP packet loss without resending packets that were already received by the receiver. 
* This especially helps if having a large receive window. If connecting to a service which does not comply with [RFC 2018](http://www.faqs.org/rfcs/rfc2018.html) then the connection will be slower with this enabled.
* The recommended setting is enabled.

**Receive Window Scaling and Time Stamping (Tcp1323Opts)** 

* allows the use of a Receive beyond 65535 bytes and the use of Time Stamping to calculate Round Trip Time(RTT) to better scale the receive window according to [RFC 1323](http://www.faqs.org/rfcs/rfc1323.html). 
* The window scaling and timestamp enables automatic handling of:
    * Adapting the receive window to the actual connection bandwidth. The sender performs a "slow start" where the announced receive window is increased exponential until packet loss is detected, where it lowers the receive window and then increases it linear until packet loss is again detected. This makes configuration of the optimal receive window less important. This doesn't work well with high bandwidth connections with high latency [RFC 3649](http://www.faqs.org/rfcs/rfc3649.html).
    * Adapting the receive window when congestion is detected due to packet loss. The sender lowers the announced receive window and performs a "slow start". This doesn't work well when needing a certain quality of service for special applications [RFC 3168](http://www.faqs.org/rfcs/rfc3168.html).
    
* The timestamp also helps identifying an individual packet on a high bandwidth network (GBit), where the packet sequence number can be reused faster than the TTL of the packet according to [RFC 1185](http://www.faqs.org/rfcs/rfc1185.html). If timestamps are enabled then the size of the TCP Header will be increased with 12 bytes, and will leave less room for data.
* The recommended setting is to have both enabled with broadband, else only Window Scaling.

**Max Duplicate Acknowledgments (MaxDupAcks)** 

* specifies that the sender is able to do a fast retransmit of a lost packet before Retransmission TimeOut (RTO) [RFC 2001](http://www.faqs.org/rfcs/rfc2001.html). 
* When a packet is sent a timer is set and if it reaches RTO the packet will be resend. 
* When a packet-number is received and it is out of order an acknowledgment (ACK) is sent with the expected packet-number. 
* This option allows the sender to retransmit the packet before the RTO, if it gets duplicate ACK with the same packet-number.
* The recommended value is 2 duplicates.

***
<br>

# [Wiki : Path MTU Discovery](https://en.wikipedia.org/wiki/Path_MTU_Discovery)

***

Path MTU Discovery (PMTUD) is a standardized technique in computer networking for determining the maximum transmission unit (MTU) size on the network path between two Internet Protocol (IP) hosts, usually with the goal of avoiding IP fragmentation.

**Implementation**

For IPv4 packets, Path MTU Discovery works by setting the Don't Fragment (DF) option bit in the IP headers of outgoing packets. Then, any device along the path whose MTU is smaller than the packet will drop it, and send back an Internet Control Message Protocol (ICMP) Fragmentation Needed (Type 3, Code 4) message containing its MTU, allowing the source host to reduce its Path MTU appropriately. The process is repeated until the MTU is small enough to traverse the entire path without fragmentation.

IPv6 routers do not support fragmentation or the Don't Fragment option. For IPv6, Path MTU Discovery works by initially assuming the path MTU is the same as the MTU on the link layer interface where the traffic originates. Then, similar to IPv4, any device along the path whose MTU is smaller than the packet will drop the packet and send back an ICMPv6 Packet Too Big (Type 2) message containing its MTU, allowing the source host to reduce its Path MTU appropriately. The process is repeated until the MTU is small enough to traverse the entire path without fragmentation.

If the Path MTU changes after the connection is set up and is lower than the previously determined Path MTU, the first large packet will cause an ICMP error and the new, lower Path MTU will be found. Conversely, if PMTUD finds that the path allows a larger MTU than is possible on the lower link, the OS will periodically reprobe to see if the path has changed and now allows larger packets. On both Linux and Windows this timer is set by default to ten minutes.

***
<br>

# [Wiki : IP fragmentation](https://en.wikipedia.org/wiki/IP_fragmentation)

***

The Internet Protocol (IP) implements datagram fragmentation, breaking it into smaller pieces, so that packets may be formed that can pass through a link with a smaller maximum transmission unit (MTU) than the original datagram size.

***
<br>

# [SmallVoid : Finding the optimal TCPIP receive window size](http://smallvoid.com/article/tcpip-rwin-size.html)

***

**What problem does the receive window solve?**

TCPIP is a reliable network protocol where each packet sent is acknowledged by receiver, and if the sender doesn't get the acknowledge packet back within a certain timeout, then it retransmits the original packet. But it is inefficient to wait for the acknowledge packet from the receiver before sending the next packet. The Receive WINdow(RWIN) solves the problem of the sender constantly waiting for the receiver acknowledge.

**How does receive window work?**

The RWIN size specifies how much data, which can be sent to the receiver without getting an acknowledge from the receiver. This allows the sender to send several packets without waiting for acknowledge. It also allows the receiver to acknowledge several packets in one stroke.

**Why is the size of the receive window important?**

If the RWIN is too large, then the sender will spend a lot of time resending the entire RWIN every time packet loss is detected by the receiver. This is especially important if on a high collision network like a 100 Mbit Ethernet HUB. If Selective Acknowledgment (SACK) is enabled then it should lessen the downside of a too large RWIN.

If the RWIN is too small, then the sender will be blocked constantly because it fills out the RWIN with packets before receiver is able to acknowledge that it has received the packets.

**What is the proper size of the receive window?**

The optimal RWIN is mainly dependent on two things:

- Latency

>The time it takes for a network packet to reach destination and get a reply back (Also known as ping time). If the latency is high and the RWIN too small, then it can allow the sender of data to fill out the RWIN before any acknowledge packet is returned.

>The latency for a connection is dependent on how quick the sender and receiver(and routers in between) are able to handle packets. If reaching the bandwidth limit of the physical line, then the latency becomes high.

- Bandwidth

>The amount of data that can be sent within a given time. If the bandwidth is high and the RWIN is too small, then sender can fill out the RWIN too fast, before the receiver can acknowledge the received data.

>The max bandwidth for a connection is dependent on the lowest available bandwidth at sender and receiver(and routers in between). The max limit can also depend on the ISP capping, or by the receiving/sending application throttling the connection.

**How to find the optimal receive window size with PING?**

To use the PING (Packet InterNet Groper) tool to find the optimal RWIN Size, ping your ISP with the Max Transfer Unit(MTU) size (The -28 is because of the IP- and ICMP-Header which the PING tool adds.)

>PING www.tiscali.dk -f -l <MTU-28> -n 10

Then insert the values of your connection in this calculation :

>Bandwidth(kbps) / 8 * Average Latency(MiliSec) = RWIN Size(Bytes)

Then round up the RWIN Size to a multiple of the Maximum Segment Size (MSS) which is equal the MTU Size subtracted the size of IP Header (20 Bytes) and TCP Header (20 bytes + ? bytes depending of options like timestamp being enabled). So if your MTU Size is 1500 then your MSS is usually 1460 (1448 if timestamp is enabled).

**How is the default receive window size calculated in Microsoft Windows?**

Microsoft Windows tries to find the most optimal RWIN during the connection handshake. For most network types the following method will calculate a reasonable RWIN, so one doesn't have to configure it manually:

1. The initial connection request is created with a RWIN of 16384 bytes

    * Win9x/WinNT4 uses a RWIN of 8192 bytes
    * Win2k3 uses a RWIN dependent on media speed (Ignores latency):
        * Below 1 Mbps: 8192 bytes
        * 1 Mbps - 100 Mbps: 16384 bytes
        * Greater than 100 Mbps: 65536 bytes
    * Windows Vista/2008 uses receive window scaling by default, and uses a default RWIN of 64K.

2. When the reply to the initial connection is received the RWIN is adjusted:

    * RWIN is rounded up to a even multiple of Maximum Segment Size (MSS).
    * RWIN is adjusted to the larger of 4 times MSS, but not higher than 65536 bytes (unless Windows Scaling is enabled).
        * If the GlobalMaxTcpWindowSize is set then it will be highest limit

Note when creating a socket it is possible to overrule the default RWIN size using setsockopt with the socket option SO_RCVBUF. If the GlobalMaxTcpWindowSize is set, then it will limit the max size of the RWIN.

More Info [KB93444](http://support.microsoft.com/kb/93444), [KB172983](http://support.microsoft.com/kb/172983)

***
<br>


[RFC 1323 - TCP Extensions for High Performance](http://www.faqs.org/rfcs/rfc1323.html)
[RFC 1191 - Path MTU discovery](http://www.faqs.org/rfcs/rfc1191.html)
[RFC 2018 - TCP Selective Acknowledgment Options](http://www.faqs.org/rfcs/rfc2018.html)
[RFC 3649 - HighSpeed TCP for Large Congestion Windows](http://www.faqs.org/rfcs/rfc3649.html)
[RFC 3168 - The Addition of Explicit Congestion Notification (ECN)](http://www.faqs.org/rfcs/rfc3168.html)
[RFC 1185 - TCP Extension for High-Speed Paths](http://www.faqs.org/rfcs/rfc1185.html)
[RFC 2001 - TCP Slow Start, Congestion Avoidance, Fast Retransmit](http://www.faqs.org/rfcs/rfc2001.html)


