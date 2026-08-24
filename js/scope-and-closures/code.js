// ============================================================
// Scope & Closures — runnable walkthrough
// Run: node js/scope-and-closures/code.js
// Every console.log below shows its output as a trailing comment.
// ============================================================

console.log("=== 1. Block scope vs function scope ===");

function blockVsFunction() {
  if (true) {
    var functionScoped = "var escapes the block";   // visible in the whole function
    let blockScoped = "let stays in the block";     // visible only inside {}
    const alsoBlockScoped = "const too";            // same as let
  }
  console.log(functionScoped); // var escapes the block
  try {
    console.log(blockScoped);   // never reached
  } catch (err) {
    console.log("let outside block ->", err.constructor.name); // let outside block -> ReferenceError
  }
}
blockVsFunction();

{
  const inner = "only inside this bare block";
  console.log(inner); // only inside this bare block
}

console.log("\n=== 2. var hoisting vs let/const TDZ ===");

// --- var: declaration hoisted AND initialized to undefined ---
console.log(hoistedVar); // undefined
var hoistedVar = 5;
console.log(hoistedVar); // 5

// --- let: hoisted but UNINITIALIZED until its line runs → Temporal Dead Zone ---
try {
  console.log(tdzVar); // throws before this line's binding is initialized
} catch (err) {
  console.log("TDZ read:", err.message); // TDZ read: Cannot access 'tdzVar' before initialization
}
let tdzVar = 10;

// --- even `typeof` cannot save you from the TDZ ---
try {
  console.log(typeof ghost); // reading `ghost` hits its own TDZ
} catch (err) {
  console.log("typeof in TDZ:", err.constructor.name); // typeof in TDZ: ReferenceError
}
let ghost = "boo";

// --- shadowing + TDZ: inner declaration poisons the whole inner scope ---
let msg = "outside";
function tdzShadow() {
  try {
    console.log(msg); // inner `msg` shadows outer one, and is still in ITS TDZ
  } catch (err) {
    console.log("shadowed TDZ:", err.message); // shadowed TDZ: Cannot access 'msg' before initialization
  }
  let msg = "inside";
}
tdzShadow();

console.log("\n=== 3. Scope chain & shadowing ===");

const count = 100;

function runShadowDemo() {
  const count = 1;            // shadows module-level `count`
  {
    const count = 2;          // shadows again, one level deeper
    console.log(count);       // 2 — nearest binding wins
  }
  console.log(count);         // 1
}
runShadowDemo();
console.log(count);           // 100 — outer untouched by inner shadows

function readsUpward() {
  const label = "I live in runShadowDemo's sibling";
  function deeper() {
    // no local `label` here → engine walks UP the lexical chain
    console.log(label);       // I live in runShadowDemo's sibling
  }
  deeper();
}
readsUpward();

console.log("\n=== 4. Counter factory: truly private state ===");

function makeCounter() {
  let count = 0; // unreachable from outside — no property, no key, nothing
  return {
    increment() { return ++count; },
    decrement() { return --count; },
    value() { return count; },
  };
}

const counterA = makeCounter();
const counterB = makeCounter(); // separate closure → independent state

counterA.increment(); // (returns 1)
counterA.increment(); // (returns 2)
counterA.decrement(); // (returns 1)
counterB.increment(); // (returns 1)

console.log(counterA.value()); // 1
console.log(counterB.value()); // 1 — B never felt A's bumps
console.log(counterA.count);   // undefined — there IS no accessible count

console.log("\n=== 5. THE loop bug: var vs let with setTimeout ===");
// Timers fire AFTER synchronous code finishes. All three `var` callbacks
// share ONE function-scoped `i`, which is 3 by the time they run.
for (var i = 0; i < 3; i++) {
  setTimeout(() => console.log("var sees:", i), 0);
}

// `let` in a for-loop creates a FRESH binding per iteration → each callback
// closes over its own copy of that iteration's value.
for (let j = 0; j < 3; j++) {
  setTimeout(() => console.log("let sees:", j), 0);
}

// Pre-ES2015 fix: an IIFE forces a fresh scope per iteration and copies the value.
for (var k = 0; k < 3; k++) {
  ((snapshot) => setTimeout(() => console.log("IIFE sees:", snapshot), 0))(k);
}

// Wait for the timers above to fire before running the final section.
setTimeout(() => {
  console.log("\n=== 6. Module pattern: an API over private state ===");

  const wallet = (() => {
    let balance = 0;                                   // private to this IIFE
    const transactions = [];                           // private history

    return {
      deposit(amount) {
        balance += amount;
        transactions.push({ type: "deposit", amount });
        return balance;
      },
      withdraw(amount) {
        if (amount > balance) return null;             // reject overdraft
        balance -= amount;
        transactions.push({ type: "withdraw", amount });
        return balance;
      },
      getBalance() {
        return balance;
      },
      historyCount() {
        return transactions.length;
      },
    };
  })();

  wallet.deposit(100);  // (returns 100)
  wallet.withdraw(30);  // (returns 70)
  wallet.withdraw(999); // (returns null — rejected)

  console.log(wallet.getBalance());    // 70
  console.log(wallet.historyCount());  // 2 — failed withdraw left no record
  console.log(wallet.balance);         // undefined — state sealed inside
}, 50);
