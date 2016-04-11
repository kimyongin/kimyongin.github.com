---
layout: post
title: "nodejs errors"
date: 2016-03-08 09:53:25
tags: rx
---

https://www.joyent.com/developers/node/design/errors

이 문서는 아래의 질문에 대한 답을 해준다.

1. function을 작성할때, 언제 error를 throw 해야될까? 언제 callback, event emitter 등을 emit 해야 될까?
2. function은 어떻게 arguments를 취해야 할까? type check를 해야될까? 조건 검사를 해야될까? (null은 아닌지? ip-address 형식인지?)
3. arguments가 잘못 들어온 경우, error를 throw 해야될까? 아니면 callback에 error를 넘겨야 할까?
4. 여러가지의 error들을 어떻게 구별할수 있을까? 
5. caller에게 어떻게 하면 좀더 자세한 error 정보를 제공할수 있을까?
6. unexpected error 를 어떻게 handle해야 할까? try/catch를 사용해야 할까?

이 문서는 아래의 순서대로 설명 한다.

1. Background : 우리가 이미 알고 있는 것들.
2. Operation errors vs Programmer errors : 2개의 다른 종류의 error에 대한 소개
3. Patterns for writing functions : 유용한 error를 제공하는 function 작성 법
4. Specific recommendations for writing new functions : robust한 function을 작성하기 위한 체크리스트
5. An example : connect function 예제
6. Summary
7. Appendix : Conventional properties for Error objects : 올바른 방법으로 부가적인 정보를 제공하기 위해 사용되는 property들

============================================================================================================================
### 1. Background
============================================================================================================================

여기서는 아래의 것들을 알고 있다고 가정한다.

1. 우리는 javascript, java, python, c++ 같은 language의 exception handle에 익숙하다. 
따라서 throw, catch가 무엇을 의미하는지 이미 알고 있다.

2. 우리는 Node.js의 asnyc operation과 callback(err, result) pattern에 익숙하다.
따라서 아래 code가 왜 error handle이 동작하지 않는지 알고 있다.

function myApiFunc(callback)
{
  /*
   * This pattern does NOT work!
   */
  try {
    doSomeAsynchronousOperation(function (err) {
      if (err)
        throw (err);
      /* continue as normal */
    });
  } catch (ex) {
    callback(ex);
  }
}

이제 우리는 error를 전달하는 3가지 방법에 익숙해져야 한다.
1. throw error (making it an exception)
2. callback에 error를 전달하는 것
3. EventEmitter의 'error' event를 emit 하는 것

마지막으로 error와 exception은 차이가 있다는 것을 알아야 한다.
error는 Error class의 instance 이고, error를 throw하면, 그것이 exception이 된다.

예를 들면..

1. 아래는 error를 exception으로 사용하는 것이다.
throw new Error('something bad happened');

2. 아래는 throw없이 just error 로 사용하는 것이다. 
callback(new Error('something bad happened'));

Node에서는 just error로 사용하는 것이 일반적이다.
왜냐하면 대부분의 error는 async하게 발생하기 때문이다.(exception을 handle 할수있는건 try/catch 뿐이다.)
try/catch를 사용하는 것은 JSON.parse나 user-input validation 같은 sync operation 일때만 사용해야 한다.

============================================================================================================================
### 2. Operational errors vs Programmer errors
============================================================================================================================

error를 아래와 같은 2가지 분류로 나누는 것은 많은 도움이 된다.

1. Operational erros : 올바르게 작성된 프로그램이 런타임에 발생하는 문제를 말한다. 이것은 bug가 아니다. 
예를 들면, 메모리가 부족하거나, 너무 많은 파일이 열려있거나, 설정이 잘못되어 외부로 route가 안되거나, server에 연결이 안되는 등의 문제들이다.

2. Programmer errors : 프로그램의 bug를 뜻한다. 이것은 코드를 수정함으로써 문제를 해결할수 있다. 
예를 들면, undefined 를 읽으려고 하거나, async function을 callback없이 호출하거나, object를 넘겨야 하는데 string을 넘기는 등의 문제들이다.

일반적으로 우리는 'error'라는 단어를 위 2가지 모두에게 사용한다. 하지만 위 둘을 매우 다르다. 

Operational error는 올바른 프로그램이라면 반드시 다루어야 되는 error 이지만 bug는 아니다.  
예를 들어 "File not found"는 operational error 이며, "최초에는 파일을 만들어야 한다" 라는 의미를 가지고 있다.

