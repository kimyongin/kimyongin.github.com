---
layout: post
title: "promise"
date: 2016-02-25 09:53:25
tags: data elasticsearch
---

원문 : [https://blog.getify.com/promises-part-1/](https://blog.getify.com/promises-part-1/)

### Part 1: The Sync Problem

### Async

async는 정확하게 무슨 뜻일까?

우리는 지금까지 ajax 요청이 끝났을때 결과를 받기위해 콜백을 전달해왔다.
그런데 어떻게 콜백이 호출되는지? 어떻게 두개의 콜백이 동시에 실행될수 있는지? JS엔진은 어떻게 이것을 관리하는지?
에 대해서 고민해본적 있는가?

async가 무엇인지 이해하려면 JS엔진의 한가지 사실을 알아야 한다.
그것은 바로 "JS엔진은 싱글쓰레드" 라는 것이다.
이것의 의미는 한번에 한조각의 JS코드가 실행된다는 뜻이다.

여기서 한조각 이라는건 무슨 뜻일까?
각각의 함수(예를 들어 콜백)는 분해할수 없는 한조각이다.
JS엔진이 함수를 실행하면, 그 함수는 시작되고 완료된다. 
무슨뜻이냐면, 한조각이 완료되고 나서야 다른 조각을 처리한다는 뜻이다.

예를 들면, JS엔진은 테마파크의 롤러코스터라고 생각할수 있다.
사람들이 롤러코스터를 타기위해서 긴줄을 서서 기다리고 있고,
한번에 한사람만 롤러코스터에 탑승할수 있는 것과 같다. 
앞사람이 내리면 다음사람이 타는 것이다. 
 
테마파크 예시 에서 줄을 서서 기다린다고 했는데, 이 줄을 "이벤트루프"라고 부른다.
"이벤트루프"는 가능한 빨리 돌아가며, 만약 이벤트루프에 함수가 기다리고 있다면 JS엔진이 그것을 실행시키고
다음 함수로 넘어가거나, 다음 함수가 이벤트루프에 들어올때까지 기다린다.

### Concurrency

위에서 JS엔진은 싱글쓰레드 라고 했다.
싱글쓰레드의 "one task at a time" 이라는 관점에서 생각해보면 
이것은 굉장히 느리고 제한적일 거라는 생각을 할 수 있다.

요청을 보내기 위해 버튼을 눌렀을때, 마우스가 멈추고 스크롤을 내릴수 없다면? 
이런 현상이 요청이 완료 될때까지 몆초동안 지속된다면? 이 얼마나 최악의 UX인가?

하지만 우리는 생각보다는 답답함을 느끼지 않는다. 
그 이유는 여러개의 task들이 동시에 동작하기 때문이다. 
그런데 어떻게 여러개의 task가 동시에 동작하는 걸까?

이것을 이해 하려면, task(작업)는 여러개의 작은 snippet(함수)으로 나뉘어지고,
각각의 snippet들이 매우 짧게 분할된 시간을 나누어서 사용한다는 것을 알아야만 한다.
이것은 보통 1밀리초 보다 짧다. JS엔진은 눈깜짝할 사이에 수백개의 작은 snippet들을 처리 하는것이다.

{% highlight javascript %}
Task A (Ajax 요청)
snippet 1 
snippet 2 
snippet 3 
snippet 4

Task B (스크롤 다운)
snippet 1
snippet 2
{% endhighlight %}

위와 같은 Task A, B가 있다.
JS엔진은 싱글쓰레드 이기 때문에 A:1과 B:1을 동시에 처리할수 없다.
대신 JS엔진이 각각의 snippet들을 번갈아가면서 처리 해주기 때문이다. 
Task B는 Task A가 완전히 끝날때까지 기다릴 필요가 없다.

그래서 아래의 순서대로 실행될수 있다.

{% highlight javascript %}
A:1
B:1
A:2
B:2 (Task B Complete!)
A:3
A:4 (Task A Complete!)
{% endhighlight %}

위와 같은 이유로 Task A and Task B can run at the same time 라고 말할수 있다.
실제로는 동시에 실행되는 것이 아니므로 좀더 정확하게 말하면, 
Task A and Task B run concurrent to each other 라고 말할수 있다.

그리고 JS 이벤트루프는 위에서 설명한 concurrency 모델이다.

※ Concurrency는 parallel과 헷갈리기 쉬운데 parallel은 두개의 쓰레드가 각각 A:1과 B:1을 수행하는 것을 말한다.
다시 말하면 B:1이 수행되기 위해서 A:1이 완료되기를 기다릴 필요가 없는 것이다. 
테마파크 예를 다시 들면, 2대의 롤러코스터가 있는것과 같은 것이다.
 
### Synchronous Async

async 코드를 작성하는데 있어서 주의할점은, 
우리가 눈으로 보는 코드와 실제 엔진이 처리하는 방식이 다르다는 것이다.

예를 들어보자.

{% highlight javascript %}
makeAjaxRequest( url, function(response){
    alert( "Response: " + response );
} );
{% endhighlight %}

위 코드의 흐름을 설명해보자. 대부분의 개발자는 아래와 같이 대답할 것이다.

1. Ajax 요청을 한다.
2. 응답이 오면(요청이 완료되면) alert이 뜬다.

그러나 이 설명은 JS엔진의 동작방식과 정확히 일치하지 않는다.
문제는 우리의 사고방식이다. 우리는 synchronous 하게 동작하는 사고방식을 가지고 있다.

우리는 "..하면", "..될때까지 기다린다." 같은 synchronous 한 단어를 사용한다.
하지만 JS엔진은 Step1~2 사이에서 요청이 완료되기를 기다리고 있지 않는다.

JS엔진이 동작하는 방식으로 정확하게 설명하면, 아래와 같다.

1. Ajax 요청을 한다.
2. 나중에 요청이 완료되면 호출될 콜백을 등록한다.
...
3. 다른 작업을 한다.
...
4. 요청이 완료되면 등록된 콜백을 찾아서 호출한다.
 
큰 차이를 못느낄수도 있지만, Step3을 건너뛰어서 생각하는 것은 큰 문제이다.
왜냐하면 async는 우리의 사고방식으로는 이해하기 어렵기 때문이다. 

### Summary

문제는 우리의 사고방식은 async를 다루기에는 맞지 않기 때문에
async한 코드를 작성하는 것이 어렵다는 것이다.

우리가 필요한건, async한 요소들을 최대한 숨겨서, 
우리의 사고방식으로 이해하기 쉽도록 sync 처럼 보이도록 표현하는 방식이다.

우리의 최종목적은 sync처럼 보이는 코드이다. 
그것의 내부적인 동작이 sync 또는 async 둘중에 어떤것이어도 상관없도록 말이다.

--------------------------------------------------------------------
--------------------------------------------------------------------

### Part 2: The Inversion Problem

### Nested Callbacks

이벤트루프 모델은 JS의 초창기 부터 내장되어 있었다. 
그리고 우리는 최근까지 아래와 같은 패턴으로 async한 코드를 작성해 왔다.
  
{% highlight javascript %}
makeAjaxRequest( url, function(response){
    alert( "Response: " + response );
} );
{% endhighlight %}

이러한 코드는 async 코드를 하나만 작성할때는 그다지 나쁘편은 아니다.
하지만 아래처럼 여러개의 async를 중첩해서 작성해야 한다면 이야기가 달라진다.

{% highlight javascript %}
btn.addEventListener( "click", function(evt){
    makeAjaxRequest( url, function(response){
        makeAjaxRequest( anotherURL + "?resp=" + response, function(response2){
            alert( "Response 2: " + response2 );
        } );
    } );
}, false );
{% endhighlight %}

### Callback Hell

위와 같이 여러개의 단계로 async를 호출해야 한다면.. 보는것처럼 중첩이 생겨서 깊이가 깊어질수 밖게 없다.
그리고 깊이가 길어질수록 코드를 작성하기도, 읽기도 어려워 진다. 이를 콜백지옥이라고 부른다.

### Trust Lost

진짜 문제는 중첩으로 인해 깊이가 깊어지는 문제가 아니다. 
아래 코드를 보자.

{% highlight javascript %}
// 이전의 코드
...
someAsyncThing( function(){
    // 나중을 위한 코드
    ...
} );
{% endhighlight %}

위 코드를 2개의 부분으로 나눌수 있다.

1. 어떤 일이 발생하기 전
2. 어떤 일이 발생한 후

여기서 문제는 (1)과 (2)사이에 무슨일이 벌어지는가? 이다.
someAsyncThing(..)이 진행중일때 우리가 someAsyncThing()에 대한 제어권을 소유하고 관리하는 것일까? 
대부분은 아니다. 여기서 중요한것은 우리가 someAsyncThing()를 신뢰할수 있는지 여부이다.

여기서 "뭘 신뢰한다는 거지??" 라는 의문을 가질수 있다. 
우리는 someAsyncThing()을 호출하면서 아래의 것들을 암묵적으로 믿고 있다.

1. Don’t call my callback too early
2. Don’t call my callback too late
3. Don’t call my callback too few times
4. Don’t call my callback too many times
5. Make sure to provide my callback with any necessary state/parameters
6. Make sure to notify me if my callback fails in some way

하지만 이건.. 지나친 신뢰이다.
그리고 여기서 드러나는 진짜 문제는 콜백에 의한 Inversion of Control(제어의 역전) 이다.

우리는 콜백을 호출하는 제어권을 someAsyncThing()에게 넘긴것이다.
이러한 제어의 역전은 우리의 코드와 다른 코드 사이의 신뢰를 잃게 한다.

### Scare Tactics

만약 someAsyncThing()이 써드파티 라이브러리의 함수라서 제어하거나, 감시할수 없다면 어떻게 될까?
그저 행운이 따르기를 바라는 수밖게 없다.

예를 들어 전자상거래 페이지를 개발한다고 했을때, 써드파티 라이브러리의 Charge 함수를 호출하면서, 
콜백 함수를 넘기고, 요청이 완료되면 콜백함수에서 금액을 충전해준다고 가정해보자.

평상시에는 잘 동작한다. 그런데 만약 써드파티 업체에서 어떤 버그를 발견하였고 
이를 해결하기 위해, 콜백함수를 5번 호출하도록 수정하였다면, 
우리 코드는 변한점이 없지만 금액 충전이 5번 되는 문제가 발생하는 것이다.

이러한 문제는 써드파티의 잘못이 아니라, 우리가 콜백이 1번만 호출된다고 신뢰 하였기 때문에 발생한 문제이다.
이것을 어떻게 해결해야 할까? flag를 추가하여 중복 실행을 막아야 할까?

### Duct-tape

flag를 사용하던, 어떤 다른 방식을 사용하던 상태를 추적해서 콜백을 한번만 호출하도록 수정했다고 해보자.
그런데 우리는 수많은 콜백중에 단 하나만 고쳤을 뿐이다. 만약 다른 콜백들에서도 이런 문제가 발생한다면 모두 고쳐야 된다.
결국 날이 갈수록 코드는 지저분해지고 거대해진다. 
 
우리가 아무리 잘나고 열심히 해도, 변하지 않는 사실은..
"상호를 신뢰 하는데 있어서 콜백은 커다란 문제"라는 것은 변하지 않는다.
우리는 계속해서 접착테잎(Duct-tape)으로 콜백 구멍을 막아야 한다.

### The Promised Solution

콜백 문제를 해결하기 위한 좋은 방법이 없을까?
있다!! 그것은 바로 Promise 이다.

Promise가 어떻게 동작하는지 설명하기 전에 그 뒤에 숨은 컨셉에 대해서 먼저 설명해보겠다.

### Fast Food Transaction

패스트푸드점을 간다고 생각해보자. 
우리는 맛있는 음식을 주문하고, 종업원이 우리에게 가격을 말하고, 우리는 종업원에게 돈을 지불했다.
이제 종업원은 우리에게 무엇을 돌려줄까?

만약 운이 좋다면, 음식이 바로 준비될것이다.
하지만 대부분은 주문번호가 적혀있는 영수증을 건내받는다.
그리고는 초조하게 음식이 나오기를 기다린다.

시간이 흘러, "주문번호 317번 나왔습니다" 라는 목소리를 들으면
우리는 카운터로 걸어가서 영수증과 음식을 교환한다.

이 상황을 Promise와 연관시켜 보자.

우리가 카운터로 가서 트랜잭션을(음식을 주문) 시작하지만, 트랜잭션은 곧바로 완료되지 않는다.
대신 우리는 잠시후에 트랜잭션이 끝날거라는 약속(주문번호가 적힌 영수증)을 받는다.
그리고 트랜잭션이 끝나면(음식이 준비되면), 우리에게 통보되고,
우리는 약속(영수증)과 처음에 원했던 그것(음식)을 교환한다.

다시 말하면, 주문번호가 적힌 영수증은 미래의 가치를 위한 약속 인것이다.
 
### Completion Event

위에서 살펴보았던 someAsyncThing()을 다시 생각해보자.
콜백을 넘기는 방식 말고 더 좋은 방식은 없을까?

우리는 someAsyncThing()을 호출하고, 
완료에 대한 통지를 받을수 있는 이벤트를 구독 할수 있다.

아래 코드를 보자.

{% highlight javascript %}
var listener = someAsyncThing(..);
listener.on( "completion", function(data){
    // keep going now!
} );
listener.on( "failure", function(err){
    // Oops, what's plan B?
} );
{% endhighlight %} 
 
이제 우리는 함수가 완료되거나 실패했을때의 이벤트를 통지 받을수 있게 되었다.

### Promise “Events”

Promise란 위에서 살펴본 완료/실패 통지를 받는 하나의 방법이라고 할 수 있다.

{% highlight javascript %}
function someAsyncThing() {
    var p = new Promise( function(resolve,reject){
        // at some later time, call `resolve()` or `reject()`
    } );
    return p;
}

var p = someAsyncThing();
p.then(
    function(){
        // success happened :)
    },
    function(){
        // failure happened :(
    }
);
{% endhighlight %}

우리는 위 코드의 then 이벤트를 반드시 구독해야 한다. 
그리고 성공/실패 시에 호출될 함수를 등록 해야 한다.

### Uninversion 

Promise를 사용하면, 콜백을 사용해서 제어가 역전되는 대신 우리가 제어를 유지할수 있게 된다.
이것은 JS async 프로그래밍에서의 중대한 발걸음 이다.

그런데 잠깐! 우리는 여전히 콜백을 넘기고 있는거 같은데??!!

맞다. Promise가 콜백의 사용을 제거 해주지는 못한다.
심지어, 어떤 경우에는 더 많은 콜백을 사용해야 될지도 모른다.

하지만, 우리가 Promise 매커니즘을 통해서 콜백을 넘기게 된다면 아래의 것들을 보장 받을수 있다.

1. Promise가 완료되면, 성공/실패 콜백중에 하나만 호출된다.
2. Promise가 일단 완료되었다면, 절대로 다시 완료 되지 않는다. (콜백을 여러번 호출하지 않는다는 뜻이다.)
3. Promise가 성공 메세지(데이터)를 리턴하면, 성공 콜백이 해당 메세지(데이터)를 받게 된다.
4. Promise가 실패 한다면(예상치 못한 예외상황 이거나 우리가 정의한 에러상황), Promise 가능한 어느위치에서 에러가 발생했든지 실패 콜백이 호출된다.
5. Promise의 결과는 한번 결정되면 변하지 않는다. 그리고 Promise 안에서 항상 접근할수 있다.

### Summary

콜백 지옥은 단순히 함수중첩을 의미하는 것이 아니다.
이것은 Inversion of Control 에 대한 문제이다.
 
--------------------------------------------------------------------
--------------------------------------------------------------------

### Part 3: The Trust Problem

이 뒤 부터는 어려워서 읽다 지침..

요약 from 용인

1. 우리는 sync한 사고방식을 가지고 있기 때무에 async한 코드를 작성하기 어렵다.
2. 콜백의 진짜 문제는 중첩으로 인해 코드가 지저분해지는 것이 아니라, 제어권을 넘기는 것이 문제이다.

위 2가지 문제를 해결하기 위해서 promise를 사용하는데..

1. promise의 then chain을 통해서 sync하게 코드를 읽을수 있게 되고. (이 부분은 위 글에서 설명이 안되네..)
2. promise가 callback의 안전한 호출을 보장해주기 때문에 제어권을 넘기는 것에 대한 염려를 줄여줄수 있게 된다.

그런데..

Promise 안에서 resolve, reject 둘다 호출 안하고 있으면 어떡하지?
타임아웃 같은게 있을려나? 없는것 같던데.. 이것도 보장해주면 좋을거 같은데..















