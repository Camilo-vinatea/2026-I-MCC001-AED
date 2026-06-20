# AVL Tree Design Spec
Date: 2026-06-20

## Context

Existing codebase has `BinaryTree<Traits>` (BST) in `containers/binarytree.h`.
`AVL<Traits>` in `containers/avl.h` must inherit maximum functionality from it.

## Constraints

- Only `insert` (no `remove`)
- Height stored as `size_t` per node
- Minimal changes to `binarytree.h`
- `AVLNode` inherits from `BinaryTree<Traits>::Node`

---

## Section 1: Changes to `binarytree.h`

Move three methods from `private:` to `protected:` — no signature or behavior change:

```cpp
protected:   // was: private
    void insertar_interno(NodePtr&, const value_type&, Ref);
    void copiar_interno(NodePtr&, const NodePtr);
    void escribir_interno(ostream&, const Node*) const;
```

Rationale: `AVL` needs to call `insertar_interno` pattern by reference;
`copiar_interno`/`escribir_interno` exposed for future subclasses.

---

## Section 2: `AVLNode<Traits>`

File: `containers/avl.h`

```cpp
template <typename Traits>
struct AVLNode : BinaryTree<Traits>::Node {
    using Base       = typename BinaryTree<Traits>::Node;
    using value_type = typename Traits::value_type;

    size_t m_height = 1;

    AVLNode(const value_type& data, Ref ref) : Base(data, ref) {}
};
```

Inherited from `BinaryTree<Traits>::Node` (no cost):
- `m_data`, `m_ref`, `m_pChild[2]`, `m_pParent`
- All accessors: `getData/setData`, `getChild/setChild`, `getRef/setRef`, `getParent/setParent`
- `to_string()`, `operator<<`, `operator>>`
- Cascade destructor (deletes entire subtree)

Added: `size_t m_height = 1`

Static helpers defined inside `AVL<Traits>`:
```cpp
static MyNode*   avl(NodePtr n)    { return static_cast<MyNode*>(n); }
static size_t    height(NodePtr n) { return n ? avl(n)->m_height : 0; }
static ptrdiff_t bf(NodePtr n)     { return height(n->getChild(0)) - height(n->getChild(1)); }
```

`static_cast` is safe: `AVL` only ever creates `AVLNode` objects, never bare `Node`.

---

## Section 3: `AVL<Traits>` class

File: `containers/avl.h`

### Rotations

Right rotation (LL fix) and left rotation (RR fix).
Each takes `NodePtr&` (reference to the caller's pointer) and rewires child links.
Updates heights bottom-up after rewiring.

```
rotate_right(y_ref):        rotate_left(x_ref):
     y → x                       x → y
    / \   / \                   / \   / \
   x   C A   y                 A   y x   C
  / \     / \                     / \ A   B
 A   B   B   C                   B   C
```

### LR / RL double rotations

`rebalance(p)`:
- `bf > 1` and left child `bf < 0` → rotate_left on left child first (LR case)
- `bf > 1` → rotate_right (LL case)
- `bf < -1` and right child `bf > 0` → rotate_right on right child first (RL case)
- `bf < -1` → rotate_left (RR case)

### `avl_insertar_interno`

Same BST descent as `BinaryTree::insertar_interno`:
```
!m_comp(v, p->getDataRef())  →  pos = 0 (left) or 1 (right)
```
Then `rebalance(p)` on the way back up (post-order, via recursion unwind).

### `insert` (public override)

```cpp
void insert(const value_type& v, Ref ref) {
    scoped_lock<mutex> lock(this->m_mtx);
    avl_insertar_interno(this->m_pRoot, v, ref);
}
```

Reuses `m_mtx` from base. Pattern identical to `BinaryTree::insert`.

---

## Inherited from BinaryTree (no override needed)

| Feature | Source |
|---|---|
| `size()`, `empty()` | BinaryTree |
| `inorder()`, `preorder()`, `postorder()` + reverse variants | BinaryTree |
| `begin/end`, `rbegin/rend`, all 6 iterator types | BinaryTree |
| `ForEach`, `ForEachPreorder`, `ForEachPostorder` + reverses | BinaryTree |
| `FirstThat`, `ReverseFirstThat` | BinaryTree |
| `toString()` | BinaryTree |
| `operator<<`, `operator>>` | BinaryTree |
| Copy constructor, move constructor, destructor | BinaryTree |
| `BinaryTreeRange` range-for support | BinaryTree |

---

## Known Limitations (out of scope)

- **Copy/move constructor**: `BinaryTree`'s copy constructor calls `copiar_interno` which creates base `Node` objects (no `m_height`). Copying an `AVL` would produce a structurally broken tree (nodes without height). Future fix: override copy/move constructor in `AVL` to create `AVLNode` objects.
- **No `remove`**: Deletion with rebalancing not implemented.

---

## Files Changed

| File | Change |
|---|---|
| `containers/binarytree.h` | `private:` → `protected:` for 3 internal methods |
| `containers/avl.h` | New: `AVLNode<Traits>` + `AVL<Traits>` |
