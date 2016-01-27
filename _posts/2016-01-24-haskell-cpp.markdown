---
layout: post
title: "Haskell and C++"
date: 2016-01-24 09:53:25
tags: Haskell cpp template
---

이 글은 아래의 원글을 읽으면서, 제가 이해한대로 다시한번 풀이해서 적어본 것입니다.
[http://bartoszmilewski.com/2009/10/21/what-does-Haskell-have-to-do-with-c/](http://bartoszmilewski.com/2009/10/21/what-does-Haskell-have-to-do-with-c/)

C++ Template Meta Programming(TMP)을 이해하고 싶다면 함수형 프로그래밍에 대해서 알아야 한다. 
이 문서에서는 Haskell과 C++TMP를 비교하면서 설명하려고 한다.

일단은 C++TMP를 Haskell의 어려운 버전 이라고 생각을 해줬으면 한다. 만약 Haskell을 모른대도 내가 설명해줄테니 걱정하지 말아라.
Haskell 같은 싱글 패러다임(only 함수형) 언어의 장점은 매우 단순한 문법을 가지고 있기 때문에 이해하는데 큰 무리없을 것이다.(예를 들어 Lisp의 경우, 문법이라고는 괄호 뿐이다.)

그리고 명심해둘 것이 있는데, Haskell은 데이터에 기반하여 런터임에 동작한다는 것이고, C++TMP는 타입에 기반하여 컴파일 타임에 동작한다는 것이다.  

<br>

-----------------------------------
-----------------------------------

## Functional Approach to Functions
 
#### - Haskell에서 팩토리얼을 구하는 함수를 살펴보자.

{% highlight cpp %}
(01) : fact 0 = 1 
(02) : fact n = n * fact (n - 1)
{% endhighlight %} 

(01)은 0!은 0임을 나타낸다. (02)는 매개변수가 0이 아닌 n!을 정의한다.
여기서는 재귀를 사용하는데, n!은 (n-1)!을 n번 하고 n이 0이 되면 (01)의 정의(fact 0 = 1)에 따라서 재귀를 멈추게 된다.
여기서 알아야 할 점은 fact 함수의 정의가 2개의 서브 정의로 나뉘어 지고, 패턴매칭을 이용 하여 동작한다는 것이다.

`fact 3` 호출은 아래와 같이 동작하게 된다.

(1). 매개변수 3이 첫번째 패턴인 0과 일치하는가? --> 일치하지 않음 --> 다음 패턴으로 이동    
(2). 매개변수 3이 두번째 패턴인 n과 일치하는가? --> 일치함 --> 재귀호출   
... (재귀호출)   
(n). 매개변수 0이 첫번째 패턴인 0과 일치하는가? --> 일치함 --> 재귀종료   

<br>

#### - C++TMP로 팩토리얼을 구하는 함수를 살펴보자.

{% highlight cpp %}
template<int n> struct
fact {
  static const int value = n * fact<n-1>::value;
};

template<> struct
fact<0> { // specialization for n = 0
  static const int value = 1;
};

int main(){
  std::cout << fact<3>::value << std::endl;
}
{% endhighlight %}

보이는가? C++TMP 문법이 Haskell의 단순하고 우아한 코드를 끔찍하게 만들어 놓은것을..
이 코드를 Haskell 코드와 비교하여 살펴보자. 순서는 바뀌었지만 2개의 정의가 있다. 순서가 바뀐 이유는 specialization declaration은 general declaration 보다 뒤에 와야 하기 때문이다.
대신 C++에서의 패턴매칭은 Haskell 처럼 정의된 순서대로 하지 않고, 가장 적합한 매칭을 찾는 방식이기 때문에 우리가 의도한 대로 동작하게 된다. 
예를 들어 `fact<0>::value`를 호출한다면 `fact<n>`이 아니라 정확하게 매칭되는 `fact<0>`을 호출한다는 것이다.

이번에는 C++TMP의 조금은 이상해 보이는 함수 호출방식인 `fact<n>::value`에 대해서 알아보자.
잘 생각해보자. `fact`가 우리가 일반적으로 작성하는 함수표현식 인가? 아니다! `fact`는 템플릿 구조체이다.
템플릿 구조체인데.. 우리는 함수를 호출하듯이 사용하고 있고, 함수라고 부르고 있다. 이건 어떻게 이해해야 할까?

<br>

#### - 함수란 무엇인가? 

일반적으로, 입력을 받아서 출력을 하는 것을 함수라고 한다. 
그리고 객체지향언어에서 사용하는 가장 보편적인 함수는 입력으로 `값`이 들어오고 출력으로 `값`이 나가는 것이다.

이제 템플릿 구조체를 함수를 바라보는 시선으로 바라보자. 
템플릿 구조체는 컴파일타임에 `타입`을 필요로 하는데 이것을 입력이라고 생각하고, 
입력으로 받은 타입에 따라 컴파일타임에 만들어진 결과`값`을 출력이라고 보는 것이다.
어떠한가? 이렇게 생각하면, 템플릿 구조체를 입력과 출력이 있는 함수라고 생각할 수 있지 않은가?

<br>

#### - 다른 예제

아래 Haskell 함수 `is_zero`는 `boolean`값을 반환한다. 

{% highlight cpp %}
is_zero 0 = True
is_zero x = False
{% endhighlight %}

이번에는 c++로 비슷한 코드를 작성해보자.  
아래 함수는 입력으로 들어오는 타입이 포인터이면 true값을 반환한다.

{% highlight cpp %}
template<class T> struct
isPtr {
  static const bool value = false;
};

template<class U> struct
isPtr<U*> {
  static const bool value = true;
};

template<class U> struct
isPtr<U* const> {
  static const bool value = true;
};

int main() {
  std::cout << isPtr<int>::value << std::endl;  // 0
  std::cout << isPtr<int*>::value << std::endl; // 1
}
{% endhighlight %}

<br>

-----------------------------------
-----------------------------------

## Lists

함수형 언어에서 가장 기본이 되는 자료구조는 list이다.
Haskell에서의 list는 대괄호를 사용하여 list를 표현한다. 예를들어 1, 2, 3 숫자들의 list는 아래와 같이 표현된다.

{% highlight cpp %}
[1, 2, 3]
{% endhighlight %}

<br>

#### - Haskell에서 list의 길이를 구하는 count 함수 예제를 살펴보자.

{% highlight cpp %}
(01) : count [] = 0
(02) : count (head:tail) = 1 + count tail
{% endhighlight %}

함수형 언어에서 list를 다룰때 역시 패턴매칭을 사용한다.
(01)의 패턴인 `[]`은 비어있는 list와 매칭되고, 
(02)의 패턴인 `(head:tail)`은 비어있지 않은 list와 매칭되며, list를 head와 tail로 나눈다.
만약 `count [1,2,3,4]` 를 호출하면 head는 1, tail은 [2,3,4]가 되며, `count tail`에 의해 재귀호출 하게 된다.

<br>

#### - C++TMP에서 list의 길이를 구하는 count 함수 예제를 살펴보자.

{% highlight cpp %} 
// just a declaration
template<class... list> struct
count;

template<> struct
count<> {
    static const int value = 0;
};

template<class head, class... tail> struct
count<head, tail...> {
    static const int value = 1 + count<tail...>::value;
};

int main(){
  std::cout << count<int, char, long>::value << std::endl; // 3
}
{% endhighlight %}

위 예제는 과거의 C++0x에서는 컴파일 되지 않는데, 
이는 모던C++에 `variadic templates` 과 `template parameter packs`이 추가됨으로써 가능해졌다.

위 count 함수는 아래와 같은 순서로 작성되었다.

1. 앞선 예제에서 살펴보았듯이, 여기서도 general declaration을 먼저 해야한다. 
다만 가변길이의 타입 파라미터를 받을수 있도록 `<class... list>`를 사용해야 한다.
2. 이제 general declaration이 존재하니, specialization declaration을 할수 있게 되었다.
C++TMP에서는 선언 순서가 상관없지만, Haskell과 동일하게 비어있는 list와 패턴매칭되는 함수, 그리고 비어있지 않는 list와 패턴매칭되는 함수 순서로 작성하였다. 
 
`Variadic templates`의 활용방법을 알고 싶다면 type-safe printf 에 대해서 공부해보면 좋을것이다.

<br>

-----------------------------------
-----------------------------------

## Higher-Order Functions and Closures

함수형 언어의 진정한 파워는 함수를 `first class`로 다루는 것으로 부터 발생한다.    

`first class`란 무엇일까? 조건은 아래와 같다.

1. 변수나 데이터 구조안에 담을 수 있다.
2. 파라미터로 전달 할 수 있다.
3. 반환값(return value)으로 사용할 수 있다.
4. 할당에 사용된 이름과 관계없이 고유한 구별이 가능하다.
5. 동적으로 프로퍼티 할당이 가능하다.

함수형 언어에서는 함수가 위의 조건들을 충족하기 때문에, 함수를 `first class object` 라고 한다.
다시 말하면 함수를 변수나 데이터 구조에 담을수 있고, 파라미터에 넘기거나 반환값으로 받을수 있다는 의미이다.

그리고, `higher-order function` 이라는 것이 있는데, 이것의 조건은 아래와 같다.

1. 하나 혹은 그 이상의 함수를 매개변수로 받는다.
2. 결과값으로 함수를 반환 한다.

이 둘을 조합해보면 함수형 언어에서는 함수를 `first class`로 취급하기 때문에 함수를 매개변수로 넘기고, 반환값으로 넘겨받을수 있다.
이 말은 곧, 함수형 언어에서는 `higher-order function`을 작성하는 것이 가능하다는 의미이다. 

그리고, 놀라운점은, C++에서는 `higher-order function`에 대한 지원이 런타임보다 컴파일타임에서 더 강력하다는 것이다.   

<br>

#### - Haskell에서의 `higher-order function` 예제를 보자.

{% highlight cpp %}
(01) : or_combinator f1 f2 = λ x -> (f1 x) || (f2 x)   
(02) : (or_combinator is_zero is_one) 2 // false : is_zero, is_one 둘다 거짓이다.
(03) : (or_combinator is_zero is_one) 0 // true : is_zero가 참이다.
{% endhighlight %}

(01)은 매개변수로 함수 2개`(f1, f2)` 를 받고,
반환값으로 x를 매개변수로 받는 이름없는 람다함수 `(λ x -> (f1 x) || (f2 x))` 를 반환하는 함수를 정의하였다.
(02)의 함수 호출을 살펴보면, 맨 뒤의 매개변수 2는 `(or_combinator is_zero is_one)`에서 반환한 람다함수의 매개변수로 사용되는 것이다.

위 예제에 대해서 좀더 이해하려면 `closure`에 대해서 알아야 한다. 여기서 `f1, f2`은 `first class object` 이다.
강조하기 위해서 다시 말하면, 매개변수로 넘긴 함수는 `object` 이다. 알다시피 `object`에는 생명주기가 있다. 생성되고 소멸된다는 뜻이다.

만약  `(or_combinator is_zero is_one)`에서 반환한 람다함수를 호출하려는데 `f1`이 소멸되었다면 어떤 일이 발생할까? 
당연히 의도하지 않은 결과가 나타날것이다. 따라서 람다함수는 자신이 호출될때까지 `f1, f2`가 소멸되지 않도록 잡고 있어야 하는데,
이러한 행위를 `capture`라고 부르며, 이렇게 함수가 생성될 당시의 컨텍스트(`f1, f2`)를 유지하는 함수를 `closure` 라고 부른다.

<br>

#### - C++TMP에서의 `higher-order function` 예제를 보자.

{% highlight cpp %}
template<template<class> class f1, template<class> class f2> struct
or_combinator {
    template<class T> struct
    lambda {
        static const bool value = f1<T>::value || f2<T>::value;
    };
};

std::cout << or_combinator<isPtr, isConst>::lambda<const int>::value << std::endl;
{% endhighlight %}

앞에서 설명했듯이, C++TMP는 템플릿 구조체가 타입을 매개변수로 받고, 값을 반환하는 함수이다. 
따라서 위 예제에서는 `or_combinator`가 템플릿 타입 매개변수 2개를 `f1, f2`로 받고, 
`lambda` 함수(템플릿 구조체)에서 `f1, f2`를 `capture`해서 가지고 있는다.   

<br>

-----------------------------------
-----------------------------------

## Higher-Order Functions Operating on Lists

좀더 나아가서, `Higher-Order Function` 과 `list`를 조합해서 사용하면 함수형 언어의 진면목을 볼수 있게 된다.
이것은 알고리즘처럼 느껴질수 있다. 예제를 보여주기에 앞서, 우리가 만들어볼 함수의 C++버전을 먼저 보여주겠다.

{% highlight cpp %}
bool pred(int a){
  return a > 10;
}
bool all(vector<int> vs){
  for each(auto v in vs){
    if(!pred(v)) return false;
  }
  return true;
}
{% endhighlight %}

위 코드는 vector의 모든 요소들이 pred 함수를 만족하는지 확인하고 있다. 
이제 이 코드를 Haskell 버전과 C++TMP 버전으로 작성해보자.

<br>

#### - Haskell에서의 list 요소들을 검사하는함수

{% highlight cpp %}
all pred [] = True
all pred (head:tail) = (pred head) && (all pred tail)
{% endhighlight %}

패턴매칭을 사용하여, 비어있는 list라면 True가 반환되고, 비어있지 않은 list라면 head, tail로 나눠서 
head에 함수를 실행하고, tail은 재귀호출에 사용된다. 이전에 살펴본 예제와 다른점은 함수를 함수의 매개변수로 넘길수 있는 `Higher-Order Function` 특징을 활용하여 
`all` 함수에 `pred`라는 함수와, 그 함수의 매개변수로 사용될 요소를 가지고 있는 list를 매개변수로 넘긴것이다. 

<br>

#### - C++TMP에서의 list 요소들을 검사하는함수

{% highlight cpp %}
template<template<class> class predicate, class... list> struct
all;

template<template<class> class predicate> struct
all<predicate> {
  static const bool value = true;
};

template<template<class> class predicate, class head, class... tail> struct
all<predicate, head, tail...> {
  static const bool value = predicate<head>::value && all<predicate, tail...>::value;
};

int main() {
  std::cout << all<isPtr, int*, int*, int*>::value << std::endl; // 1
  std::cout << all<isPtr, int*, int*, int*, int>::value << std::endl; // 0
}
{% endhighlight %}

<br>

#### - 이번에는 함수형 언어의 또다른 장점인 [fold](https://en.wikipedia.org/wiki/Fold_(higher-order_function)) 에 대한 예제 이다.

In functional programming, fold – also known variously as reduce, accumulate, aggregate, compress, or inject – 
refers to a family of higher-order functions that analyze a recursive data structure and through use of a given combining operation, 
recombine the results of recursively processing its constituent parts, building up a return value.

다시 설명하면, list를 재귀호출하면서 요소에 대해서 결합(combine) 연산을 하여, 요소들을 하나씩 줄여나가(접어나가:fold) 최종 결과값을 만들어내는 것이다. 
아래는 fold 방식이 아닌 전형적인 for문을 돌면서 값을 누적하는 total 함수에 대한 C++ 예제이다. 이것을 Haskell과 C++TMP로 작성해보자. 

{% highlight cpp %}
int add(int v1, int v2){
  return v1 + v2;
}
int total(int init, vector<int> vs){
  for each(auto v in vs){
    init = add(init, v);
  }  
  return init;
}
{% endhighlight %}
 
<br>
 
#### - Haskell에서의 누적값을 구하는 함수

{% highlight cpp %}
foldr f init [] = init
foldr f init (head:tail) = f head (foldr f init tail)

add_it elem so_far = elem + so_far
sum_it lst = foldr add_it 0 lst
{% endhighlight %}

foldr함수에 첫번째 파라미터로 add_it 함수를, 두번째 파라미터로 초기값(init:0), 세번째 파라미터로 list를 넘긴다.
패턴매칭을 통해 list가 비어있으면 초기값을 반환하고, list가 비어있지 않으면 head, tail로 나누어서 재귀호출을 한다.
재귀호출을 풀이하면 아래와 같다.

{% highlight cpp %}
foldr add_it 0 [1,2,3]
--> add_it 1 (foldr add_it 0 [2,3])
--> add_it 1 (add_it 2 (foldr add_it 0 [3]))
--> add_it 1 (add_it 2 (add_it 3 (0))
--> add_it 1 (add_it 2 (3))
--> add_it 1 (5)
--> 6
{% endhighlight %}

<br>
 
#### - C++TMP에서의 누적값을 구하는 함수

{% highlight cpp %}
template<int V1, int V2> struct
add_it {
  static const int value = V1 + V2;
};

template<template<int, int> class, int, int...> struct
fold_right;

template<template<int, int> class f, int init> struct
fold_right<f, init> {
  static const int value = init;
};

template<template<int, int> class f, int init, int head, int...tail> struct
fold_right<f, init, head, tail...> {
  static const int value = f<head, fold_right<f, init, tail...>::value>::value;
};

int main() {
  std::cout << fold_right<add_it, 0>::value << std::endl; // 0
  std::cout << fold_right<add_it, 0, 1, 2, 3, 4, 5>::value << std::endl; // 15
}
{% endhighlight %}

<br>

-----------------------------------
-----------------------------------

## List Comprehension

Haskell에는 list에 대한 연산을 할때 명시적인 재귀호출 없이 연산을 할수 있는 `List Comprehension`이라는 것이 있다.
이것은 기존의 list를 기반으로 새로운 list를 만들어내는 것이다. 이것은 `집합(set)` 에서 차용해온 것인데, 
아마도 집합에서 다음과 같은 정의 : `S is a set of elements`를 본적이 있을텐데 바로 이것을 이용하는 것이다. 

`[x * x | x <- [3, 4, 5]]`

이것은 x * x 요소들의 집합을 의미하며, x는 [3,4,5]로부터 가져온다.
이글의 초반부에 만들어본 count 예제가 기억나는가? 그것을 `List Comprehension` 을 사용하면 한줄에 작성이 가능하다.

이 다음부터 잘 못알아먹겠다... ㅠㅜ 
