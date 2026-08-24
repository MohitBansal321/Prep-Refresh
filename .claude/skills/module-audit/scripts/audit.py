#!/usr/bin/env python
"""Audit study modules against the repo's template invariants.

Usage:
    python .claude/skills/module-audit/scripts/audit.py                 # whole repo
    python .claude/skills/module-audit/scripts/audit.py dsa-patterns    # one area
    python .claude/skills/module-audit/scripts/audit.py js/event-loop   # one module
    python .claude/skills/module-audit/scripts/audit.py --compile       # also compile every .cpp
    python .claude/skills/module-audit/scripts/audit.py --quiet         # failures only

Exit code 1 if any FAIL is reported.
"""
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", ".."))

# area -> module depth below the area dir, required files, expected diagrams, README headings
AREAS = {
    "js": dict(
        depth=1,
        required=["README.md", "code.js", "exercises.md", "cheatsheet.md"],
        diagrams=[],
        headings=["Summary", "Key Takeaways", "Further Reading"],
    ),
    "typescript-basics": dict(
        depth=1,
        required=["README.md", "code.ts", "cheatsheet.md"],
        diagrams=[],
        headings=["Intent", "Problem", "Solution", "Common Mistakes", "When To Use",
                  "When NOT To Use", "Summary", "Key Takeaways", "Further Reading"],
    ),
    "sys-design": dict(
        depth=2,
        required=["README.md", "code.ts", "exercises.md", "cheatsheet.md"],
        diagrams=["class-diagram.md", "sequence-diagram.md", "flow-diagram.md"],
        headings=["Intent", "Real Life Analogy", "Problem", "Why Not Other Solutions?",
                  "Solution", "Architecture", "Execution Flow", "Implementation",
                  "Code Walkthrough", "Advantages", "Disadvantages", "Tradeoffs",
                  "Complexity", "Common Mistakes", "When To Use", "When NOT To Use",
                  "Interview Discussion", "Summary", "Key Takeaways", "Further Reading"],
    ),
    "dsa-patterns": dict(
        depth=2,
        required=["README.md", "code.cpp", "exercises.md", "cheatsheet.md"],
        diagrams=["recognition-diagram.md", "flow-diagram.md", "trace-diagram.md"],
        headings=["Intent", "Problem", "Solution", "Complexity", "Common Mistakes",
                  "When To Use", "When NOT To Use", "Interview Discussion",
                  "Key Takeaways", "Further Reading"],
    ),
}

SKIP_DIRS = ("node_modules", "images", "problems", ".git")

LINK_RE = re.compile(r"\[[^\]]*\]\(([^)\s]+)(?:\s+\"[^\"]*\")?\)")
FENCE_RE = re.compile(r"```(\w+)\n(.*?)```", re.S)
CODEY = re.compile(r"\b(for|while|if|return|class |function)\b")


def read(path):
    with open(path, encoding="utf-8", errors="replace") as fh:
        return fh.read()


def find_modules(target):
    """Yield (area, module_dir) for every module under `target` (or the whole repo)."""
    for area, cfg in AREAS.items():
        area_dir = os.path.join(ROOT, area)
        if not os.path.isdir(area_dir):
            continue
        parents = [area_dir]
        for _ in range(cfg["depth"] - 1):
            nxt = []
            for p in parents:
                for d in sorted(os.listdir(p)):
                    full = os.path.join(p, d)
                    if os.path.isdir(full) and d not in SKIP_DIRS:
                        nxt.append(full)
            parents = nxt
        for p in parents:
            for d in sorted(os.listdir(p)):
                mod = os.path.join(p, d)
                if not os.path.isdir(mod) or d in SKIP_DIRS:
                    continue
                if not os.path.exists(os.path.join(mod, "README.md")):
                    continue
                if target and not os.path.abspath(mod).startswith(target):
                    continue
                yield area, mod


def check_links(md_path, issues):
    base = os.path.dirname(md_path)
    for target in LINK_RE.findall(read(md_path)):
        if target.startswith(("http://", "https://", "mailto:", "#")):
            continue
        clean = target.split("#")[0].replace("%20", " ")
        if not clean:
            continue
        if not os.path.exists(os.path.join(base, clean)):
            issues.append(("FAIL", "%s -> broken link %s" % (os.path.basename(md_path), target)))


def index_state(area, mod):
    """Return (registered, tier_cell) by looking this module up in its area INDEX.md."""
    index = os.path.join(ROOT, area, "INDEX.md")
    if not os.path.exists(index):
        return True, ""
    rel = os.path.relpath(mod, os.path.join(ROOT, area)).replace(os.sep, "/")
    needle = rel + "/README.md"
    for line in read(index).splitlines():
        if needle in line:
            cells = [c.strip() for c in line.strip().strip("|").split("|")]
            return True, (cells[1] if len(cells) > 1 else "")
    return False, ""


