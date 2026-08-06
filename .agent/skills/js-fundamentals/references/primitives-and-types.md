# Primitives & Types

## The 7 Primitive Types

| Type | Example | `typeof` result |
|---|---|---|
| `string` | `"hello"` | `"string"` |
| `number` | `10` | `"number"` |
| `bigint` | `123n` | `"bigint"` |
| `boolean` | `true` | `"boolean"` |
| `undefined` | `undefined` | `"undefined"` |
| `null` | `null` | **`"object"`** ⚠️ |
| `symbol` | `Symbol("id")` | `"symbol"` |

## typeof Gotchas

```js
typeof null        // "object"  — legacy bug, never fixed
typeof NaN         // "number"  — NaN is technically a number
typeof function(){} // "function" — not a primitive, but special typeof
```

## Immutability of Primitives

Primitive values cannot be mutated in-place. "Changing" a string creates a new one:

```js
let str = "hello";
str[0] = "H";       // silently fails
console.log(str);   // "hello" — unchanged

str = "H" + str.slice(1);
console.log(str);   // "Hello" — new string assigned to variable
```

## Compared By Value (not reference)

```js
let a = "Mohit";
let b = "Mohit";
console.log(a === b); // true — same value
```

Objects are compared by reference:

```js
let obj1 = { name: "Mohit" };
let obj2 = { name: "Mohit" };
console.log(obj1 === obj2); // false — different references
```

## Autoboxing

Primitives have no methods, but JS wraps them temporarily:

```js
let s = "hello";
s.toUpperCase(); // JS internally does: new String("hello").toUpperCase()
console.log(s);  // "hello" — original unchanged, wrapper discarded
```

## NaN Handling

```js
const val = "hello" * 2; // NaN
val === NaN;              // false — NaN !== NaN by spec!

// Correct checks:
Number.isNaN(val);        // true (recommended, ES6)
isNaN(val);               // true (older, has quirks with type coercion)
```

## undefined — When JS Uses It Automatically

```js
let x;                    // declared, not assigned → undefined
function greet() {}       // no return → undefined
const obj = {};
obj.missing;              // non-existent property → undefined
function add(a, b) { return a + b; }
add(1);                   // missing param b → undefined → result NaN
```

### undefined vs null

| | `undefined` | `null` |
|---|---|---|
| Meaning | No value assigned yet | Intentional absence of value |
| Set by | JavaScript engine | Developer |
| `typeof` | `"undefined"` | `"object"` |
| `== null` | `true` | `true` |
| `=== null` | `false` | `true` |
