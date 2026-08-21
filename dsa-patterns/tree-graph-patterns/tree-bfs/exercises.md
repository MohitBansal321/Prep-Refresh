# Tree BFS — Exercises

Work through these in order. The goal is to build three reflexes: (1) recognizing that a problem's natural unit of work is a **level**, even when the word "level" never appears in the statement; (2) writing the queue-and-snapshot skeleton so automatically that the only thing you actually think about is the per-node line inside the inner loop; and (3) knowing exactly when the pattern stops being Tree BFS and becomes Graph BFS.

> Rule of thumb for every exercise: before writing a single line, say out loud where the **level-size snapshot** goes, what the **per-level state** is (a fresh vector? a cursor? nothing?), and what the **per-node work** is. If you can answer those three, the code is already written. If you cannot, you are not ready to type yet. All five problems below are different from the four worked solutions in [problems/](problems/) — do not look those up for the answer, look them up for the *shape*.

---

## Easy — Average of Levels in Binary Tree

**LeetCode 637 — Average of Levels in Binary Tree.**

Given the root of a binary tree, return a `vector<double>` of the average value of the nodes on each level, ordered from the root's level downward.

**Task:** adapt [problems/01-binary-tree-level-order-traversal.cpp](problems/01-binary-tree-level-order-traversal.cpp) directly. The traversal is byte-for-byte the same; replace the per-level `vector<int> level_values` with a running `long long level_sum`, and at the commit point push `static_cast<double>(level_sum) / level_size` instead of the vector. Note that `level_size` — the snapshot you already took — *is* the denominator, so you get it for free and never need a separate counter.

**Think about:** why is `long long` the right accumulator type here rather than `int`? LeetCode's constraints allow up to 10⁴ nodes with values up to 2³¹−1, so a single wide level can overflow a 32-bit sum before the division ever happens. **Then answer:** this is the first exercise where the snapshot serves *two* purposes at once (loop bound and arithmetic denominator). Name the other three problems in [problems/](problems/) and say, for each, whether its snapshot is used for one purpose or two.

---

## Medium — Binary Tree Right Side View

**LeetCode 199 — Binary Tree Right Side View.**

Imagine standing to the right of a binary tree and looking at it. Return the values of the nodes you can see, ordered top to bottom — that is, the **rightmost** node of every level.

**Task:** start from [problems/01-binary-tree-level-order-traversal.cpp](problems/01-binary-tree-level-order-traversal.cpp) again, but do not build a per-level vector at all. Inside the inner loop, test `if (i == level_size - 1)` and push that node's value — the last node dequeued from a level's snapshot batch is, by FIFO ordering, the rightmost node on that level. Solve it a second way as well: keep the batch's last-seen node in a `TreeNode* last` cursor and commit `last->val` after the inner loop, which is structurally the same move [problems/04-populating-next-right-pointers-ii.cpp](problems/04-populating-next-right-pointers-ii.cpp) makes with its `prev` pointer.

**Think about:** the naive "just follow `node->right` down from the root" approach is wrong. Construct the smallest tree that breaks it (hint: a root whose right child is a leaf, and whose left child has a left child of its own), and state in one sentence why "rightmost on a level" is a fundamentally different question from "reachable by only taking right turns." **Then answer:** if you solved it with the `last` cursor version, where exactly must that cursor be declared, and what specific bug appears if you declare it above the outer loop instead? You have seen that bug named before — say which file's header comment calls it out.

---

## Hard — Serialize and Deserialize Binary Tree

**LeetCode 297 — Serialize and Deserialize Binary Tree.**

Design an algorithm to encode an arbitrary binary tree into a single string, and another to decode that string back into the identical tree structure. Values may be negative; nodes may have one child or none.

**Task:** solve both halves with BFS, not recursion — the README's Real Interview/Production Examples section calls out level-by-level serialization as a real production shape precisely because a deserializer can then rebuild the tree top-down, allocating each level before it needs to know anything about the next one. For `serialize`, run the standard skeleton but push children **unconditionally**, emitting a `"#"` placeholder for each null instead of skipping it — the placeholders are what make the string unambiguous. For `deserialize`, use a *second* queue holding the parent nodes still waiting for children, and consume the token stream two at a time; the `buildTree` helper at the bottom of [problems/04-populating-next-right-pointers-ii.cpp](problems/04-populating-next-right-pointers-ii.cpp) is already exactly this algorithm, written for a `vector<int>` with an `INT_MIN` sentinel instead of a string with `"#"` tokens — read it, then port it.

**Think about:** this is the one exercise here where the "always null-check before pushing" rule from the README's Common Mistakes is *deliberately broken* on the serialize side. Explain why that is correct rather than contradictory — what changes about the invariant when the queue's job is to produce a faithful transcript rather than to visit real nodes? **Then answer:** two questions on the encoding itself. First, must you emit placeholders for the children of nodes on the very last level, or can the string stop early? Prove your answer by writing out the exact strings for a single node and for a left-skewed chain of three. Second, `deserialize` never needs a level-size snapshot at all — why not? What replaces it as the thing that keeps parents and children aligned?

