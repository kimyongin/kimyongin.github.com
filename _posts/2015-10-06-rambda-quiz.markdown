---
layout: post
title: "rambda quiz : currying, compose (작성중)"
date: 2015-10-06 12:53:25
tags: rambda.js currying compose
---

누군가가 currying, compose에 대한 [문제](http://jsbin.com/cisiju)를 냈다. <br>
rambda.js에 대해서 먼저 알아보자. http://jsbin.com/cisiju/237/

#### [Documentation](http://ramdajs.com/0.18.0/docs)

- inc
  - Number -> Number
  - Increments its argument.
  - `R.inc(42); //=> 43`
  
> Number를 넣으면 Number가 나온다.

- add
  - Number -> Number -> Number
  - Adds two numbers. Equivalent to a + b **but curried**.
  - `R.add(2, 3); //=>  5`
  - `R.add(7)(10); //=> 17`
  
> Number를 넣으면 Number를 넣었을때 Number가 나오는 함수를 반환한다. <br>
> 다르게 표현하면 Number -> (Number -> Number) 로 표현할수도 있겠지?

- all
{% highlight javascript%}
var lessThan2 = R.flip(R.lt)(2);
var lessThan3 = R.flip(R.lt)(3);
R.all(lessThan2)([1, 2]); //=> false
R.all(lessThan3)([1, 2]); //=> true
{% endhighlight %}



#### Data

{% highlight javascript%}
var articles = [
  {
    title: 'Everything Sucks',
    url: 'http://do.wn/sucks.html',
    author: {
      name: 'Debbie Downer',
      email: 'debbie@do.wn'
    }
  },
  {
    title: 'If You Please',
    url: 'http://www.geocities.com/milq',
    author: {
      name: 'Caspar Milquetoast',
      email: 'hello@me.com'
    }
  }
];
{% endhighlight %}

<br> 

***************************************************
***************************************************

#### Challenge 1

Return a list of the author names in articles using only get, _.compose, and _.map.

{% highlight javascript%}
var names = _.identity; // change this
assertEqualArrays(
  ['Debbie Downer', 'Caspar Milquetoast'],
  names(articles)
);
{% endhighlight %}

<br> 

***************************************************
***************************************************

#### Challenge 2

Make a boolean function that says whether a given person wrote any of the articles. Use the names function you wrote above with _.compose and _.contains.

{% highlight javascript%}
var isAuthor = _.identity; // change this
assertEqual(
  false,
  isAuthor('New Guy', articles)
);
assertEqual(
  true,
  isAuthor('Debbie Downer', articles)
);
{% endhighlight %}

<br> 

***************************************************
***************************************************

#### Challenge 3
There is more to point-free programming than compose! Let's build ourselves another function that combines functions to let us write code without glue variables.

{% highlight javascript%}
var fork = _.curry(function(lastly, f, g, x) {
  return lastly(f(x), g(x));
});
{% endhighlight %}

<br> 

***************************************************
***************************************************

#### Challenge 4
As you can see, the fork function is a pipeline like compose, except it duplicates its value, sends it to two functions, then sends the results to a combining function.

Your challenge: implement a function to compute the average values in a list using only fork, _.divide, _.sum, and _.size.

{% highlight javascript%}
var avg = _.identity; // change this
assertEqual(3, avg([1,2,3,4,5]));
{% endhighlight %}
