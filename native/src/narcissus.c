#include <jni.h>
#include <stdbool.h>

// -----------------------------------------------------------------------------------------------------------------

// Prelookup of frequently-used classes and methods:

jclass Integer_class;
jclass int_class;
jmethodID int_value_methodID;

jclass Long_class;
jclass long_class;
jmethodID long_value_methodID;

jclass Short_class;
jclass short_class;
jmethodID short_value_methodID;

jclass Character_class;
jclass char_class;
jmethodID char_value_methodID;

jclass Boolean_class;
jclass boolean_class;
jmethodID boolean_value_methodID;

jclass Byte_class;
jclass byte_class;
jmethodID byte_value_methodID;

jclass Float_class;
jclass float_class;
jmethodID float_value_methodID;

jclass Double_class;
jclass double_class;
jmethodID double_value_methodID;

// Pre-look-up classes and methods for primitive types and Class, and allocate new global refs for them so they can be used across JNI calls
JNIEXPORT void JNICALL Java_narcissus_Narcissus_init(JNIEnv *env, jclass ignored) {
    Integer_class = (*env)->NewGlobalRef(env, (*env)->FindClass(env, "java/lang/Integer"));
    int_class = (*env)->NewGlobalRef(env, (*env)->GetStaticObjectField(env, Integer_class, (*env)->GetStaticFieldID(env, Integer_class, "TYPE", "Ljava/lang/Class;")));
    int_value_methodID = (*env)->GetMethodID(env, Integer_class, "intValue", "()I");

    Long_class = (*env)->NewGlobalRef(env, (*env)->FindClass(env, "java/lang/Long"));
    long_class = (*env)->NewGlobalRef(env, (*env)->GetStaticObjectField(env, Long_class, (*env)->GetStaticFieldID(env, Long_class, "TYPE", "Ljava/lang/Class;")));
    long_value_methodID = (*env)->GetMethodID(env, Long_class, "longValue", "()J");

    Short_class = (*env)->NewGlobalRef(env, (*env)->FindClass(env, "java/lang/Short"));
    short_class = (*env)->NewGlobalRef(env, (*env)->GetStaticObjectField(env, Short_class, (*env)->GetStaticFieldID(env, Short_class, "TYPE", "Ljava/lang/Class;")));
    short_value_methodID = (*env)->GetMethodID(env, Short_class, "shortValue", "()S");

    Character_class = (*env)->NewGlobalRef(env, (*env)->FindClass(env, "java/lang/Character"));
    char_class = (*env)->NewGlobalRef(env, (*env)->GetStaticObjectField(env, Character_class, (*env)->GetStaticFieldID(env, Character_class, "TYPE", "Ljava/lang/Class;")));
    char_value_methodID = (*env)->GetMethodID(env, Character_class, "charValue", "()C");

    Boolean_class = (*env)->NewGlobalRef(env, (*env)->FindClass(env, "java/lang/Boolean"));
    boolean_class = (*env)->NewGlobalRef(env, (*env)->GetStaticObjectField(env, Boolean_class, (*env)->GetStaticFieldID(env, Boolean_class, "TYPE", "Ljava/lang/Class;")));
    boolean_value_methodID = (*env)->GetMethodID(env, Boolean_class, "booleanValue", "()Z");

    Byte_class = (*env)->NewGlobalRef(env, (*env)->FindClass(env, "java/lang/Byte"));
    byte_class = (*env)->NewGlobalRef(env, (*env)->GetStaticObjectField(env, Byte_class, (*env)->GetStaticFieldID(env, Byte_class, "TYPE", "Ljava/lang/Class;")));
    byte_value_methodID = (*env)->GetMethodID(env, Byte_class, "byteValue", "()B");

    Float_class = (*env)->NewGlobalRef(env, (*env)->FindClass(env, "java/lang/Float"));
    float_class = (*env)->NewGlobalRef(env, (*env)->GetStaticObjectField(env, Float_class, (*env)->GetStaticFieldID(env, Float_class, "TYPE", "Ljava/lang/Class;")));
    float_value_methodID = (*env)->GetMethodID(env, Float_class, "floatValue", "()F");

    Double_class = (*env)->NewGlobalRef(env, (*env)->FindClass(env, "java/lang/Double"));
    double_class = (*env)->NewGlobalRef(env, (*env)->GetStaticObjectField(env, Double_class, (*env)->GetStaticFieldID(env, Double_class, "TYPE", "Ljava/lang/Class;")));
    double_value_methodID = (*env)->GetMethodID(env, Double_class, "doubleValue", "()D");
}

