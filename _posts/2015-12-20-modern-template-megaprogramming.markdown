---
layout: post
title: "template meta programming - ex1"
date: 2015-12-20 12:53:25
tags: cpp
---

`메타 프로그래밍`이란 무엇인가? 쉽게 말하면 코드(프로그램)을 조작하는 프로그램을 말한다. 
`C++ 컴파일러` 역시 C++ 코드를 조작해서 어셈블리 언어를 만들어내는 `메타 프로그램`이다.
이 글에서는 메타 프로그램인 `C++ 컴파일러`를 이용하는 `템플릿 메타 프로그래밍`에 대해서 알아볼 것 이다.

<br>

# 1에서 100까지 합을 구하려고 한다. 어떻게 해야 할까? 

- '반복문으로 돌면서 값을 누적하는 것' 만이 유일한 답일까?
{% highlight cpp %}
int accumulate(int count){
    int value = 0;
    for(int n = 0; n <= count; n++){
        value += n;
    }
}
std::cout << accs(100);
{% endhighlight %}

- 미리 계산해놓고 배열에 저장해놓고 사용하는 것도 방법이겠지?
{% highlight cpp %}
int accumulate[] = {0,1,3,6,10 .... ,5050};
std::cout << accumulate[100];   
{% endhighlight %}

(1), (2) 둘중에 뭐가 더 빠를까?? 당연히 (2)번이 더 빠르다. 
왜 그럴까? (2)는 컴파일 타임에 모든값이 결정 되어있고, 런타임에는 계산을 하지 않기 때문이다.
또 다른 방법은 없을까?

- 템플릿 메타 프로그래밍을 이용해 보자.
{% highlight cpp %}
template<int N>
struct Accumulate
{
	static int const value = N + Accumulate<N-1>::value;
};

template<>
struct Accumulate<0>
{
	static int const value = 0;
};
...
// main
std::cout << Accumulate<100>::value << std::endl;
{% endhighlight %}

위 Accumulate의 value는 컴파일러에 의해 컴파일타임에 값이 결정 되어진다. (위 템플릿 코드는 컴파일러에 의해 재귀호출 된다는 뜻이다.)

어떤가? 졸라 멋있지 않은가?

<br>  

# Swap 함수를 만들어보자.

컴파일 시점에 다룰 수 있는 요소들을 메타 데이터(metadata)라 부른다.
메타 데이터는 크게 type 데이터와 non-type 데이터로 구분된다.
(위 Accumulate 예제에서는 non-type을 다루었다.)

이번에는 type을 다루는 예제를 다루어 보겠다.

`std::vector<int> v = { 1,2,3,4,5 };` 이 있다. 첫번째 요소와 마지막 요소의 값을 swap 하려면 성능 좋은 표준 함수 `std::swap`를 이용하면 된다.
{% highlight cpp %}
template <class ForwardIterator1, class ForwardIterator2>
void SwapForSameType(ForwardIterator1 i1, ForwardIterator2 i2) {
	std::swap(*i1, *i2);
}
...
// main
std::vector<int> v1 = { 1,2,3,4,5 };
SwapForSameType(v1.begin(), v1.end()-1);
{% endhighlight %}

이번에는 `std::vector<int> v1 = { 1,2,3,4,5 };`의 첫번째 요소와 `std::vector<long> v2 = { 1,2,3,4,5 };`의 마지막 요소를 swap 하려면 어떻게 해야할까?
이번에는 타입이 다르기 때문에(int, long) std::swap은 사용할 수 없다. 
{% highlight cpp %}
template <class ForwardIterator1, class ForwardIterator2>
void SwapForDiffType(ForwardIterator1 i1, ForwardIterator2 i2) {
	typename std::iterator_traits<ForwardIterator1>::value_type tmp = *i1;
	*i1 = *i2;
	*i2 = tmp;
}
...
// main
std::vector<int> v1 = { 1,2,3,4,5 };
std::vector<long> v2 = { 1,2,3,4,5 };
SwapForDiffType(v1.begin(), v2.end()-1);
{% endhighlight %}

