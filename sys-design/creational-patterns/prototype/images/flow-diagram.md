# Prototype Pattern — Flow Diagram

Step-by-step control flow of creating a new object from a prototype, plus the
deep-vs-shallow branch that decides whether you get an independent object or a bug.

```mermaid
flowchart TD
    Start([Need a new object like an existing one]) --> HaveProto{Prototype<br/>already built?}
    HaveProto -- No --> Build["Build it ONCE<br/>(DB + Redis + assembly)"]
    Build --> Register["Register in registry under a key"]
    Register --> Ask
    HaveProto -- Yes --> Ask["Ask registry: create(key)"]
    Ask --> Clone["prototype.clone()"]
    Clone --> Deep["Deep-copy nested state<br/>arrays / objects / Map / Date"]
    Deep --> Identity["Return NEW instance of the<br/>SAME class (preserve prototype chain)"]
    Identity --> Tweak["Client mutates the clone<br/>(name, tags, ...)"]
    Tweak --> Safe{Original<br/>affected?}
    Safe -- "No (deep copy)" --> End([Independent object ready])
    Safe -. "Yes (shallow copy bug!) " .-> Bug[["Aliasing bug:<br/>original corrupted"]]
```

**Key idea:** the branch that matters is at the bottom. A correct **deep** `clone()`
leads to an independent object (solid path). A **shallow** copy shares nested
references and leads to the aliasing bug (dotted path), where mutating the clone
silently corrupts the original. Everything above the branch — build once, register,
clone, preserve class identity — exists to make that deep, independent copy possible.
