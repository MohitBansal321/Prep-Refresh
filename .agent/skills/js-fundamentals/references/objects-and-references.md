# Objects & References

## Pass-by-Reference vs Reassignment

When you pass an object to a function, the function gets a **copy of the reference** (not the object itself).

```js
const obj = { name: "Mohit" };

function mutate(o) {
    o.name = "Mohit Bansal"; // ✅ Mutates original — same reference
}

function reassign(o) {
    o = { name: "New Person" }; // ❌ Local reassignment — doesn't affect original
}

mutate(obj);
console.log(obj.name); // "Mohit Bansal"

reassign(obj);
console.log(obj.name); // "Mohit Bansal" — unchanged
```

### Mental Model

```
obj ──→ { name: "Mohit" }     (heap memory)
         ↑
mutate(o) — o points to SAME object, so o.name = ... works

reassign(o) — o is re-pointed to a NEW object, original untouched
```

## Object.freeze — Shallow Immutability

```js
const config = Object.freeze({ api: "https://example.com", retries: 3 });
config.retries = 5;         // silently fails (or throws in strict mode)
console.log(config.retries); // 3

// ⚠️ Nested objects are NOT frozen:
const nested = Object.freeze({ settings: { dark: true } });
nested.settings.dark = false; // ✅ This WORKS — freeze is shallow
```

## Shallow vs Deep Copy

### Shallow Copy (spread operator / Object.assign)

```js
const original = { a: 1, nested: { b: 2 } };
const shallow = { ...original };

shallow.a = 99;
console.log(original.a);        // 1 — primitive copied by value ✅

shallow.nested.b = 99;
console.log(original.nested.b); // 99 — nested object shared ⚠️
```

### Deep Copy (structuredClone)

```js
const original = { a: 1, nested: { b: 2 } };
const deep = structuredClone(original);

deep.nested.b = 99;
console.log(original.nested.b); // 2 — fully independent ✅
```

## Common Mutation Traps

| Trap | Why it fails |
|---|---|
| `arr2 = arr1` then modifying `arr2` | Both point to the same array |
| `Object.freeze` on nested objects | Only freezes top level |
| Function reassignment of param | Reassigning `param = newObj` doesn't affect caller |
| `const` prevents reassignment, not mutation | `const obj = {}; obj.x = 1;` works fine |