def audit_module(area, mod, do_compile):
    cfg = AREAS[area]
    issues = []
    have = set(os.listdir(mod))

    for f in cfg["required"]:
        if f not in have:
            issues.append(("FAIL", "missing %s" % f))

    registered, tier = index_state(area, mod)
    if not registered:
        issues.append(("FAIL", "not registered in %s/INDEX.md" % area))
    full = (not tier) or tier.lower().startswith("full")
    lvl = "FAIL" if full else "WARN"

    for d in cfg["diagrams"]:
        p = os.path.join(mod, "images", d)
        if not os.path.exists(p):
            issues.append((lvl, "missing images/%s" % d))
        elif "```mermaid" not in read(p):
            issues.append(("FAIL", "images/%s has no mermaid block" % d))

    if area == "dsa-patterns":
        pdir = os.path.join(mod, "problems")
        if not os.path.isdir(pdir):
            issues.append((lvl, "missing problems/"))
        else:
            entries = os.listdir(pdir)
            cpps = [f for f in entries if f.endswith(".cpp")]
            if len(cpps) < 4:
                issues.append(("WARN", "problems/ has %d worked solutions, spec says 4" % len(cpps)))
            if "README.md" not in entries:
                issues.append(("FAIL", "missing problems/README.md"))

    readme = os.path.join(mod, "README.md")
    if os.path.exists(readme):
        found = set(re.findall(r"^##\s+(.+?)\s*$", read(readme), re.M))
        for h in cfg["headings"]:
            if h not in found:
                issues.append(("WARN", "README missing '## %s'" % h))

    cs = os.path.join(mod, "cheatsheet.md")
    if os.path.exists(cs):
        body = read(cs)
        first = body.split("\n", 1)[0]
        if "One-Minute Cheatsheet" not in first:
            issues.append(("WARN", "cheatsheet title is not '<Topic> - One-Minute Cheatsheet'"))
        if "### Remember In One Sentence" not in body:
            issues.append(("WARN", "cheatsheet missing '### Remember In One Sentence'"))
        if "## Recall Questions" not in body:
            issues.append(("FAIL", "cheatsheet has no '## Recall Questions' - cannot enter the ladder"))
        else:
            tail = body.split("## Recall Questions", 1)[1]
            n = len(re.findall(r"^\s*\d+\.\s+\S", tail, re.M))
            if n < 8:
                issues.append(("WARN", "only %d recall questions (spec: 8-10)" % n))

    ex = os.path.join(mod, "exercises.md")
    if os.path.exists(ex):
        body = read(ex)
        for level in ("Easy", "Medium", "Hard"):
            if not re.search(r"^##\s+%s" % level, body, re.M):
                issues.append(("WARN", "exercises.md has no '## %s' section" % level))
        for lang, code in FENCE_RE.findall(body):
            if lang.lower() in ("cpp", "c++", "ts", "typescript", "js", "javascript"):
                lines = [l for l in code.strip().splitlines() if l.strip()]
                if len(lines) > 12 and len(CODEY.findall(code)) >= 3:
                    issues.append(("WARN", "exercises.md has a %d-line %s block - possible leaked solution"
                                   % (len(lines), lang)))

    for dirpath, dirnames, filenames in os.walk(mod):
        dirnames[:] = [d for d in dirnames if d not in SKIP_DIRS or d in ("images", "problems")]
        for f in filenames:
            if f.endswith(".md"):
                check_links(os.path.join(dirpath, f), issues)

    if do_compile:
        for dirpath, _, filenames in os.walk(mod):
            for f in sorted(filenames):
                if not f.endswith(".cpp"):
                    continue
                src = os.path.join(dirpath, f)
                out = os.path.join(tempfile.gettempdir(), "audit_out.exe")
                r = subprocess.run(["g++", "-std=c++17", "-Wall", src, "-o", out],
                                   capture_output=True, text=True)
                rel = os.path.relpath(src, mod).replace(os.sep, "/")
                if r.returncode != 0:
                    head = r.stderr.strip().splitlines()
                    issues.append(("FAIL", "%s does not compile: %s" % (rel, head[0] if head else "?")))
                elif r.stderr.strip():
                    issues.append(("WARN", "%s compiles with -Wall warnings" % rel))
    return dedupe(issues)


def dedupe(issues):
    seen = set()
    out = []
    for item in issues:
        if item not in seen:
            seen.add(item)
            out.append(item)
    return out


def audit_top_level():
    """Link-check the index and lookup guides, which no module owns."""
    out = {}
    for area in AREAS:
        for name in ("INDEX.md", "README.md", "PATTERN-RECOGNITION-GUIDE.md",
                     "QUICK-RECALL-GUIDE.md", "LEARNING-PATHS.md"):
            p = os.path.join(ROOT, area, name)
            if os.path.exists(p):
                found = []
                check_links(p, found)
                if found:
                    out["%s/%s" % (area, name)] = dedupe(found)
    return out


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    do_compile = "--compile" in sys.argv
    quiet = "--quiet" in sys.argv
    target = os.path.abspath(args[0]) if args else None

    fails = warns = mods = 0
    for area, mod in find_modules(target):
        mods += 1
        issues = audit_module(area, mod, do_compile)
        f = sum(1 for level, _ in issues if level == "FAIL")
        fails += f
        warns += len(issues) - f
        rel = os.path.relpath(mod, ROOT).replace(os.sep, "/")
        if not issues:
            if not quiet:
                print("ok   %s" % rel)
            continue
        if quiet and f == 0:
            continue
        print("%s %s" % ("FAIL" if f else "warn", rel))
        for level, msg in issues:
            if quiet and level != "FAIL":
                continue
            print("       [%s] %s" % (level, msg))

    if not target:
        for name, issues in sorted(audit_top_level().items()):
            fails += len(issues)
            print("FAIL %s" % name)
            for level, msg in issues:
                print("       [%s] %s" % (level, msg))

    print("")
    print("%d modules audited - %d failures, %d warnings" % (mods, fails, warns))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
