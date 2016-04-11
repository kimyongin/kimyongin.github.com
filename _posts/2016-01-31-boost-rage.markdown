---
layout: post
title: "boost range"
date: 2016-01-31 09:53:25
tags: cpp boost
---

{% highlight cpp %}
#include <boost/range/adaptor/adjacent_filtered.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/assign.hpp>
#include <functional>
#include <iostream>
#include <vector>

int main(int argc, const char* argv[])
{
	using namespace boost::assign; 
	using namespace boost::adaptors;

	std::vector<int> input; 
	input += 1, 1, 2, 2, 2, 3, 4, 5, 6;

	boost::copy(
		input | adjacent_filtered(std::not_equal_to<int>()),
		std::ostream_iterator<int>(std::cout, ","));

	return 0;
}
{% endhighlight %}

1. input += 1;

std::vector<T>에 += 연산자는 존재하지 않는다.
하지만 위 코드에서는 +=를 사용하고 있다. 어떻게 된걸까?
디버깅해보면 boost/assign/std/vector.hpp의 아래코드가 호출되는것을 확인할수 있다.

{% highlight cpp %}
namespace boost { 
  namespace assign {
    template< class V, class A, class V2 >
    inline list_inserter< assign_detail::call_push_back< std::vector<V,A> >, V > 
    operator+=( std::vector<V,A>& c, V2 v )
    {
        return push_back( c )( v );  // v = 1
    }
  }
}
{% endhighlight %}

어떻게 저곳의 += 연산자를 호출하게 되는걸까? 그 이유는 using문으로 namespace를 알려줬기 때문이다.
함수를 찾는 규칙에 대해서 좀더 깊게 이해하기 위해 ADL에 대해서 알아보자.

{% highlight cpp %}
namespace A {
	struct X {};
	void f(int) { std::cout << "Call A::f(int)" << std::endl; }
	void g(X) { std::cout << "Call A::g(X)" << std::endl; }
	void h() { std::cout << "Call A::h()" << std::endl; }
}

int main()
{
	A::X x1;
	f(1); // compile error : can not find f
	g(x1); // ok
	h(); // compile error : can not find h
}
{% endhighlight %}

f(1)과 h()는 컴파일 에러가 발생한다. 왜냐하면 namespace를 적어주지 않았기 때문이다.
그런데 g(x1)은 컴파일이 성공한다. g(x1)은 어떻게 A::g를 찾아서 컴파일 한걸까? 
그 이유는 매개변수로 사용된 x1의 namespace가 A라는것을 알고있기 때문에 규칙에 따라 namespace A에서 검색을 하여 찾아낸 것이다.. 
그리고 이렇게 매개변수를 이용해서 룩업하는 규칙을 Argument Dependent Lookup (ADL) 이라고 부른다.

ADL은 기존의 클래스를 수정하지 않고 기능을 확장하기 위해 많이 사용된다.
위 코드에서도, std::vector<T>에는 += 연산자가 존재하지 않지만, ADL을 이용해서 기능을 추가하고 있음을 알수 있다.

2. return push_back( c )( v ); 

+= 연산자안에서 호출하는 push_back 이다. 

{% highlight cpp %}
namespace boost{
  namespace assign{
    template< class C >
    inline list_inserter< assign_detail::call_push_back<C>, 
                          BOOST_DEDUCED_TYPENAME C::value_type >
    push_back( C& c )
    {
      static BOOST_DEDUCED_TYPENAME C::value_type* p = 0;
      return make_list_inserter( assign_detail::call_push_back<C>( c ), p );
    }
  }
}
{% endhighlight %}

assign_detail::call_push_back<C>( c ) 를 호출하여 아래 템플릿 클래스의 오브젝트를 생성하고 있다.
call_push_back 오브젝트는 멤버로 생성자 매개변수로 넘어온 vector 오브젝트를 참조로 가지고 있으며, 
operator()(T r)을 호출하여 vector에 요소를 추가할수 있도록 만들어져있다. 여기서 operator()를 재정의했다는것이 중요하다.

{% highlight cpp %}
namespace boost{
  namespace assign_detail{
    template< class C >
    class call_push_back
    {
        C& c_;
    public:
        call_push_back( C& c ) : c_( c )
        { }
        
