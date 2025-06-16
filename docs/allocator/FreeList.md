---
title: "The K&R Storage Allocator"
type: "algorithm"
version: "1"
date: "2025-06-15"
license: "cc-by-nc-sa-4.0"
---

# The K&R Storage Allocator

## Introduction

K&R C is quite out of date and the original code for the storage allocator is
riddled with issues.

It's difficult to read and rationalize what's happening due to a myriad of
these issues.

Let's see if we can modernize it and make it legible and compiler friendly.

## The Free List

The **Storage Allocator** is actually a **Free List**. The first of its kind
(in C, that is).

The original implementation used a `union` and a `struct`. The `union` was to
coerce the compiler into aligning _free blocks_ of allocated memory.

We don't have this issue today. We can simplify the original implementation
quite a bit.

```c
typedef struct FreeList {
    struct FreeList* next; /* next block if on `FreeList` */
    size_t size; /* size of this block */
} FreeList;
```

The structure is actually a circular linked-list. This means that the `Node`
(or `FreeList`) references itself.

We only ever append to the list and record the allocated memory region in
bytes. These are the only 2 fields we'll ever need.

## Opacity or Transparency? That is the question.

We'll stick to the original implementation as much as reasonably possible.

The original implementation is opaque and hides a lot of the internal details.
For example, pointer arithmetic is dense, control flow is obscure, and the use
of union makes field usage ambiguous. While efficient for its time, it lacks
the clarity modern readers expect.

We'll add in some utilities later on to mitigate this.

For now, we need a sentinel value to track the original head of the list and a
global head pointer. We'll make these static because the user is never intended
to interact with these variables. If they do, it could cause _undefined
behavior_ (UB).

```c
static FreeList* base = NULL; /** sentinel value */
static FreeList* head = NULL; /* start of `FreeList` (head) */
```

We also need to initialize and terminate the list.

### Initializing the FreeList

To initialize the list, we'll want to `malloc()` some data. You can use the
stack instead of the heap, but I wanted some consistency here. I also wanted to
free and reset the entire list if desired (probably not necessary, but fun
either way).

```c
bool freelist_initialize(void) {
    if (NULL == base) {
        base = (FreeList*) memory_alloc(sizeof(FreeList), alignof(FreeList));
        if (NULL == base) {
            return false;
        }
    }

    if (NULL == head) {
        base->next = base;
        base->size = 0;
        head = base;
    }

    return true;
}
```

First, we `malloc()` an aligned sentinel value to the base. You can use
`malloc()` here or `posix_memalign()`. `memory_alloc()` is simply a
`posix_memalign()` wrapper that handles the nuances for me.

Then we link the `base` to itself. This creates a circular linked-list. We
simply initialize the `size` to `0` because there's nothing allocated besides
the `head`. Finally, we assign the sentinel `base` to the `head`.

There is a reason behind the madness which will be revealed later on.

We use a boolean value to indicate the state of the initialization process. If
it's already initialized, it will simply return `true`, `false` otherwise.

I set it up this way to guard against reinitialization (which is bad... very
bad), but this allows us to call this anytime it's needed (or required).

For example, reinitializing the `FreeList` could cause serious issues like
memory leaks, dangling pointers, or even double-free errors if the caller still
holds references to previously allocated memory. By making initialization
idempotent and guarded, we at least make an attempt to eliminate that class of
bugs.

### Terminating the FreeList

Terminating the `FreeList` is a bit more challenging. I actually struggled with
this and GPT helped me resolve the initial issues I had with it. There's no
reference or guide for doing it this way (that I know of), so I take help
anyway I can get it.

The real challenge was handling the circular nature of the list without causing
infinite loops or accidentally freeing the sentinel. It’s easy to write a loop
that traverses forever or starts freeing memory from the wrong place.

```c
bool freelist_terminate(void) {
    if (NULL == base) {
        return false;
    }

    FreeList* current = base->next;
    while (current != base) {
        FreeList* next = current->next;
        memory_free(current);
        current = next;
    }

    memory_free(base);
    base = NULL;
    head = NULL;
    return true;
}
```

We need to check if the `base` is `NULL`. If it is, then there's probably no
list. If there is a `base`, then there probably is a list.

We grab the `current` node from the linked-list and set it to the next node.
Remember, the first node is the sentinel which is the _real_ head of the list.
I think this is the part that tripped me up on my first try.

