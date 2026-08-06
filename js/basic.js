// In JavaScript, a primitive (primitive value, primitive data type) is data 
// that is not an object and has no methods or properties.
// primitive data type are immutable (unchangeable)
// Javascript has exactly 7 primitive data types
const str = "hello";   // string
const num = 10;        // number
const bigInt = 1234567890123456789012345678901234567890n; // bigint
const bool = true;     // boolean
const undef = undefined; // undefined
const nul = null;      // null
const sym = Symbol("hello"); // symbol

console.log(typeof str, str);  // string hello
console.log(typeof num, num);  // number 10
console.log(typeof bigInt, bigInt); // bigint 1234567890123456789012345678901234567890n
console.log(typeof bool, bool); // boolean true
console.log(typeof undef, undef); // undefined undefined
console.log(typeof nul, nul); // object null
console.log(typeof sym, sym); // symbol Symbol(hello)


// 1. Immutable - Values Cannot Be Changed
// Once a primitive value is created, it cannot be altered. When you “change” a string, you’re actually creating a new string.
let name = "Mohit";
name.toLowerCase(); // Creates "Alice" but doesn't change name
console.log(name); // Mohit




// 2. Compared By Value
// When you compare two primitives, JavaScript compares their actual values, not where they’re stored in memory.
let a = "Mohit";
let b = "Mohit";
console.log(a == b); // true
console.log(a === b); // true  same value

let obj1 = {
    name: "Mohit"
}
let obj2 = {
    name: "Mohit"
}
console.log(obj1 == obj2); // false
console.log(obj1 === obj2); // false  different objects



// 3. No Methods (But Autoboxing Magic)
// Primitives don’t have methods, but JavaScript automatically wraps them in objects when you try to call methods. This is called “autoboxing.”
// 🔬 What Actually Happens Internally
// let temp = new String("hello");
// temp.toUpperCase();
// temp = null; // destroyed
let str1 = "hello";
str1.toUpperCase();   // toUpperCase() returns a new string 
console.log(str1); // still "hello"




//  String are imutable 
let str2 = "hello";
str2[0] = "m";
console.log(str2); // "hello"

str2 = 'M' + str2.slice(1);
console.log(str2); // "Mello"

console.log(typeof NaN);  // number
const value = "hello" * 2;
console.log(value); // NaN
// Don't do this
if (value === NaN) { console.log("value is NaN Never true") }  // Never true!

// Do this instead
if (Number.isNaN(value)) { console.log("value is NaN ES6") }  // ES6, recommended
if (isNaN(value)) { console.log("value is NaN Older") }         // Older, has quirks


