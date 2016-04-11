var eventuality = function (that) {
    var registry = {};
    that.fire = function (event) {
		var array, func, handler, i;
		var	type = typeof event === 'string' ? event : event.type;
        if (registry.hasOwnProperty(type)) {
            array = registry[type];
            for (i = 0; i < array.length; i += 1) {
                handler = array[i];
                func = handler.method;
                if (typeof func === 'string') {
                    func = this[func];
                }
                func.apply(this, handler.parameters || [event]);
                //func.call(this, [handler.parameters] || [event]);
            }
        }
        return this;
    };
    that.on = function (type, method, parameters) {
        var handler = {
            method: method,
            parameters: parameters
        };
        if (registry.hasOwnProperty(type)) {
            registry[type].push(handler);
        } else {
            registry[type] = [handler];
        }
        return this;
    };
    return that;
};

var clickFunction = function(event){
	console.log('clicked ' + event.data);
};

var button = {};
eventuality(button);
button.on('clicked', clickFunction);
button.fire({type:'clicked', data:'A'});
button.fire({type:'clicked', data:'B'});
button.fire({type:'clicked', data:'C'});

var button2 = {};
eventuality(button2);
button2.on('clicked', clickFunction, [{data:'A1'},{data:'A2'}]);
button2.on('clicked', clickFunction, [{data:'B'}]);
button2.on('clicked', clickFunction, [{data:'C'}]);
button2.fire({type:'clicked'});