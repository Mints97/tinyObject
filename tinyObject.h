#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

/* Static assertion - produces error with a message at compile time */
#define STATIC_ASSERT(condition, message) extern char STATIC_ASSERTION__##message[1]; \
    extern char STATIC_ASSERTION__##message[(condition)? 1 : 2]

/* tinyObject utility macros */
#define MAKE_FIELD(type, name, val) type name
#define DEFINE_FIELD(type, name, val)
#define INIT_FIELD(type, name, val) thisObject->name = (type)val

#define PRIVATE static

#define METHOD(classType, type, name, args) type classType##_##name args

#define MAKE_VIRTUAL_METHOD(classType, type, name, args) type (*name)args
#define INIT_VIRTUAL_METHOD(classType, type, name, args) thisObject->name = &classType##_##name

/* Determine if the compiler uses the regular struct member alignment logic, otherwise, inheritance won't work */
struct _ASSERTION_TEST_STRUCT { char a; int b; };
struct _ASSERTION_TEST_STRUCT2 { char a; int b; long c[100000]; long d[100000]; };
STATIC_ASSERT(offsetof(struct _ASSERTION_TEST_STRUCT, b) == offsetof(struct _ASSERTION_TEST_STRUCT2, b),
				This_compiler_follows_an_irregular_logic_in_struct_member_alignment_and_cannot_be_used_to_compile_tinyObject);

#define MAKE_THIS(type) struct type##_s *thisObject
#define ALLOC_THIS(type) struct type##_s *thisObject = (struct type##_s*)malloc(sizeof(struct type##_s))
#define this thisObject

#define MAKE_CLASS(type) \
	typedef struct type##_s val_##type, *type; \
	struct type##_s { CLASS_##type }; \
	void delete##type (struct type##_s *thisObject)

#define MAKE_DEFAULT_CONSTRUCTOR_PROTOTYPES(type) \
	struct type##_s *new##type(); \
	void init##type(struct type##_s *thisObject);

#define MAKE_DEFAULT_INITIALIZER(type) void init##type(struct type##_s *thisObject){ CLASS_##type }

#define MAKE_DEFAULT_CONSTRUCTOR(type) \
	struct type##_s *new##type(){ \
		struct type##_s *thisObject = (struct type##_s*)malloc(sizeof(struct type##_s)); \
		if (!thisObject) return NULL; \
		CLASS_##type \
		return thisObject; \
	}

#define MAKE_DEFAULT_CONSTRUCTORS(type) \
	MAKE_DEFAULT_INITIALIZER(type) \
	MAKE_DEFAULT_CONSTRUCTOR(type)

#define MAKE_DEFAULT_DESTRUCTOR(type) void delete##type(struct type##_s *thisObject){ free(thisObject); }

#define SELFREF_INIT void *currThis
#define $(object) (((currThis = (void*)object), object)
#define _(object) ((object)

#define CURR_THIS(type) (struct type##_s*)currThis
#define MAKE_METHOD_ALIAS(type, method) , type##_##method )
#define MAKE_VIRTUAL_METHOD_ALIAS(type, method) ->method )



#define CLASS_Class \
	FIELD(int, a, NULL);

/* Class Object */
#define CLASS_Object \
	DEF_FIELD(int, var1); \
	FIELD(int, var2, 2); \
	VIRTUAL_METHOD(Object, void, test, (MAKE_THIS(Object), int a));

	METHOD(Object, int, testNonV, (MAKE_THIS(Object), int b));

	/* Self-reference mechanism for methods */
	#define _test(a) MAKE_VIRTUAL_METHOD_ALIAS(Object, test(CURR_THIS(Object), a))
	#define _testNonV(b) MAKE_METHOD_ALIAS(Object, testNonV(CURR_THIS(Object), b))


/* Class DerivedClass, inherited from Object */
#define CLASS_DerivedClass /* inherits from */ CLASS_Object \
	FIELD(int, var3, 3);

/* Class DoubleDerivedClass, inherited from DerivedClass */
#define CLASS_DoubleDerivedClass /* inherits from */ CLASS_DerivedClass \
	FIELD(int, var4, 4);

/* Class TripleDerivedClass, inherited from DerivedClass */
#define CLASS_TripleDerivedClass /* inherits from */ CLASS_DoubleDerivedClass \
	FIELD(int, var5, 5);


/* Make the classes */
#define FIELD(type, name, val) MAKE_FIELD(type, name, val)
#define DEF_FIELD(type, name) MAKE_FIELD(type, name, )
#define VIRTUAL_METHOD(classType, type, name, args) MAKE_VIRTUAL_METHOD(classType, type, name, args)

MAKE_CLASS(Class);
void initClass(Class thisObject);
Class newClass(int a, int b);

MAKE_CLASS(Object);
MAKE_DEFAULT_CONSTRUCTOR_PROTOTYPES(Object);

MAKE_CLASS(DerivedClass);
MAKE_DEFAULT_CONSTRUCTOR_PROTOTYPES(DerivedClass);

MAKE_CLASS(DoubleDerivedClass);
MAKE_DEFAULT_CONSTRUCTOR_PROTOTYPES(DoubleDerivedClass);

MAKE_CLASS(TripleDerivedClass);
MAKE_DEFAULT_CONSTRUCTOR_PROTOTYPES(TripleDerivedClass);

#undef FIELD
#undef DEF_FIELD
#undef VIRTUAL_METHOD