// -----------------------------------------------------------------------------------------------------------------

void throwIllegalArgumentException(JNIEnv* env, char* msg) {
    (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/IllegalArgumentException"), msg);
}

// -----------------------------------------------------------------------------------------------------------------

// Unbox a jobjectArray of method invocation args into a jvalue array. Returns 0 if an exception was thrown, or 1 if unboxing succeeded.
int unbox(JNIEnv *env, jobject method, jobjectArray args, jsize num_args, jvalue* arg_jvalues) {
    // Get parameter types of method
    jclass methodClass = (*env)->GetObjectClass(env, method);
    jmethodID getParameterTypesMethodID = (*env)->GetMethodID(env, methodClass, "getParameterTypes", "()[Ljava/lang/Class;");
    jobject parameterTypes = (*env)->CallObjectMethod(env, method, getParameterTypesMethodID);
    jsize num_params = (*env)->GetArrayLength(env, parameterTypes);

    // Check arg arity
    if (num_args != num_params) {
        throwIllegalArgumentException(env, "Tried to invoke method with wrong number of arguments");
        return 0;
    }
    
    // Unbox args
    for (jsize i = 0; i < num_args; i++) {
        jobject paramType = (*env)->GetObjectArrayElement(env, parameterTypes, i);
        jobject arg = (*env)->GetObjectArrayElement(env, args, i);
        jclass argType = arg == NULL ? (jclass) NULL : (*env)->GetObjectClass(env, arg);

        if ((*env)->IsSameObject(env, paramType, int_class)) {
            if (arg == NULL) {
                throwIllegalArgumentException(env, "Tried to unbox a null argument; expected Integer");
                return 0;
            } else if ((*env)->IsSameObject(env, argType, Integer_class)) {
                arg_jvalues[i].i = (*env)->CallIntMethod(env, arg, int_value_methodID);
            } else {
                throwIllegalArgumentException(env, "Tried to unbox arg of wrong type; expected Integer");
                return 0;
            }
        } else if ((*env)->IsSameObject(env, paramType, long_class)) {
            if (arg == NULL) {
                throwIllegalArgumentException(env, "Tried to unbox a null argument; expected Long");
                return 0;
            } else if ((*env)->IsSameObject(env, argType, Long_class)) {
                arg_jvalues[i].j = (*env)->CallLongMethod(env, arg, long_value_methodID);
            } else {
                throwIllegalArgumentException(env, "Tried to unbox arg of wrong type; expected Long");
                return 0;
            }
        } else if ((*env)->IsSameObject(env, paramType, short_class)) {
            if (arg == NULL) {
                throwIllegalArgumentException(env, "Tried to unbox a null argument; expected Short");
                return 0;
            } else if ((*env)->IsSameObject(env, argType, Short_class)) {
                arg_jvalues[i].s = (*env)->CallShortMethod(env, arg, short_value_methodID);
            } else {
                throwIllegalArgumentException(env, "Tried to unbox arg of wrong type; expected Short");
                return 0;
            }
        } else if ((*env)->IsSameObject(env, paramType, char_class)) {
            if (arg == NULL) {
                throwIllegalArgumentException(env, "Tried to unbox a null argument; expected Character");
                return 0;
            } else if ((*env)->IsSameObject(env, argType, Character_class)) {
                arg_jvalues[i].c = (*env)->CallCharMethod(env, arg, char_value_methodID);
            } else {
                throwIllegalArgumentException(env, "Tried to unbox arg of wrong type; expected Character");
                return 0;
            }
        } else if ((*env)->IsSameObject(env, paramType, boolean_class)) {
            if (arg == NULL) {
                throwIllegalArgumentException(env, "Tried to unbox a null argument; expected Boolean");
                return 0;
            } else if ((*env)->IsSameObject(env, argType, Boolean_class)) {
                arg_jvalues[i].z = (*env)->CallBooleanMethod(env, arg, boolean_value_methodID);
            } else {
                throwIllegalArgumentException(env, "Tried to unbox arg of wrong type; expected Boolean");
                return 0;
            }
        } else if ((*env)->IsSameObject(env, paramType, byte_class)) {
            if (arg == NULL) {
                throwIllegalArgumentException(env, "Tried to unbox a null argument; expected Byte");
                return 0;
            } else if ((*env)->IsSameObject(env, argType, Byte_class)) {
                arg_jvalues[i].b = (*env)->CallByteMethod(env, arg, byte_value_methodID);
            } else {
                throwIllegalArgumentException(env, "Tried to unbox arg of wrong type; expected Byte");
                return 0;
            }
        } else if ((*env)->IsSameObject(env, paramType, float_class)) {
            if (arg == NULL) {
                throwIllegalArgumentException(env, "Tried to unbox a null argument; expected Float");
                return 0;
            } else if ((*env)->IsSameObject(env, argType, Float_class)) {
                arg_jvalues[i].f = (*env)->CallFloatMethod(env, arg, float_value_methodID);
            } else {
                throwIllegalArgumentException(env, "Tried to unbox arg of wrong type; expected Float");
                return 0;
            }
        } else if ((*env)->IsSameObject(env, paramType, double_class)) {
            if (arg == NULL) {
                throwIllegalArgumentException(env, "Tried to unbox a null argument; expected Double");
                return 0;
            } else if ((*env)->IsSameObject(env, argType, Double_class)) {
                arg_jvalues[i].d = (*env)->CallDoubleMethod(env, arg, double_value_methodID);
            } else {
                throwIllegalArgumentException(env, "Tried to unbox arg of wrong type; expected Double");
                return 0;
            }
        } else {
            // Arg does not need unboxing
            arg_jvalues[i].l = arg;
        }
    }
    return 1;
}

// -----------------------------------------------------------------------------------------------------------------

// Find a class by name with no security checks. Name should be of the form "java/lang/String", or "[Ljava/lang/Object;" for an array class.
JNIEXPORT jobject JNICALL Java_narcissus_Narcissus_findClassInternal(JNIEnv *env, jclass ignored, jstring class_name_internal) {
    const char* class_name_internal_chars = (*env)->GetStringUTFChars(env, class_name_internal, NULL);
    jclass class_ref = (*env)->FindClass(env, class_name_internal_chars);
    (*env)->ReleaseStringUTFChars(env, class_name_internal, class_name_internal_chars);
    return class_ref;
}

// -----------------------------------------------------------------------------------------------------------------

// Get declared methods without any visibility checks
JNIEXPORT jobjectArray JNICALL Java_narcissus_Narcissus_getDeclaredMethods(JNIEnv *env, jclass ignored, jclass cls) {
    const jclass clsDescriptor = (*env)->GetObjectClass(env, cls); // Class -> Class.class
    const jmethodID methodID = (*env)->GetMethodID(env, clsDescriptor, "getDeclaredMethods0", "(Z)[Ljava/lang/reflect/Method;");
    if (methodID == 0) {
        return NULL;
    }
    return (*env)->CallObjectMethod(env, cls, methodID, (jboolean) 0);
}

// Get declared constructors without any visibility checks
JNIEXPORT jobjectArray JNICALL Java_narcissus_Narcissus_getDeclaredConstructors(JNIEnv *env, jclass ignored, jclass cls) {
    const jclass clsDescriptor = (*env)->GetObjectClass(env, cls); // Class -> Class.class
    const jmethodID methodID = (*env)->GetMethodID(env, clsDescriptor, "getDeclaredConstructors0", "(Z)[Ljava/lang/reflect/Constructor;");
    if (methodID == 0) {
        return NULL;
    }
    return (*env)->CallObjectMethod(env, cls, methodID, (jboolean) 0);
}

// Get declared fields without any visibility checks
JNIEXPORT jobjectArray JNICALL Java_narcissus_Narcissus_getDeclaredFields(JNIEnv *env, jclass ignored, jclass cls) {
    const jclass clsDescriptor = (*env)->GetObjectClass(env, cls); // Class -> Class.class
    const jmethodID methodID = (*env)->GetMethodID(env, clsDescriptor, "getDeclaredFields0", "(Z)[Ljava/lang/reflect/Field;");
    if (methodID == 0) {
        return NULL;
    }
    return (*env)->CallObjectMethod(env, cls, methodID, (jboolean) 0);
}

// -----------------------------------------------------------------------------------------------------------------

// Field getters:

JNIEXPORT jint JNICALL Java_narcissus_Narcissus_getIntFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    return (*env)->GetIntField(env, obj, fieldID);
}

