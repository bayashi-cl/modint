#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

// forward declarations.
extern PyTypeObject ModIntType;

struct ModIntObject {
    PyObject_HEAD
    static inline unsigned long MOD = 998244353;
    unsigned long value;
};

// utility macros
#define Modint_Check(op) PyObject_TypeCheck(op, &ModIntType)
#define GET_MODINT_VALUE(o) (((ModIntObject*)(o))->value)
#define GET_MODINT_MOD(o) (((ModIntObject*)(o))->MOD)

// generate ModInt without take mod
static PyObject* new_rawModInt(int value) {
    PyTypeObject* type = &ModIntType;
    ModIntObject* self = (ModIntObject*)type->tp_alloc(type, 0);

    self->value = value;
    return (PyObject*)self;
}

static void ModInt_dealloc(ModIntObject* self) {
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* ModInt_new(PyTypeObject* type,
                            PyObject* args,
                            PyObject* kwds) {
    ModIntObject* self;
    self = (ModIntObject*)type->tp_alloc(type, 0);
    if (self != NULL) self->value = 0;
    return (PyObject*)self;
}

static int ModInt_init(ModIntObject* self, PyObject* args, PyObject* kwds) {
    static char* kwlist[] = {(char*)"value", NULL};
    long long value = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|L", kwlist, &value)) {
        return -1;
    }

    // safe mod
    value %= self->MOD;
    if (value < 0) value += self->MOD;

    self->value = (unsigned long)value;
    return 0;
}

static PyMemberDef ModInt_members[] = {
    {"value", T_ULONG, offsetof(ModIntObject, value), READONLY},
    {NULL} /* Sentinel */
};

// property
static PyGetSetDef ModInt_getsetters[] = {
    {NULL} /* Sentinel */
};

// methods
// class methods
// getter/setter
static PyObject* ModInt_mod(PyObject* cls, PyObject* dummy) {
    return PyLong_FromUnsignedLong(ModIntObject::MOD);
}
static PyObject* ModInt_set_mod(PyObject* cls, PyObject* args) {
    unsigned long mod = 0;
    if (not PyArg_ParseTuple(args, "K", &mod)) return NULL;
    ModIntObject::MOD = mod;
    Py_RETURN_NONE;
}

static PyMethodDef ModInt_methods[] = {
    {"mod", (PyCFunction)ModInt_mod, METH_NOARGS | METH_CLASS, "get mod"},
    {"set_mod", (PyCFunction)ModInt_set_mod, METH_VARARGS | METH_CLASS,
     "set mod"},
    {NULL} /* Sentinel */
};

// operators

static PyObject* ModInt_add(PyObject* lhs, PyObject* rhs) {
    PyObject* result = Py_NotImplemented;
    unsigned long value = 0, mod = GET_MODINT_MOD(lhs);

    if (Modint_Check(lhs)) {
        if (Modint_Check(rhs)) {
            // ModInt + ModInt
            value = GET_MODINT_VALUE(lhs) + GET_MODINT_VALUE(rhs);
            if (value > mod) value -= mod;
            result = new_rawModInt(value);
        } else if (PyLong_Check(rhs)) {
            // ModInt + int
            value = GET_MODINT_VALUE(lhs) + PyLong_AsLongLong(rhs) % mod;
            if (value > mod) value -= mod;
            result = new_rawModInt(value);
        }
    } else if (PyLong_Check(lhs) && Modint_Check(rhs)) {
        // int + ModInt
        value = PyLong_AsLongLong(lhs) % mod + GET_MODINT_VALUE(rhs);
        if (value > mod) value -= mod;
        result = new_rawModInt(value);
    }
    if (result == Py_NotImplemented) Py_INCREF(result);
    return result;
}

static PyNumberMethods Modint_as_number = {
    .nb_add = ModInt_add,
    .nb_subtract = 0,
    .nb_multiply = 0,
    .nb_power = 0,
    .nb_negative = 0,
    .nb_positive = 0,
    .nb_absolute = 0,
    .nb_bool = 0,
    .nb_int = 0,
    .nb_floor_divide = 0,
    .nb_true_divide = 0,
};

PyDoc_STRVAR(ModInt_doc,
             R"(ModInt(value: int = 0)

--
It is the constructor for the integer type. value is stored in modint
after taking mod.
)");

PyTypeObject ModIntType = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)  //
                   .tp_name = "modint.modint.ModInt",
    .tp_basicsize = sizeof(ModIntObject),
    .tp_itemsize = 0,

    .tp_dealloc = (destructor)ModInt_dealloc,

    .tp_as_number = &Modint_as_number,

    .tp_flags = Py_TPFLAGS_DEFAULT /* | Py_TPFLAGS_BASETYPE */,

    .tp_doc = ModInt_doc,

    .tp_methods = ModInt_methods,
    .tp_members = ModInt_members,
    .tp_getset = ModInt_getsetters,

    .tp_init = (initproc)ModInt_init,
    .tp_new = ModInt_new,
};

PyDoc_STRVAR(modint_doc,
             R"(modint module

--
It is the struct that treats the modular arithmetic.
All the remaining parts of AC Library works without modint,
so you don't necessarily read this to use the remaining parts.
)");

static PyModuleDef modintmodule = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "modint",
    .m_doc = modint_doc,
    .m_size = -1,
};

PyMODINIT_FUNC PyInit_modint(void) {
    PyObject* m;
    if (PyType_Ready(&ModIntType)) return NULL;

    m = PyModule_Create(&modintmodule);
    if (m == NULL) return NULL;

    Py_INCREF(&ModIntType);
    if (PyModule_AddObject(m, "ModInt", (PyObject*)&ModIntType) < 0) {
        Py_DECREF(&ModIntType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
