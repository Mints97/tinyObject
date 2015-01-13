#include "tinyObject.h"

int main(void){
	SELFREF_INIT;

	int a;

	Class newclass = newClass(1, 2);
	Object object = newObject();
	DerivedClass derived = newDerivedClass();

	val_Object objectVal;
	initObject(&objectVal);

	derived->var1 = 0;

	printf("object->var1 = %d, object->var2 = %d\n\n", object->var1, object->var2);
	printf("derived->var1 = %d, derived->var2 = %d, derived->var3 = %d\n\n", derived->var1, derived->var2, derived->var3);
	
	printf("((Object)derived)->var1 = %d, ((Object)derived)->var2 = %d\n\n", ((Object)derived)->var1, ((Object)derived)->var2);

	printf("Calling virtual method test: $(object)_test(10)\n");
	$(object)_test(10);

	printf("Calling non-virtual method testNonV: $(object)_testNonV(10)\n");
	a = _(object)_testNonV(10);

	printf("The method returned %d\n", a);

	scanf("%d");
	return 0;
}
