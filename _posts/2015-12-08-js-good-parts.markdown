---
layout: post
title: "javascript the good parts"
date: 2015-12-07 12:53:25
tags: cpp
---

더글라스 크락포드 - 자바스크립트 핵심

### Chapter2. 문법 

- 문자열을 변하지 않습니다(immutable). 일단 문자열이 한번 만들어지면, 이 문자열은 결코 변하지 않습니다. 하지만 여러 문자열을 + 연산자로 연결하여 새로운 문자열을 만들 수 있습니다. 분리된 문자열이 + 연산자로 연결되든 그냥 문자열 하나든 문자들의 순서가 같으면 같은 문자열 입니다. 그래서 다음의 예는 참(true) 입니다.
{% highlight javascript%}
'c' + 'a' + 't' === 'cat'
{% endhighlight %}

- 다른 언어들과 달리 자바스크립트에서 블록(`{`, `}`) 은 새로운 유효범위(scope)를 생성하지 않습니다. 이러한 이유로 변수는 블록 안에서가 아니라 함수의 첫 부분에서 정의해야 합니다.

- 다음은 거짓에 해당하는 값들입니다.
	- false
	- null
	- undefined
	- 빈 문자열 ''
	- 숫자 0
	- NaN 
	
- return 문은 함수에서 호출한 곳으로 되돌아가는 역할을 합니다. 또한 이 문장은 반환값을 지정합니다. 표현식이 지정되지 않으면 undefined를 반환 합니다.

- break 문은 반복문이나 switch 문에서 흐름을 벗어나게 하는 역할을 합니다. 이 문장은 라벨 이름을 취할 수 있는데 라벨이 주어지면 라벨이 붙은 문장의 끝으로 이동 합니다.

- typeof 연산자의 결과값에는 number, string, boolean, undefined, function, object 등이 있습니다. 피연산자가 배열이나 null이면 결과는 모두 object인데 이는 약간 문제가 있습니다.

<br>

### Chapter3.객체(Object)

- 자바스크립트에서 단순한(primitives) 데이터 타입은 number, string, boolean, null, undefined가 있습니다. 이들을 제외한 다른 값들은 모두 객체 입니다. number, string, boolean은 메소드가 있기 때문에 유사 객체라고 할 수 있습니다. 하지만 이들은 값이 한번 정해지면 변경할 수가 없습니다(immutable). 자바스크립트의 객체는 변형 가능한 속성들의 집할이라고 할수 있습니다. 

- 자바스크립트에는 객체 하나에 있는 속성들을 다른 객체에 상속하게 해주는 프로토타입 연결 특성이 있습니다. 이 특성을 활용하면, 객체를 초기화하는 시간과 메모리 사용을 줄일 수 있습니다.

- 객체 리터럴은 새로운 객체를 생성할 때 매우 편리한 표기법을 제공 합니다.
{% highlight javascript%}
var stooge = {
	"first-name":"Jerome",
	last_name:"Howard"
}
{% endhighlight %}

- `||` 연산자를 사용하여 다음과 같이 기본값을 지정할 수 있습니다.
{% highlight javascript%}
var middle_name = obj["middle-name"] || "(none)";
obj["middle-name"] = obj["middle-name"] || "(none)"; // 값이 있으면 유지하고, 없으면 넣는다. 
{% endhighlight %}

- 존재하지 않는 속성, 즉 undefined의 속성을 참조하려 할 때 TypeError 예외가 발생합니다. 이런 상황을 방지하기 위해서 다음과 같이 `&&` 연산자를 사용할 수 있습니다.
{% highlight javascript%}
flight.equipment // undefined
flight.equipment.model // throw TypeError
flight.equipment && flight.equipment.model // undefined 
{% endhighlight %}

- 객체는 참조 방식으로 전달됩니다. 결코 복사되지 않습니다.
{% highlight javascript%}
var f = function(o)
{
  o.data = 10;
}
var obj = {data:20};
console.log(obj.data); // 20
f(obj);
console.log(obj.data); // 10
{% endhighlight %}

