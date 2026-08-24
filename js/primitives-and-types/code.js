// ============================================================
// PRIMITIVES AND TYPES — runnable walkthrough
// Run from repo root:  node js/primitives-and-types/code.js
// Every console.log below has its real output as a trailing comment.
// ============================================================

// ------------------------------------------------------------
// SECTION 1 — What is a primitive? The 7 primitive types
// ------------------------------------------------------------
// In JavaScript, a primitive (primitive value, primitive data type) is data
// that is not an object and has no methods or properties of its own.
// Primitive data types are immutable (unchangeable).
// JavaScript has exactly 7 primitive data types:
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
console.log(typeof nul, nul); // object null   <-- the famous bug/quirk
console.log(typeof sym, sym); // symbol Symbol(hello)

// typeof returns a STRING describing the type — so `typeof x === "string"` etc.
console.log(typeof typeof str); // string

// ------------------------------------------------------------
// SECTION 2 — Primitives are immutable (values cannot be changed)
// ------------------------------------------------------------
// Once a primitive value is created, it cannot be altered. When you "change"
// a string, you're actually creating a new string and pointing the variable at it.
let name = "Mohit";
name.toLowerCase(); // Creates a NEW string "mohit" but doesn't change name
console.log(name); // Mohit

// Strings are immutable: index assignment silently fails (non-strict mode)
let str2 = "hello";
str2[0] = "m";        // ignored — no error thrown in sloppy mode!
console.log(str2); // hello

str2 = 'M' + str2.slice(1); // build a NEW string instead
console.log(str2); // Mello

// Numbers are immutable too: Math methods RETURN new values,
// they never mutate the original (unlike Array.prototype.sort on arrays).
const n = 5;
console.log(n.toFixed(2), n); // 5.00 5

// ------------------------------------------------------------
// SECTION 3 — Compared / copied BY VALUE (vs objects: by reference)
// ------------------------------------------------------------
// When you compare two primitives, JavaScript compares their actual values,
// not where they're stored in memory. Copying a primitive copies the VALUE.
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
console.log(obj1 === obj2); // false  different objects (different references)

// Copying: primitives copy the value; changing the copy doesn't affect the original.
let p1 = 42;
let p2 = p1;
p2 = 100;
console.log(p1, p2); // 42 100

// Objects copy the REFERENCE: both variables point to the same object.
let o1 = { count: 1 };
let o2 = o1;
o2.count = 99;
console.log(o1.count, o2.count); // 99 99  <-- same object!

// Same story when passing into functions:
function tryToChangePrimitive(x) { x = 999; }
function changeObject(objArg) { objArg.count = -1; }
let prim = 7;
let obj = { count: 7 };
tryToChangePrimitive(prim);
changeObject(obj);
console.log(prim, obj.count); // 7 -1  (primitive safe, object mutated)

// ------------------------------------------------------------
// SECTION 4 — No methods... but auto-boxing magic (wrapper objects)
// ------------------------------------------------------------
// Primitives don't have methods, but when you access a property or call a
// method on one, JavaScript temporarily wraps it in a wrapper object
// (String, Number, Boolean). This is called "auto-boxing".
//
// What actually happens internally for str1.toUpperCase():
//   let temp = new String("hello");  // temporary wrapper
//   temp.toUpperCase();              // method called on the wrapper
//   temp = null;                     // wrapper discarded immediately
let str1 = "hello";
str1.toUpperCase();   // toUpperCase() returns a new string
console.log(str1); // still "hello"

// Even simple property access works via boxing:
console.log("abc".length); // 3

// The wrapper types are REAL constructors — but you should never use them:
const boxedStr = new String("abc");
console.log(typeof boxedStr);          // object  (NOT "string")
console.log(boxedStr === "abc");       // false   (object vs primitive)
console.log(boxedStr == "abc");        // true    (loose == coerces!)

// typeof can see through boxing only when the wrapper is unwrapped:
const unboxed = new String("abc").valueOf();
console.log(typeof unboxed);           // string
console.log(unboxed === "abc");        // true

