// ============================================================
// ES6+ Features — runnable walkthrough
// Run: node js/es6-features/code.js
// Every console.log shows its actual output as a trailing comment.
// ============================================================

// ------------------------------------------------------------
// 1. Template literals
// ------------------------------------------------------------
const name = "Ada";
const score = 91.567;
const passing = score >= 60;

console.log(`${name} scored ${score.toFixed(1)}%`);
// Ada scored 91.6%
console.log(`Status: ${passing ? "PASS" : "FAIL"} (${score > 100 ? "invalid" : "valid"})`);
// Status: PASS (valid)

const multiLine = `Report for ${name}
--------------------
score: ${score}`;
console.log(multiLine);
// Report for Ada
// --------------------
// score: 91.567

// Interpolation is string coercion via toString():
console.log(`${null} / ${undefined}`);
// null / undefined

// ------------------------------------------------------------
// 2. Destructuring — objects, arrays, nested, defaults, renaming
// ------------------------------------------------------------
const user = { id: 7, name: "Grace", role: "admin", address: { city: "London" } };

// 2a. Basic object destructuring
const { id, role } = user;
console.log(id, role);
// 7 admin

// 2b. Renaming + defaults (defaults apply ONLY when value is undefined)
const { role: userRole = "guest" } = {};
const { email = "n/a" } = user;                 // missing -> default
const { nickname = "none" } = { nickname: null }; // null does NOT trigger default
console.log(userRole, email, nickname);
// guest n/a null

// 2c. Array destructuring: positions, skipping, rest
const rgb = [255, 128, 0];
const [red, , blue] = rgb;
const [head, ...tail] = rgb;
console.log(red, blue);      // 255 0
console.log(head, tail);     // 255 [128, 0]

// 2d. Nested pattern against a messy API-shaped response
const response = {
  data: {
    user: { name: "Ada", tags: ["admin", "beta"], address: { city: "London" } },
    meta: { page: 1 },
  },
};
const {
  data: {
    user: { name: apiName, tags: [primaryTag], address: { city } },
    meta: { page },
  },
} = response;
console.log(apiName, primaryTag, city, page);
// Ada admin London 1

// 2e. Swapping variables without a temp
let a = 1;
let b = 2;
[a, b] = [b, a];
console.log(a, b);
// 2 1

// 2f. Destructuring in function parameters (options object done right)
function createUser({ username, tier = "free", isActive = true }) {
  return `${username}: ${tier}, active=${isActive}`;
}
console.log(createUser({ username: "ada" }));
// ada: free, active=true

// ------------------------------------------------------------
// 3. Spread — copies and the SHALLOW-COPY TRAP
// ------------------------------------------------------------
const base = [1, 2];
const extended = [...base, 3];
console.log(extended);
// [1, 2, 3]

const original = { profile: { city: "London" }, tags: ["a"] };
const shallowCopy = { ...original, age: 36 };

console.log(shallowCopy !== original);        // true  (top level IS new)
shallowCopy.age = 37;                         // safe: own property
console.log(original.age);                    // undefined (original untouched)

// ...but nested values are SHARED references:
shallowCopy.profile.city = "Paris";           // mutates through the shared ref!
console.log(original.profile.city);
// Paris   <-- the trap: spread copied the reference, not the object

shallowCopy.tags.push("b");
console.log(original.tags);
// [ 'a', 'b' ]  <-- same array on both sides

// structuredClone makes a real deep copy of plain data:
const deepCopy = structuredClone(original);
deepCopy.profile.city = "Tokyo";
console.log(original.profile.city, deepCopy.profile.city);
// Paris Tokyo   (original now safe from deep changes)

// Object spread precedence: later keys win -> "defaults first" idiom
const defaults = { theme: "dark", retries: 3, verbose: false };
const config = { ...defaults, retries: 5 };
console.log(config);
// { theme: 'dark', retries: 5, verbose: false }