JNIEXPORT jlong JNICALL Java_narcissus_Narcissus_getLongFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    return (*env)->GetLongField(env, obj, fieldID);
}

JNIEXPORT jshort JNICALL Java_narcissus_Narcissus_getShortFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    return (*env)->GetShortField(env, obj, fieldID);
}

JNIEXPORT jchar JNICALL Java_narcissus_Narcissus_getCharFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    return (*env)->GetCharField(env, obj, fieldID);
}

JNIEXPORT jboolean JNICALL Java_narcissus_Narcissus_getBooleanFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    return (*env)->GetBooleanField(env, obj, fieldID);
}

JNIEXPORT jbyte JNICALL Java_narcissus_Narcissus_getByteFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    return (*env)->GetByteField(env, obj, fieldID);
}

JNIEXPORT jfloat JNICALL Java_narcissus_Narcissus_getFloatFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    return (*env)->GetFloatField(env, obj, fieldID);
}

JNIEXPORT jdouble JNICALL Java_narcissus_Narcissus_getDoubleFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    return (*env)->GetDoubleField(env, obj, fieldID);
}

JNIEXPORT jobject JNICALL Java_narcissus_Narcissus_getObjectFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    return (*env)->GetObjectField(env, obj, fieldID);
}

// Field setters:

JNIEXPORT void JNICALL Java_narcissus_Narcissus_setIntFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field, jint val) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    (*env)->SetIntField(env, obj, fieldID, val);
}

