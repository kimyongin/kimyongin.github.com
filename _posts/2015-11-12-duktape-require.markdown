---
layout: post
title: "cpp에서 ducktape으로 javascript를 실행"
date: 2015-11-12 12:53:25
tags: cpp js ducktape
---

#### [duktape](http://duktape.org/index.html)
> Duktape is an embeddable Javascript engine, with a focus on portability and compact footprint. 
> Duktape is easy to integrate into a C/C++ project: add duktape.c, duktape.h, and duk_config.h to your build, and use the Duktape API to call Ecmascript functions from C code and vice versa.

<br>

#### - duktape을 왜 살펴보게 되었냐면..
[RabbitMQ](https://www.rabbitmq.com/getstarted.html) 클라이언트 모듈을 사용하고 싶은데, 튜토리얼에서 Python | Java | Ruby | PHP | C# | Javascript | Go 에 대해서만 안내하고 있더라.
검색해보면 [AMQP-CPP](https://github.com/CopernicaMarketingSoftware/AMQP-CPP)라고 나오긴 하는데, 얼마전에 알게된 Duktape이 생각나서 Javascript 사용해보면 어떨까? 해서 보게 되었다.

<br>

#### - duktape에서 require의 샘플을 만들어보자.

아래와 같이 2개의 js 파일이 있고, 이를 NodeJS로 실행시키면 아래와 같은 결과가 나온다.
 
{% highlight javascript%}
// fileA.js
exports.hello = function() { console.log("Hello from fileA!"); };

// fileB.js
var modfileA = require('./fileA');
modfileA.hello();
{% endhighlight %}

    > node.exe fileB.js
    Hello from fileA!

<br>
	
이번에는 duktape을 이용해서 만들어 보자. duktape으로 할 때는 js파일에 약간의 수정이 필요하다.

{% highlight javascript%}
// fileA.js
exports.hello = function() { print("Hello from fileA!"); };

// fileB.js
Duktape.modSearch = function (id) {
	print(id);
    if (id === 'foo') {
        return 'exports.hello = function() { print("Hello from foo!"); };';
    } else if (id === 'bar') {
        return 'exports.hello = function() { print("Hello from bar!"); };';
    } else {
		var res = readFile(id + '.js');
		if (typeof res === 'string') {
			return res;
		}
	}
    throw new Error('module not found: ' + id);
};

var modfoo = require('foo');
modfoo.hello();

var modfileA = require('fileA');
modfileA.hello();
{% endhighlight %}

{% highlight cpp%}
// hello.exe
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "duktape.h"

static int fileio_readfile(duk_context *ctx) {
	const char *filename = duk_to_string(ctx, 0);
	if (!filename) {
		goto error;
	}
	duk_push_string_file(ctx, filename);
	return 1;
error:
	return DUK_RET_ERROR;
}

void fileio_register(duk_context *ctx) {
	duk_push_global_object(ctx);
	duk_push_c_function(ctx, fileio_readfile, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "readFile");
	duk_pop(ctx);
}

int main(int argc, const char *argv[]) {
	duk_context *ctx = NULL;
	ctx = duk_create_heap_default();
	if (!ctx) {
		printf("Failed to create a Duktape heap.\n");
		exit(1);
	}
	fileio_register(ctx);
	if (duk_peval_file(ctx, "fileB.js") != 0) {
		printf("Error: %s\n", duk_safe_to_string(ctx, -1));
		goto finished;
	}
	duk_pop(ctx);  /* ignore result */
finished:
	duk_destroy_heap(ctx);
	exit(0);
}
{% endhighlight %}

    > hello.exe
	foo
	Hello from foo!
	fileA
	Hello from fileA!
	
<br>

#### - fileB.js에 있는 `Duktape.modSearch`를 보자

[How to use modules](http://wiki.duktape.org/HowtoModules.html)를 보면 아래와 같이 적혀있다.

> The module search function encapsulates all platform specific concerns, such as module search paths and file system access, related to finding a module matching a certain identifier:

아무튼 module search 부분을 직접 만들어줘야 한다. 

위 예제를 보면 `modSearch` 내부에서 `readFile`을 호출하고 있는데, 해당 함수는 내가 `hello.exe`에서 cpp로 구현한 `readFile` 함수를 호출해서 해당 플랫폼에 맞는 방식으로 파일을 읽어들여서 반환하게 된다.

<br>

#### - 샘플은 아래 링크에 가서 받으세요
 
[https://github.com/kimyongin/ducktape_example](https://github.com/kimyongin/ducktape_example)