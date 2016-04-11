
Object.create = function(o){
	var F = function(){};
	F.prototype = o;
	return new F();
};

// ------------------------------------------------------------
// Mammal (Parent)
// ------------------------------------------------------------
var myMammal = {
	name : 'Herb the Mammal',
	get_name : function () {
		return this.name;
	},
	says : function () {
		return this.saying || 'silent...';
	}
};

// ------------------------------------------------------------
// Cat (Child)
// ------------------------------------------------------------
var myCat = Object.create(myMammal);
myCat.name = 'Henrietta';
myCat.saying = 'meow';
myCat.purr = function (n) {
	return "purrr...";
};
myCat.get_name = function () {
	return this.says() + ' ' + this.name + ' ' + this.says();
};

// ------------------------------------------------------------
// Example
// ------------------------------------------------------------
var says = myMammal.says();
console.log(says);
//var purr = myMammal.purr(5); // undefined
//console.log(purr);
var name = myMammal.get_name();
console.log(name);

var says = myCat.says();
console.log(says);
var purr = myCat.purr(5);
console.log(purr);
var name = myCat.get_name();
console.log(name);
