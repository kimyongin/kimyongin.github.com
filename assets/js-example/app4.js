
Object.prototype.method = function(name, func){
	if(!this.prototype[name]){
		this.prototype[name] = func;
	}
	return this;
};

Object.method('superior', function(name){
	var that = this;
	var method = that[name];
	return function(){
		return method.apply(that, arguments);
	};
});

// ------------------------------------------------------------
// Mammal (Parent)
// ------------------------------------------------------------
var mammal = function (name) {
	var that = {};
	that.name = name;
	that.get_name = function () {
		return this.name;
	};
	that.says = function () {
		return this.saying || 'silent...';
	};
	return that;
};

// ------------------------------------------------------------
// Cat (Child)
// ------------------------------------------------------------
var cat = function (name) {
	var that = mammal(name);
	that.saying = 'meow';
	that.purr = function (n) {
		return "purrr...";
	};
	that.get_name = function () {
		return this.says() + ' ' + this.name + ' ' + this.says();
	};
	return that;
};

var coolcat = function(name) {
	var that = cat(name);
	var super_get_name = that.superior('get_name');
	that.get_name = function(){
		return 'cool cat ' + super_get_name(); 	
	};
	return that;
}

// ------------------------------------------------------------
// Example
// ------------------------------------------------------------
var myCat = cat('Henrietta');
//var myCat = new Cat('Henrietta');
var says = myCat.says();
console.log(says);
var purr = myCat.purr(5);
console.log(purr);
var name = myCat.get_name();
console.log(name);

var myCoolcat = coolcat('Jessie');
var name = myCoolcat.get_name();
console.log(name);