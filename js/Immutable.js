//  Immutable means “cannot be changed.” Primitive values are immutable. You cannot alter the value itself.


const str = "hello";
str[0] = "H";
console.log(str); // "hello"

str = "H" + str.slice(1);
console.log(str); // "Mello"

// 
const obj = {
    name: "Mohit",
    age: 25
}

obj.name = "John";  // Works! Mutating the object
obj.age = 26;      //  Works! adding a property 
console.log(obj); // { name: "John", age: 25 }

obj = {
    name: "John",
    age: 26
}
console.log(obj); // { name: "John", age: 26 }


