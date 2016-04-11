
Function.prototype.method = function(name, func){
	if(!this.prototype[name]){
		this.prototype[name] = func;
	}
	return this;
};

Function.method('inherit', function(Parent){
	this.prototype = new Parent();
	return this;	
});

// ------------------------------------------------------------
// Mammal (Parent)
// ------------------------------------------------------------
var Mammal = function (name) {
	this.name = name;
	this.get_name = function () {
		return this.name;
	};
	this.says = function () {
		return this.saying || 'silent...';
	};
};

// ------------------------------------------------------------
// Cat (Child)
// ------------------------------------------------------------
var Cat = function (name) {
	this.name = name;
	this.saying = 'meow';	
}
.inherit(Mammal)
.method('purr', function (n) {
		return "purrr...";
})
.method('get_name', function () {
		return this.says() + ' ' + this.name + ' ' + this.says();
});

// ------------------------------------------------------------
// Example
// ------------------------------------------------------------
var myMammal = new Mammal('Herb the Mammal');
//var myMammal = new Mammal('Herb the Mammal');
var says = myMammal.says();
console.log(says);
//var purr = myMammal.purr(5); // undefined
//console.log(purr);
var name = myMammal.get_name();
console.log(name);

var myCat = new Cat('Henrietta');
//var myCat = new Cat('Henrietta');
var says = myCat.says();
console.log(says);
var purr = myCat.purr(5);
console.log(purr);
var name = myCat.get_name();
console.log(name);
