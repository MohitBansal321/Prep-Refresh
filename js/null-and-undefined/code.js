// null-and-undefined
// undefined means "no value has been assigned." JavaScript uses it automatically in several situations.
// null, by contrast, is a value YOU assign on purpose to say "this is intentionally empty."

// ============================================================================
// SECTION 1 — Where `undefined` comes from (the 4 automatic sources)
// ============================================================================

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
console.log(add(1)); // NaN  (b was never provided -> b is undefined -> 1 + undefined = NaN)
console.log(add(1, undefined)); // NaN  (explicit undefined behaves the same as missing)

// Bonus source: void operator ALWAYS produces undefined (used to force it)
console.log(void 0 === undefined); // true
console.log(void "anything"); // undefined

// ============================================================================
// SECTION 2 — What `null` means: intentional absence
// ============================================================================

// null is a value you ASSIGN to say "I deliberately cleared / have no object here."
let user = { name: "Mohit" };
user = null; // "the user reference is intentionally emptied"
console.log(user); // null

// A common convention: undefined = "not yet set", null = "deliberately set to nothing"
let profile = {
    nickname: undefined, // never chosen
    spouse: null,        // looked up, confirmed none
};
console.log(profile.nickname, profile.spouse); // undefined null

// typeof quirk: null reports itself as "object" (a historical bug kept for compatibility)
console.log(typeof undefined); // "undefined"
console.log(typeof null);      // "object"   <- the famous quirk!
// Safe null check:
console.log(user === null); // true

// ============================================================================
// SECTION 3 — == vs === between null and undefined
// ============================================================================

// Loose equality treats them as "equivalent emptiness"
console.log(null == undefined);  // true
console.log(null != undefined);  // false

// Strict equality sees them as different types
console.log(null === undefined); // false
console.log(null !== undefined); // true

// Practical trick: `x == null` is TRUE only for null and undefined — a compact "is empty" guard
let maybe;
console.log(maybe == null);  // true (undefined passes the check)
console.log("x" == null);    // false

// But they are NOT equal to anything else via ==
console.log(null == 0);        // false
console.log(null == "");       // false
console.log(undefined == 0);   // false
console.log(null == false);    // false

// ============================================================================
// SECTION 4 — Falsy values (the complete list of 6)
// ============================================================================

// Everything below is falsy. EVERYTHING else is truthy (including [], {}, "0").
const falsyValues = [false, 0, "", null, undefined, NaN];
for (const v of falsyValues) {
    console.log(Boolean(v), JSON.stringify(v));
}
// false false        (JSON.stringify(false) -> "false")
// false 0
// false ""           (JSON.stringify("") -> '""')
// false null
// false undefined    (JSON.stringify(undefined) returns undefined itself)
// false null         (JSON.stringify(NaN) -> "null")

// Surprising TRUTHY values people get wrong:
console.log(Boolean([]));       // true  (empty array!)
console.log(Boolean({}));       // true  (empty object!)
console.log(Boolean("0"));      // true  (non-empty string)
console.log(Boolean("false"));  // true  (non-empty string!)

// ============================================================================
// SECTION 5 — Optional chaining (?.) — safe deep reads
// ============================================================================

const order = {
    id: 7,
    customer: {
        address: { city: "Pune" },
    },
};

// Without ?. : reading a deep property of something that isn't there THROWS
try {
    console.log(order.user.name); // TypeError: Cannot read properties of undefined (reading 'name')
} catch (e) {
    console.log("threw:", e.message); // threw: Cannot read properties of undefined (reading 'name')
}

// With ?. : the chain short-circuits to undefined instead of throwing
console.log(order.customer?.address?.city); // Pune
console.log(order.user?.name);              // undefined
console.log(order.user?.address?.city);     // undefined

// Works on methods and array items too
const api = {
    fetch() { return "data"; },
};
console.log(api.fetch?.());  // data
console.log(api.save?.());   // undefined (method missing -> no crash)

const list = null;
console.log(list?.[0]);      // undefined

// NOTE: ?. short-circuits at null OR undefined — but a PRESENT property of value
// null still yields null (it does not convert anything):
const weird = { tag: null };
console.log(weird.tag?.toUpperCase()); // undefined (null short-circuits BEFORE calling toUpperCase)

// ============================================================================
// SECTION 6 — Nullish coalescing (??) vs logical OR (||)
// ============================================================================

// || falls back on ANY falsy value. ?? falls back ONLY on null/undefined.
const volumeSetting = 0;

console.log(volumeSetting || 50);  // 50   <- BUG! 0 is a valid volume, || discards it
console.log(volumeSetting ?? 50);  // 0    <- correct: 0 is not nullish, keep it

console.log("" || "default");      // default  (empty string discarded — may be intended or not)
console.log("" ?? "default");      // ""       (empty string is NOT nullish)

console.log(false || true);        // true
console.log(false ?? true);        // false

console.log(null ?? "fallback");   // fallback
console.log(undefined ?? "fallback"); // fallback

// Chaining: first non-nullish wins
console.log(null ?? undefined ?? 3 ?? 10); // 3

// Mixing ?? with || or && requires parentheses (syntax rule):
// console.log(null || undefined ?? "x"); // SyntaxError if uncommented
console.log((null || undefined) ?? "x"); // x

// ============================================================================
// SECTION 7 — Default parameters trigger on `undefined` ONLY (never null)
// ============================================================================

function connect(host = "localhost") {
    return host;
}
console.log(connect());           // localhost (missing -> undefined -> default kicks in)
console.log(connect(undefined));  // localhost (EXPLICIT undefined also triggers the default)
console.log(connect(null));       // null      <- default does NOT fire; null is "a value you chose"
console.log(connect(""));         // ""        (also a real value)

// This is why passing null into APIs can sneak past defaults and poison downstream code.

// ============================================================================
// SECTION 8 — Common bugs from each, and the fixes
// ============================================================================

// BUG 1: reading a property off undefined ("Cannot read properties of ...")
// FIX: optional chaining + a nullish fallback.
function getCity(orderLike) {
    return orderLike?.customer?.address?.city ?? "unknown";
}
console.log(getCity(order));      // Pune
console.log(getCity({}));         // unknown

// BUG 2: using || where 0 / "" are legitimate values.
function applyDiscount(couponCount) {
    const count = couponCount ?? 1; // ?? keeps 0; only null/undefined fall back
    return count * 10;
}
console.log(applyDiscount(0)); // 0   (correctly treats "no coupons" as zero)
console.log(applyDiscount(2)); // 20

// The ?? fix pattern:
const retries = 0;
const attempts = retries ?? 3;
console.log(attempts); // 0  (correctly preserved)

// BUG 3: expecting a default parameter to catch null.
function render(label = "Untitled") {
    return `[${label}]`;
}
console.log(render(null)); // [null]  <- surprise! the default only fires for undefined

// BUG 4: typeof null === "object" fooling an "is an object" check.
function describe(v) {
    if (typeof v === "object") {
        return "object-ish"; // WRONG for null!
    }
    return "other";
}
console.log(describe(null)); // object-ish  <- classic interview trap
// Correct guard:
function describeFixed(v) {
    if (v !== null && typeof v === "object") return "object";
    return "other";
}
console.log(describeFixed(null)); // other

// ============================================================================
// SECTION 9 — Mental model recap (run this last)
// ============================================================================

console.log("--- mental model ---");
console.log("undefined = JS says: nothing assigned yet"); // string printed as-is
console.log("null = YOU say: deliberately empty");
console.log("== lumps them together; === separates them");
console.log("|| checks falsy; ?? checks nullish; ?. prevents the throw");
