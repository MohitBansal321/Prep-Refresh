// undefined
// undefined means “no value has been assigned.” JavaScript uses it automatically in several situations:

// 1. Variable declared but not initialized
let name;
console.log(name); // undefined

// 2. Function with no return value
function greet() {
    console.log("Hello");
}
console.log(greet()); // undefined

// 3. Accessing non-existent object property
const obj = { name: "Mohit" };
console.log(obj.age); // undefined

// 4. Function parameters not provided
function add(a, b) {
    return a + b;
}
console.log(add(1)); // undefined