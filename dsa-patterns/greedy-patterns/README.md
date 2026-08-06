# Greedy Patterns

A single family with one pattern, because "greedy" is less a specific technique than a **proof style**: sort by the right key, make the locally-best choice at each step, and never revisit it. The hard part is never the code — it's convincing yourself the local choice is always safe, which is why greedy problems are often paired with a short exchange-argument or contradiction proof.

| Pattern | Core idea | Used for |
|---------|-----------|----------|
| [Greedy](greedy/README.md) | Sort by a key, then make one irrevocable locally-optimal choice per step | Interval scheduling, jump games, gas station, task assignment |

## How to tell it applies

- You can sort the input by *some* key (end time, ratio, distance) such that processing it in that order and committing to each choice never needs to be undone.
- Contrast with **Dynamic Programming**: if you find yourself needing to consider *both* "take it" and "skip it" and compare results, that's DP, not Greedy — the defining feature of Greedy is that you only ever explore *one* branch.

## Recommended study order

1. **Greedy** — start with classic interval scheduling (maximize non-overlapping intervals) since the exchange-argument proof is the clearest one to internalize first, then move to jump games and assignment-style problems.

Greedy is built as a Compact-tier module (README + code.cpp only). See [`../INDEX.md`](../INDEX.md) for details.
