# DP on Grids — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing the "grid + monotone movement + count/optimize a path" signal that means "fill a table," and (2) correctly identifying, before writing any code, what `dp[row][col]` *means*, which neighbors it depends on, and what the base cases along the top row and left column are.

> Rule of thumb for every exercise: before writing a single line, ask "can I only arrive at a cell from already-filled neighbors?" If movement lets you reach a cell from below or from the right, stop — you are in BFS/Dijkstra territory, not this pattern. If yes, write down the recurrence in plain English first; the code is then mechanical.

---

## Easy — Triangle

**LeetCode 120 — Triangle.**

Given a `triangle` array (row `i` has `i + 1` numbers), return the minimum path sum from top to bottom. At each step you may move to an adjacent number on the row below.

**Constraints to notice:** this is the grid recurrence wearing a triangle costume — cell `(r, c)` is reachable only from `(r-1, c-1)` and `(r-1, c)`, i.e. still only from "already-computed" cells if you fill top to bottom. The edges of each row have only one parent, exactly like the top row / left column base case in a rectangular grid. Negative values are allowed, so you cannot prune by "this partial sum is already too big."

**Task:** solve it with a bottom-up table in O(n^2) time, then collapse your space usage to O(n) (one rolling row) — the same compression move as going from a full 2D grid table to a single row.

**Think about:** why does bottom-up (starting from the triangle's last row) make the final answer just `dp[0][0]`, while top-up would require taking a `min` over the entire last row at the end? Which direction has fewer edge cases, and why?

---

## Medium — Minimum Path Cost of a Grid

**LeetCode 2304 — Minimum Path Cost of a Grid.**

You are given a 0-indexed grid of zeros and ones, plus a `cost` matrix of size `(rows * cols) x (rows * cols)`. You start in any cell of column 0 and must reach some cell of the last column, moving only to the cell directly right, diagonally right-up, or diagonally right-down. Moving from cell `(r1, c1)` to `(r2, c2)` costs `cost[grid[r1][c1]][grid[r2][c2]]`. Return the minimum total cost.

**Constraints to notice:** movement is still strictly left-to-right (columns never decrease), so the acyclic-dependency property holds even though the cost lives on *transitions between cells* rather than on cells themselves. The state is still `(row, col)` — but the value being combined is an edge weight, not a node weight.

**Task:** define `dp[r][c]` = minimum cost to *arrive at* column `c`, row `r`, and derive it from the (up to three) cells in column `c - 1` that can reach it. Watch the boundary rows: from row 0 there is no up-diagonal predecessor, from row `rows - 1` no down-diagonal one.

**Think about:** how does this recurrence differ structurally from Minimum Path Sum's `min(fromTop, fromLeft) + grid[r][c]`? Write both side by side and identify exactly which piece moved from "node" to "edge." Could you still roll the table to O(rows) space here, and why?

---

## Hard — Dungeon Game

**LeetCode 174 — Dungeon Game.**

A knight starts at the top-left of a grid containing empty rooms (0), demons (negative values draining health), and magic orbs (positive values). He must reach the bottom-right, moving only right or down, and his health must **never** drop to 0 or below at any point along the way. Return the *minimum initial health* needed.

**Constraints to notice:** movement is still right/down — the recognition signal holds — but the naive forward fill fails: minimizing damage taken so far does not minimize the starting health required, because a cheap prefix can walk you into a suffix you cannot survive. The correct quantity to tabulate is backward-looking: `dp[r][c]` = minimum health needed *when entering* `(r, c)` so that the rest of the journey is survivable.

**Task:** fill the table from the bottom-right corner upward (right-to-left within each row, bottom-to-top across rows). The recurrence reads off the *right* and *below* neighbors instead of above/left, and the base case at the princess's cell is derived from needing health > 0 after absorbing her cell's value.

**Then answer:** why does forward DP fail here when it succeeds for Minimum Path Sum? State it precisely in terms of what quantity each formulation is optimizing (hint: min-sum-so-far vs. min-required-starting-health — one of them composes left-to-right, the other only right-to-left).

---

## Real-World Challenge — Warehouse Robot Cost Map with a Memory Budget

You run fulfillment-center software. A robot must travel from the depot (top-left of an `R x C` cost map) to a packing station (bottom-right), moving only east or south (the aisles are one-way). Each tile has a traversal cost (seconds: congestion, floor type, scan stations). Two hard constraints from your platform team:

1. The robot's onboard planner runs on a microcontroller with enough RAM for roughly two rows of the map — not the whole `R x C` table.
2. Some tiles are periodically blocked by parked pallets; the planner must treat them as impassable without changing its memory footprint.

**Task:**
1. Design (and implement, with `std::vector<std::vector<int>>` standing in for the cost map) a planner that computes the minimum travel time using O(C) memory — one rolling row plus whatever scalar temporaries you need. Verify it produces identical answers to a full-table version on several random maps.
2. Layer in blocked tiles: a blocked tile contributes no cost and no paths — decide what value your rolling row stores for it and prove by hand-tracing a 3x3 example that blockages correctly "cut" routes that pass through them.
3. Discuss: your ops team now wants the robot to also be able to move north inside an aisle to dodge a pallet. Explain exactly where the rolling-row argument collapses once northward movement is allowed, and what algorithm family you would switch to (name it, and state what it optimizes and at what complexity).

---

## Bonus Challenge — Maximal Square

**LeetCode 221 — Maximal Square.**

Given an `m x n` binary matrix filled with 0s and 1s, find the largest square containing only 1s and return its area.

**Task:** solve it with a grid DP where `dp[r][c]` = side length of the largest all-ones square whose *bottom-right corner* is at `(r, c)` — note the deliberate shift: the answer stored at a cell describes a square ending there, not a path arriving there. The recurrence takes the **min of three neighbors** (up, left, and up-left diagonal) plus one.

**Then, generalize in writing (no code required):** the template in [code.cpp](code.cpp) combines exactly two neighbors (above, left). This exercise combines three (above, left, and the diagonal). In your own words, explain what the general rule actually is for "which neighbors am I allowed to read" in a grid DP — it is not literally "above and left," so what is the real requirement? Justify your answer using the Core Idea section of the [README](README.md).

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