반대로, Programmer error는 bug다. 그것들은 우리가 만들어낸 실수들이다.
예를 들어, user input의 validation 을 하지 않았거나, 변수 이름을 잘못 적은 실수 들이다.

이 차이점은 매우 중요하다. : 
Operational error는 프로그램의 정상동작 범주에 있으며, Programmer error는 bug 이다.

가끔은 이 두가지 error가 같은 원인으로 부터 발생할수도 있다. 
예를 들어 Http server가 undefined 변수를 사용해서 crash가 발생한다면, 이것은 Programmer error 이다.
그리고 때마침 어떤 Client가 Request를 하는데 server가 위의 bug로 crash 상태라면 ECONNRESET error를 보게될것이고, 
이것은 정상적인 프로그램이라면 반드시 handle해야되는 Operational error 이다.

또한, operational error를 handle하는것이 실패 했다면, 이것은 programmer error 이다.
예를 들어, 프로그램이 server에 connect를 시도하는데 ECONNREFUSED error를 받았는데, socket의 error event를 등록하지 않았고,
이로인해 프로그램이 crash 된다면, 이것은 programmer error 이다.
다시 말해, connection 실패는 operational error 이지만, 이 error를 handle하는 것이 실패한것은 programmer error 이다.

이러한 차이점들은 어떻게 error를 전달하고 어떻게 handle할지에 대한 기초가 된다.  
따라서 이 차이점을 정확하게 이해가 넘어가도록 하자.

### Handling operational errors

error handle를 하지않는 프로그램에 error handle를 쉽게 끼워넣을수 있는 방법은 없다. 
performace나 security 같은 부분을 중앙에서 handle하기 어렵듯이, error handle를 중앙에 모아서 handle하는 것은 쉽지 않다.

어떤코드가 실패할수 있다면, 실패가 어떻게 발생한것인지, 어떤의미를 가지고 있는지, 어떻게 handle해야 할것인가를 반드시 고민해야 한다.
여기서 중요한점은 error handle는 세심하게 다뤄져야 한다는 것이다. 
왜냐하면 어떤(what) error가 왜(why) 발생했는지에 따라서 handle방법이 다르기 때문이다.

우리는 같은 error를 call stack의 여러 level에서 handle할수 있다.
lower level에서 caller에게 유용한 예외를 전달하기는 어렵고, 
top level의 caller만이 적합한 응답을 알고 있기 때문에 재시도 operation을 하거나, user에게 error를 report 하거나, 또는 다른 어떤것을 할수 있다.
그렇다고해서 모든 error를 top level에서 handle하라는 뜻은 아니다. 
왜냐하면 top level에서는 error가 발생한 세부적인 상황을 알수가 없고, 어디까지 정상적으로 handle되었는지, 정확히 어디서 실패했는지를 알수가 없기 때문이다.

좀더 구체화 해보자. 아래에 어떤 error가 발생했을때 우리가 할수 있는 몆가지가 있다.
 
1. Deal with the failure directly : 때로는 error를 handle하는 방법이 명확할때가 있다. 
예를 들어 log file을 여는데 ENOENT error가 발생했다면. 그것은 아마도 프로그램을 처음 동작시킨 경우일 것이다. 그러면 log file을 생성하면 될뿐이다.
좀더 흥미로운 예를 들어보자. server와 지속적으로 connection을 유지한다고 했을때, socket hang-up error가 발생할수 있다.
이것은 네트워크 문제이거나, 과부하가 걸려서 생기는 문제일수 있다. 이럴때는 그냥 re-connect 하면 될뿐이다.(re-connect와 re-try는 엄연히 다르다.)

2. Propagate the failure to your client : error를 handle하는 방법을 모르겠다면, 가장 단순한 방법은, 지금 하던 작업을 중단하고 caller에게 알려주는 것이다.
이것은 error가 금방 수정되지 않을것 같은 경우에 적합하다. 예를 들어 user가 잘못된 json을 입력했다면, 그것을 다시 parsing 하는 것은 무의미 하기 때문이다.

