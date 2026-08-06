/**
 * COMPOSITE PATTERN — Production-style TypeScript example
 * -------------------------------------------------------
 * Scenario: We are building the backend for a cloud file browser (think an
 * S3-style object explorer, a Google-Drive-like storage service, or the volume
 * inside a Docker image). Storage is a TREE: directories contain files AND
 * other directories, nested arbitrarily deep.
 *
 * The client (e.g. a StorageService in NestJS) constantly asks questions that
 * must recurse through the whole subtree:
 *   - "How many bytes does this folder occupy?"  -> getSizeInBytes()
 *   - "How many files are under here?"            -> countFiles()
 *   - "Render the tree for the UI."               -> render()
 *
 * We do NOT want the client to write `if (node is File) ... else if (node is
 * Directory) ...` everywhere. We want to call `node.getSizeInBytes()` on ANY
 * node and have it Just Work — whether it is a single file or a folder holding
 * ten thousand files.
 *
 * We solve this with the Composite Pattern:
 *   - FileSystemNode -> Component  (the uniform interface every node honors)
 *   - FileNode       -> Leaf       (no children; the recursion base case)
 *   - DirectoryNode  -> Composite  (holds children; delegates + aggregates)
 *   - StorageService -> Client     (treats leaves and composites identically)
 *
 * Design decisions demonstrated here (see README for the full discussion):
 *   1. SAFETY over transparency: child-management methods (add/remove) live ONLY
 *      on DirectoryNode, not on the shared FileSystemNode interface. The client
 *      cannot call `file.add(...)` and get a runtime surprise.
 *   2. CACHING aggregates: DirectoryNode memoizes its total size and invalidates
 *      the cache (up the parent chain) on mutation, so repeated reads are O(1).
 *   3. DEPTH SAFETY: a Visitor + an ITERATIVE traversal are provided so very deep
 *      trees do not blow the call stack.
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 1. COMPONENT — the uniform interface every node in the tree honors.
//    The Client depends ONLY on this. It never checks whether a node is a
//    file or a directory before calling these methods.
// =============================================================================

export interface FileSystemNode {
  /** File or directory name (not the full path). */
  getName(): string;

  /**
   * Total size in bytes. For a file this is its own size; for a directory it is
   * the RECURSIVE sum of everything beneath it. This single method is why the
   * client can stay ignorant of the leaf/composite distinction.
   */
  getSizeInBytes(): number;

  /** Number of FILES (leaves) at or below this node. A file counts as 1. */
  countFiles(): number;

  /** Human-readable tree rendering, indented by depth. */
  render(indent?: string): string;

  /**
   * Accept a visitor (Visitor pattern). Lets us add new tree-wide operations
   * (virus scan, checksum, permission audit) WITHOUT editing every node class.
   * Composite and Visitor are natural partners.
   */
  accept<R>(visitor: FileSystemVisitor<R>): R;
}

// =============================================================================
// 2. LEAF — an individual object with no children. This is the base case that
//    stops the recursion.
// =============================================================================

export class FileNode implements FileSystemNode {
  constructor(
    private readonly name: string,
    private readonly sizeInBytes: number,
    /** e.g. "image/png"; part of realistic file metadata. */
    private readonly contentType: string = "application/octet-stream",
  ) {
    if (sizeInBytes < 0) {
      throw new Error(`File "${name}" cannot have negative size`);
    }
  }

  getName(): string {
    return this.name;
  }

  getSizeInBytes(): number {
    // Base case: a leaf just returns its own size. No recursion.
    return this.sizeInBytes;
  }

  countFiles(): number {
    return 1;
  }

  getContentType(): string {
    return this.contentType;
  }

  render(indent = ""): string {
    return `${indent}📄 ${this.name} (${formatBytes(this.sizeInBytes)})`;
  }

  accept<R>(visitor: FileSystemVisitor<R>): R {
    return visitor.visitFile(this);
  }
}

// =============================================================================
// 3. COMPOSITE — a node that HOLDS children and delegates work to them, then
//    aggregates the results. This is where the recursion lives.
// =============================================================================

export class DirectoryNode implements FileSystemNode {
  /** The children. Kept private so the tree can maintain its invariants. */
  private readonly children: FileSystemNode[] = [];

