---
layout: post
title: "C++ Template Template Parameter"
date: 2016-01-09 09:53:25
tags: cpp template
---

[http://www.informit.com/articles/article.aspx?p=376878](http://www.informit.com/articles/article.aspx?p=376878)

본론에 들어가기 앞서 사전지식에 대해서 눈높이를 해보자.

{% highlight cpp %}
template <typename T> class A; // (1)
template <typename> class A; // (2)
{% endhighlight %}

(1)과 (2)에는 차이점이 있다. (1)은 template parameter의 이름을 T라고 하였고, (2)는 이름을 주지 않았다.
(2)와 같은 코드는 문제가 있는걸까? 아니다. 왜냐하면.. template parameter에 이름을 주는것은 옵션이기 때문이다.
왜냐하면, 만약 그 어디에서도 template parameter 이름을 사용하지 않는다면, 이름은 필요없기 때문이다. 아래의 `adder` 처럼 말이다. 

{% highlight cpp %}
template <typename> 
int adder(int a) { return a++;} 
{% endhighlight %}

<br>

-----------------------------------------------------
-----------------------------------------------------
이제 본론으로 들어가겠다. 설명을 위해 STL을 사용하지 말고 우리만의 `Stack`을 다시 만든다고 가정해보자.

{% highlight cpp %}
template <typename T, class Container>
class Stack {
  public:
    ~Stack();
    void push( const T & );
    //...
  private:
    Container s_;
};

Stack<int, List<int>> stack_Using_List1; // (1) ok
Stack<int, Deque<int>> stack_Using_Deque2;  // (2) ok
Stack<string, List<int>> stack_Using_List3; // (3) error
{% endhighlight %}

아마도 위와 같이 만들것이다. (1)은 컨테이너를 `List`를 사용하도록 하고, (2)는 `Deque`을 사용하도록 하였다.
그리고 (3)은 컴파일 에러가 발생한다. 왜냐하면 `List<int>` 컨테이너에 `string`을 넣을수 없기 때문이다. 
이러한 잘못된 사용을 막기 위해서 아래처럼 고쳐보자.  

{% highlight cpp %}
template <typename T, class Container = Deque<T>>
class Stack {
  public:
    ~Stack();
    void push( const T & );
    //...
  private:
    Container s_;
};

Stack<int> stack1; // (1) ok. container is Deque<int>
Stack<string> stack2; // (2) ok. container is Deque<string>
{% endhighlight %}

위 코드는 컨테이너를 `Deque`으로 고정해버리는 타협을 했다. 
다른 더 좋은 방법은 없을까? 여기서 이번 글의 핵심인 template template parameter이 등장한다.

{% highlight cpp %}
template <typename T, template <typename> class Container>
class Stack{
  public:
    ~Stack();
    void push( const T & );
    //...
  private:
    Container<T> s_;
};

Stack<int,List> aStack1; // (1) ok
Stack<string,Deque> aStack2; // (2) ok
{% endhighlight %}

이제 (1),(2) 같은 보다 좋은 코드를 작성하는 것이 가능해졌다. 이 코드는 그냥 느낌적으로 보면, 그다지 어렵지 않은 코드다.
간략하게 설명하면 "1번째 parameter로 넘어온 T가 2번째 parameter인 Container의 template parameter로 사용하고 있구나.." 정도로 이해하면 된다. 

그런데 뭔가 햇갈리지 않는가? 일단 코드부터가 헷갈리게 한다. `template <typename T, template <typename> class Container>` 보기만 해도 어지럽다.

우선 용어부터 정리해보자.
 
- 첫번째 파라미터인 T는 template parameter의 이름이다.
- 두번째 파라미터인 Container는 template template parameter 이름이다.

template template parameter !? 이게 뭘까?? 
template을 2번 적은건 오타가 아니다. 

<br>

-----------------------------------------------------
-----------------------------------------------------
아래 코드를 보자.

{% highlight cpp %}
template <typename T>
class List;

template <typename Container> 
class Wrapper{
  //...
  Container s_;
};

Wrapper<List1<int>> w1; // (1) fine, List<int> is a type 
Wrapper<List1> w2; // (2) error! List is a template          
{% endhighlight %}

- `List<int>`는 `List` 템플릿 클래스를 `int` 타입으로 인스턴스화한 타입
- `List`는 템플릿 클래스

위 설명이 이해가 되는가? 그렇다면 (2)에서 왜 컴파일 에러가 발생하는지도 이해가 될것이다.
`Wrapper` 템플릿 클래스가 인스턴스화 될려면, 멤버변수인 `s_`를 어떤 타입으로 인스턴스 할지 정하는데 필요한 타입을 알려줘야 한다.
따라서 타입을 넘기는 (1)은 컴파일이 되는 반면, 템플릿 클래스를 넘기는 (2)는 컴파일 에러가 발생하는 것이다.    

다시 아래코드를 보자. 

{% highlight cpp %}
template <typename T, template <typename> class Container>
class Stack{
  // ...
  Container<T> s_;
};

Stack<int,List> aStack1; // (1) ok
Stack<std::string,Deque> aStack2; // (2) ok
{% endhighlight %}

`Stack<int,List> aStack1;` 의 2번째 파라미터 `List`는 템플릿 클래스이다. 
**다시 한번 강조해서 말하면 `타입`이 아니라 `템플릿 클래스`를 넘기고 있다.**

그렇다!! 

template template parameter는 `타입`이 아니라 `템플릿 클래스`를 넘기기 위해서 사용하는 문법이다.  