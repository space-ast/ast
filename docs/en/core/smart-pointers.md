# Smart Pointers

The ast project provides three smart pointer implementations for safely managing object lifecycles, avoiding memory leaks and dangling pointer issues.

## SharedPtr

A shared pointer implementation similar to C++ standard library's `std::shared_ptr`, which manages resources through reference counting.

### Important Limitation

**SharedPtr can only be used with classes derived from Object**, as it relies on reference counting methods (`incRef()` and `decRef()`) provided by the Object base class.

### Main Features

- Supports multiple pointers pointing to the same object, with lifecycle managed through reference counting
- Automatically releases the object when the last reference is destroyed
- Provides automatic type conversion and arrow operator overloading

### Core Methods

| Method | Description |
|--------|-------------|
| Constructor | Supports default construction, construction from raw pointer, and copy construction |
| Destructor | Decreases reference count and releases object if necessary |
| `operator=` | Assignment operator overloading, supports assignment from other SharedPtr or raw pointer |
| `operator->` | Provides access to managed object members |
| `get()` | Retrieves the raw pointer |

### Usage Example

```cpp
// Create shared pointer object (only for Object subclasses)
SharedPtr<MyObject> ptr1 = new MyObject();  // MyObject must inherit from Object

// Share ownership
SharedPtr<MyObject> ptr2 = ptr1;

// Access object members
ptr1->doSomething();

// Get raw pointer
MyObject* rawPtr = ptr1.get();
```

## ScopedPtr

A scoped pointer for managing resources with exclusive ownership, with its lifecycle strictly limited to the scope.

### Main Features

- Exclusive ownership, prohibits copy and assignment operations
- Automatically releases resources when scope ends
- Provides custom deleters with special handling for Object type (calls `destruct()` method)
- Can be used with both regular types and Object subclasses

### Core Methods

| Method | Description |
|--------|-------------|
| Constructor | Supports default construction and construction from raw pointer |
| Destructor | Automatically releases managed resources |
| `reset()` | Resets the pointer, releases old resources and takes over new ones |
| `get()` | Retrieves the raw pointer |

### Usage Example

```cpp
{  // Scope begins
    // Can be used with regular types
    ScopedPtr<int> intPtr = new int(42);
    
    // Can also be used with Object subclasses (special deletion handling)
    ScopedPtr<MyObject> objPtr = new MyObject();  // MyObject must inherit from Object
    objPtr->doSomething();
    
    // Reset pointer
    objPtr.reset(new MyObject());
    
    // Automatically released when scope ends
}
```

## WeakPtr

A weak reference pointer similar to C++ standard library's `std::weak_ptr`, which doesn't increase reference count, avoiding circular reference issues.

### Important Limitation

**WeakPtr can only be used with classes derived from Object**, as it relies on weak reference counting methods (`incWeakRef()`, `decWeakRef()`) and destruction state checking (`isDestructed()`) provided by the Object base class.

### Main Features

- Does not affect object lifecycle, only increases weak reference count
- Can detect whether the object has been destroyed
- Provides safe mechanism for accessing objects

### Core Methods

| Method | Description |
|--------|-------------|
| Constructor | Supports default construction, construction from raw pointer, and copy construction |
| Destructor | Decreases weak reference count |
| `operator=` | Assignment operator overloading |
| `lock()` | Retrieves raw pointer (does not check if object is destroyed) |
| `get()` | Safely retrieves pointer, checking if object is destroyed |

### Usage Example

```cpp
// Create shared pointer (only for Object subclasses)
SharedPtr<MyObject> shared = new MyObject();  // MyObject must inherit from Object

// Create weak reference pointer
WeakPtr<MyObject> weak = shared.get();

// Safely access object
if (MyObject* obj = weak.get()) {
    obj->doSomething();
}
```

## Smart Pointer Usage Recommendations

1. **Prefer ScopedPtr** when the resource is only used within a single scope
2. **Use SharedPtr** when multiple pointers need to share the same resource (only applicable for Object subclasses)
3. **Use WeakPtr** when you need to reference a shared resource without affecting its lifecycle (only applicable for Object subclasses), especially to avoid circular references
4. **Avoid raw pointers** where possible, use smart pointers to manage dynamic resources
5. **Pay attention to type safety** to ensure the smart pointer's template type matches the actual managed object type, with special attention that SharedPtr and WeakPtr can only be used with Object subclasses