3. Retry the operation : Web Service 같은 remote service에서 error가 발생했을때는, 때로는 re-try가 해결책일수도 있다.
예를 들어 remote service가 503(service unavailable)을 반환했다면, 몆초후에 re-try 해보는 것이 좋다.
그리고 만약 re-try를 할것이라면, 몆번 할것인지, 얼마나 기다릴수 있는지 등을 명확하게 문서화 해두어야 한다.
그렇다고 반드시 re-try를 하라는 뜻은 아니다. 예를 들어 다음과 같이 client <--> webapp <--> database 로 layer가 나누어진 경우 webapp에서 database에 계속해서 re-try 하는것보다
최초 error 발생시 즉각적으로 user에게 알려주는것이 더 좋을수도 있다. 그렇지 않다면 user를 무한정 기다리게 하게 되는 경우가 발생할수 있기 때문이다.

4. Blow up : 발생할수 없는 error 이거나, programmer error 처럼 발생하는 경우, log를 남기고 그대로 crash 하게 두어도 좋다.
예를 들어 같은 프로그램에서 listening을 하고 있고 그 프로그램에 localhost로 접속하는 것이 실패하는 상황 같은, 일반적으로 발생할수 없는 error 인 경우는
그냥 crash하게 두라는 뜻이다. 또는 out of memory 같은 error는 handle할수 있는 방법이 없기 때문에 crash 하게 두는것이 resonable 하다.

5. Log the error and do nothing else : 때로는 아무것도 할수 없을때가 있다. re-try, abort, crash .. 등등 그 어떤것도 선택할수 없는 상황이라면 log를 남기는것만 유일한 handle방법이다.
예를 들어 여러개의 remote service를 묶어서 dns로 서비스 하는데 그중 하나의 service가 dns에서 빠져나갔다고 해서 우리가 할수 있는 것은 아무것도 없다.

### (Not) handling programmer errors

programmer error를 handle할수 있는 방법은 없다. 

다시 말하면 이 programmer error는 operational error에서 했듯이, 더 많은 코드로 해결할수 있는 문제가 아니라는 뜻이다.
일부 사람들은 programmer error를 recover 하려고 시도하기도 한다. 단, 이러한 시도는 fast fail을 위한 것에만 허용된다.
programmer error는 코드를 처음 작성할때는 생각하지 못했던 것이다. 그렇다면 이 error가 이곳 말고 다른곳에 영향을 끼치치 않는다고 어떻게 확신을 할수 있겠는가?
따라서 handle, recover 등의 시도는 바람직하지 않다.

REST server가 있는데, 여러개의 request handler중에 하나가 잘못된 변수 이름을 사용해서 reference error를 throw하고 있다고 가정해보자
여기에는 수많은 심각한 bug를 야기할수 있는 여러가지 길이 있다.

1. 만약 request들이 공유하고 있는 어떤 자원의 상태가 null 이나 undefined로 남아있게 된다면, 
이것은 다음번 request가 해당 공유자원을 사용하려고 할때 문제를 발생시킬 것이다.

2. database connection이 leak 되고 있다면, 시간이 갈수록 동시에 handle할수 있는 request의 양이 줄어들 것이다.
결국에는 몆개의 connection만 남게 될것이고, 더이상은 concurrently 하게 handle하지 못하고 series 하게 handle하게 될것이다.

4. authenticate 상태가 남아있는 connection이, 그대로 이후의 connection에 계속해서 사용되게 된다면,
엉뚱한 user 정보로 request하게 되는 문제를 발생시킬 것이다.

5. socket이 열려있는 상태로 남아있다면, Node는 2분의 timeout을 준다. 
하지만 이 timeout이 override 되었다면 socket을 닫지 못하기 때문에 file descriptor 에 대한 leak이 발생한다.
결국은 사용할수 있는 file descriptor가 부족해지고 crash가 발생하게 된다.
만일 timeout을 override 하지 않았더라도, client는 2분동안 hang 되게 되고, hang-up error가 발생하게 된다.

6. 메모리 참조가 계속 남아있게 된다면, out of memory가 발생하게 되거나, GC가 더 많은 시간을 소비하도록 한다.
이는 결국 성능상의 문제를 일으키게 된다.

### The best way to recover from programmer errors is to crash immediately

programmer error가 발생하면 즉시 crash 하도록 하고, 자동으로 재시작 하도록 만들어야 한다.
crash가 가장 빠른 recover 방법이다. 이방법의 유일한 단점은 연결되어있는 client가 잠시동안 작업을 진행할수 없는 것이다.
하지만 아래의 것들을 기억해야 한다.

