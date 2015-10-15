---
layout: post
title: "http로 파일다운로드시 99%에서 멈추는 현상 분석"
date: 2015-10-15 12:53:25
tags: http window wireshark
---

Apache/2.2.15 (CentOS) 에서 HTTP 프로토콜로 파일을 다운로드 받는데, 간혹가다 99%에서 멈추는 현상이 발생한다. 그래서 와이어샤크로 캡처를 해서 분석 해보았다. (일반적인 내용들도 포함되어 있으니 적당히 생략해가면서 보길 바란다.)

- 클라이언트 코드1 : [BOOST ASIO 사용](https://github.com/kimyongin/kimyongin.github.com/blob/master/assets/http-capture/HTTPManager2.cpp)  
- 클라이언트 코드2 : [WinINet 사용](https://github.com/kimyongin/kimyongin.github.com/blob/master/assets/http-capture/HTTPManager.cpp)
- 192.168.246.31 : Client
- 210.122.35.55 : Server

***************************************************
***************************************************

#### 연결수립, HTTP요청, HTTP응답 수신 시작

[![]({{ site.url }}/assets/http-capture/cap01.png)]({{ site.url }}/assets/http-capture/cap01.png)

- 43 ~ 45 : TCP HandShaking 을 통한 연결수립
- 46 : Client가 Server에게 파일을 요청하는 "Http Request" 전송 (1패킷으로 모두 전송 완료됨)
- 47 : Server가 Client에게 "Http Request"를 잘 받았다는 응답(ACK) 전송
- 48 : Server가 Client에게 "Http Response" 전송 (Content부분에 파일이 포함되어 있음)
- 49 : Server가 Client에게 Seq=1301 부터 Len=1300 크기만큼 전송
- 50 : Client가 Server에게 ACK=2601(1301+1300) 을 보내달라고 응답
- 51 : Server가 Client에게 Seq=2601 부터 Len=1300 크기만큼 전송
- 52 : Server가 Client에게 Seq=3901(2601+1300) 부터 Len=1300 크기만큼 전송
- 53 : Client가 Server에게 ACK=5201(3901+1300) 을 보내달라고 응답
- 54 : ...

(51)~(52)를 보면 알수 있듯이.. 서버는 클라이언트의 ACK 신호를 기다리지 않고. 연달아서 2개의 패킷을 전송한다. 그리고 클라이언트는 (52)에 대해서만 응답을 보낸다. 왜냐하면, 매번 ACK를 보내고, 기다리기에는 너무 시간이 오래걸리기 때문이다.

<br>

***************************************************
***************************************************

#### Packet Loss

[![]({{ site.url }}/assets/http-capture/cap02.png)]({{ site.url }}/assets/http-capture/cap02.png)

- 150 : Server가 Client에게 Seq=88401 부터 Len=1300 크기만큼 전송
- 151 : Server가 Client에게 Seq=91001(88401+1300+1300) 부터 Len=1300 크기만큼 전송 
- 152 : Client가 Server에게 ACK=89701(88401+1300)을 보내달라고 응답
- 153 : Server가 Client에게 Seq=89701(88401+1300) 부터 Len=1300 크기만큼 전송
- 154 : Client가 Server에게 ACK=92301(91001+1300)을 보내달라고 응답

(151)을 보면 Seq=89701이 들어와야 하는데 Seq=91001이 들어왔음을 확인할수 있다. 이는 패킷이 손실됬을수도 있고, 조금 늦게 도착하는걸수도 있다. 어찌됬든 패킷이 아직 도착하지 않았음으로, 클라이언트가 (152)에서 누락된 패킷을 보내달라고 응답한다. 그러자 (153)에서 누락된 패킷이 도착함을 확인할수 있다.

<br>

***************************************************
***************************************************

#### Packet Loss, Dup Ack

[![]({{ site.url }}/assets/http-capture/cap03.png)]({{ site.url }}/assets/http-capture/cap03.png)

- 948 : Client가 Server에게 Ack=566801을 보내달라고 응답
- 949 : Server가 Client에게 Seq=568101(566801+1300) 부터 Len=1300 크기만큼 전송
- 950 : Client가 Server에게 Ack=566801을 보내달라고 응답 (2번째 ACK)
- 951 : Server가 Client에게 Seq=568101 부터 Len=1300 크기만큼 전송

(948) 에서 이미 Seq=566801을 보내달라고 했는데, (949)에서 서버가 Seq=568101을 보냈다. 
그래서 (950)에서 다시한번 Seq=566801을 보내달라고 다시한번 DUP ACK를 했다.
그때서야 (951)에서 Seq=566801이 들어왔음을 확인할수 있다.

<br>

***************************************************
***************************************************

#### Packet Loss, Dup Ack, Fast Retransmission

[![]({{ site.url }}/assets/http-capture/cap04.png)]({{ site.url }}/assets/http-capture/cap04.png)

- 371123~371131 : Seq=46293948이 계속 지연되서 클라이언트가 서버에게 DUP ACK를 계속해서 보내고 있다. 
- 371133 : 서버가 클라이언트에게 Seq=46293948를 빨리 재전송(Fast Retransmission)을 하고 있다.

<br>

***************************************************
***************************************************

#### Window Size

[![]({{ site.url }}/assets/http-capture/cap05.png)]({{ site.url }}/assets/http-capture/cap05.png)

- 이제 파일다운로드를 99%정도 완료한 상태이다. 
- (129680)~(1296899) : 클라이언트의 윈도우 사이즈가 점점 작아진다. 이는 클라이언트에서 수신받은 패킷에 대해서 응용프로그램에서 처리를 빨리 하지 않아서인듯 하다.
- (129689)를 열어서 보면 아래처럼 윈도우 사이즈가 81까지 줄어들었다.

[![]({{ site.url }}/assets/http-capture/cap06.png)]({{ site.url }}/assets/http-capture/cap06.png)
[![]({{ site.url }}/assets/http-capture/cap07.png)]({{ site.url }}/assets/http-capture/cap07.png)
[![]({{ site.url }}/assets/http-capture/cap08.png)]({{ site.url }}/assets/http-capture/cap08.png)

시간이 흘러 응용프로그램에서 쌓여있는 패킷을 어느정도 처리하면,  (1296900), (1296901)에서 윈도우 사이즈가 늘어났음(TCP Window Update)을 서버로 전송한다.

<br>

***************************************************
***************************************************

#### 바로 여기가 문제다.

서버는 클라이언트의 윈도우 사이즈가 작으면 더이상의 전송을 중단하고, 클라이언트의 윈도우 사이즈가 늘어나기를 기다렸다가 전송을 한다. 하지만, 지금 문제 상황은 윈도우 사이즈가 늘어남을 알렸음에도 불구하고, 서버에서 패킷 전송을 다시 시작하지 않는다. 더 이상한점은, 윈도우 사이즈는 그 전에도 계속해서 늘었다 줄었다 한다. 그런데 꼭! 99%에 도달했을때 저 현상이 발생하면 서버가 전송을 다시 시작하지 않는다는 것이다.

뭘까??? 몇번 더 테스트 해보았는데.. 꼭 저런 형태로 TCP Window Update를 보내면서 멈추지는 않는다. 하지만 99%에서 멈추는건 동일하고, 패킷을 보면 클라이언트든 서버든 윈도우 사이즈가 작은 상태임은 맞다.

그런데 정말 윈도우 사이즈가 힌트이긴 한걸까?? 
이제 어딜 더 봐야하는거지?? ㅠㅜ 모르겠다.