JNIEXPORT void JNICALL Java_narcissus_Narcissus_setLongFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field, jlong val) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    (*env)->SetLongField(env, obj, fieldID, val);
}

JNIEXPORT void JNICALL Java_narcissus_Narcissus_setShortFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field, jshort val) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    (*env)->SetShortField(env, obj, fieldID, val);
}

JNIEXPORT void JNICALL Java_narcissus_Narcissus_setCharFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field, jchar val) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    (*env)->SetCharField(env, obj, fieldID, val);
}

JNIEXPORT void JNICALL Java_narcissus_Narcissus_setBooleanFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field, jboolean val) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    (*env)->SetBooleanField(env, obj, fieldID, val);
}

JNIEXPORT void JNICALL Java_narcissus_Narcissus_setByteFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field, jbyte val) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    (*env)->SetByteField(env, obj, fieldID, val);
}

JNIEXPORT void JNICALL Java_narcissus_Narcissus_setFloatFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field, jfloat val) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    (*env)->SetFloatField(env, obj, fieldID, val);
}

JNIEXPORT void JNICALL Java_narcissus_Narcissus_setDoubleFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field, jdouble val) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    (*env)->SetDoubleField(env, obj, fieldID, val);
}

JNIEXPORT void JNICALL Java_narcissus_Narcissus_setObjectFieldVal(JNIEnv *env, jclass ignored, jobject obj, jobject field, jobject val) {
    jfieldID fieldID = (*env)->FromReflectedField(env, field);
    (*env)->SetObjectField(env, obj, fieldID, val);
}

// -----------------------------------------------------------------------------------------------------------------

// Call methods

JNIEXPORT void JNICALL Java_narcissus_Narcissus_callVoidMethod(JNIEnv *env, jclass ignored, jobject obj, jobject method, jobjectArray args) {
    jmethodID methodID = (*env)->FromReflectedMethod(env, method);
    jsize num_args = (*env)->GetArrayLength(env, args);
    if (num_args == 0) {
        (*env)->CallVoidMethod(env, obj, methodID);
    }
    jvalue arg_jvalues[num_args];
    if (unbox(env, method, args, num_args, arg_jvalues)) {
        (*env)->CallVoidMethodA(env, obj, methodID, arg_jvalues);
    }
}

JNIEXPORT jint JNICALL Java_narcissus_Narcissus_callIntMethod(JNIEnv *env, jclass ignored, jobject obj, jobject method, jobjectArray args) {
    jmethodID methodID = (*env)->FromReflectedMethod(env, method);
    jsize num_args = (*env)->GetArrayLength(env, args);
    if (num_args == 0) {
        return (*env)->CallIntMethod(env, obj, methodID);
    }
    jvalue arg_jvalues[num_args];
    return unbox(env, method, args, num_args, arg_jvalues) ? (*env)->CallIntMethodA(env, obj, methodID, arg_jvalues) : (jint) 0;
}

JNIEXPORT jlong JNICALL Java_narcissus_Narcissus_callLongMethod(JNIEnv *env, jclass ignored, jobject obj, jobject method, jobjectArray args) {
    jmethodID methodID = (*env)->FromReflectedMethod(env, method);
    jsize num_args = (*env)->GetArrayLength(env, args);
    if (num_args == 0) {
        return (*env)->CallLongMethod(env, obj, methodID);
    }
    jvalue arg_jvalues[num_args];
    return unbox(env, method, args, num_args, arg_jvalues) ? (*env)->CallLongMethodA(env, obj, methodID, arg_jvalues) : (jlong) 0;
}