We check if the `current` node is equal to the `base` node because the `head`
node is the `base` node. That was an unexpected tongue twister, but that's the
best I got at the moment.

From there, we simply traverse the list and attempt to free each `next` node.
Note that I don't need to check if `next` is `NULL` because it is a circular
linked-list. Also, on the off-chance that a node is `NULL`, `memory_free()`
will return and not call `free()` as a result.

While it's true we might concern ourselves with a potential infinite loop, it
should become quite clear later on that this would be due to a bug in the
implementation and not the fault of the user.

Once we've freed all nodes, we free the `base` and set `base` and `head` to
`NULL` and return `true` upon success.

## Free List Blocks

This next part is quite involved and will take multiple components to piece
together so that it's both legible and useful. We'll use `static` here because
these functions are intentionally kept private. We never expose these to the
API or the end-user. They are solely internal details.

### Hey there, Neighbor!

Let's say we have a region of memory managed by the `FreeList`. How do we know
where it starts, where it ends, and whether it's adjacent to the next block?

Assume the `FreeList` holds a block of allocated memory and points to it. This
means we need the start address and size of the address. If the end of this
block is equal to the next block, then it must be adjacent to the current
block.

```c
static bool freelist_block_is_neighbor(FreeList* a, FreeList* b) {
    return a + a->size == b;
}
```

This is pretty trivial for all intents and purposes. But lets clarify.

Suppose `a` is at address `0x1000` and has a `size` of `3` (i.e. spans 3
`FreeList` units). Then we can say:

```c
a + a->size == 0x1000 + 3 * sizeof(FreeList)
```

- `a` points to the current address block.
- The `size` of `a` tells us where the address space ends.
- If it is equal to b, then it must be its neighbor.

In C, pointer arithmetic on structs works like it does on arrays. `a + 1`
advances the pointer by `sizeof(*a)` bytes. So `a + a->size` gives us the
address just beyond the end of block `a`. If that equals `b`, then `b` must
begin immediately after `a` - they're neighbors in memory.

## Merging Neighbors

Lets say we're looking to insert a block that a user has freed. This means we
need to check the upper and lower boundaries. In other words, do we need to
merge the next neighbor or the previous neighbor with the current block.

### Merging the Next Neighbor

When a user frees a block, we attempt to insert it back into the `FreeList`. To
do this cleanly, we want to **merge** the block with its neighbors if they are
adjacent in memory — this reduces fragmentation and keeps the list compact.

Let’s focus on merging the **next** neighbor (i.e., the block that comes
_after_ the one being inserted).

Here’s the merge function we’ll use:

```c
static void freelist_block_merge_upward(FreeList* a, FreeList* prev) {
    a->size += prev->next->size;
    a->next = prev->next->next;
}
```

> Note: `a` is the block being inserted, and `prev->next` is the block
> immediately after it. If they are adjacent, we merge them by extending
> `a->size` and skipping over the now-merged neighbor in the linked-list.

In practice, this function is only called when we’ve confirmed that the two
blocks are contiguous:

```c
// Merge with upper neighbor if possible
if (freelist_block_is_neighbor(block, current->next)) {
    freelist_block_merge_upward(block, current);
} else {
    block->next = current->next;
}
```

Let’s walk through what this does:

- `block` is the memory region that the user just freed.
- `current` is a node in the `FreeList` — we’ve found the spot where `block`
  should be inserted.
- If `block` ends exactly where `current->next` begins, they are neighbors.
- In that case, we merge them by:

  1. Extending `block->size` by the size of `current->next`.
  2. Skipping `current->next` entirely in the linked-list.

This means:

```txt
Before merge:
[current] -> [block] -> [block->next] -> [...]

After merge:
[current] -> [merged block] -> [...]
```

If `block` and `current->next` are _not_ adjacent, we simply link `block` into
the list like this:

```c
block->next = current->next;
```

This ensures that `block` is still part of the `FreeList`, but no unnecessary
merging happens.

### Merging the Previous Neighbor

Merging with the **previous** neighbor is slightly more subtle than merging
forward, but the goal is the same: **reduce fragmentation** by coalescing
adjacent free blocks.

When inserting a block back into the `FreeList`, we check whether it directly
follows the previous block. If it does, we merge the two into one larger free
block.