- 모든 객체는 속성을 상속하는 프로토타입 객체에 연결돼 있습니다. 객체 리터럴로 생성되는 모든 객체는 자바스크립트의 표준 객체인 Object의 속성인 prototype(Object.prototype) 객체에 연결됩니다.

- 이제 Object 객체에 create 라는 메소드를 추가하고, 다음과 같이 상속을 할수 있습니다.
{% highlight javascript%}
Object.create = function (o) {
	var F = function () {};
	F.prototype = o;
	return new F();
};
var another_stooge = Object.create(stooge);
{% endhighlight %}

- 프로토타입 연결은 오로지 객체의 속성을 읽을 때만 사용합니다. 객체에 있는 특정 속성의 값을 읽으려고 하는데 해당 속성이 객체에 없는 경우 자바스크립트는 이 속성을 프로토타입 객체에서 찾으려고 합니다. 이러한 시도는 프로토타입 체인의 가장 마지막에 있는 Object.protoytype 까지 계속해서 이어집니다. 만약 찾으려는 속성이 프로토타입 체인 어디에도 존재하지 않는 경우 undefinede를 반환합니다. 이러한 일련의 내부 동작을 위임(delegation)이라고 합니다. 

- 프로토타입 관계는 동적 관계입니다. 만약 프로토타입에 새로운 속성이 추가되면 해당 프로토타입을 근간으로 하는 객체들에는 즉각적으로 이 속성이 나타납니다.
{% highlight javascript%}
var stooge = { };
var another_stooge = Object.create(stooge);
stooge.data = 10;
console.log(another_stooge.data); // 10
{% endhighlight %}

- typeof는 해당 객체의 속성이 아니라 프로토타입 체인 상에 있는 속성을 반환 할수 있기 때문에 주의할 필요가 있습니다. hasOwnProperty 메소드는 프로토타입 체인을 바라보지 않습니다.
{% highlight javascript%}
var stooge = { data:10 };
var another_stooge = Object.create(stooge);
console.log(typeof another_stooge.data); // number
console.log(another_stooge.hasOwnProperty('data')); // false
console.log(stooge.hasOwnProperty('data')); // true
{% endhighlight %}

- delete 연산자를 사용하면 객체의 속성을 삭제할 수 있습니다. delete 연산자는 해당 속성이 객체에 있을 경우에 삭제를 하며 프로토타입 연결 상에 있는 객체들은 접근하지 않습니다.
{% highlight javascript%}
var stooge = { data:10 };
var F = function(){this.data = 20;};
F.prototype = stooge;
var another_stooge = new F();

console.log(stooge.data); // 10
console.log(another_stooge.data); // 20
delete another_stooge.data;
console.log(another_stooge.data); // 10 (프로토타입 체인에 의해 stooge의 data에 접근) 
{% endhighlight %}

- 전역변수 사용을 최소화 하는 방법 한 가지는 애플리케이션에서 전역변수 사용을 위해 다음과 같이 전역변수 하나를 만드는 것입니다.
{% highlight javascript%}
var MYAPP = {};
MyApp.stooge = {
	first_name = "joe",
	last_name = "howard"
};
{% endhighlight %}

<br>

### Chapter4.함수(function)

- 객체는 프로토타입 객체로 숨겨진 연결을 갖는 이름/값 쌍들의 집합체 입니다. 

- 객체 중에서 객체 리터럴로 생성되는 객체는 Object.prototype에 연결됩니다. 반면에 함수 객체는 Function.prototype에 연결됩니다. (Function은 Object.prototype에 연결됩니다.)

- 또한 모든 함수는 숨겨져 있는 두개의 추가적인 속성이 있는데, 이 속성들은 함수의 문맥(context)과 함수의 행위를 구현하는 코드(code) 입니다.

- 또한 모든 함수 객체는 prototype이라는 속성이 있습니다. 이 속성의 값은 함수 자체를 값으로 갖는 constructor라는 속성이 있는 객체 입니다. 이는 Function.prototype으로 숨겨진 연결과는 구분 됩니다.