// Array spread also flattens one level during concatenation:
const merged = [...base, ...rgb];
console.log(merged);
// [ 1, 2, 255, 128, 0 ]

// ------------------------------------------------------------
// 4. Rest parameters
// ------------------------------------------------------------
function summarize(label, first, ...rest) {
  // `rest` is a REAL array — map/filter/reduce all work on it
  const total = rest.reduce((sum, n) => sum + n, first);
  return `${label}: ${total} across ${rest.length + 1} numbers`;
}
console.log(summarize("total", 10, 20, 30, 40));
// total: 100 across 4 numbers

// Rest in destructuring collects leftovers (array AND object forms)
const [winner, ...others] = ["Ada", "Grace", "Linus"];
const { profile, ...metaOnly } = { profile: { city: "Paris" }, age: 36, id: 9 };
console.log(winner, others);          // Ada [ 'Grace', 'Linus' ]
console.log(metaOnly);                // { age: 36, id: 9 }

// Old-school `arguments` comparison: array-like, NOT an array
function legacy() {
  console.log(Array.isArray(arguments));
}
legacy(1, 2);
// false

// ------------------------------------------------------------
// 5. Set dedupe + Map usage
// ------------------------------------------------------------
const ids = [3, 1, 3, 2, 1, 3];
const unique = [...new Set(ids)];
console.log(unique);
// [ 3, 1, 2 ]

// Set membership is O(1)-average vs includes()' O(n)
const seen = new Set(ids);
console.log(seen.has(2), seen.has(99), seen.size);
// true false 3

const visited = new Set();
visited.add("home").add("about").add("home");   // duplicates silently ignored
for (const item of visited) process.stdout.write(item + " ");
console.log();                                   // home about
// Set preserves INSERTION order and never stores dupes.

// Map: any key type, insertion-order iteration, O(1) size
const cache = new Map();
cache.set("answer", 42);
cache.set({ id: 1 }, "value keyed by an OBJECT");
cache.set(42, "number key");

console.log(cache.size);
// 3

console.log(cache.get({ id: 1 }));
// undefined  <-- key identity! a NEW object isn't the SAME key
const keyObj = { id: 1 };
cache.set(keyObj, "now reachable");
console.log(cache.get(keyObj));
// now reachable

cache.delete("answer");
console.log(cache.has("answer"), cache.size);
// false 3

for (const [k, v] of cache) console.log(k, "->", v);
// { id: 1 } -> now reachable
// 42 -> number key

// WeakMap idea: entry vanishes when the KEY object is garbage-collected.
// Not iterable, no size — that's the price of not keeping keys alive.
let temp = { token: "abc" };
const metadata = new WeakMap();
metadata.set(temp, { lastUsed: Date.now() });
temp = null; // only strong reference gone -> entry becomes collectable
console.log("WeakMap holds:", "unobservable by design");
// WeakMap holds: unobservable by design

// ------------------------------------------------------------
// 6. for...of vs for...in — the classic trap
// ------------------------------------------------------------
const langs = ["js", "go", "ts"];
langs.extra = "?"; // enumerable extra property — the trap's bait

for (const v of langs) process.stdout.write(v + " ");
console.log();
// js go ts    <-- VALUES via the iterator protocol

for (const k in langs) process.stdout.write(k + " ");
console.log();
// 0 1 2 extra <-- ENUMERABLE STRING KEYS, including extras!

// Type check proves for...in hands you strings:
for (const k in langs) {
  console.log(`${k}: ${typeof k}`);
  break; // one line is enough to make the point
}
// 0: string

// Plain objects have NO iterator -> for...of throws. Use entries():
const scores = { ada: 91, grace: 88 };
for (const [key, value] of Object.entries(scores)) {
  console.log(`${key}=${value}`);
}
// ada=91
// grace=88

// Strings are iterable (for...of walks characters):
for (const ch of "hi") process.stdout.write(ch + " ");
console.log();
// h i

console.log("--- done ---");
// --- done ---