1. 이러한 error는 언제나 bug 이다. 우리는 시스템이나 네트워크 실패에 대해서 이야기 하는것이 아니다. 
프로그램안에 존재하는 실제 bug에 대한 이야기다.

2. 운영중인 프로그램이 crash가 발생해서, 연결이 끊어지는 문제가 있다고 치자.
하지만 진짜 문제는 연결이 끊어지는 것이 아니라. server가 so buggy 한것이 문제다.

만약 너무 자주 server가 crash해서 client의 접속이 자주 끊어진다면
포커스를 crash를 피하는 것이 아니라, crash 원인을 찾는데 맞춰야 한다.
원인을 찾는 좋은 방법은 Node가 uncaught exception이 발생하는 경우 dump를 남기도록 설정하는 것이다.

마지막으로 server측의 programmer error는 client측에서는 operational error 라는 것을 기억해야 한다.
따라서 client는 server의 crash를 handle해야만 한다.  

============================================================================================================================
### 3. Patterns for writing functions
============================================================================================================================

지금까지 error를 어떻게 handle할지에 대해서 알아보았다. 
이제부터는 새로운 function을 작성할때 어떻게 error를 전달할지에 대해서 알아보겠다.

가장 중요한것은 function이 무엇을 하는지에 대한 문서화이다.
어떤 arguments를 취하는지, 어떤것을 return 하는지, 어떤 error가 발생할수 있는지, error가 무슨 의미인지 등에 대한 문서화 말이다.

### Throw, Callback, or EventEmitter?

여기 3가지의 error를 전달하는 패턴이 있다.

1. throw는 error를 sync하게 전달한다. 
이것은 function이 호출된 같은 context 에서 호출된다. 
만약 caller에서 try/catch를 사용한다면 caller는 error를 catch 할수 있다.
만약 caller에서 아무것도 하지 않았다면, 프로그램은 crash 된다.

2. Callback은 async하게 event를 전달하는 가장 간단한 방법이다.
caller가 callback function을 넘기면, callee는 async 작업이 완료되면 callback을 호출하는 것이다.
일반적인 패턴은 callback(err, result) 형태이다. 그리고 작업의 결과에 따라서 error나 result 둘중에 하나만 null이 아니다.

3. callback을 사용하는 대신 좀더 복잡한 방법은 EventEmitter 방식이다.
EventEmitter object를 받은 caller는 error event를 listen 해야 한다.
이 방식은 아래와 같은 2가지 경우에 유용하다.

3.1. 복수의 error, 복수의 result를 반환하고 싶은경우, 예를 들어 db에서 data를 읽어오는데 모든 data가 준비 될때까지 기다렸다가 callback을 호출하는 것이 아니라.
data가 stream으로 도착하는 즉시 row event를 발생시켜서 조금씩 준비되는대로 data를 전달하는 것이다. 
그리고 모두 완료되면 end event를 발생시키고, 진행중에 error가 발생하면 error event를 발생시키는 것이다.

3.2. object가 복잡한 state machine인 경우, 예를 들어 socket 같은 경우, connect, end, timeout, drain, close event를 가지고 있는 EventEmitter object 이다.
이러한 방식을 사용하는 경우, error event가 언제 발생되었는지 명확히 하는것이 중요하다.

지금까지 우리는 callback과 event emitter를 async error를 전달하는데 사용하는것으로 뭉뚱그렸다.
하지만 만약 error를 async하게 전달하고 싶다면, 2가지 방법을 혼용하지 않고 하나만 사용해야 한다.

그렇다면.. 어떨때 throw를 사용해야 할까? 어떨때 callback을? event emitter를? 사용해야 할까?
위 결정은 아래의 것들에 달려있다.
1. error가 operational error 인가? programmer error 인가?
2. function이 sync 한가? asnyc 한가?

Node 프로그래밍의 대부분은 async function의 operational error 이다.
이들의 대부분은 argument로 callback을 받고, callee는 callback에 error를 넘긴다.
이 방법은 아주 잘 동작하고, 많은 사람들이 사용하고 있다. 

그 다음으로 가장 많은것은 sync function의 operational error 이다.
예를 들면 JSON.parse 같은 것이다. 이러한 function은 error가 발생하는 경우 sync하게 전달해야 한다.
따라서 throw 하거나 return 할수 있다.

