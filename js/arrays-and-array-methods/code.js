// ============================================================
// Arrays and Array Methods — runnable companion to README.md
// Run: node js/arrays-and-array-methods/code.js
// ============================================================

// ------------------------------------------------------------
// 1) Arrays are objects: typeof quirks and Array.isArray
// ------------------------------------------------------------
const arr = [10, 20, 30];

console.log(typeof arr);                 // object
console.log(typeof []);                  // object
console.log(Array.isArray(arr));         // true
console.log(Array.isArray({}));          // false
console.log(arr["1"]);                   // 20   <- element access via STRING key

arr.customProp = "I live on the array object";
console.log(arr.length);                 // 3    <- extra props don't affect length
console.log(1 in arr);                   // true (index slot exists)

const holey = [];
holey[5] = "x";
console.log(holey.length);               // 6    <- assignment past end extends length

// ------------------------------------------------------------
// 2) Mutating vs non-mutating methods
// ------------------------------------------------------------
// --- mutators change the original array ---
const stack = [1, 2, 3];
stack.push(4);                           // add to end
console.log(stack);                      // [1, 2, 3, 4]
const popped = stack.pop();              // remove from end
console.log(popped);                     // 4
console.log(stack);                      // [1, 2, 3]

const queue = [2, 3, 4];
queue.unshift(1);                        // add to front
console.log(queue);                      // [1, 2, 3, 4]
queue.shift();                           // remove from front
console.log(queue);                      // [2, 3, 4]

const spliceSource = ["a", "b", "c", "d"];
const removed = spliceSource.splice(1, 2, "X"); // surgery in place
console.log(spliceSource);               // [ 'a', 'X', 'd' ]
console.log(removed);                    // [ 'b', 'c' ]

// --- non-mutators leave the original untouched ---
const base = [3, 1, 2];
console.log(base.slice(0, 2));           // [3, 1]
console.log(base.concat([9]));           // [3, 1, 2, 9]
console.log(base.map(n => n * 10));      // [30, 10, 20]
console.log(base.filter(n => n > 1));    // [3, 2]
console.log(base);                       // [3, 1, 2]  <- still intact

// --- sort/reverse mutate AND return the same array (the classic trap) ---
const trap = [3, 1, 2];
const sortedRef = trap.sort();
console.log(trap === sortedRef);         // true   <- same reference, it mutated!
console.log(trap);                       // [1, 2, 3]

// --- ES2023 non-mutating twins ---
const nums2023 = [3, 1, 2];
console.log(nums2023.toSorted());        // [1, 2, 3]
console.log(nums2023.toReversed());      // [2, 1, 3]
console.log(nums2023.toSpliced(0, 1));   // [2, 3]
console.log(nums2023.with(0, 99));       // [99, 1, 2]
console.log(nums2023);                   // [3, 1, 2]  <- original never touched

// ------------------------------------------------------------
// 3) The sort() gotcha: default is LEXICOGRAPHIC
// ------------------------------------------------------------
console.log([10, 9, 100].sort());        // [10, 100, 9]   <- string order!
console.log(["banana", "Apple"].sort()); // [ 'Apple', 'banana' ]  <- case-sensitive
console.log([NaN].sort());               // [ NaN ] (fine, but see includes note later)

const scores = [10, 9, 100];
scores.sort((a, b) => a - b);            // comparator: negative -> a first
console.log(scores);                     // [9, 10, 100]
scores.sort((a, b) => b - a);
console.log(scores);                     // [100, 10, 9]

// sorting objects requires a comparator on a field
const users = [
  { name: "Bo", age: 31 },
  { name: "Al", age: 25 },
];
users.sort((a, b) => a.age - b.age);
console.log(users.map(u => u.name));     // [ 'Al', 'Bo' ]

// stable string ordering with localeCompare
console.log(["ä", "z", "a"].sort((a, b) => a.localeCompare(b))); // [ 'a', 'ä', 'z' ]

// ------------------------------------------------------------
// 4) reduce as the Swiss Army knife
// ------------------------------------------------------------
// sum
const sum = [1, 2, 3, 4].reduce((acc, n) => acc + n, 0);
console.log(sum);                        // 10

// max in one pass
const max = [5, 12, 7].reduce((acc, n) => (n > acc ? n : acc));
console.log(max);                        // 12

// frequency count
const words = ["a", "b", "a", "c", "b", "a"];
const freq = words.reduce((acc, w) => {
  acc[w] = (acc[w] ?? 0) + 1;
  return acc;
}, {});
console.log(freq);                       // { a: 3, b: 2, c: 1 }

// groupBy — the most requested reduce interview exercise
const people = [
  { name: "Bo", dept: "eng" },
  { name: "Al", dept: "sales" },
  { name: "Cy", dept: "eng" },
  { name: "Di", dept: "sales" },
];
const byDept = people.reduce((acc, p) => {
  (acc[p.dept] ??= []).push(p);
  return acc;
}, {});
console.log(byDept.eng.map(p => p.name));   // [ 'Bo', 'Cy' ]
console.log(byDept.sales.map(p => p.name)); // [ 'Al', 'Di' ]

