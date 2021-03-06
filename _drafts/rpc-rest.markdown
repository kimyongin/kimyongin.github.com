---
layout: post
title:  RPC. REST.
date:   2015-09-16 12:53:25
---

[wiki:RPC](https://en.wikipedia.org/wiki/Remote_procedure_call)

***

a remote procedure call (RPC) is an inter-process communication that allows a computer program to cause a subroutine or procedure to execute in another address space (commonly on another computer on a shared network) without the programmer explicitly coding the details for this remote interaction. 

That is, the programmer writes essentially the same code whether the subroutine is local to the executing program, or remote. 

An important difference between remote procedure calls and local calls is that remote calls can fail because of unpredictable network problems. Also, callers generally must deal with such failures without knowing whether the remote procedure was actually invoked. 

**Sequence of events during an RPC**

1. The client calls the client stub. The call is a local procedure call, with parameters pushed on to the stack in the normal way.
2. The client stub packs the parameters into a message and makes a system call to send the message. Packing the parameters is called marshalling.
3. The client's local operating system sends the message from the client machine to the server machine.
4. The local operating system on the server machine passes the incoming packets to the server stub.
5. The server stub unpacks the parameters from the message. Unpacking the parameters is called unmarshalling.
6. Finally, the server stub calls the server procedure. The reply traces the same steps in the reverse direction.

*In computer science, marshalling or marshaling is the process of transforming the memory representation of an object to a data format suitable for storage or transmission*

**Standard contact mechanisms**

To let different clients access servers, a number of standardized RPC systems have been created. Most of these use an interface description language (IDL) to let various platforms call the RPC. The IDL files can then be used to generate code to interface between the client and servers.

***

RPC는 원격지의 프로시져를 호출하는 것을 말한다. 개발자는 호출하는 프로시져가 로컬에 있던 원격지에 있던, 동일한 코드를 작성하고 (사실은 원격지 프로시져를 호출하는것이지만 마치 로컬 프로시져를 호출하듯이), RPC Library가 알아서 통신해서 프로시져를 호출하게 된다. 다만 로컬 호출과 다른점은 예상치못한 네트웍장애가 발생할수 있다는거다. 그래서 호출하는쪽에서는 호출실패에 대한 처리를 해줘야 한다. 수많은 표준 RPC가 있는데, 대부분은 IDL을 사용해서 서버와 클라이언트간에 인터페이스를 맞춘다.

<br>
[wiki:XML-RPC](https://en.wikipedia.org/wiki/XML-RPC)

***

XML-RPC is a remote procedure call (RPC) protocol which **uses XML to encode its calls and HTTP as a transport mechanism**. "XML-RPC" also refers generically to the use of XML for remote procedure call, independently of the specific protocol. 

XML-RPC, the protocol, was created in 1998 by Dave Winer of UserLand Software and Microsoft. As new functionality was introduced, the standard evolved into what is now **SOAP**.

XML-RPC works by sending a HTTP request to a server implementing the protocol. The client in that case is typically software wanting to call a single method of a remote system. Multiple input parameters can be passed to the remote method, one return value is returned. The parameter types allow nesting of parameters into maps and lists, thus larger structures can be transported. Therefore, XML-RPC can be used to transport objects or structures both as input and as output parameters.

An example of a typical XML-RPC request would be:

```xml
<?xml version="1.0"?>
<methodCall>
  <methodName>examples.getStateName</methodName>
  <params>
    <param>
        <value><i4>40</i4></value>
    </param>
  </params>
</methodCall>
```

An example of a typical XML-RPC response would be:

```xml
<?xml version="1.0"?>
<methodResponse>
  <params>
    <param>
        <value><string>South Dakota</string></value>
    </param>
  </params>
</methodResponse>
```

**Identification of clients for authorization purposes can be achieved using popular HTTP security methods.** Basic access authentication is used for identification, HTTPS is used when identification (via certificates) and encrypted messages are needed. Both methods can be combined.

**In comparison to REST, where resource representations(documents) are transferred, XML-RPC is designed to call methods.**

**Critics of XML-RPC argue that RPC calls can be made with plain XML, and that XML-RPC does not add any value over XML.** Both XML-RPC and XML require an application-level data model, such as which field names are defined in the XML schema or the parameter names in XML-RPC. Furthermore, **XML-RPC uses about 4 times the number of bytes compared to plain XML to encode the same objects, which is itself verbose compared to JSON.** ([what-is-the-benefit-of-xml-rpc-over-plain-xml](http://stackoverflow.com/questions/1376688/what-is-the-benefit-of-xml-rpc-over-plain-xml))

***

XML-RPC는 XML로 인코딩하고, HTTP로 전송한다. XML-RPC는 후에 발전하여 SOAP이 나오게 된다. HTTP를 사용하기 때문에, HTTP에 기본인증, HTTPS 등을 사용할수가 있다. REST랑 비교하자면, REST는 리소스를 다루는거고, RPC는 메소드 호출을 다루는거다. RPC는 사실 plain-xml을 사용해도 되지만, xml-rpc를 사용하면 얻는 이점들이 있다.(xml-rpc는 plain xml에 비해 4배나 크고, json에 비교하면 엄청나게 크다)

<br>
[wiki:SOAP](https://en.wikipedia.org/wiki/SOAP)

***

SOAP has three major characteristics:
- extensibility (security and WS-routing are among the extensions under development)
- neutrality (SOAP can operate over any transport protocol such as HTTP, SMTP, TCP, UDP, or JMS)
- independence (SOAP allows for any programming model)

Both SMTP and HTTP are valid application layer protocols used as transport for SOAP, **but HTTP has gained wider acceptance as it works well with today's internet infrastructure**; specifically, HTTP works well with network firewalls. SOAP may also be used over HTTPS with either simple or mutual authentication; **This is a major advantage over other distributed protocols like GIOP/IIOP or DCOM, which are normally filtered by firewalls.** 

**Advantages**

- SOAP is versatile enough to allow for the use of different transport protocols. The standard stacks use HTTP as a transport protocol, but other protocols such as SMTP can also be used. SOAP can also be used over JMS and Message Queues.
- Since the SOAP model tunnels fine in the HTTP post/response model, it can tunnel easily over existing firewalls and proxies, without modifications to the SOAP protocol, and can use the existing infrastructure.

**Disadvantages**

- When using standard implementations and the default SOAP/HTTP binding, the XML infoset is serialized as XML. Because of the verbose XML format, **SOAP can be considerably slower than competing middleware technologies such as CORBA or ICE.** This may not be an issue when only small messages are sent. To improve performance for the special case of XML with embedded binary objects, the Message Transmission Optimization Mechanism was introduced.
- When relying on HTTP as a transport protocol and not using WS-Addressing or an ESB, the roles of the interacting parties are fixed. Only one party (the client) can use the services of the other. Developers must use polling instead of notification in these common cases.
- The verbosity of the protocol led to the domination in the field by services leveraging the REST architectural style.

***

SOAP은 security, routing등을 적용할수 있고, HTTP뿐만 아니라, SMTP, TCP등의 다른프로토콜 위에서 사용가능하다. 또한 언어독립적이기 때문에 다양한 프로그래밍에서 사용될수 있다. SOAP은 여러가지 전송방식중에 HTTP가 많이 사용되게 되었는데, 방화벽은 기본적으로 80포트를 열어놓기 때문이다. 단점은 XML을 사용하기 때문에 CORBA, ICE에 비해서 무겁다. 또한 SOAP프로토콜 자체가 복잡하고 무겁기 때문에, 결국 REST 아키텍쳐가 인기를 끌게 된다.

<br>
[wiki:JSON-RPC](https://en.wikipedia.org/wiki/JSON-RPC)

***

JSON-RPC is a remote procedure call protocol encoded in JSON. It is a very simple protocol (and very similar to XML-RPC), defining only a handful of data types and commands. **JSON-RPC allows for notifications (data sent to the server that does not require a response)** and **for multiple calls to be sent to the server which may be answered out of order**.

***

