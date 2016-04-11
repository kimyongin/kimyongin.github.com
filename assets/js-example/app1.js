
Object.create = function(o){
	var F = function(){};
	F.prototype = o;
	return new F();
};

Function.prototype.new = function(){
	var that = Object.create(this.prototype);
	var other = this.apply(that, arguments);
	return (typeof other === 'object' && other) || that;
};

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
	this.purr = function (n) {
		return "purrr...";
	};
	this.get_name = function () {
		return this.says() + ' ' + this.name + ' ' + this.says();
	};
};
Cat.prototype = Mammal.new();
//Cat.prototype = new Mammal();

// ------------------------------------------------------------
// Example
// ------------------------------------------------------------
var myMammal = Mammal.new('Herb the Mammal');
//var myMammal = new Mammal('Herb the Mammal');
var says = myMammal.says();
console.log(says);
//var purr = myMammal.purr(5); // undefined
//console.log(purr);
var name = myMammal.get_name();
console.log(name);

var myCat = Cat.new('Henrietta');
//var myCat = new Cat('Henrietta');
var says = myCat.says();
console.log(says);
var purr = myCat.purr(5);
console.log(purr);
var name = myCat.get_name();
console.log(name);
