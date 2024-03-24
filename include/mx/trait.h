#ifndef _MX_TRAIT_H_
#define _MX_TRAIT_H_

/**
 * @file trait.h Interface Traits and Fat Pointers
 * 
 * This is the MX interface library. Interfaces define common traits C and C++
 * objects can have. This functionality is exposed through a fat pointer, which
 * is just two pointers: the object reference and its virtual table (vtable).
 * 
 * @section new_interface Creating an Interface
 * The convention in the library is to prefix an interface with an I, and use
 * Pascal case naming conventions for the interface structure.
 * 
 * Define the interface like any other C structure, except, only include function
 * pointers and other interfaces in it. Then use `fatptr_define` to create a fat
 * pointer for the given interface.
 * 
 * @code
 * typedef struct IFoo {
 *  IObject Object;     // Base interface.
 *  void (*bar)(void *self);  // Virtual method objects can implement.
 * } IFoo;
 * 
 * fatptr_define(IFoo);
 * @endcode
 * 
 * Now, in order to create a fat pointer with this interface, you can use the
 * generic type fatptr_t(T). Notice the parenthesis, because this is a C
 * library.
 * 
 * A suggestion is to create inline static functions right after this interface
 * declaration to aid calling the virtual methods of the trait defined.
 * 
 * @code
 * MX_INLINE void IFoo_bar(fatptr_t(IFoo) ptr) {
 *  fatptr_vcall(ptr, bar);
 * }
 * @endcode
 * 
 * In order to implement these traits, create a source code file, and implement
 * the trait functions as you like. These functions can be static. Make sure
 * the virtual methods and the implementations have the same function signature.
 * 
 * Then, create a vtable for the object using the fat_vtable(type, trait), and
 * refrerence the implemented functions.
 * 
 * @code
 * typedef struct MyFoo
 * {
 *   // Private implementation data.
 * } MyFoo;
 * 
 * static void MyFoo_IFoo_bar(void *self)
 * {
 *  printf("Hello World!\n");
 * }
 * 
 * IFoo fat_vtable(MyFoo, IFoo) = { MyFoo_IFoo_bar };
 * @endcode
 * 
 * Now you have sucessfully implemented this trait. Now let's see an example
 * of how this is used.
 * 
 * @code
 * void FooConsumer(fatptr_t(IFoo) foo)
 * {
 *  fat_vcall(foo, bar);
 * }
 * 
 * int main(void)
 * {
 *      MyFoo foo = {};
 *      fatptr_t(IFoo) foo_trait = fat_cast(foo, MyFoo, IFoo);
 *      FooConsumer(foo_trait);
 * }
 * @endcode
 * 
 * If everything went right, this should print <code>Hello World!</code> and exit.
 * 
 * @section fat_functions "fat" Functions
 * The structure fatptr_t(T) serves as the main type for this library, where
 * T is the interface that defines the object traits. Ideally all objects should
 * implement the IObject trait, but this isn't enforced at all.
 * 
 * You can use fat_new(ptr, vtable, trait) in your code to create a fat pointer
 * to the object "ptr", with a "vtable" (the implementation of the traits
 * defined by the interface) for the interface "trait".
 * 
 * You can use fat_cast(object, type, trait) to cast the value "object" of
 * "type" to a fat pointer with traits "trait". This will look up the object
 * vtable and create a fat pointer automatically.
 * 
 * fat_reinterpret(value, trait) will reinterpret the fatptr "value" a
 * fatptr_t(trait). This type of cast is only valid as long as the first member
 * of the source fat pointer interface is also an interface.
 * 
 * fat_vnill(fatptr, vmethod) checks if a virtual method "vmethod" of the fat
 * pointer is nill. This is mostly useful for optional traits.
 * 
 * fat_vcall(fatptr, vmethod, ...) will invoke the "vmethod" of "fatptr". This
 * is a GNU-C macro so your mileage may vary using it on niche compilers.
 */

#include "mx/base.h"

/**
 * Fat pointer to any object.
 */
#define fatptr_t(trait) struct __mx_fatptr_##trait##_t

/**
 * @brief Define the fat pointer structure.
 * @param base Base pointer type
 */
#define fatptr_define(trait) fatptr_t(trait) { void *ptr; const trait *traits; }

/**
 * Fat pointer vtable for object #type for traits #trait
*/
#define fat_vtable(type, trait) __mx_fat_vtable_##type##_##trait

/**
 * Create a fat pointer to #ptr, with #vtable of #trait.
 */
#define fat_new(ptr, vtable, trait) ((fatptr_t(trait)){ (ptr), &(vtable)})

/**
 * Cast a value #object of type #type into fat pointer of #trait.
 */
#define fat_cast(object, type, trait) fat_new(&(object), fat_vtable(type, trait), trait)

/**
 * Reinterpret a fatptr into another fatptr.
*/
#define fat_reinterpret(value, trait) (*((fatptr_t(trait)*)&(value)))

/**
 * True if vmethod is null.
 */
#define fat_vnil(fatptr, vmethod) ((fatptr)->traits.vmethod == NULL)

/**
 * Base traits of all MX objects.
 */
typedef struct IObject
{
    /**
     * Get the size of this object in bytes.
     * @param self Instance object.
     * @return The size of the object in bytes.
     */
    size_t (*get_size)(void *self);
    /**
     * Get the type object for this.
     * @param self Instance object.
     */
    void * (*get_type)(void *self);
    /**
     * Convert this object to a string.
     * @param self Instance object.
     * @param[out] buffer The buffer to write the string into.
     * @param[in]  max The maximum number of bytes to write.
     * @return The size of the written string.
     */
    size_t (*to_string)(void *self, char *buffer, size_t max);
    /**
     * Destructor function for this object.
     * @param self Instance object.
     */
    void   (*destruct)(void* self);
} IObject;
fatptr_define(IObject);

// This is GNU C :/

#define fatptr_vcall(fatptr, vmethod, ...) ((fatptr).traits->vmethod(fatptr.ptr, ## __VA_ARGS__))

/**
 * Get the size of this object in bytes.
 * @param self Instance object.
 * @return The size of the object in bytes.
 */
MX_INLINE size_t IObject_get_size(fatptr_t(IObject) obj)
{
    return fatptr_vcall(obj, get_size);
}

/**
 * Get the type object for this.
 * @param self Instance object.
 */
MX_INLINE void* IObject_get_type(fatptr_t(IObject) obj)
{
    return fatptr_vcall(obj, get_type);
}

/**
 * Convert this object to a string.
 * @param self Instance object.
 * @param[out] buffer The buffer to write the string into.
 * @param[in]  max The maximum number of bytes to write.
 * @return The size of the written string.
 */
MX_INLINE size_t IObject_to_string(fatptr_t(IObject) obj, char *buffer, size_t max)
{
    return fatptr_vcall(obj, to_string, buffer, max);
}

/**
 * Destructor function for this object.
 * @param self Instance object.
 */
MX_INLINE void IObject_destruct(fatptr_t(IObject) obj)
{
    return fatptr_vcall(obj, destruct);
}

#endif
