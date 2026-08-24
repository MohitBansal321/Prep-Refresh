// ============================================================================
// OBJECTS AND REFERENCES — runnable companion to README.md
// Run:  node js/objects-and-references/code.js
// Predict each output BEFORE reading the trailing comment.
// ============================================================================

// ----------------------------------------------------------------------------
// SECTION 1 — Objects are reference types; primitives are not
// A variable holding an object holds a *reference* (pointer) to heap storage,
// never the object itself. Primitives are copied by value.
// ----------------------------------------------------------------------------

let a = 10;
let b = a; // b gets a COPY of the primitive value
b = 20;

console.log(a); // 10  (primitive copy — a is untouched)

let objA = { name: "Mohit" };
let objB = objA;            // objB gets a COPY OF THE REFERENCE — same object
objB.name = "Changed";

console.log(objA.name);     // "Changed"  (one object, two names for it)
console.log(objA === objB); // true       (=== compares references: identical here)

// Two structurally identical objects are still different objects:
const x = { id: 1 };
const y = { id: 1 };

console.log(x === y); // false  (two separate heap allocations)
console.log(x.id === y.id); // true   (the VALUES inside are equal primitives)

// ----------------------------------------------------------------------------
// SECTION 2 — Mutation through shared references (objects AND arrays)
// One alias mutates → every holder of the reference sees it.
// ----------------------------------------------------------------------------

const original = { role: "admin" };
const alias = original; // NOT a copy — a second arrow to the same object
alias.role = "user";

console.log(original.role); // "user"  (mutation leaked through the shared reference)

const arr1 = [1, 2, 3];
const arr2 = arr1;
arr2.push(4);

console.log(arr1);          // [1, 2, 3, 4]  (arrays are objects — same deal)
console.log(arr1 === arr2); // true

// ----------------------------------------------------------------------------
// SECTION 3 — Reassignment vs mutation inside functions (pass-by-sharing)
// Original demo from js/object.js, kept intact and annotated.
//
// function rename(obj):  MUTATION      → follows the reference into the shared
//                                        object; caller sees the change.
// function rename2(obj): REASSIGNMENT  → rebinds the LOCAL parameter variable to
//                                        a brand-new object; caller's variable
//                                        still points at the old object, so this
//                                        does not change anything outside.
// ----------------------------------------------------------------------------

const obj = {
    name: "Mohit"
};

rename(obj);
rename2(obj);   // does not change reassignment doesn't work


function rename(obj) {
    obj.name = "Mohit Bansal";
}

function rename2(obj) {
    obj = {
        name: "Mohit Bansal 2"
    }
}


console.log(obj); // { name: 'Mohit Bansal' }  (rename worked; rename2's local rebinding did not escape)

// Same principle with arrays — push is visible, reassignment is not:
function reseed(arr) {
    arr.push(99);   // mutation → visible to caller
    arr = [7, 8];   // reassignment → only changes the local parameter
}

const nums = [1];
reseed(nums);
console.log(nums); // [1, 99]

// ----------------------------------------------------------------------------
// SECTION 4 — Shallow copy: spread and Object.assign
// Both duplicate the TOP level only. Nested objects are copied as references,
// so mutating them through the "copy" still hits the original.
// ----------------------------------------------------------------------------

const src = { profile: { city: "Delhi" }, tags: ["a"], count: 1 };

const spreadCopy = { ...src };
spreadCopy.count = 42;                 // top-level primitive → safe
spreadCopy.profile.city = "Mumbai";    // nested object → SHARED reference!

console.log(src.count);        // 1    (top-level write did not leak)
console.log(src.profile.city); // "Mumbai"  (shallow copy betrayed us)

const assignCopy = Object.assign({}, src);
assignCopy.tags.push("z");             // tags array is shared too

console.log(src.tags);         // ["a", "z"]
console.log(spreadCopy === src);           // false (new outer object...)
console.log(spreadCopy.profile === src.profile); // true (...but SAME nested object)

// ----------------------------------------------------------------------------
// SECTION 5 — JSON round-trip deep copy, and its limits
// JSON.parse(JSON.stringify(x)) is a real deep copy — but only survives data
// that JSON can represent. Watch what it silently destroys.
// ----------------------------------------------------------------------------