// Adding a property to a primitive: it lands on the throwaway wrapper and vanishes.
let pet = "cat";
pet.color = "black";   // boxed, property set on box, box thrown away
console.log(pet.color); // undefined  <-- classic interview trap

// null and undefined have NO wrappers — any property access throws.
// (Uncomment to see: null.foo;  -> TypeError)

// ------------------------------------------------------------
// SECTION 5 — NaN behavior ("Not a Number" IS a number)
// ------------------------------------------------------------
console.log(typeof NaN);  // number
const value = "hello" * 2;
console.log(value); // NaN

// NaN is the ONLY value not equal to itself:
console.log(NaN === NaN); // false
console.log(NaN == NaN);  // false

// Don't do this
if (value === NaN) { console.log("value is NaN Never true") }  // Never true!

// Do this instead
if (Number.isNaN(value)) { console.log("value is NaN ES6") }  // ES6, recommended
if (isNaN(value)) { console.log("value is NaN Older") }         // Older, coerces first — has quirks

// isNaN() quirks: it COERCES before checking, so non-numbers pass through Number() first.
console.log(isNaN("abc"));       // true   (Number("abc") is NaN)
console.log(isNaN("123"));       // false  (Number("123") is 123)
console.log(Number.isNaN("abc"));// false  (no coercion — "abc" literally isn't NaN)
console.log(Number.isNaN(0 / 0)); // true  (0/0 is genuinely NaN)

// Other operations that produce NaN:
console.log(parseInt("abc"));    // NaN
console.log(Math.sqrt(-1));      // NaN
console.log(undefined + 1);      // NaN

// ------------------------------------------------------------
// SECTION 6 — Number gotchas (floating point + safe integers)
// ------------------------------------------------------------
// JS numbers are IEEE-754 double-precision floats. Binary can't represent
// 0.1 or 0.2 exactly, so their sum lands a hair above 0.3:
console.log(0.1 + 0.2);            // 0.30000000000000004
console.log(0.1 + 0.2 === 0.3);    // false  <-- famous interview question

// Workarounds: compare with a tolerance...
console.log(Math.abs(0.1 + 0.2 - 0.3) < Number.EPSILON); // true
// ...or work in integer units (cents!) and divide once at the end:
console.log((10 + 20) / 100);      // 0.3

// Safe integers: only integers up to 2^53 - 1 are guaranteed exact.
console.log(Number.MAX_SAFE_INTEGER);        // 9007199254740991
console.log(Number.MAX_SAFE_INTEGER + 1 === Number.MAX_SAFE_INTEGER + 2); // true  <-- precision lost!
console.log(Number.isSafeInteger(Number.MAX_SAFE_INTEGER));     // true
console.log(Number.isSafeInteger(Number.MAX_SAFE_INTEGER + 1)); // false

// parseInt stops at the first non-digit; parseFloat handles decimals;
// Number() is all-or-nothing:
console.log(parseInt("42px"));   // 42
console.log(Number("42px"));     // NaN
console.log(parseFloat("3.14rem")); // 3.14

// ------------------------------------------------------------
// SECTION 7 — bigint (arbitrary-precision integers)
// ------------------------------------------------------------
// Append `n` to a literal (or call BigInt()) for exact integers of ANY size.
const huge = 1234567890123456789012345678901234567890n;
console.log(huge + 1n); // 1234567890123456789012345678901234567891n  (exact!)

console.log(typeof 1n); // bigint
console.log(9007199254740993n === 9007199254740993n); // true  (exact as bigint...)
// ...but the same literal WITHOUT `n` loses precision:
console.log(9007199254740993 === 9007199254740992);   // true  <-- wrong! both round to ...992

// Mixing bigint with number is a TypeError (no silent coercion):
try {
    console.log(1n + 1); // throws before logging
} catch (e) {
    console.log(e instanceof TypeError); // true
}

// Loose equality allows bigint<->number comparison; strict does not:
console.log(1n == 1);   // true
console.log(1n === 1);  // false  (different types)

