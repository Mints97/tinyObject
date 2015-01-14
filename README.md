tinyObject
=======

An easy-to-use object-oriented system for the C programming language.

Currently at version 0.3 (Alpha)

tinyObject is a (tiny) object-oriented system for the C programming language. It was designed with ease of use and extension being the primary concern. This has been achieved with a lot of preprocessor abuse. Poor preprocessor.

tinyObject sports a simplistic object-oriented model with inheritance and (explicit-cast-requiring) polymorphism, public and private methods, public fields and a self-reference mechanism. Updates are planned to add support for namespaces, exception handling, runtime type-safety checks and private fields.

tinyObject is currently used in [tinyGUI](https://github.com/Mints97/tinyGUI), a (tiny) Windows GUI library for C. Originally, tinyObject was part of tinyGUI.

###How does it work?
tinyObject emulates inheritance by using a couple of macros to copy the members of the base type to the beginning of the derived type. Please note that, technically, this is undefined behaviour according to the C standard. However, normally, compilers that do not reorder struct members (which is mandated by the standard) align the copied members in the derived type struct the same way as they were aligned in the base type (if they are not packed differently). In fact, if a compiler was not to support this, it would have lots, lots and lots of trouble adhering to the union-of-structs-that-share-a-common-initial-sequence rule (C11 standard, 6.5.2.3 - 6). If you know of such an implementation of C where this doesn't hold, please notify me.

##Syntaxis

In tinyObject, a class consist of
- a declaration listing its public members;
- public method declarations;
- a class creation statement;
- method definitions;
- the class constructors.

The first three are normally contained in a .h file and the last two in a .c file (a separate translation unit from the main program file). It is not necessary to use a .c file, but, without a separate .c file, you won't be able to encapsulate private methods or functions that are actually virtual methods.

###Declaring a class

This should be done in a header (.h) file shared by the main program and the .c file, right after all the tinyObject utility macro definitions (you can place them in a separate header file for convenience).

```C
/* Class Object */
#define CLASS_Object \
	DEF_FIELD(int, var1); \
	DEF_FIELD(int, var2);
```
The above is a macro declaration of a class with two defined fields of type int. All simple so far. Now let's add something new to the declaration:

```C
/* Class Object */
#define CLASS_Object \
	DEF_FIELD(int, var1); \
	DEF_FIELD(int, var2); \
	FIELD(int, var3, 100500);
```
We have added a field with a value (an initialized field). To set it to the specified value, you have to call a default constructor or use a default initialization sequence macro for this class. More on this later. Now, let's add virtual methods:

```C
/* Class Object */
#define CLASS_Object \
	DEF_FIELD(int, var1); \
	DEF_FIELD(int, var2); \
	FIELD(int, var3, 100500); \
	VIRTUAL_METHOD(Object, void, test, (MAKE_THIS(Object), int a));
```
The virtual method macro takes the class name as the first argument, the method type as the second, its name as the third and its bracket-enclosed parameters as the fourth. The first parameter is used for self-reference, if your method doesn't require an object instance, it might be better to not specify it.

In tinyObject, each class serves as a vtable for itself, so the virtual method definition is, in reality, a declaration of an initialized field with its type being a function pointer. This definition also ensures that this pointer is initialized to point to a function with a mangled name classname_methodname (Object_test in this case). It can be viewed as syntactic sugar.

To finish up, let's add a non-virtual public method to the class. It is not really part of the class declaration, but it could do good to keep them together:

 ```C
/* Class Object */
#define CLASS_Object \
	DEF_FIELD(int, var1); \
	DEF_FIELD(int, var2); \
	FIELD(int, var3, 100500); \
	VIRTUAL_METHOD(Object, void, test, (MAKE_THIS(Object), int a));

	METHOD(Object, int, testNonV, (MAKE_THIS(Object), int b));
```

The syntaxis is the same as with virtual methods. This creates a function prototype with a similarly mangled name.

In all above examples, if you are using a class type anywhere in the declarations, you should use its struct name as an incomplete type, as class types are not declared before a class creation statement. Each class has the following struct name:
`struct <type>_s`
where <type> is the class name. For example, for Object, it will be
`struct Object_s`
Important: the class types are actually pointers to these structs! However, each class also has a value type, named `val_<type>`
Alternatively, you can typedef the struct before the class declaration. In C, multiple typedefs for the same type are not a problem.
If you are using a class type in a non-virtual method declaration, you can also just move it to after the creation statements for the needed classes, although that could result in messy code. However, this has its pros, as it enables you to provide the method definition right there, too.

As you can see, we have virtual methods that have the following calling syntax:
`object->method(object, parameters)`
and non-virtual methods with an even messier syntax:
`ObjectType_method(object, parameters)`

tinyObject provides an easy way to create syntax sugar for a unified way to access these, which can be used with a "hackish" self-reference mechanism. Here's an example for our methods:

```C
	/* Self-reference mechanism for methods */
	#define _test(a) MAKE_VIRTUAL_METHOD_ALIAS(Object, test(CURR_THIS(Object), a))
	#define _testNonV(b) MAKE_METHOD_ALIAS(Object, testNonV(CURR_THIS(Object), b))
```

Going a bit ahead, now these methods can be invoked in the following way:

```C
int main(void){
	SELFREF_INIT;
	/* ... */
	$(object)_test(10);
	$(object)_testNonV(10);
}
```

Here's how it works: the macro SELFREF_INIT creates a variable (currentThis) which has a void pointer type. The macro $(object) sets it to the value of the object passed into it. CURR_THIS(Object) casts this variable back to the needed type to perform the method call. If you're performing multiple calls on the same object, you can replace the $ macro with the analogous _(object) macro to avoid the tiny overhead caused by currThis assignment.

The main con of this method is that it worsens the system's type un-safety: using this mechanism, you can't see the types of parameters that the method expects. This can be remedied by properly documenting your methods.

###Inheritance
To inherit from a class, simply place its declaration macro at the beginning of the derived class declaration like this:
```C
/* Class DerivedClass, inherited from Object */
#define CLASS_DerivedClass /* inherits from */ CLASS_Object \
	FIELD(int, var4, 4);
```
This can be used for a sort-of multiple inheritance, where a class can inherit members (fields and virtual methods) from several base classes, but can be properly type-casted to only one of them - the first one in the list.


###Creating a class

After declaring a class, we'd want to create it. The syntax is as follows:

```C
/* Make the classes */
#define FIELD(type, name, val) MAKE_FIELD(type, name, val) /* Optional if you haven't declared any initialized fields in your classes */
#define DEF_FIELD(type, name) MAKE_FIELD(type, name, ) /* Optional if you haven't declared any uninitialized fields in your classes */
#define VIRTUAL_METHOD(classType, type, name, args) MAKE_VIRTUAL_METHOD(classType, type, name, args) /* Optional if you haven't declared any virtual methods in your classes */

MAKE_CLASS(Object);
MAKE_CLASS(DerivedClass);

#undef FIELD /* Optional if you haven't declared any initialized fields in your classes */
#undef DEF_FIELD /* Optional if you haven't declared any uninitialized fields in your classes */
#undef VIRTUAL_METHOD /* Optional if you haven't declared any virtual methods in your classes */
```

This automatically creates a struct for the specified class and a typedef, which looks like this:
```C
typedef struct <type>_s val_<type>, *<type>;
```
Where <type> is the name of your class.

This also automatically creates a destructor prototype, which looks like this:
```C
void delete<type>(<type> thisObject);
```

If you're defining your methods and constructors in a separate .c file, now's the time to define the constructor prototypes.

###Creating constructors

Each tinyObject class can have two constructors.

One is the "constructor-allocator" (or simply "constructor"), a "classic" constructor that should allocate space on the heap for an object instance, initialize it and return it. It looks like this:
```C
<type> new<type>(<arguments...>);
```

The other is the "constructor-initializer" (or simply "initializer"), which takes in (a pointer to) an object instance as its first argument and initializes it. It looks like this:
```C
void init<type>(<type> thisObject, <arguments...>);
```

To create the default constructor prototypes (i.e. the constructor-allocator taking no arguments, the initializer taking only one), you can use the following macro:

```C
MAKE_DEFAULT_CONSTRUCTOR_PROTOTYPES(type)
```

You need to create a constructor prototype in the header file only if you're defining your methods and constructors in a separate .c file.

To create a constructor, you need to do the following:

```C
/* Make the constructors */
#define FIELD(type, name, val) INIT_FIELD(type, name, val) /* Optional if you haven't declared any initialized fields in your classes */
#define DEF_FIELD(type, name) DEFINE_FIELD(type, name, ) /* Optional if you haven't declared any uninitialized fields in your classes */
#define VIRTUAL_METHOD(classType, type, name, args) INIT_VIRTUAL_METHOD(classType, type, name, args) /* Optional if you haven't declared any virtual methods in your classes */

MAKE_DEFAULT_CONSTRUCTORS(Object);
MAKE_DEFAULT_CONSTRUCTORS(DerivedClass);

#undef FIELD /* Optional if you haven't declared any initialized fields in your classes */
#undef DEF_FIELD /* Optional if you haven't declared any uninitialized fields in your classes */
#undef VIRTUAL_METHOD /* Optional if you haven't declared any virtual methods in your classes */
```

`MAKE_DEFAULT_CONSTRUCTORS(type)` is a macro that automatically creates a constructor-allocator and an initializer with default signatures (see above) which automatically initialize the initialized fields and virtual method pointers (for the latter to work, you need to declare or define the virtual methods BEFORE the constructor, more on that later). To separately create a default constructor-allocator and a default initializer, use MAKE_DEFAULT_CONSTRUCTOR(type) and MAKE_DEFAULT_INITIALIZER(type).

You can also create a custom constructor:

```C
Object newObject(){
	ALLOC_THIS(Object);
	this->var1 = 1;
	return this;
}
```

`ALLOC_THIS(type)` is a macro that allocates sufficient heap memory to store an instance of the needed type and assigns it to a variable `thisObject` of the needed type. It can be accessed with the macro `this`. Just like in a regular object-oriented language, eh?

To make a construtor that initializes the initialized fields and virtual method pointers, insert its class declaration macro name after ALLOC_THIS. For example:

```C
Object newObject(){
	ALLOC_THIS(Object);
	CLASS_Object;
	this->var1 = 1;
	return this;
}
```

An initializer follows the same semantics, but, of course, it doesn't need to allocate the object, and it doesn't need to return anything since it is a void function.

###Creating methods
To automatically create methods with mangled names, you can use the same METHOD macro described above:

```C
METHOD(Object, int, testNonV, (MAKE_THIS(Object), int b)){
	printf("Public non-virtual method testNonV reporting. thisObject->var1 = %d, The parameter is %d\n", this->var1, b);
	return 1;
}
```

Alternatively, you can declare it as

```C
int Object_testNonV(MAKE_THIS(Object), int b){
	/* Method code goes here... */
}
```

You might want to make virtual method definitions static just in case:
```C
static METHOD(Object, void, test, (MAKE_THIS(Object), int a)){
	printf("Public virtual method test reporting. thisObject->var1 = %d, The parameter is %d\n", this->var1, a);
}
```

You might want to do the same for private methods. To tell them apart from virtual methods, you can use the `PRIVATE` macro (which is simply the `static` keyword. This macro is technically totally useless, but can make code more explicit and thus easier to understand).

Note that you should create virtual methods or their prototypes strictly before default initialization of their object(s) occurs (it should most often happen in constructors).


###Creating destructors
This is easy. The destructor signature has already been given:
```C
void delete<type>(<type> thisObject);
```

Its prototype is automatically created for each new class. However, normally, you should bother implementing it only if your object has fields that point to memory that needs to be `free`'d before the object is destroyed. Otherwise, calling the standard `free` on an allocated instance of the object will suffice.


###Using the objects

Now comes the fun part! We've got our classes with public fields and virtual and non-virtual methods, made types and constructors/destructors for them. This means that the classes are ready to rumble!

```C
/* Create an instance of class Object on the heap. The constructor takes no arguments */
Object object = newObject();

/* Create an instance of DerivedClass in the same fashion */
DerivedClass derived = newDerivedClass();

/* Create an instance of class Object on the stack and initialize it */
val_Object objectVal;
initObject(&objectVal);

```

```C
/* Access an object field */
derived->var1 = 0;

/* Call object method */
$(object)_test(10);

/* Call another method on the same object */
_(object)_testNonV(10);
```

You can see a short example program in main.c.

Don't forget to be careful with object casting, as it technically violates the strict aliasing rule, and optimising compilers could have the option to treat an object and itself cast to a different type as two different objects.