Here’s the function:

```c
static void freelist_block_merge_downward(FreeList* a, FreeList* b) {
    a->size += b->size;
    a->next = b->next;
}
```

Let’s clarify the parameters:

- `a` is the _previous_ block in the list.
- `b` is the block being inserted (i.e. just freed).
- If `a + a->size == b`, we merge them.

> In this context, “merging downward means extending the **previous** block `a`
> downward to absorb `b`, eliminating the gap between them.

Here’s how it’s used in practice:

```c
// Merge with lower neighbor if possible
if (freelist_block_is_neighbor(current, block)) {
    freelist_block_merge_downward(current, block);
} else {
    current->next = block;
}
```

Let’s break it down:

- `block` is the block we want to insert (i.e., just freed).
- `current` is the last block we visited while traversing the list — it comes
  _before_ `block` in memory.
- If `current` ends where `block` begins, we merge them by:

  1. Extending `current->size` to include `block`.
  2. Skipping `block` in the linked-list by pointing `current->next` to
     `block->next`.

Diagram:

```txt
Before merge:
[current] -> [block] -> [...]

After merge:
[current (now larger)] -> [...]
```

If the blocks are **not** neighbors, we simply insert `block` after `current`:

```c
current->next = block;
```

This preserves the structure of the circular `FreeList` without coalescing.

## Free Block Insertion

Once a new block is created — either by splitting or allocating new memory — we
need to insert it back into the `FreeList`. This process is crucial for
maintaining the integrity of the allocator. We always insert blocks in sorted
address order and merge adjacent blocks to minimize fragmentation.

### Inserting New Blocks

Let’s start with the function signature and walk through it.

```c
static void freelist_block_insert(void* ptr)
```

Since the `FreeList` is managed internally, we don't need to return anything.
We simply pass in a pointer to the user’s freed memory, and the allocator takes
care of reintegrating it.

The first thing we do is guard against a `NULL` pointer:

```c
if (NULL == ptr) {
    return;
}
```

Then we derive the block header from the pointer. Remember, when we allocate
memory, the header is stored _before_ the user’s visible pointer:

```c
FreeList* block = ((FreeList*) ptr) - 1; // get header
FreeList* current = head;
```

### Traversing the List

This part of the function determines where the block should be inserted:

```c
while (!(block > current && block < current->next)) {
    if (current >= current->next && (block > current || block < current->next)) {
        break; // wrapped around
    }
    current = current->next;
}
```

This loop can be tricky, so let’s break it down.

- The goal is to find the correct spot to insert the block in address order.
- Normally, we insert between `current` and `current->next` if `block` falls
  between them.
- But if the list wraps around (i.e., `current >= current->next`), we handle it
  with a special condition:

  - If the block is greater than `current` **or** less than `current->next`, it
    belongs at the start or end of the list.

This logic ensures the list stays properly sorted, even across the circular
boundary.

### Merging with Neighbors

Once we’ve found the insertion point, we check if the block is adjacent to its
neighbors and merge if possible.

```c
// Merge with upper neighbor if possible
if (freelist_block_is_neighbor(block, current->next)) {
    freelist_block_merge_upward(block, current);
} else {
    block->next = current->next;
}

// Merge with lower neighbor if possible
if (freelist_block_is_neighbor(current, block)) {
    freelist_block_merge_downward(current, block);
} else {
    current->next = block;
}
```

Here's what happens:

- We check if `block` ends where `current->next` begins — if so, we merge
  `block` with the next block.
- Then we check if `current` ends where `block` begins — if so, we merge
  `block` into `current`.
- If neither merge is possible, we just link `block` between `current` and
  `current->next`.

These merges are what keep the freelist compact and efficient.

### Finalizing the Insertion

After insertion or merging, we update the freelist `head`:

```c
head = current;
```

This helps with future searches and maintains consistency in list traversal.

### Complete Function

