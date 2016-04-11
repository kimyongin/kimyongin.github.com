// -----------------------------------------------------------------
// 오브젝트에 재귀 메소드 추가하는 방법
// -----------------------------------------------------------------
// step.01 : 메소드로 추가할 함수 준비 
// -----------------------------------------------------------------
function chrip(n) {
    return n > 1 ? chrip(n - 1) + "-chrip" : "chrip";
}
console.log(chrip(5));

// -----------------------------------------------------------------
// step.02 : 문제 있는 버전
// -----------------------------------------------------------------
var ninja = {
    chrip: function (n) {
        return n > 1 ? ninja.chrip(n - 1) + "-chrip" : "chrip";
    }
}
console.log(ninja.chrip(3));
var samurai = { chrip: ninja.chrip };
ninja = {};
//console.log(samurai.chrip(3)); TypeError: ninja.chrip is not a function 

// -----------------------------------------------------------------
// step.03 : 문제 있는 버전
// -----------------------------------------------------------------
var ninja = {
    chrip: function (n) {
        return n > 1 ? this.chrip(n - 1) + "-chrip" : "chrip";
    }
}
console.log(ninja.chrip(3));
var samurai = { mychrip: ninja.chrip };
ninja = {};
//console.log(samurai.mychrip(3)); TypeError: this.chrip is not a function 

// -----------------------------------------------------------------
// step.final : 최종
// -----------------------------------------------------------------
var ninja = {
    chrip: function signal (n) {
        // inline function
        return n > 1 ? signal(n - 1) + "-chrip" : "chrip";
    }
}
console.log(ninja.chrip(3));
var samurai = { mychrip: ninja.chrip };
ninja = {};
console.log(samurai.mychrip(3)); 
// ninja.signal(3); TypeError: ninja.signal is not a function  