function에서 만약 어떤 operational error가 발생하고 그것을 async하게 전달한다면,
모든 operational error에 대해서 async하게 전달해야 한다. (일부는 async, 일부는 sync하게 전달해서는 안된다는 뜻이다.)

일반적인 규칙은 operational error는 throw를 사용해서 sync하게 전달하거나, 
callback, eventemitter를 사용해서 asnyc하게 전달해야 한다. 둘을 혼용해서는 안된다.
이러한 규칙이 있음으로써, caller가 error를 handle하기 위해 try/catch를 하거나 callback을 넘기거나 둘중에 하나만 쓸수 있게 해준다.

이제 programmer error가 남았다. 이것들은 언제나 bug 임을 기억하자.
이것들은 function의 첫 라인에서 argument들의 type을 체크하거나 조건을 검사함으로써 즉시 알아낼수 있다.
예를 들어 누군가가 async function을 호출하면서 callback을 넘기지 않았다면 즉시 error를 throw 해야 한다.

programmer error는 절대로 handle하지 않기 때문에 위에서 말한 try/catch 혹은 callback(or emiter) 둘중에 하나만 사용해라 라는 규칙은 변하지 않는다.
(callback을 사용해서 operational error를 handle하고 있는데, programmer error를 handle하기 위해 try/catch를 추가해서 혼용하지 말라는 뜻이다.)

### Bad input: programmer error or operational error?

잘못된 user input은 programmer error 일까, operational error 일까?
간단하다. 그것은 function이 어떤 type을 허용하는지, 어떻게 해석하는지에 대해서 정의되어 있는 문서에 달려있다.
만약 우리가 문서화 하지 않은 다른 error가 발생하는 경우라면, 그것은 programmer error 이다.
만약 문서에는 적혀있지만 아직 적용은 안되있는 경우라면, 그것은 operational error 이다.

user input에 대해서 어떻게 제약사항을 둘지는 우리의 판단에 달려있다. 
ip address와 callback 을 넘겨받는 connect function을 호출한다고 생각해보자. 
그리고 user가 'bob' 같은 유효하지 않은 ip address를 넘기는 경우 문서에 아래 2가지 방법중 하나를 명시할수 있다.
- 문서 : ipv4형식에 맞는 문자열만 허용하며, 그 외에는 즉시 error를 throw 한다. (이 방식을 추천한다.)
- 문서 : 어떤 문자열도 다 받으며, 만약 해당 문자열에 접속이 불가능한 경우 async하게 '해당 주소에는 접속할수 없습니다' 라는 error를 전달한다.

위와 같은 문서는 operational error와 programmer error에 대한 지침을 일관되게 한다.
위와 같은 지침으로 인해 bad input이 operational error와 programmer error를 구분하도록 하고 있는 것이다.

보통, user input validation은 매우 느슨하다. 예를 들자면 매우 다양한 input을 받을수 있는 Date.parse 같은 경우가 그렇다.
하지만 대부분의 다른 function에 대해서는 느슨하기 보다는 엄격하게 하기를 추천한다.
규칙을 좀더 명확하게 하기 위해서 노력하는 시간을 아끼려 하지 말고, 디버깅 하는 시간을 줄이는 것이 훨씬 좋을 것이다.
게다가, 기존의 function을 개선해서 다음버전의 function을 만들때 validation 체크를 느슨하게 풀어주었고, 
만약 bug가 발견된다면. 이를 고치기 위해서는 하위 호환성을 포기하지 않는한 고칠수 없게 된다. (한번 느슨해진 function은 다시 엄격해질수 없다는 뜻이다.)

### What about domains and process.on('uncaughtException')?

Operational error는 항상 handle 되어야 한다. 그것도 아주 명확하게 try/catch, callback, event emitter 등의 방식으로 말이다.
따라서 process.on('uncaughtException')에서는 Programmer error만이 handle 되게 된다. 
그리고 앞에서 말했듯이 Programmer error는 handle 하지 않고 즉시 crash 하게 두는 것을 추천하기 때문에 process.on('uncaughtException')은 권장하지 않는다.

============================================================================================================================
### 4. Specific recommendations for writing new functions
============================================================================================================================

지금까지 많은 지침들을 알아봤다. 이제 좀더 디테일하게 알아보자.

