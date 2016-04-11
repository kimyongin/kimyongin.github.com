// -----------------------------------------------------------------
// 소수 구하기
// -----------------------------------------------------------------
// step.01 : 기본 함수 준비 
// -----------------------------------------------------------------
console.log("Step.01 --------------------");
function isPrime(value){
    var prime = value != 1;
    for(var i = 2; i < value && prime != false; i++){
        if(value % i == 0) {
            prime = false;
        }
    }
    return prime;
}

// -----------------------------------------------------------------
// step.02 : 메모이제이션 적용
// -----------------------------------------------------------------
console.log("Step.02 --------------------");
function isPrimeEx(value){
    isPrimeEx.cache = isPrimeEx.cache || {};
    if (isPrimeEx.cache[value] != null){
        console.log('cached');
        return isPrimeEx.cache[value];
    } 
    
    var prime = value != 1;
    for(var i = 2; i < value && prime != false; i++){
        if(value % i == 0) {
            prime = false;
        }
    }
    return isPrimeEx.cache[value] = prime;
}
console.log(isPrimeEx(5));
console.log(isPrimeEx(5));
console.log(isPrimeEx(5));

// -----------------------------------------------------------------
// step.03 : 기본 함수를 수정하지 않고, 메모이제이션 적용 
// -----------------------------------------------------------------
console.log("Step.03 --------------------");
Function.prototype.cache = function(key){
    this._values = this._values || {};
    if (this._values[key] !== undefined){
        console.log('cached');
        return this._values[key];
    }else{
        return this._values[key] = this.apply(this, arguments);
    }
}
console.log(isPrime.cache(7));
console.log(isPrime.cache(7));
console.log(isPrime.cache(7));

// -----------------------------------------------------------------
// step.04 : 기본 함수를 수정하지 않고, 클로저를 이용한 메모이제이션 적용 
// -----------------------------------------------------------------
console.log("Step.04 --------------------");
Function.prototype.setCache = function(){
    var fn = this;
    return function(){
        return fn.cache.apply(fn, arguments);  
    };
}
var isPrime2 = isPrime.setCache();
console.log(isPrime2(11));
console.log(isPrime2(11));
console.log(isPrime2(11));