---

## Real-World Challenge — Tenant Hierarchy Config Rollout

You own a NestJS service that manages a hierarchy of customer tenants: a parent enterprise account, its sub-accounts, their sub-accounts, and so on, stored in Postgres as an adjacency list (`tenants(id, parent_id, name)`). Product wants to roll a risky config change out **one hierarchy level at a time**, pausing between levels so an on-call engineer can inspect metrics and abort before the change reaches more customers.

**Task:**

1. Implement `getLevel(rootTenantId, depth)`: return exactly the tenant IDs at a given depth below the root. Use the level-size snapshot as the batch boundary, exactly as in [code.cpp](code.cpp)'s `levelOrder`. Note the real-world twist that the C++ version does not have: each level's children come from a Postgres query, so the traversal must issue **one query per level** (`SELECT id FROM tenants WHERE parent_id = ANY($1)`), not one per node. Explain why the snapshot maps naturally onto exactly one batched query per level, and estimate how many queries a 5-level, 10,000-tenant hierarchy costs under each approach.

2. Add per-level rollout metrics: for each level, record the tenant count, the count that applied the change successfully, and the failure count. This is per-level aggregation — the commit point after the inner loop, as in the Easy exercise above, not per-node bookkeeping.

3. Make the rollout **resumable**. The process may be killed between levels, and on restart must continue from the next unprocessed level rather than starting over. Persist the frontier (the current queue's contents) plus the level number to Redis at each commit point. Explain why the commit point — after the inner loop finishes, never in the middle of it — is the only safe place to checkpoint, and what specifically goes wrong if you checkpoint mid-level and then crash.

4. Add `findNearestOverride(tenantId)`: search **outward from a leaf tenant toward the root** for the closest ancestor that has an explicit config override, returning the number of hops. This is `minDepth`'s early exit ([problems/03-minimum-depth-of-binary-tree.cpp](problems/03-minimum-depth-of-binary-tree.cpp)) pointed in the opposite direction — the first match found is provably the nearest, so you stop immediately.

5. **Discuss:** the README's Disadvantages section says Tree BFS costs O(n) space because the widest level lives in the queue at once. In this service the "queue" is a Redis list of tenant IDs, so O(n) means a real memory line item on a real box, and one enterprise account with 200,000 direct sub-accounts would put all 200,000 IDs in one frontier. Work through three options and argue for one: (a) accept it, since IDs are small; (b) chunk each level into fixed-size pages and process a level as several sequential batches; (c) switch to a DFS walk with an O(h) stack. For each, say what you lose — specifically, which of the four features above (level batching, per-level metrics, resumability, nearest-ancestor search) still works, and which one breaks first. Then answer the design question underneath: does option (b) still deserve to be called Tree BFS, and does that label matter?

---

## Bonus Challenge — All Nodes Distance K in Binary Tree

**LeetCode 863 — All Nodes Distance K in Binary Tree.**

Given the root of a binary tree, a target node, and an integer `k`, return the values of all nodes that are **exactly distance `k`** from the target node, where distance is the number of edges between two nodes — counted in *any* direction, up toward the root as well as down.

**Task:** this problem is in this module deliberately, as the exercise that shows you the exact boundary where Tree BFS ends. The distance-`k` question is a "fewest hops" question, so BFS is right — but hops may travel *upward*, and a binary tree's `left`/`right` pointers only go down. So: first do a preparation pass (a plain DFS or BFS) building an `unordered_map<TreeNode*, TreeNode*>` of child → parent. Now every node has three neighbours — `left`, `right`, and `parent` — and you can run the standard queue-and-snapshot loop over that neighbourhood, treating `k` as a level counter and returning the whole level when the counter hits `k`.

**Think about:** the moment you added parent pointers, the structure stopped being a tree and became an **undirected graph**, and you now need a `visited` set — otherwise you walk from a node to its parent and immediately back down to the same node, oscillating forever and inflating every distance. Reconcile this precisely with the README's Similar Patterns table, which states that Tree BFS never needs `visited`. Both statements are true; explain exactly what changed about the input, not about the algorithm.

**Then, generalize in writing (no code required):** using the Architecture and Complexity sections of the [README](README.md), argue whether this problem belongs in this module or in [../graph-bfs-dfs/](../graph-bfs-dfs/). Be specific about which parts of your solution are pure Tree BFS (name the exact lines: the snapshot, the level counter, the commit) and which parts are Graph BFS (the neighbour enumeration, the `visited` set). Then answer the question underneath: if a `visited` set is what distinguishes the two patterns, and adding parent pointers to a tree forces you to add one, is "Tree BFS" a fact about the algorithm or a fact about the input you were handed? Compare with the sibling module's treatment of the same boundary — [../tree-dfs/](../tree-dfs/) makes the parallel argument for why generic graph DFS with a `visited` set is pure overhead on a genuine tree.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