  /**
   * Cached total size. `null` means "dirty / not computed yet". We memoize
   * because getSizeInBytes() is called repeatedly (UI, quota checks, billing)
   * and recomputing an O(n) subtree walk every time is wasteful.
   */
  private cachedSize: number | null = null;

  /**
   * Back-reference to the parent so cache invalidation can propagate UPWARD.
   * When a file is added deep in the tree, every ancestor's cached size is now
   * stale and must be cleared.
   */
  private parent: DirectoryNode | null = null;

  constructor(private readonly name: string) {}

  // --- Child-management methods live ONLY here (the "safety" variant). --------
  // A FileNode has no add/remove, so the client physically cannot misuse it.

  add(node: FileSystemNode): this {
    this.children.push(node);
    if (node instanceof DirectoryNode) {
      node.parent = this;
    }
    this.invalidateSizeCache();
    return this; // fluent API for building trees
  }

  remove(node: FileSystemNode): void {
    const index = this.children.indexOf(node);
    if (index === -1) {
      throw new Error(`"${node.getName()}" is not a child of "${this.name}"`);
    }
    this.children.splice(index, 1);
    if (node instanceof DirectoryNode) {
      node.parent = null;
    }
    this.invalidateSizeCache();
  }

  /** Read-only view of children — never hand out the mutable internal array. */
  getChildren(): readonly FileSystemNode[] {
    return this.children;
  }

  // --- Uniform Component methods: delegate to children, then aggregate. -------

  getName(): string {
    return this.name;
  }

  getSizeInBytes(): number {
    if (this.cachedSize !== null) {
      return this.cachedSize; // O(1) fast path
    }
    // Aggregate: ask EVERY child for its size and sum. Each child answers the
    // SAME question; we neither know nor care if a child is a file or a folder.
    let total = 0;
    for (const child of this.children) {
      total += child.getSizeInBytes(); // recursion happens here
    }
    this.cachedSize = total;
    return total;
  }

  countFiles(): number {
    let count = 0;
    for (const child of this.children) {
      count += child.countFiles(); // files -> 1, dirs -> their own recursion
    }
    return count;
  }

  render(indent = ""): string {
    const header = `${indent}📁 ${this.name}/ (${formatBytes(
      this.getSizeInBytes(),
    )}, ${this.countFiles()} files)`;
    const body = this.children
      .map((child) => child.render(indent + "  "))
      .join("\n");
    return body ? `${header}\n${body}` : header;
  }

  accept<R>(visitor: FileSystemVisitor<R>): R {
    return visitor.visitDirectory(this);
  }

  /**
   * Invalidate this node's cached size AND every ancestor's, because their
   * totals now include stale data. This keeps the memoization correct under
   * mutation — the classic "cache invalidation" problem, solved locally.
   */
  private invalidateSizeCache(): void {
    this.cachedSize = null;
    this.parent?.invalidateSizeCache();
  }
}

// =============================================================================
// 4. VISITOR — add new whole-tree operations without touching node classes.
//    (Open/Closed Principle: open for new operations, closed for modification.)
// =============================================================================

export interface FileSystemVisitor<R> {
  visitFile(file: FileNode): R;
  visitDirectory(dir: DirectoryNode): R;
}

/**
 * Example visitor: find every file larger than a threshold. Adding this did NOT
 * require editing FileNode or DirectoryNode — that is the payoff of Visitor.
 */
export class LargeFileFinder implements FileSystemVisitor<FileNode[]> {
  constructor(private readonly thresholdBytes: number) {}

  visitFile(file: FileNode): FileNode[] {
    return file.getSizeInBytes() >= this.thresholdBytes ? [file] : [];
  }

  visitDirectory(dir: DirectoryNode): FileNode[] {
    return dir
      .getChildren()
      .flatMap((child) => child.accept(this)); // recurse via double-dispatch
  }
}

// =============================================================================
// 5. ITERATIVE TRAVERSAL — depth-safe alternative to recursion for VERY deep
//    trees (recursion risks a stack overflow around ~10k+ frames in Node).
//    This is a generator, so callers can stream nodes lazily.
// =============================================================================