1. Be clear about what your function does.
이것은 가장 중요하다. 문서에서는 아래의 것들을 명확히 해야한다.
- 어떤 argument를 넘겨야 하는지
- arguemnt의 type은 무엇인지
- 추가적인 조건(e.g. valid ip address)

만약 문서에 어떤것을 잘못 적었거나, 빼먹었다면 그것은 programmer error 이므로 곧바로 error를 throw 해야 한다.

추가적으로 아래의 것들을 문서에 넣을수 있다.
- 어떤 operational error가 발생할수 있는지
- operational error가 발생했을때 어떻게 처리하면 되는지
- return 값은 무엇인지

2. Use Error objects for all errors, and implement the Error contract
모든 error는 Error Class를 사용하거나, Error Class의 subclass를 사용해야 한다. 
그리고 name과 message properties를 넣어야 한다.

3. Use the Error's name property to distinguish errors programmatically.
error의 종류가 무엇인지 확인하려면 name property를 이용하면 된다.
새로운 name을 굳이 만들필요는 없다. 이미 많이 사용되는 이름을 그대로 사용해도 된다. 
예를 들자면 다음과 같은 것들 말이다. "ServiceUnavailableError", "TypeError" , "RangeError"

4. Augment the Error object with properties that explain details 
Error를 좀더 자세하게 설명을 하려면 Property를 추가하면 된다.
예를 들어 server에 connect하는 function이 error를 전달할때는, remoteIp : 'xxx.xxx.xxx' 같은 방식으로 property를 추가하면 된다.
만약 시스템 에러라면 syscall : 'xxx system api' 같은 방식으로 추가하면 된다.

좀더 구체화 하면.
- name : 여러가지 종류의 error를 프로그램적으로 구별할수 있도록 사용될수 있어야 한다.
- message : 사람이 읽고 이해할수 있는 메세지를 담고 있어야 한다.  
- stack : 건드리지 말고 V8엔진이 만들어주는대로 사용해라.

5. If you pass a lower-level error to your caller, consider wrapping it instead.
async funcA에서 async funcB를 호출하고, 만약 funcB에서 error가 발생하면 어떻게 해야할까?
여러가지 방법이 있겠지만, 여기서는 funcB에서 발생한 error를 그대로 전달하기로 했다고 해보자.
(그대로 전달 하지 않고, funcA에서 re-try 할수도 있고, 무시 하는 방법도 있다.)

이럴때는 그대로 전달하는것 보다는 한번 wrapping 하는것을 고려해보자.
wrapping 하게 된다면 low-level에서 발생한 error를 포함할 뿐만 아니라, current-level의 정보까지 포함시켜서 좀더 유용한 정보를 전달할수 있게 된다.
'verror'라는 모듈이 이러한 작업을 간단하게 할수 있도록 도와준다.

만약 Error를 wrapping 하기로 결정했다면 아래의 것들을 고려해야 한다.
1. 기존 error를 수정하지 않아야 한다. 이 말은 caller가 wrapping 되어있는 기존의 error를 그대로 사용할수 있어야 한다는 뜻이다.
2. error의 name을 그대로 사용하거나, 좀더 유용한 이름을 사용해야 한다. 
예를 들어 bottom-level의 error가 node의 plain error 라면, upper-level의 error는 'InitializationError'와 같은 형식이 될수도 있다.
3. 기존 error의 property를 보존해야 한다.

참고로 Joyent(Node.js 개발사)는 'verror' 모듈을 사용한다.

### Summary
1. Operational error는 예상가능하고, 피할수 없는 error 이며, Programmer error는 bug 이다.
2. Operational error는 반드시 handle 되어야 하며, Programmer error는 handle, 혹은 recover 될수 없다.
만약 Programmer error를 handle, recover 하려고 시도한다면, 그것은 debug를 어렵게 만들뿐이다.
3. function은 operational error를 sync(throw), async(callback, event emitter) 둘중에 한가지 방식으로만 전달해야 한다.
두가지 방법을 혼용해서는 안된다.
4. 새로운 function을 작성할때는, 문서를 명확하게 해야 한다. argument의 type과 조건은 무엇인지, error는 어떻게 전달하는지 등에 대해서 말이다.
5. 놓치거나 잘못된 argument는 programmer 에러이며, 이러한 에러는 즉시 throw 해야 한다. 다시 말하지만, 문서를 위반하는 error는 programmer error 이다.
6. error를 전달할때는 Error 클래스를 사용해야 한다.

