// ============================================================================
// this & Prototypes — runnable walkthrough
// Run with:  node js/this-and-prototypes/code.js
//
// Every console.log below has its real output as a trailing comment.
// `this` is always identified via a `name` property, never by dumping objects.
// ============================================================================

'use strict';

function section(title) {
  console.log(`\n=== ${title} ===`);
}

// ----------------------------------------------------------------------------
// 1. RULE: DEFAULT BINDING — a plain call gets globalThis (sloppy) or
//    undefined (strict). This whole file is strict mode, so we show both by
//    using a non-strict function for the sloppy case.
// ----------------------------------------------------------------------------
section('1. Default binding');

// NOTE: strictness is inherited from the surrounding code, so once this file
// declares 'use strict', inner functions cannot "opt back out". We build a
// genuinely sloppy function with `new Function` (its body is non-strict by
// default) to show the legacy behavior:
const defaultSloppy = new Function(
  "return 'sloppy plain call -> this === globalThis: ' + (this === globalThis)"
);
function defaultStrict() {
  return this; // undefined under strict mode
}

console.log(defaultSloppy()); // sloppy plain call -> this === globalThis: true
console.log('strict plain call -> this is undefined:', defaultStrict() === undefined); // true

// ------------------------------------------------------------------------------
// 2. RULE: IMPLICIT (OBJECT) BINDING — the thing before the dot becomes this.
// ------------------------------------------------------------------------------
section('2. Implicit binding');

const teacher = {
  name: 'Implicit-Teacher',
  introduce() {
    return `Hi from ${this.name}`;
  },
};

console.log(teacher.introduce()); // Hi from Implicit-Teacher

// Only the LAST dot counts:
const outer = {
  name: 'outer',
  inner: { name: 'inner', say() { return `say from ${this.name}`; } },
};
console.log(outer.inner.say()); // say from inner

// ------------------------------------------------------------------------------
// 3. RULE: EXPLICIT BINDING — call / apply / bind force a context.
// ------------------------------------------------------------------------------
section('3. Explicit binding (call / apply / bind)');

function greet(greeting, punctuation) {
  return `${greeting}, ${this.name}${punctuation}`;
}
const cat = { name: 'Explicit-Cat' };
const dog = { name: 'Explicit-Dog' };

console.log(greet.call(cat, 'Hello', '!')); // Hello, Explicit-Cat!
console.log(greet.apply(dog, ['Yo', '?'])); // Yo, Explicit-Dog?

// bind returns a NEW function whose context is locked forever:
const boundGreet = greet.bind({ name: 'Explicit-Bound' }, 'Hey');
console.log(boundGreet('.')); // Hey, Explicit-Bound.

// Re-binding changes nothing — first bind wins:
const reBound = boundGreet.bind(dog);
console.log(reBound('!')); // Hey, Explicit-Bound!

// ------------------------------------------------------------------------------
// 4. RULE: NEW BINDING — `new` creates a fresh object and calls the function
//    with `this` set to it. Highest precedence of the four "real" rules.
// ------------------------------------------------------------------------------
section('4. new binding');

function Robot(name) {
  // Behind the scenes of `new Robot(...)`:
  //   1. create {}          2. link {}.[[Prototype]] -> Robot.prototype
  //   3. run body with this = {}   4. return this automatically
  this.name = name;
}
const bot = new Robot('New-Robot');
console.log(`new binding -> this.name = ${bot.name}`); // new binding -> this.name = New-Robot

// `new` even overrides hard binding:
function Precedence() {}
const boundCtor = Precedence.bind({ name: 'ignored-bound-context' });
const pInstance = new boundCtor();
console.log('new beats bind -> instance instanceof Precedence:', pInstance instanceof Precedence); // true

// ------------------------------------------------------------------------------
// 5. RULE: LEXICAL BINDING — arrows have NO own `this`; they inherit it from
//    the enclosing scope. Regular nested functions get their own per rules 1-4.
// ------------------------------------------------------------------------------
section('5. Lexical binding (arrow functions)');

const counter = {
  name: 'Lexical-Outer',
  run() {
    const arrow = () => `arrow inherits -> ${this.name}`;
    function regularFn() {
      return `regular has own this -> ${(this && this.name) || '(none)'}`;
    }
    console.log(arrow());
    console.log(regularFn());
  },
};
counter.run();
// arrow inherits -> Lexical-Outer
// regular has own this -> (none)

// Arrows ignore call/bind/apply for `this` (top-level arrow here sees the
// module scope's this, which has no name property):
const arrowFn = () => `arrow ignores call() -> name: ${(this && this.name) || '(none)'}`;
console.log(arrowFn.call(counter)); // arrow ignores call() -> name: (none)

// ------------------------------------------------------------------------------
// 6. LOSS OF THIS — extracting a method or passing it as a callback detaches
//    it from its receiver. Plus the three standard fixes.
// ------------------------------------------------------------------------------
section('6. Lost this + fixes');

const user = {
  name: 'Ada',
  // We guard with `this &&` only so the script keeps running: under strict
  // mode a detached call makes `this` undefined, and touching this.name
  // directly would throw "Cannot read properties of undefined".
  getName() {
    return `user is ${this && this.name}`;
  },
};

// Bug 1: extracted method reference
const extracted = user.getName;
console.log(extracted()); // user is undefined

