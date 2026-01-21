# Smart Pointers

*Note: This document is AI-translated.*

The ast project provides three smart pointer implementations for safely managing object lifecycles, avoiding memory leaks and dangling pointer issues.

## SharedPtr

A shared pointer implementation similar to C++ standard library's `std::shared_ptr`, using reference counting to implement resource sharing.

### Important Limitations

**SharedPtr can only be used for classes inherited from the Object type**, as it relies on the reference counting methods (`incRef()` and `decRef()`) provided by the Object base class.

### Main Features

- Supports multiple pointers pointing to the same object, managing object lifecycle through reference counting
- When the last reference is destroyed, the object is correctly released
- Provides automatic type conversion and arrow operator overloading

### Core Methods

| Method | Description |
|--------|-------------|
| Constructor | Supports default construction, construction from raw pointer, and copy construction |
| Destructor | Decreases reference count and releases object when necessary |
| `operator=` | Assignment operator overloading, supporting assignment from other SharedPtr or raw pointers |
| `operator->` | Provides access to members of the managed object |
| `get()` | Gets the raw pointer |

### Usage Example

@[code](/examples/util/smartpointer/sharedptr.cpp)

## ScopedPtr

A scoped pointer for managing resources with exclusive ownership, with lifecycle strictly limited to the scope.

### Main Features

- Exclusive ownership, prohibits copy and assignment operations
- Automatically releases resources when the scope ends
- Provides custom deleters with special handling for Object types (calls `destruct()` method)
- Can be used for both regular types and subclasses of Object

### Core Methods

| Method | Description |
|--------|-------------|
| Constructor | Supports default construction and construction from raw pointer |
| Destructor | Automatically releases the managed resource |
| `reset()` | Resets the pointer, releases the original resource and takes over a new resource |
| `get()` | Gets the raw pointer |

### Usage Example

@[code](/examples/util/smartpointer/scopedptr.cpp)

## WeakPtr

A weak reference pointer similar to C++ standard library's `std::weak_ptr`, does not increase reference count, avoiding circular reference issues.

### Important Limitations

**WeakPtr can only be used for classes inherited from the Object type**, as it relies on the weak reference counting methods (`incWeakRef()`, `decWeakRef()`) and destruction status check (`isDestructed()`) provided by the Object base class.

### Main Features

- Does not affect object lifecycle, only increases weak reference count
- Can detect if the object has been destroyed
- Provides safe mechanisms to access objects

### Core Methods

| Method | Description |
|--------|-------------|
| Constructor | Supports default construction, construction from raw pointer, and copy construction |
| Destructor | Decreases weak reference count |
| `operator=` | Assignment operator overloading |
| `lock()` | Gets the raw pointer (does not check if object is destroyed) |
| `get()` | Safely gets the pointer, checking if object is destroyed |

### Usage Example

@[code](/examples/util/smartpointer/weakptr.cpp)

## Smart Pointer Usage Recommendations

1. **Prefer ScopedPtr**: When resources are only used within a single scope
2. **Use SharedPtr**: When multiple pointers need to share the same resource (only for subclasses of Object)
3. **Use WeakPtr**: When needing to reference a shared resource without affecting its lifecycle (only for subclasses of Object), especially to avoid circular references
4. **Avoid Raw Pointers**: Whenever possible, use smart pointers instead of raw pointers to manage dynamic resources
5. **Note Type Safety**: Ensure that the template type of the smart pointer matches the actual managed object type, especially noting that SharedPtr and WeakPtr can only be used for subclasses of Object