// Division truncates (bigint is integer-only):
console.log(7n / 2n); // 3n
// Math.* methods don't accept bigint:
try {
    const bi = 10n;
    console.log(Math.round(bi)); // throws before logging
} catch (e) {
    console.log(e instanceof TypeError); // true
}
// No fractional values allowed either: BigInt(0.5) would throw RangeError.

// ------------------------------------------------------------
// SECTION 8 — symbol (unique identifiers)
// ------------------------------------------------------------
// Every Symbol is unique, even with the same description:
const s1 = Symbol("id");
const s2 = Symbol("id");
console.log(s1 === s2); // false
console.log(s1.description); // id

// Use case 1: collision-proof object keys (hidden-ish, skipped by most iteration):
const user = { name: "Ada" };
user[s1] = 42;   // computed bracket syntax — a REAL symbol key
console.log(Object.keys(user));        // [ 'name' ]   symbols invisible here
console.log(JSON.stringify(user));     // {"name":"Ada"}  symbols skipped
console.log(Object.getOwnPropertySymbols(user).length); // 1  <- findable explicitly

// Use case 2: well-known symbols customize language behavior.
// Example: make an object iterable by defining [Symbol.iterator]:
const range = {
    from: 1,
    to: 3,
    [Symbol.iterator]() {
        let current = this.from;
        const last = this.to;
        return {
            next: () => ({ done: current > last, value: current++ })
        };
    }
};
console.log([...range]); // [ 1, 2, 3 ]

// Use case 3: registry keys via Symbol.for() — SAME key gives the SAME symbol:
const regA = Symbol.for("app.token");
const regB = Symbol.for("app.token");
console.log(regA === regB); // true
console.log(Symbol.keyFor(regA)); // app.token

// Symbols are NOT primitives-with-wrappers like strings: new Symbol() throws.
try {
    console.log(new Symbol("x")); // throws before logging
} catch (e) {
    console.log(e instanceof TypeError); // true
}

// ------------------------------------------------------------
// SECTION 9 — == vs === (coercion basics)
// ------------------------------------------------------------
// === (strict): no type conversion — different types means false.
// ==  (loose):  coerces operands to a common type before comparing.
console.log(1 == "1");    // true   (string "1" coerced to number 1)
console.log(1 === "1");   // false  (different types, no coercion)
console.log(null == undefined);  // true  (special case: treated as loosely equal)
console.log(null === undefined); // false
console.log(null == 0);   // false  (null only loosely equals undefined)
console.log("" == 0);     // true   ("" coerces to 0) — why people avoid ==
console.log("0" == 0);    // true
console.log(false == ""); // true   (both coerce to 0)
console.log(NaN == 1);       // false (NaN compares false with everything)
console.log(NaN < 1);        // false (even ordering comparisons fail)

// Boolean coercion gotcha: truthy/falsy values.
// Falsy: false, 0, "", null, undefined, NaN (and -0, 0n).
console.log(Boolean(0), Boolean(""), Boolean(null), Boolean(undefined)); // false false false false
console.log(Boolean("0"), Boolean([]), Boolean({})); // true true true  <- surprise: "0", [], {} are truthy!

// Rule of thumb: use === everywhere; reach for == only deliberately
// (e.g. `x == null` catches both null and undefined).

// ------------------------------------------------------------
// SECTION 10 — typeof edge cases (quick-fire recap)
// ------------------------------------------------------------
console.log(typeof null);            // object  <- historic bug, kept forever
console.log(typeof function(){});    // function  <- special-cased, though functions ARE objects
console.log(typeof []);              // object  <- arrays are objects; use Array.isArray
console.log(Array.isArray([]));      // true
console.log(typeof undefined);       // undefined
console.log(typeof undeclaredVariable123); // undefined  <- safe on undeclared vars (no throw!)
console.log(typeof class {});        // function  (classes are functions under the hood)
console.log(typeof new Date());      // object
console.log(typeof /regex/);         // object

// The reliable way to test for null (since typeof lies about it):
const maybeNull = null;
console.log(maybeNull === null); // true