export function* walkIterative(
  root: FileSystemNode,
): Generator<FileSystemNode> {
  // Explicit stack on the heap instead of the call stack.
  const stack: FileSystemNode[] = [root];
  while (stack.length > 0) {
    const node = stack.pop()!;
    yield node;
    if (node instanceof DirectoryNode) {
      // Push children so they get visited next (LIFO => depth-first).
      for (const child of node.getChildren()) {
        stack.push(child);
      }
    }
  }
}

// =============================================================================
// 6. CLIENT — business logic. Depends ONLY on FileSystemNode. Notice there is
//    NOT A SINGLE `instanceof` check here: the whole point of Composite.
// =============================================================================

export interface StorageQuota {
  maxBytes: number;
}

export class StorageService {
  constructor(private readonly quota: StorageQuota) {}

  /** Works identically whether given a single file or a huge folder tree. */
  usageReport(node: FileSystemNode): string {
    const used = node.getSizeInBytes();
    const pct = ((used / this.quota.maxBytes) * 100).toFixed(1);
    return (
      `"${node.getName()}" uses ${formatBytes(used)} across ` +
      `${node.countFiles()} files (${pct}% of quota).`
    );
  }

  assertWithinQuota(node: FileSystemNode): void {
    if (node.getSizeInBytes() > this.quota.maxBytes) {
      throw new Error(
        `Quota exceeded: "${node.getName()}" needs ` +
          `${formatBytes(node.getSizeInBytes())} but limit is ` +
          `${formatBytes(this.quota.maxBytes)}.`,
      );
    }
  }
}

// =============================================================================
// 7. UTIL — small helper so sizes print nicely. Kept pure and injectable-free
//    since it has no dependencies.
// =============================================================================

export function formatBytes(bytes: number): string {
  if (bytes < 1024) return `${bytes} B`;
  const units = ["KB", "MB", "GB", "TB"];
  let value = bytes / 1024;
  let unit = 0;
  while (value >= 1024 && unit < units.length - 1) {
    value /= 1024;
    unit++;
  }
  return `${value.toFixed(1)} ${units[unit]}`;
}

// =============================================================================
// 8. COMPOSITION ROOT / DEMO — build a tree, then run client operations that
//    are blissfully unaware of the leaf/composite distinction.
// =============================================================================

function buildSampleTree(): DirectoryNode {
  const root = new DirectoryNode("project");

  const src = new DirectoryNode("src")
    .add(new FileNode("index.ts", 2_048, "text/typescript"))
    .add(new FileNode("app.ts", 8_192, "text/typescript"));

  const assets = new DirectoryNode("assets")
    .add(new FileNode("logo.png", 512_000, "image/png"))
    .add(new FileNode("hero.jpg", 3_145_728, "image/jpeg"));

  src.add(assets); // a directory inside a directory — arbitrary nesting

  root
    .add(src)
    .add(new FileNode("README.md", 4_096, "text/markdown"))
    .add(new FileNode("package.json", 1_024, "application/json"));

  return root;
}

function main(): void {
  const tree = buildSampleTree();

  console.log("=== Tree ===");
  console.log(tree.render());

  const service = new StorageService({ maxBytes: 10 * 1024 * 1024 }); // 10 MB
  console.log("\n=== Usage (called on the ROOT directory) ===");
  console.log(service.usageReport(tree));

  console.log("\n=== Usage (called on a single FILE — same method!) ===");
  const singleFile = new FileNode("standalone.bin", 700_000);
  console.log(service.usageReport(singleFile));

  console.log("\n=== Visitor: files >= 400 KB ===");
  const big = tree.accept(new LargeFileFinder(400 * 1024));
  big.forEach((f) => console.log(`  ${f.getName()} (${formatBytes(f.getSizeInBytes())})`));

  console.log("\n=== Iterative walk (depth-safe) ===");
  for (const node of walkIterative(tree)) {
    console.log(`  visited: ${node.getName()}`);
  }

  console.log("\n=== Cache invalidation on mutation ===");
  console.log(`  before: ${formatBytes(tree.getSizeInBytes())}`);
  tree.add(new FileNode("huge.zip", 5_000_000));
  console.log(`  after adding huge.zip: ${formatBytes(tree.getSizeInBytes())}`);
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main();
}