// Bug 2: method passed as callback (same thing, hidden behind an API)
function fakeTimeout(fn) { return fn(); } // simulates calling it later, receiver-less
console.log(fakeTimeout(user.getName)); // user is undefined

// Fix 1: hard-bind once
const boundName = user.getName.bind(user);
console.log(boundName()); // user is Ada

// Fix 2: arrow wrapper at the call site keeps the receiver lexically
console.log(fakeTimeout(() => user.getName())); // user is Ada

// Fix 3: wrapper that re-attaches at call time
function callWithUser(fn, ctx) { return fn.call(ctx); }
console.log(callWithUser(user.getName, user)); // user is Ada

// Class methods are strict-mode, so detachment fails loudly (undefined), not silently.

// ------------------------------------------------------------------------------
// 7. CONSTRUCTOR FUNCTION + SHARED PROTOTYPE METHOD — state per instance,
//    behavior shared ONCE on the constructor's .prototype.
// ------------------------------------------------------------------------------
section('7. Constructor functions + shared prototype');

function Animal(name) {
  this.name = name; // per-instance state
}
Animal.prototype.speak = function () {
  return `${this.name} makes a sound`; // ONE shared function object
};
Animal.prototype.species = 'generic-animal'; // shared data lives there too

const rex = new Animal('Rex');
const bella = new Animal('Bella');

console.log(rex.speak()); // Rex makes a sound
console.log(bella.speak()); // Bella makes a sound

// The proof of sharing: both instances see the SAME function object...
console.log('rex.speak === bella.speak:', rex.speak === bella.speak); // true
// ...and the method was never copied onto the instances:
console.log("own keys of rex:", Object.keys(rex)); // own keys of rex: [ 'name' ]
console.log("rex.hasOwnProperty('speak'):", rex.hasOwnProperty('speak')); // false

// ------------------------------------------------------------------------------
// 8. CLASS SYNTAX — same mechanism underneath: methods land on ClassX.prototype.
// ------------------------------------------------------------------------------
section('8. Classes are prototype sugar');

class AnimalClass {
  constructor(name) {
    this.name = name;
  }
  speak() {
    return `${this.name} says hi`;
  }
}

const kitty = new AnimalClass('Kitty');
console.log(kitty.speak()); // Kitty says hi
console.log('method home is ClassX.prototype:', typeof AnimalClass.prototype.speak); // method home is ClassX.prototype: function
console.log('kitty.speak === AnimalClass.prototype.speak:', kitty.speak === AnimalClass.prototype.speak); // true
console.log('a class is still a function:', typeof AnimalClass); // a class is still a function: function

// ------------------------------------------------------------------------------
// 9. PROTOTYPE CHAIN LOOKUP with Object.create — reads walk up the chain,
//    writes create own properties, and own values SHADOW inherited ones.
// ------------------------------------------------------------------------------
section('9. Prototype chain, shadowing, hasOwnProperty');

const grandparent = {
  familyName: 'Chain-Family',
  greet() {
    return `hello from ${this.familyName}`;
  },
  level: 0,
};
const parentObj = Object.create(grandparent); // [[Prototype]] -> grandparent
parentObj.level = 1;
const child = Object.create(parentObj);       // [[Prototype]] -> parentObj
child.level = 2;

console.log(child.level); // 2  — own property shadows everything above
delete child.level;
console.log(child.level); // 1  — now parentObj's copy shows through
console.log(child.greet()); // hello from Chain-Family — found two links up

// Lookup mechanics, verified directly:
console.log('getPrototypeOf(child) === parentObj:', Object.getPrototypeOf(child) === parentObj); // true
console.log('getPrototypeOf(parentObj) === grandparent:', Object.getPrototypeOf(parentObj) === grandparent); // true

// Own vs inherited:
console.log("child.hasOwnProperty('greet'):", child.hasOwnProperty('greet')); // false — inherited
child.ownProp = 'mine';
console.log("child.hasOwnProperty('ownProp'):", child.hasOwnProperty('ownProp')); // true
console.log("'greet' in child:", 'greet' in child); // true — `in` checks the WHOLE chain
// hasOwnProperty itself is inherited from Object.prototype — the top of every chain.

// ------------------------------------------------------------------------------
// 10. INSTANCEOF — checks whether Ctor.prototype appears anywhere in the
//     object's prototype chain. Structural, not historical.
// ------------------------------------------------------------------------------
section('10. instanceof mechanics');

class Vehicle {}
class Car extends Vehicle {}
const car = new Car();

console.log('car instanceof Car:', car instanceof Car); // true — direct link
console.log('car instanceof Vehicle:', car instanceof Vehicle); // true — one hop further up
console.log('car instanceof Object:', car instanceof Object); // true — top of every chain

// It follows the CURRENT chain, not how the object was built:
Object.setPrototypeOf(car, {});
console.log('after rewiring chain -> car instanceof Car:', car instanceof Car); // false!
console.log('after rewiring chain -> car instanceof Vehicle:', car instanceof Vehicle); // false!

// And any object linked to Dog.prototype passes instanceof without ever calling Dog:
function Dog(name) { this.name = name; }
const fakeDog = Object.create(Dog.prototype);
console.log('fakeDog instanceof Dog:', fakeDog instanceof Dog); // true — no constructor ran

console.log('\n(done)');