// flatten one level manually (what flat() does for you)
const nested = [[1, 2], [3], [4, 5]];
const flatManual = nested.reduce((acc, inner) => acc.concat(inner), []);
console.log(flatManual);                 // [1, 2, 3, 4, 5]

// filter + map fused into one reduce pass
const evensSquared = [1, 2, 3, 4, 5, 6].reduce(
  (acc, n) => (n % 2 === 0 ? [...acc, n * n] : acc),
  []
);
console.log(evensSquared);               // [4, 16, 36]

// reduce without an initial value uses element 0 as seed...
console.log([1, 2, 3].reduce((a, b) => a + b)); // 6
// ...and throws on an empty array:
try {
  [].reduce((a, b) => a + b);
} catch (e) {
  console.log(e.message);                // Reduce of empty array with no initial value
}

// ------------------------------------------------------------
// 5) find / some / every / includes
// ------------------------------------------------------------
const nums = [1, 3, 5, 8, 9];
console.log(nums.find(n => n % 2 === 0));   // 8
console.log(nums.findIndex(n => n > 5));    // 3
console.log(nums.some(n => n > 8));         // true
console.log(nums.every(n => n > 0));        // true
console.log(nums.includes(5));              // true
console.log([NaN].includes(NaN));           // true  <- SameValueZero
console.log([NaN].indexOf(NaN));            // -1    <- indexOf cannot see NaN!

// ------------------------------------------------------------
// 6) Method chaining pipeline
// ------------------------------------------------------------
const orders = [
  { id: 1, total: 250, status: "paid" },
  { id: 2, total: 80,  status: "open" },
  { id: 3, total: 500, status: "paid" },
  { id: 4, total: 300, status: "paid" },
];

const revenueFromBigPaidOrders = orders
  .filter(o => o.status === "paid")     // shrink first
  .filter(o => o.total >= 250)
  .map(o => o.total)                    // then transform
  .reduce((sum, t) => sum + t, 0);      // then collapse

console.log(revenueFromBigPaidOrders);  // 1050

// pipeline of string ops — each link returns a new array
const tags = ["  js ", " TS", "", "node  "];
const cleanTags = tags
  .map(t => t.trim())
  .filter(t => t.length > 0)
  .map(t => t.toLowerCase());
console.log(cleanTags);                 // [ 'js', 'ts', 'node' ]

// ------------------------------------------------------------
// 7) flat / flatMap
// ------------------------------------------------------------
console.log([1, [2, [3, [4]]]].flat());          // [1, 2, [3, [4]]]
console.log([1, [2, [3, [4]]]].flat(2));         // [1, 2, 3, [4]]
console.log([1, [2, [3, [4]]]].flat(Infinity));  // [1, 2, 3, 4]

const sentences = ["hello world", "hi there"];
console.log(sentences.flatMap(s => s.split(" ")));
// [ 'hello', 'world', 'hi', 'there' ]

// ------------------------------------------------------------
// 8) Sparse arrays (know them so you can avoid them)
// ------------------------------------------------------------
const sparse = [1, , 3];                 // hole at index 1
console.log(sparse.length);              // 3
console.log(sparse[1]);                  // undefined
console.log(1 in sparse);                // false <- slot does not exist
console.log(sparse.map(n => n * 2));     // [ 2, <1 empty item>, 6 ]
console.log(sparse.join("-"));           // "1--3"

// delete creates holes; splice removes properly
const bad = [1, 2, 3];
delete bad[1];
console.log(bad);                        // [ 1, <1 empty item>, 3 ]

const good = [1, 2, 3];
good.splice(1, 1);
console.log(good);                       // [1, 3]

// ------------------------------------------------------------
// 9) Array.from / Array.of tricks
// ------------------------------------------------------------
console.log(Array.from("abc"));                            // [ 'a', 'b', 'c' ]
console.log(Array.from(new Set([1, 1, 2])));               // [1, 2]
console.log([...new Set([1, 1, 2, 3, 3])]);                // [1, 2, 3] <- dedupe idiom
console.log(Array.from({ length: 3 }, (_, i) => i * 10));  // [0, 10, 20]

// build a range function with Array.from
const range = (start, end) => Array.from({ length: end - start }, (_, i) => start + i);
console.log(range(3, 7));                // [3, 4, 5, 6]

// new Array(n) is a length-trap; Array.of always does what you mean
console.log(new Array(3));               // [ <3 empty items> ]
console.log(new Array(3).fill(0));       // [0, 0, 0]
console.log(Array.of(3));                // [3]
console.log(Array.of(1, 2));             // [1, 2]

// ------------------------------------------------------------
// 10) Interview pattern mini-demos: dedupe, chunk, intersection
// ------------------------------------------------------------
// chunk(array, size) via slice
function chunk(array, size) {
  const result = [];
  for (let i = 0; i < array.length; i += size) {
    result.push(array.slice(i, i + size));
  }
  return result;
}
console.log(chunk([1, 2, 3, 4, 5], 2));  // [ [1, 2], [3, 4], [5] ]

// intersection using a Set for O(n) lookups
function intersect(a, b) {
  const setB = new Set(b);
  return [...new Set(a)].filter(x => setB.has(x));
}
console.log(intersect([1, 2, 2, 3], [2, 3, 4])); // [2, 3]

console.log("All sections ran.");
