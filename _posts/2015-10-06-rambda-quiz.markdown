---
layout: post
title: "rambda quiz : currying, compose"
date: 2015-10-06 12:53:25
tags: rambda.js currying compose
---

누군가가 currying, compose에 대한 [문제]((http://jsbin.com/jevag))를 냈다. rambda.js를 이용해서 풀면 된다.

Curried functions are easy to compose. Using _.map, _.size, and _.split we can make a function that returns the lengths of the words in a string.

#### Example

{% highlight javascript%}
var _ = R;
var lengths = _.compose(
  _.map(_.size), _.split(' ')
);
console.log(lengths('once upon a time'));
console.log(_.split(' ', 'once upon a time'));
{% endhighlight %}

#### Background Code

{% highlight javascript%}
var get = _.curry(function(x, obj) { return obj[x]; });
function assertEqualArrays(x,y) {
  if(x.length !== y.length)
    throw("expected "+x+" to equal "+y);
  for(var i in x) {
    if(x[i] !== y[i]) {
      throw("expected "+x+" to equal "+y);
    }
  }
}
function assertEqual(x,y){
  if(x !== y)
    throw("expected "+x+" to equal "+y);
}
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