        template< class T >
        void operator()( T r ) 
        {
            c_.push_back( r );
        }
    };
  }
}
{% endhighlight %}

이제 위에서 만들어진 call_push_back 오브젝트를 make_list_inserter 함수의 매개변수로 넘기는데,
그 안에서 list_inserter 템플릿 클래스를 오브젝트화 하고 있다. 
한가지 주의깊게 봐야할점은 첫번째 파라미터의 템플릿 파라미터 이름이 Function 이라는 것이다.
좀더 정확하게 말하면 Function Object라고 부르는게 맞다. 왜냐하면 call_push_back 은 operator()를 재정의 했기 때문에
오브젝트이지만 함수처럼 사용되기 때문이다.

{% highlight cpp %}
namespace boost{
  namespace assign_detail{
    template< class Function, class Argument >
    inline list_inserter<Function,Argument>
    make_list_inserter( Function fun, Argument* )
    {
        return list_inserter<Function,Argument>( fun );
    }
  }
}
{% endhighlight %}

아래는 list_inserter 템플릿 클래스 이다. 보다시피 생성자로 넘어온 call_push_back 인스턴스를 insert_라는 맴버변수로 가지고 있는다.

{% highlight cpp %}
namespace boost{
  namespace assign_detail{
    template< class Function, class Argument = assign_detail::forward_n_arguments >   
    class list_inserter
    {
    public:
        list_inserter( Function fun ) : insert_( fun )
        {}
        // ... (생략)
        list_inserter& operator()()
        {
            insert_( Argument() );
            return *this;
        }
        template< class T >
        list_inserter& operator,( const T& r )
        {
            insert_( r  );
            return *this;
        }
    private:
        list_inserter& operator=( const list_inserter& );
        Function insert_;
    };
  }
}
{% endhighlight %}

이제 push_back( c ) 에서 반환한 list_inserter 오브젝트의 operator()를 호출하면서 v = 1를 넘기고,
function object(call_push_back 오브젝트)의 operator()를 호출하여 가지고 있는 vector 오브젝트에 요소를 추가하게 된다.
그리고 마지막으로 자기자신의 참조를 반환한다. 

input += 1이 반환하는 것은 list_inserter 오브젝트의 참조다. 그리고 list_inserter에는 정의되어있는 operator,()를 호출하여
다시한번 function object(call_push_back 오브젝트)의 operator()를 호출하여 가지고 있는 vector 오브젝트에 요소를 추가하게 된다.
그리고 마지막으로 자기자신의 참조를 반환한다. 

결국 1, 2, 2, 2, 3, 4, 5, 6 계속 반복하여 vector에 요소를 추가하게 된다.

3. input | adjacent_filtered(std::not_equal_to<int>())

adjacent_filtered는 boost/range/adaptor/adjacent_filtered.hpp 에 다음과 같이 정의되어 있다.

{% highlight cpp %}
namespace adaptors{
  const range_detail::forwarder<range_detail::adjacent_holder>
    adjacent_filtered =
      range_detail::forwarder<range_detail::adjacent_holder>();
}
{% endhighlight %}

따라서 adjacent_filtered(std::not_equal_to<int>())는 range_detail::forwarder<range_detail::adjacent_holder> 오브젝트의 operator()를 호출하게 된다.

namespace boost{
  namespace range_detail{  
    template< class T >
    struct holder
    {
      T val;
      holder( T t ) : val(t)
      { }
    };
    
    template< class T >
    struct adjacent_holder : holder<T>
    {
      adjacent_holder( T r ) : holder<T>(r)
      { }
    };
    
    template< template<class> class Holder >
    struct forwarder
    {
      template< class T >
      Holder<T> operator()( T t ) const
      {
        return Holder<T>(t);
      }
    };
  }
}

여기에서는 forwarder의 operator()에 std::not_eqal_to<int> function object를 넘기고 있으며, 그것을 adjacent_holder 오브젝트에 담아서 반환해준다.

template<class _Ty = void>
struct not_equal_to
{	
  // functor for operator!=
  typedef _Ty first_argument_type;
  typedef _Ty second_argument_type;
  typedef bool result_type;

  _CONST_FUN bool operator()(const _Ty& _Left, const _Ty& _Right) const
  {	
    // apply operator!= to operands
    return (_Left != _Right);
  }
};