const messy = {
    when: new Date("2024-01-01T00:00:00.000Z"),
    gap: undefined,
    fn: () => "hi",
    score: NaN,
    nested: { ok: 1 }
};

const jsonCopy = JSON.parse(JSON.stringify(messy));

console.log(typeof jsonCopy.when); // string  (Date became an ISO string — type lost)
console.log(jsonCopy.gap);         // undefined... actually: the KEY was dropped entirely
console.log("gap" in jsonCopy);    // false  (undefined values vanish in JSON)
console.log(jsonCopy.fn);          // undefined  (functions are dropped)
console.log(jsonCopy.score);       // null  (NaN serializes to null)
console.log(jsonCopy.nested);      // { ok: 1 }  (plain data copies fine)

// Cycles throw outright:
const cyclic = {};
cyclic.self = cyclic;
try {
    JSON.parse(JSON.stringify(cyclic));
} catch (e) {
    console.log(e.constructor.name); // TypeError  (Converting circular structure to JSON)
}

// ----------------------------------------------------------------------------
// SECTION 6 — structuredClone: the modern deep copy
// Deep-copies nesting, Dates, Maps, Sets, even cycles. Loses functions and
// custom prototypes (you get plain objects / plain data back).
// ----------------------------------------------------------------------------

const deep = structuredClone(src);
deep.profile.city = "Goa";
deep.tags.push("deep");

console.log(src.profile.city); // "Mumbai"  (value set in Section 4 — crucially NOT "Goa": the clone's write stayed isolated)
console.log(src.tags);         // ["a", "z"]  (push happened on the CLONE's array)
console.log(deep.tags);        // ["a", "z", "deep"]

const clonedDate = structuredClone({ when: new Date("2024-01-01T00:00:00.000Z") });
console.log(clonedDate.when instanceof Date); // true  (Date survives as a real Date)

const clonedCycle = structuredClone(cyclic);
console.log(clonedCycle.self === clonedCycle); // true  (cycle reconstructed correctly)

// ----------------------------------------------------------------------------
// SECTION 7 — Nested-object mutation traps (the React/Redux classic)
// A fresh outer object wrapping old inner references LOOKS like a new state
// but shares everything one level down.
// ----------------------------------------------------------------------------

const stateV1 = { user: { name: "Mohit", prefs: { theme: "dark" } }, version: 1 };

// Looks like immutability, isn't:
const stateV2 = { ...stateV1, version: 2 };
stateV2.user.name = "HACKED";

console.log(stateV1.user.name); // "HACKED"  (user object was shared all along)
console.log(stateV1.version);   // 1         (only the primitive was safely new)

// Correct immutable update: rebuild EVERY mutated level of the path
const stateV3 = {
    ...stateV1,
    user: { ...stateV1.user, prefs: { ...stateV1.user.prefs, theme: "light" } },
};

console.log(stateV3.user.prefs.theme);        // "light"
console.log(stateV1.user.prefs.theme);        // "dark"  (v1 finally safe)
console.log(stateV3.user.prefs !== stateV1.user.prefs); // true

// Object.freeze is also SHALLOW:
const frozen = Object.freeze({ meta: { clicks: 0 } });
frozen.meta.clicks = 5; // inner object was never frozen

console.log(frozen.meta.clicks); // 5

// ----------------------------------------------------------------------------
// SECTION 8 — Arrays as references: mutators vs non-mutators
// sort/splice/push/reverse mutate IN PLACE (dangerous on shared arrays).
// map/filter/slice/toSorted/toReversed return NEW arrays.
// ----------------------------------------------------------------------------

const shared = ["c", "a", "b"];
const sameRef = shared;
shared.sort();

console.log(sameRef);        // ["a", "b", "c"]  (sort mutated in place, alias saw it)

const base = [3, 1, 2];
const sortedCopy = [...base].sort();   // defensive copy before sorting
const mapped = base.map(n => n * 10);

console.log(base);        // [3, 1, 2]  (original preserved)
console.log(sortedCopy);  // [1, 2, 3]
console.log(mapped);      // [30, 10, 20]
console.log(mapped === base); // false  (map always builds a new array)

console.log("done");
