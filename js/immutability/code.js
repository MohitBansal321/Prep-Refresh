// ============================================================
// IMMUTABILITY — runnable examples
// Run: node js/immutability/code.js
// Immutability means "cannot be changed". Primitives are ALWAYS
// immutable; objects and arrays are NOT unless you make them so.
// ============================================================

// ------------------------------------------------------------
// SECTION 1 — Primitive values are immutable
// (migrated & corrected from js/Immutable.js)
// You cannot alter the value itself; you can only point the
// variable at a NEW value.
// ------------------------------------------------------------

const str = "hello";
// str[0] = "H";          // silently does nothing on strings
console.log(str);          // hello  (unchanged)

let name = "Mohit";
name.toLowerCase();        // creates a NEW string "mohit", throws the old one away... nothing points to it
console.log(name);         // Mohit  — original untouched

name = name.toUpperCase(); // REASSIGNMENT: variable now points to a new string
console.log(name);         // MOHIT

// The original file reassigned a `const`, which throws:
try {
    const fixed = "hello";
    fixed = "H" + fixed.slice(1); // TypeError: Assignment to constant variable.
} catch (e) {
    console.log(e.name);   // TypeError
}

// ------------------------------------------------------------
// SECTION 2 — Objects are mutable (even through `const`)
// (migrated from js/Immutable.js)
// `const` locks the BINDING (variable -> reference), not the VALUE.
// ------------------------------------------------------------

const obj = {
    name: "Mohit",
    age: 25,
};

obj.name = "John";     // Works! mutating the object the const points at
obj.city = "Delhi";    // Works! adding a property
console.log(obj);      // { name: 'John', age: 25, city: 'Delhi' }

// obj = { name: "John", age: 26 };  // would THROW — cannot reassign a const binding
// What "immutability" asks us to do instead: build a NEW object
const updated = { ...obj, age: 26 };
console.log(updated);  // { name: 'John', age: 25, city: 'Delhi', age: 26 }
console.log(obj.age);  // 25      — original still intact

// ------------------------------------------------------------
// SECTION 3 — The shared-reference mutation trap
// ------------------------------------------------------------

const original = { user: { name: "Mohit" }, tags: ["a", "b"] };
const shallowCopy = { ...original };       // new object, SAME inner references

shallowCopy.user.name = "HACKED";          // mutates the SHARED inner object
shallowCopy.tags.push("c");                // mutates the SHARED array

console.log(original.user.name);           // HACKED  — copy leaked into original!
console.log(original.tags);                // [ 'a', 'b', 'c' ]

// True isolation needs a deep copy:
const deepCopy = structuredClone(original);
deepCopy.user.name = "safe";
deepCopy.tags.push("d");
console.log(original.user.name);           // HACKED  (still, but unchanged by this op)
console.log(deepCopy.tags);                // [ 'a', 'b', 'c', 'd' ]

// ------------------------------------------------------------
// SECTION 4 — Immutable state update of a NESTED object
// Classic React/reducer pattern: copy every level you touch.
// ------------------------------------------------------------

const state = {
    user: { name: "Mohit", address: { city: "Delhi", pin: "110001" } },
    posts: ["p1", "p2"],
};

// WRONG: mutation — same object identity, nothing can detect the change
// state.user.address.city = "Mumbai";

// RIGHT: spread a fresh object at EACH level along the path
const newState = {
    ...state,
    user: {
        ...state.user,
        address: { ...state.user.address, city: "Mumbai" },
    },
};

console.log(newState.user.address.city);   // Mumbai
console.log(state.user.address.city);      // Delhi   — old state preserved
console.log(state === newState);           // false   — new top-level reference
console.log(state.user === newState.user); // false   — changed branch copied...
console.log(state.posts === newState.posts);// true   — untouched branches shared (cheap!)

// ------------------------------------------------------------
// SECTION 5 — Mutating vs non-mutating array operations
// ------------------------------------------------------------

const nums = [3, 1, 2];

const sorted = [...nums].sort((a, b) => a - b);  // copy first, then sort
console.log(nums);                        // [ 3, 1, 2 ]  — original safe
console.log(sorted);                      // [ 1, 2, 3 ]

// Modern ES2023 non-mutating twins:
console.log(nums.toSorted((a, b) => a - b)); // [ 1, 2, 3 ]
console.log(nums.toReversed());              // [ 2, 1, 3 ]
console.log(nums.with(0, 99));               // [ 99, 1, 2 ]
console.log(nums);                           // [ 3, 1, 2 ]  — still intact

// Build-new instead of push/splice:
let list = ["a", "b"];
list = [...list, "c"];                    // append immutably
console.log(list);                        // [ 'a', 'b', 'c' ]
console.log(list.filter(x => x !== "b")); // [ 'a', 'c' ] — remove immutably
console.log(list.map(x => x.toUpperCase())); // [ 'A', 'B', 'C' ] — transform immutably

// ------------------------------------------------------------
// SECTION 6 — Object.freeze and the shallow-freeze trap
// ------------------------------------------------------------

const frozen = Object.freeze({ level: 1, inner: { x: 10 } });

try { frozen.level = 99; } catch (e) { /* non-strict: silent */ }
console.log(frozen.level);                 // 1  — top level protected

frozen.inner.x = 99;                       // NOT stopped — freeze is SHALLOW
console.log(frozen.inner.x);               // 99  — inner object mutated!

// Deep freeze helper (exercise: write your own version)
function deepFreeze(o) {
    Object.getOwnPropertyNames(o).forEach(p => {
        if (typeof o[p] === "object" && o[p] !== null) deepFreeze(o[p]);
    });
    return Object.freeze(o);
}

const deepFrozen = deepFreeze({ level: 1, inner: { x: 10 } });
deepFrozen.inner.x = 42;                   // silent no-op in non-strict mode
console.log(deepFrozen.inner.x);           // 10  — truly immutable now

console.log(Object.isFrozen(frozen));      // true

console.log("done");                       // done