- 함수 객체는 함수 리터널로 생성할 수 있습니다. 함수 리터럴로 생성한 함수 객체는 외부 문맥으로의 연결이 있는데 이를 클로저(closure)라고 합니다. 클로저는 강력한 표현력의 근원입니다.

- 함수 호출시 모든 함수는 명시되어 있는 매개변수에 더해서 this와 arguments라는 추가적인 매개변수 두개를 받게 됩니다.

- 설계상의 문제로 arguments는 실제 배열은 아닙니다. arguments는 배열 같은 객체 입니다. 왜냐하면 length라는 속성이 있지만, 모든 배열이 가지는 메소드들은 없습니다.

<br>

### Chapter4-3.함수 호출 패턴

- 함수를 호출하는데 4가지 패턴이 있고, 각각의 패턴에 따라 this라는 추가적인 매개변수를 다르게 초기화 합니다.

- 메소드 호출 패턴 : 함수를 객체의 속성에 저장하는 경우 이 함수를 메소드라고 부릅니다. 메소드를 호출할 때 this는 메소드를 포함하고 있는 객체에 바인딩 됩니다. this와 객체의 바인딩은 호출시에 일어납니다.
{% highlight javascript%}
var myObject = {
	value : 0,
	increment : function(inc){
		this.value += inc;	
	}
}
myObject.increment(10);
myObject.increment(10);
console.log(myObject.value); // 20
{% endhighlight %}

- 함수 호출 패턴 : 함수를 이 패턴으로 호출할 때 this는 전역객체에 바인딩 됩니다. 이런 특성은 언어설계 단계에서의 실수 입니다. 만약 언어를 바르게 설계했다면, 내부 함수를 호출할 때 이 함수의 this는 외부 함수의 this 변수에 바인딩 되어야 합니다.
{% highlight javascript%}
var myObject = {
    value: 1,
    getValue: function(){
      return this.value;
    }
};
myObject.double = function () {
    var that = this;
    var double = function () {
		  console.log(this.value); // undefined
    	console.log(that.value); // 1
        that.value = that.value + that.value;
    };
    double();
};
myObject.double();
console.log(myObject.getValue()); // 2
{% endhighlight %}

- 생성자 호출 패턴 : 자바스크립트는 프로토타입에 의해서 상속이 이루어지는 언어 입니다. 하지만 클래스 기반의 언어들을 생각나게 하는 객체 생성 문법을 제공합니다. 함수를 new 라는 전치 연산자와 함께 호출하면, 호출한 함수의 prototype 속성의 값에 연결되는 
(숨겨진) 링크를 갖는 객체가 생성되고, 이 새로운 객체는 this에 바인딩 됩니다. 일반적으로 생성자는 이니셜을 대문자로 표기하여 이름을 지정합니다.
{% highlight javascript%}
var Quo = function (data){
  this.data = data;
}
Quo.prototype.get_data = function(){
  return this.data;
}
Quo.get_data2 = function(){
  return this.data;
}
var quo = new Quo(123);
console.log(quo.get_data()); // 123
console.log(quo.get_data2()); // TypeError : get_data2 is not a function 
{% endhighlight %}

- apply 호출 패턴 : 자바스크립트는 함수형 객체지향 언어이기 때문에, 함수는 메소드를 가질 수 있습니다. (함수의) apply 메소드는 함수를 호출할 때 사용할 인수들의 배열을 받아들입니다. 또한 이 메소드는 this의 값을 선택할 수 있도록 해줍니다.
{% highlight javascript%}
var myObject = {
    value: 1,
    getValue: function(){
      return this.value;
    }
};
myObject.double = function () {
    var double = function () {
		console.log(this.value); // 1
        this.value = this.value + this.value;
    };
    double.apply(this);
};
myObject.double();
console.log(myObject.getValue()); // 2
{% endhighlight %}

<br>

### Chapter4-7.기본 타입에 기능 추가