```c
static void freelist_block_insert(void* ptr) {
    if (NULL == ptr) {
        return;
    }

    FreeList* block = ((FreeList*) ptr) - 1;
    FreeList* current = head;

    while (!(block > current && block < current->next)) {
        if (current >= current->next && (block > current || block < current->next)) {
            break; // wrapped around
        }
        current = current->next;
    }

    // Merge with upper neighbor if possible
    if (freelist_block_is_neighbor(block, current->next)) {
        freelist_block_merge_upward(block, current);
    } else {
        block->next = current->next;
    }

    // Merge with lower neighbor if possible
    if (freelist_block_is_neighbor(current, block)) {
        freelist_block_merge_downward(current, block);
    } else {
        current->next = block;
    }

    head = current;
}
```

This is one of the most important parts of the allocator. It ensures freed
blocks are recycled properly and merged when possible — giving us a consistent,
efficient freelist over time.

## Bytes, Units, and Alignment

Now that we’ve laid out the foundational pieces of the allocator — list
management, block insertion, and merging — we can finally create new blocks on
demand.

This might seem simple at first glance, but there's a surprising amount of
nuance involved in turning **bytes** into properly aligned **freelist units**.

### What is a "unit"?

In this allocator, a **unit** is the basic allocation block — it's equivalent
to the size of a `FreeList` node.

We don’t just allocate raw bytes — we allocate memory in **multiples of
`sizeof(FreeList)`**. This simplifies alignment and block sizing internally.

So when we allocate, we don’t ask, “how many bytes?” — we ask, “how many units
of memory do we need?”

### So how does malloc fit in?

We’ll go over `freelist_malloc()` in detail soon, but here’s the important part
for context:

```c
uintptr_t payload_size = memory_align_up(size, alignof(FreeList));
size_t nunits = (payload_size + sizeof(FreeList) - 1) / sizeof(FreeList) + 1;
```

- The user requests some number of **bytes**.
- We align that size up to the next multiple of `FreeList` alignment.
- Then we convert the aligned byte size into a **unit count**.
- The final `+1` is to include space for the block header.

Why units? Because internally, every free block is sized and traversed in units
— not bytes. It's a lightweight abstraction that makes coalescing and splitting
easier.

### Creating New Blocks

When we run out of memory in the freelist, we fall back to allocating new
memory from the system. That’s the job of:

```c
static FreeList* freelist_block_new(size_t nunits);
```

Let’s walk through the function:

```c
size_t nbytes = nunits * sizeof(FreeList);
```

We convert the unit count back into raw bytes — this is the actual memory we
request.

Then we allocate aligned memory:

```c
FreeList* block = (FreeList*) memory_alloc(nbytes, alignof(FreeList));
if (NULL == block) {
    return NULL;
}
```

Next, we store the number of units in the header:

```c
block->size = nbytes / sizeof(FreeList);
```

This might seem circular — we converted bytes to units and back again — but
it's just a bookkeeping step to ensure `block->size` matches the internal unit
count expected by the rest of the allocator.

We then insert the new block into the freelist:

```c
freelist_block_insert(block + 1);
```

> Why `block + 1`? Because we always insert the **payload** portion of the
> block — the part after the header. Inside `freelist_block_insert()`, we
> subtract one to get back to the header.

Finally, we return the full block in case the caller (e.g. `freelist_malloc`)
needs it:

```c
return block;
```

### Full Implementation

```c
static FreeList* freelist_block_new(size_t nunits) {
    size_t nbytes = nunits * sizeof(FreeList);

    FreeList* block = (FreeList*) memory_alloc(nbytes, alignof(FreeList));
    if (NULL == block) {
        return NULL;
    }

    block->size = nbytes / sizeof(FreeList);
    freelist_block_insert(block + 1);
    return block;
}
```

### Why not just use bytes?

You might wonder why we don’t just pass around byte counts. The reason is:
**units are simpler and cheaper** to work with.

If we passed raw bytes:

- We'd constantly have to convert between bytes and units in every freelist
  operation.
- We'd risk misalignments or partial block comparisons.

Sticking to `sizeof(FreeList)`-aligned units lets the allocator stay small,
fast, and safe — exactly what the original K\&R design intended (minus the
union hacks).

### Final Note

This is a deceptively simple function — and that’s the beauty of it. Once all
the pieces are in place (alignment, headers, coalescing), allocating new memory
becomes a one-liner that fits into the larger machinery of the allocator with
minimal overhead.

## References

- K&R C - 8.7: A Storage Allocator
- https://stackoverflow.com/q/13159564
- https://stackoverflow.com/q/1119134
- https://stackoverflow.com/q/2513505
