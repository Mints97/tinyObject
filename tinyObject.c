#include "tinyObject.h"


static METHOD(Object, void, test, (MAKE_THIS(Object), int a)){
	printf("Public virtual method test reporting. thisObject->var1 = %d, The parameter is %d\n\n", this->var1, a);
}

METHOD(Object, int, testNonV, (MAKE_THIS(Object), int b)){
	printf("Public non-virtual method testNonV reporting. thisObject->var1 = %d, The parameter is %d\n\n", this->var1, b);
	return 1;
}


/* Make the constructors */
#define FIELD(type, name, val) INIT_FIELD(type, name, val)
#define DEF_FIELD(type, name) DEFINE_FIELD(type, name, )
#define VIRTUAL_METHOD(classType, type, name, args) INIT_VIRTUAL_METHOD(classType, type, name, args)

MAKE_DEFAULT_INITIALIZER(Class);

Class newClass(int a, int b){
	ALLOC_THIS(Class);
	this->a = a;
	return this;
}

MAKE_DEFAULT_INITIALIZER(Object);

Object newObject(){
	ALLOC_THIS(Object);
	CLASS_Object;
	this->var1 = 1;
	return this;
}

MAKE_DEFAULT_CONSTRUCTORS(DerivedClass);
MAKE_DEFAULT_CONSTRUCTORS(DoubleDerivedClass);
MAKE_DEFAULT_CONSTRUCTORS(TripleDerivedClass);

#undef FIELD
#undef DEF_FIELD
#undef VIRTUAL_METHOD
