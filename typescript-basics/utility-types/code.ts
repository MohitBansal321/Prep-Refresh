/**
 * BUILT-IN UTILITY TYPES — Production-style TypeScript example
 * --------------------------------------------------------------
 * Scenario: We have one canonical `User` entity (as stored in a database).
 * Several parts of the application need DIFFERENT SHAPES of that same
 * entity: a PATCH-update body, a public profile view, a safe API response
 * (never leaking the password hash), and a role -> permissions lookup.
 *
 * Instead of hand-writing a parallel interface for each shape (which would
 * silently drift out of sync whenever `User` changes), we DERIVE every shape
 * from `User` using TypeScript's built-in utility types:
 *   - Partial<T>    -> make every property optional
 *   - Required<T>   -> make every property mandatory
 *   - Readonly<T>   -> make every property immutable
 *   - Pick<T, K>    -> keep only some properties (allow-list)
 *   - Omit<T, K>    -> drop some properties (deny-list)
 *   - Record<K, V>  -> build an object type from a key set
 *   - ReturnType<T> -> extract a function's return type
 *   - Parameters<T> -> extract a function's parameter tuple
 *   - Exclude/Extract<T, U> -> filter members of a union type
 *
 * All of these are themselves just mapped types and conditional types —
 * we show the equivalent hand-rolled versions in comments so the "magic"
 * is demystified.
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 1. SOURCE OF TRUTH — the one real entity. Every derived shape below is
//    computed FROM this type, never hand-duplicated.
// =============================================================================

export type UserRole = "admin" | "editor" | "viewer";
export type Permission = "read" | "write" | "delete" | "publish";

export interface User {
  id: string;
  name: string;
  email: string;
  passwordHash: string;
  role: UserRole;
  createdAt: Date;
}

// =============================================================================
// 2. Partial<T> — every property becomes optional.
//    Equivalent hand-rolled mapped type:
//      type Partial<T> = { [K in keyof T]?: T[K] };
//    Use case: a PATCH /users/:id body — the caller sends only the fields
//    they want to change. `id` is removed first via Omit, because identity
//    should never be part of an update payload.
// =============================================================================

export type UpdateUserDto = Partial<Omit<User, "id">>;

export function applyUserUpdate(user: User, patch: UpdateUserDto): User {
  // Object.assign / spread only overwrites the keys actually present in patch.
  return { ...user, ...patch };
}

// =============================================================================
// 3. Required<T> — every property becomes mandatory (removes "?").
//    Equivalent hand-rolled mapped type:
//      type Required<T> = { [K in keyof T]-?: T[K] };
//    Use case: a "draft" profile where some fields are optional while the
//    user is filling out a form, and a "complete" profile that guarantees
//    every field has been filled in before we allow account activation.
// =============================================================================

export interface UserDraft {
  name?: string;
  email?: string;
  role?: UserRole;
}

export type CompleteUserDraft = Required<UserDraft>;

export function activateAccount(draft: CompleteUserDraft): void {
  // TypeScript guarantees every field is present here — no "possibly undefined" checks needed.
  console.log(`[activateAccount] Activating ${draft.name} <${draft.email}> as ${draft.role}`);
}

// =============================================================================
// 4. Readonly<T> — every property becomes immutable after creation.
//    Equivalent hand-rolled mapped type:
//      type Readonly<T> = { readonly [K in keyof T]: T[K] };
//    Use case: a user object handed out from an in-memory cache. Callers may
//    read it freely but must not mutate the cached instance directly.
// =============================================================================

export type FrozenUser = Readonly<User>;

export function getCachedUser(cache: Map<string, User>, id: string): FrozenUser | undefined {
  const user = cache.get(id);
  return user; // structurally assignable to Readonly<User>; mutation is now a compile error for callers
}

// =============================================================================
// 5. Pick<T, K> — keep only the listed keys (an allow-list).
//    Equivalent hand-rolled mapped type:
//      type Pick<T, K extends keyof T> = { [P in K]: T[P] };
//    Use case: a public profile page. Deliberately conservative — only the
//    fields explicitly named here are ever exposed, even if User grows.
// =============================================================================

export type PublicProfile = Pick<User, "id" | "name" | "email">;

export function toPublicProfile(user: User): PublicProfile {
  return { id: user.id, name: user.name, email: user.email };
}

// =============================================================================
// 6. Omit<T, K> — keep every key EXCEPT the listed ones (a deny-list).
//    Equivalent hand-rolled type (Omit is Pick + Exclude combined):
//      type Exclude<T, U> = T extends U ? never : T;
//      type Omit<T, K extends keyof any> = Pick<T, Exclude<keyof T, K>>;
//    Use case: the shape returned from every API response. Built as
//    "everything except passwordHash" so any NEW field added to User is
//    included automatically, while the one field that must never leak stays excluded.
// =============================================================================

export type SafeUser = Omit<User, "passwordHash">;

export function toSafeUser(user: User): SafeUser {
  // Explicit destructure-and-drop at runtime — the type system does not
  // erase the field from the actual object, so we must remove it ourselves.
  const { passwordHash, ...safe } = user;
  return safe;
}

// =============================================================================
// 7. Record<K, V> — build an object type mapping every member of a key
//    union K to value type V.
//    Equivalent hand-rolled mapped type:
//      type Record<K extends keyof any, V> = { [P in K]: V };
//    Use case: a role -> permissions lookup table. Because UserRole is a
//    literal union, TypeScript FORCES every role to have an entry — forget
//    one and the object literal fails to compile.
// =============================================================================

export const rolePermissions: Record<UserRole, Permission[]> = {
  admin: ["read", "write", "delete", "publish"],
  editor: ["read", "write", "publish"],
  viewer: ["read"],
  // Try commenting out "viewer" above -> TypeScript will refuse to compile.
};

export function canUserDo(role: UserRole, action: Permission): boolean {
  return rolePermissions[role].includes(action);
}

// =============================================================================
// 8. ReturnType<T> / Parameters<T> — extract types OUT OF a function's
//    signature instead of hand-declaring them separately.
//    Equivalent hand-rolled conditional types (using `infer`):
//      type ReturnType<T extends (...a: any) => any> = T extends (...a: any) => infer R ? R : never;
//      type Parameters<T extends (...a: any) => any>  = T extends (...a: infer P) => any ? P : never;
// =============================================================================

function createUser(name: string, email: string, role: UserRole): User {
  return {
    id: `usr_${Math.random().toString(36).slice(2, 8)}`,
    name,
    email,
    passwordHash: "hashed:" + Math.random().toString(36).slice(2),
    role,
    createdAt: new Date(),
  };
}

// Instead of declaring "type NewUser = User" by hand, we pull it straight
// out of the factory's return type. If createUser's return shape ever
// changes, NewUser changes with it automatically.
export type NewUser = ReturnType<typeof createUser>;

// Parameters<typeof createUser> = [string, string, UserRole]
export type CreateUserArgs = Parameters<typeof createUser>;

/** A generic logging wrapper that works for ANY function, typed via Parameters/ReturnType. */
function withLogging<F extends (...args: any[]) => any>(
  label: string,
  fn: F,
): (...args: Parameters<F>) => ReturnType<F> {
  return (...args: Parameters<F>): ReturnType<F> => {
    console.log(`[withLogging] calling ${label} with`, args);
    const result = fn(...args);
    console.log(`[withLogging] ${label} returned`, result);
    return result;
  };
}

