---
layout: post
title: curry, bind, lambda
date: 2015-09-16 12:53:25
tags: cpp bind
---

Scala의 [Currying](https://en.wikipedia.org/wiki/Currying)을 CPP로 따라해보고 싶었다. 

Currying이 뭐냐하면.. 2개의 파라미터를 받아야 하는 함수를 1개의 파라미터만 받아도 되는 새로운 함수로 만드는것? 이라고 설명할수 있을것 같다.

`f:(x,y)->z` 

> x,y를 넣으면 z라는 **값**을 반환

`curry(f):x->(y-z)` 

> f를 curry 하면 'y를 넣으면 z를 반환하는 **함수**'를 반환


아래는 Scala에서 짝수를 구하는 예시이다. [링크](http://docs.scala-lang.org/ko/tutorials/tour/currying.html)

`modN`은 2개의 파라미터를 받는 함수인데, currying해서 1개의 파라미터만 받는 함수로 만들어서 filter에 넘기고 있다.

{% highlight java%}
def filter(xs: List[Int], p: Int => Boolean): List[Int] =
    if (xs.isEmpty) xs
    else if (p(xs.head)) xs.head :: filter(xs.tail, p)
    else filter(xs.tail, p)

def modN(n: Int)(x: Int) = ((x % n) == 0)
val nums = List(1, 2, 3, 4, 5, 6, 7, 8)

println(filter(nums, modN(2)))
{% endhighlight %}

이걸 CPP에서는 어떻게 할수 있을까?

멤버를 가지고 있는 Function Object를 만들어서 하면 될거 같아서 만들어봤다.

{% highlight cpp%}
std::vector<int> list = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

template<typename T, typename F>
T filter(T list, F f)
{
	T result;
	for (auto iter = std::begin(list); iter != std::end(list); iter++){
		if (f(*iter)){
			std::copy_n(iter, 1, std::inserter(result, std::end(result)));
		}
	}
	return result;
}
{% endhighlight %}

{% highlight cpp%}
template<typename F, typename R, typename T>
class binder1
{
private:
	F f;
	T v;
public:
	binder1(F f, T v)
	{
		this->f = f;
		this->v = v;
	}
	R operator()(T v)
	{
		return this->f(this->v, v);
	}
};

bool mod(int denominator, int value)
{
	return (value % denominator) == 0;
}

// 2로 나눠서 나머지가 0
auto result = filter(list, binder1<std::function<bool(int, int)>, bool, int>(mod, 2));
{% endhighlight %}

그럼 between도 해보자.

{% highlight cpp%}
template<typename F, typename R, typename T>
class binder2
{
private:
	F f;
	T v1;
	T v2;
public:
	binder2(F f, T v1, T v2)
	{
		this->f = f;
		this->v1 = v1;
		this->v2 = v2;
	}
	R operator()(T v)
	{
		return this->f(this->v1, this->v2, v);
	}
};

bool between(int left, int right, int value)
{
	return left < value ? value < right : false;
}

// 3과 7사이
auto result = filter(list, binder2<std::function<bool(int, int, int)>, bool, int>(between, 3, 7));
{% endhighlight %}

binder1, binder2... binder ??? 개수만큼 만들어야 하나?

일반화 시킬수 없을까? 3과 7사이 이면서 짝수를 찾아보자.

{% highlight cpp%}
template<typename F, typename R, typename S, typename T>
class binderN
{
private:
	F f;
	std::tuple<T, T, T> vs;  // std::tuple<T...> vs 이렇게 하고 싶은데.. 컴파일에러가 발생한다.
public:
	template<typename ...V>
	binderN(F f, V... v)
	{
		this->f = f;
		vs = std::make_tuple(v...);
	}
	R operator()(S s)
	{
		return this->f(std::get<0>(vs), std::get<1>(vs), std::get<2>(vs), s);  // 이부분도 어떻게 해야할지 모르겠다.
	}
};

bool between_and_mod(int left, int right, int denominator, int value)
{
	return ((left < value) ? ((value < right) ? (value % denominator) == 0 : false) : false);
}

auto result = filter(list, binderN<std::function<bool(int, int, int, int)>, bool, int, int>(between_and_mod, 3, 7, 2));
{% endhighlight %}

어떻게 해야되지?? parameter pack 어떻게 쓰는건지 잘 모르겠다.. ㅠㅜ

아래처럼 lambda를 이용해서도 할 수 있다.

{% highlight cpp%}

template <typename R, typename A, typename B>
std::function<std::function<R(B)>(A)> curry(std::function<R(A, B)> func)
{
	return [=](A a)->std::function<R(B)>{
		return [=](B b)->R{
			return func(a, b);
		};
	};
}

template <typename R, typename A, typename B>
std::function<R(A, B)> uncurry(std::function<std::function<R(B)>(A)> func)
{
	return[=](A a, B b)->R{
		return func(a)(b);
	};
}

template <typename R, typename A, typename B, typename C>
std::function<std::function<R(C)>(A,B)> curry(std::function<R(A, B, C)> func)
{
	return [=](A a, B b)->std::function<R(C)>{
		return [=](C c)->R{
			return func(a, b, c);
		};
	};
}

template <typename R, typename A, typename B, typename C, typename D>
std::function<std::function<R(D)>(A,B,C)> curry(std::function<R(A, B, C, D)> func)
{
	return [=](A a, B b, C c)->std::function<R(D)>{
		return [=](D d)->R{
			return func(a, b, c, d);
		};
	};
}

auto result11 = filter(list, curry<bool, int, int>(mod, 2));
auto result21 = filter(list, curry<bool, int, int, int>(between, 3, 7));
auto result32 = filter(list, curry<bool, int, int, int, int>(between_and_mod, 3, 7, 2));

std::function<std::function<bool(int)>(int)> fnCurryMod = curry<bool, int, int>(mod);
assert(fnCurryMod(2)(3) == false);

std::function<bool(int,int)> fnUncurryMod = uncurry<bool, int, int>(fnCurryMod);
assert(fnUncurryMod(2, 3) == false);

{% endhighlight %}