JNIEXPORT jshort JNICALL Java_narcissus_Narcissus_callShortMethod(JNIEnv *env, jclass ignored, jobject obj, jobject method, jobjectArray args) {
    jmethodID methodID = (*env)->FromReflectedMethod(env, method);
    jsize num_args = (*env)->GetArrayLength(env, args);
    if (num_args == 0) {
        return (*env)->CallShortMethod(env, obj, methodID);
    }
    jvalue arg_jvalues[num_args];
    return unbox(env, method, args, num_args, arg_jvalues) ? (*env)->CallShortMethodA(env, obj, methodID, arg_jvalues) : (jshort) 0;
}

JNIEXPORT jchar JNICALL Java_narcissus_Narcissus_callCharMethod(JNIEnv *env, jclass ignored, jobject obj, jobject method, jobjectArray args) {
    jmethodID methodID = (*env)->FromReflectedMethod(env, method);
    jsize num_args = (*env)->GetArrayLength(env, args);
    if (num_args == 0) {
        return (*env)->CallCharMethod(env, obj, methodID);
    }
    jvalue arg_jvalues[num_args];
    return unbox(env, method, args, num_args, arg_jvalues) ? (*env)->CallCharMethodA(env, obj, methodID, arg_jvalues) : (jchar) 0;
}

JNIEXPORT jboolean JNICALL Java_narcissus_Narcissus_callBooleanMethod(JNIEnv *env, jclass ignored, jobject obj, jobject method, jobjectArray args) {
    jmethodID methodID = (*env)->FromReflectedMethod(env, method);
    jsize num_args = (*env)->GetArrayLength(env, args);
    if (num_args == 0) {
        return (*env)->CallBooleanMethod(env, obj, methodID);
    }
    jvalue arg_jvalues[num_args];
    return unbox(env, method, args, num_args, arg_jvalues) ? (*env)->CallBooleanMethodA(env, obj, methodID, arg_jvalues) : (jboolean) 0;
}

JNIEXPORT jbyte JNICALL Java_narcissus_Narcissus_callByteMethod(JNIEnv *env, jclass ignored, jobject obj, jobject method, jobjectArray args) {
    jmethodID methodID = (*env)->FromReflectedMethod(env, method);
    jsize num_args = (*env)->GetArrayLength(env, args);
    if (num_args == 0) {
        return (*env)->CallByteMethod(env, obj, methodID);
    }
    jvalue arg_jvalues[num_args];
    return unbox(env, method, args, num_args, arg_jvalues) ? (*env)->CallByteMethodA(env, obj, methodID, arg_jvalues) : (jbyte) 0;
}

JNIEXPORT jfloat JNICALL Java_narcissus_Narcissus_callFloatMethod(JNIEnv *env, jclass ignored, jobject obj, jobject method, jobjectArray args) {
    jmethodID methodID = (*env)->FromReflectedMethod(env, method);
    jsize num_args = (*env)->GetArrayLength(env, args);
    if (num_args == 0) {
        return (*env)->CallFloatMethod(env, obj, methodID);
    }
    jvalue arg_jvalues[num_args];
    return unbox(env, method, args, num_args, arg_jvalues) ? (*env)->CallFloatMethodA(env, obj, methodID, arg_jvalues) : (jfloat) 0;
}

JNIEXPORT jdouble JNICALL Java_narcissus_Narcissus_callDoubleMethod(JNIEnv *env, jclass ignored, jobject obj, jobject method, jobjectArray args) {
    jmethodID methodID = (*env)->FromReflectedMethod(env, method);
    jsize num_args = (*env)->GetArrayLength(env, args);
    if (num_args == 0) {
        return (*env)->CallDoubleMethod(env, obj, methodID);
    }
    jvalue arg_jvalues[num_args];
    return unbox(env, method, args, num_args, arg_jvalues) ? (*env)->CallDoubleMethodA(env, obj, methodID, arg_jvalues) : (jdouble) 0;
}

JNIEXPORT jobject JNICALL Java_narcissus_Narcissus_callObjectMethod(JNIEnv *env, jclass ignored, jobject obj, jobject method, jobjectArray args) {
    jmethodID methodID = (*env)->FromReflectedMethod(env, method);
    jsize num_args = (*env)->GetArrayLength(env, args);
    if (num_args == 0) {
        return (*env)->CallObjectMethod(env, obj, methodID);
    }
    jvalue arg_jvalues[num_args];
    return unbox(env, method, args, num_args, arg_jvalues) ? (*env)->CallObjectMethodA(env, obj, methodID, arg_jvalues) : (jobject) NULL;
}

