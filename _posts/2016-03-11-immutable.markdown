---
layout: post
title: "immutable.js"
date: 2016-03-11 09:53:25
tags: immutable
---

### Immutable collections for JavaScript

Immutable data는 ...
변하지 않는다. 
개발을 심플하게 해준다. 
방어적인 복사를 하지 않는다.
더 좋은 memoization을 할 수 있게 해준다.
간단한 로직으로 변경을 확인 할 수 있게 해준다.

Persistent data는 ..
변경 API를 제공하는데, 이는 내부 data를 변경하지 않고 항상 새로운 변경된 data를 건내준다.  

Immutable.js는 아래의 Persistent Immutable data structure를 제공해준다.
List, Stack, Map, OrderedMap, Set, OrderedSet, Record

위의 data structure 들은
Clojure, Scala 덕분에 대중화된 hash maps tries, vector tries 를 이용해서 자료구조를 공유하고
복사나 임시 data를 최소화 함으로써, modern javascript VM에서 에서 매우 효율적으로 동작한다.

Immutable.js은 또한 Lazy Seq 를 제공한다.
중간 대리자가 필요없이 map 이나 filter 같은 collection method를 바로 chaining 을 할수 있으며,
Range나 Repeat을 이용해서 Seq를 생성할수 있게 해준다.

### The case for Immutability

개발할때 가장 어려운점은 변경(mutation)을 추적하고, 상태를 유지하는 것이다.
immutable data를 사용해서 개발을 하게 되면, 우리는 data의 흐름에 대한 생각을 바꿀수 있게 된다.

data event를 구독 하는것은 성능상에 큰 오버헤드를 발생시킨다.
때로는 아주 심각하기도 하며, 동기화(sync)를 안해서 개발자가 실수를 하기 쉽도록 한다.
이를 해결하기 위해 immutable data는 절대로 변하지 않는다.
model의 변경을 구독하는 시대는 이제 끝났으며, 위에서 말한 새로운 immutable data가 전달 될 것이다.

이러한 data 흐름을 갖는 model은 React와 Flux와 잘 어울린다.

변경을 구독하기 보다는 immutable data를 전달 하게 된다면, 
무언가 변경되더라도 오직 내가 할 일에만 집중하면 된다. 
그리고 변경이 되었는지 확인하고 싶다면 equality를 사용하면 된다.

Immutable collection들은 object가 아니라 value로 다뤄진다.
object는 시간이 흘러 변하는 것을 뜻한다면, value는 특정 시간의 상태를 뜻한다.
이것은 immutable data를 적절하게 사용하기 위한 가장 중요한 원리이다.

Immutable.js collection을 value로 다루기 위해서는 
object reference identity를 판단하는데 사용하는 === operator 가 아니라
Immutable.is() 와 Immutable.equal() method를 사용해서 value equality 를 판단해야 한다.

var map1 = Immutable.Map({a:1, b:2, c:3});
var map2 = map1.set('b', 2); // (1)
assert(map1.equals(map2) === true); 
var map3 = map1.set('b', 50);
assert(map1.equals(map3) === false);

Note: 성능 최적화 측면에서 Immutable은 작업 결과가 동일하다면(1) 기존의 collection을 반환한다.
따라서 무언가 변경된것이 없다면 === operator를 사용해서 reference equality 를 판단해도 된다.
이것은 memoization 을 사용할때 유용하다. 왜냐하면 momoization에서 deeper equality check(=value equality)를 하게 되면 많은 비용이 들기 때문이다.
또한 ===dms Immutable.is(), Immutable.equals() 에서 성능 최적화를 위해 내부적으로 사용된다.

만약. object가 불변(immutable)이라면, object를 통째로 복사하지 않고, 다른 reference로 매우 간단하게 복사 한다.
그렇게하는 이유는 reference가 object보다 훨씬 작기 때문이다. 이로인해 메모리를 절약하고 성능을 향상 시킬수 있게 된다.

var map1 = Immutable.Map({a:1, b:2, c:3});
var clone = map1;

### JavaScript-first API

immutable은 Clojure, Scala, Haskell 등의 funcional programminng 으로부터 영향을 받았지만,
Javascript를 위해 설계되었다. 그래서 ES6의 Array, Map, Set과 비슷한 Object Oriented API를 가지고 있다.

immutable collection의 다른점은 collection을 수정하는 push, set, slice, concat .. 같은 method들이다.
이것들은 새로운 array를 반환하는것이 아니라, 새로운 immutable collection을 반환한다. 

Array의 대부분의 method와 유사한 method를 Immutable.List에서도 찾을수 있다.
또한 Map역시 Immutable.Map에서 찾을수 있고, Set역시 Immutable.Set에서 찾을수 있다.
다시말하면 forEach(), map() 같은 method들이 Immutable collection에도 존재한다는 뜻이다.

### Accepts raw JavaScript objects.

기존의 Javascrip와 상호 동작하기 위해서, 
immutable은 Iterable이 기대되는 어떤 method 에든 성능저하 없이 Plain Javascript Array와 Object를 넘길수 있다.

var map1 = Immutable.Map({a:1, b:2, c:3, d:4});
var map2 = Immutable.Map({c:10, a:20, t:30});
var obj = {d:100, o:200, g:300};
var map3 = map1.merge(map2, obj);
// Map { a: 20, b: 2, c: 10, d: 100, t: 30, o: 200, g: 300 }

이것이 가능한 이유는 Javascript Array와 Object를 iterable로 다루기 때문이다.
iterable은 Javascript Object의 복잡한 collection method를 활용 할 수 있게 해준다. 그렇지 않으면 흩어져 있는 native API를 갖게 된다.
Seq는 lazily 하고 중간 결과를 캐시하지 않기 때문에, 이러한 작업들은 굉장히 효율적일수 있다.

var myObject = {a:1,b:2,c:3};
Immutable.Seq(myObject).map(x => x * x).toObject();
// { a: 1, b: 4, c: 9 }

기억 해야될 것은 Javascript Object를 Immutable Map 생성자로 사용되면,
Javascript Object의 Properties는 항상 string 이다. 심지어 quote를 사용하지 않았더라도 말이다.
하지만 Immutable Map은 어떤 type의 key도 허용한다.

var obj = { 1: "one" };
Object.keys(obj); // [ "1" ]
obj["1"]; // "one"
obj[1];   // "one"

var map = Immutable.fromJS(obj);
map.get("1"); // "one"
map.get(1);   // undefined !!!

Javascript Object의 Property 에 접근할때 key가 string으로 convert 된다.
하지만 Immutable Map key는 get에 어떤 type도 넣을수 있기 때문에 convert 되지 않는다.

### Converts back to raw JavaScript objects.

모든 Immutable Iterable는 Plain Javascript Array와 Object로 convert 될수 있다.
toArray()와 toObject()는 shallowly(얕게) convert 시키며, toJS()는 deeply(깊게) convert 시킨다.
모든 Immutable Iterable은 또한 toJSON을 구현하였으며, JSON.stringify에게 넘긴다.

var deep = Immutable.Map({ a: 1, b: 2, c: Immutable.List.of(3, 4, 5) });
deep.toObject() // { a: 1, b: 2, c: List [ 3, 4, 5 ] }
deep.toArray() // [ 1, 2, List [ 3, 4, 5 ] ]
deep.toJS() // { a: 1, b: 2, c: [ 3, 4, 5 ] }
JSON.stringify(deep) // '{"a":1,"b":2,"c":[3,4,5]}'

### Embraces ES6






































































 



  



 


 