const loggedCreateUser = withLogging("createUser", createUser);

// =============================================================================
// 9. Exclude<T, U> / Extract<T, U> — filter members of a UNION type.
//    Note the difference in axis from Pick/Omit: those filter OBJECT KEYS,
//    these filter UNION MEMBERS.
// =============================================================================

/** Every role except "viewer" — e.g. roles allowed to log into the admin dashboard. */
export type StaffRole = Exclude<UserRole, "viewer">;

/** Only the roles that can publish content. */
export type PublishingRole = Extract<UserRole, "admin" | "editor">;

function isStaffRole(role: UserRole): role is StaffRole {
  return role !== "viewer";
}

// =============================================================================
// 10. DEMO — exercise every utility type against the same User domain.
// =============================================================================

async function main(): Promise<void> {
  // --- Partial: apply a partial update ---
  const original = createUser("Ada Lovelace", "ada@example.com", "editor");
  const updated = applyUserUpdate(original, { name: "Ada, Countess of Lovelace" });
  console.log("[Partial] updated name:", updated.name);

  // --- Required: only compiles once every field of the draft is filled in ---
  const draft: UserDraft = { name: "Grace Hopper", email: "grace@example.com", role: "admin" };
  activateAccount(draft as CompleteUserDraft);

  // --- Readonly: cached user cannot be mutated by callers ---
  const cache = new Map<string, User>([[original.id, original]]);
  const cached = getCachedUser(cache, original.id);
  console.log("[Readonly] cached user id:", cached?.id);
  // cached!.name = "Hacked"; // <- would be a compile error: Cannot assign to 'name' because it is a read-only property.

  // --- Pick: public profile only exposes id/name/email ---
  const profile = toPublicProfile(updated);
  console.log("[Pick] public profile:", profile);

  // --- Omit: safe user has no passwordHash field ---
  const safe = toSafeUser(updated);
  console.log("[Omit] safe user keys:", Object.keys(safe));

  // --- Record: exhaustive role -> permission lookup ---
  console.log("[Record] can viewer delete?", canUserDo("viewer", "delete"));
  console.log("[Record] can admin delete?", canUserDo("admin", "delete"));

  // --- ReturnType / Parameters: wrap createUser generically ---
  const wrappedUser: NewUser = loggedCreateUser("Alan Turing", "alan@example.com", "viewer");
  console.log("[ReturnType] wrapped user email:", wrappedUser.email);

  // --- Exclude / Extract: filter the role union ---
  const roles: UserRole[] = ["admin", "editor", "viewer"];
  const staffRoles = roles.filter(isStaffRole);
  console.log("[Exclude] staff roles:", staffRoles);
  console.log("[Extract] publishing roles are a subtype of:", ["admin", "editor"] as PublishingRole[]);
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main().catch((e) => {
    console.error(e);
    process.exit(1);
  });
}