만약 우리가 통합 swap 라이브러리를 개발해야 한다면 이렇게 개발해야 될 것이다.
{% highlight cpp %}
template<typename T1, typename T2>
void Swap(bool isSameType, T1 t1, T2 t2)
{
    if(isSameType)
        SwapForSameType(t1, t2);
    else
        SwapForDiffType(t1, t2);
}
...
// main
std::vector<int> v1 = { 1,2,3,4,5 };
std::vector<long> v2 = { 1,2,3,4,5 };
Swap(false, v1.begin(), v2.end() - 1);
{% endhighlight %}

애석하게도 위 코드는 컴파일 에러가 발생한다. 왜냐하면.. 런타임에 if(isSameType)에 의해 무엇을 호출할지 결정되기 때문에, 컴파일러는 SwapForSameType, SwapForDiffType 둘다 인스턴스화 해야되기 때문이다.
(std::swap은 타입이 다르면 컴파일 에러가 발생한다.) 

만약에 std::swap이 타입이 다르다면 컴파일 에러를 발생시키지 않고 내부적으로 SwapForDiffType의 버전을 사용한다고 쳐도, 여전히 성능상 문제가 존재한다. 
왜냐하면 std::swap을 1000000번 호출한다고 해보자. 그때마다 if(isSameType)을 평가해야 하기 때문이다. 

정말 통합 Swap 함수는 만들수가 없는걸까? 

여기서 문제는, 사용하지도 않을 SwapForSameType을 인스턴스화 하려고 하기 때문이다.  
그렇다면 필요한것만 인스턴스로 만들면 되는거잖아? 그럴려면 어떻게 해야할까? 

정답은 런타임에 if(isSameType)에 무엇을 호출할지 결정하는 것이 아니라, 컴파일 타임에 미리 결정해 버리는 것이다.
{% highlight cpp %}
namespace std 
{
	template <bool use_swap> struct my_iter_swap_impl;

	template <>
	struct my_iter_swap_impl<true>
	{
		template <class ForwardIterator1, class ForwardIterator2>
		static void do_it(ForwardIterator1 i1, ForwardIterator2 i2)
		{
			// i1과 i2가 같은 타입이고, 레퍼런스 타입인 경우에는 속도가 빠른 std::swap을 사용한다.
			std::cout << "my_iter_swap_impl<true>" << std::endl;
			std::swap(*i1, *i2);
		}
	};

	template <>
	struct my_iter_swap_impl<false>
	{
		template <class ForwardIterator1, class ForwardIterator2>
		static void do_it(ForwardIterator1 i1, ForwardIterator2 i2)
		{
			// std::swap이 불가능한 경우, 아래의 속도가 느린 swap을 사용한다.
			std::cout << "my_iter_swap_impl<false>" << std::endl;
			typename iterator_traits<ForwardIterator1>::value_type tmp = *i1;
			*i1 = *i2;
			*i2 = tmp;
		}
	};

	template <class ForwardIterator1, class ForwardIterator2>
	void my_iter_swap(ForwardIterator1 i1, ForwardIterator2 i2)
	{
		typedef iterator_traits<ForwardIterator1> traits1;
		typedef typename traits1::value_type v1;
		typedef typename traits1::reference r1;

		typedef iterator_traits<ForwardIterator2> traits2;
		typedef typename traits2::value_type v2;
		typedef typename traits2::reference r2;

		bool const use_swap = std::is_same<v1, v2>::value
			&& std::is_reference<r1>::value
			&& std::is_reference<r2>::value;

		my_iter_swap_impl<use_swap>::do_it(i1, i2);
	}
}
...
// main
std::vector<int> v1 = { 1,2,3,4,5 };
std::vector<long> v2 = { 1,2,3,4,5 };
std::my_iter_swap(v1.begin(), v2.end()-1); 
{% endhighlight %}

위 코드에서 use_swap은 컴파일타임에 결정 된다!!! 아름답다 ㅠㅜ

<br>  

# 마무리
 
성능 최적화가 필요하다면, 런타임에 작동하는 코드를 컴파일 타임으로 옮겨 봅시다.