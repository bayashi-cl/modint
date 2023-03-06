#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include <utility>

constexpr unsigned long py_mod(long long x, unsigned long m) {
    x %= m;
    if (x < 0) x += m;
    return x;
}

constexpr std::pair<long long, long long> inv_gcd(long long a, long long b) {
    a = py_mod(a, b);
    if (a == 0) return {b, 0};

    long long s = b, t = a;
    long long m0 = 0, m1 = 1;

    while (t) {
        long long u = s / t;
        s -= t * u;
        m0 -= m1 * u;

        auto tmp = s;
        s = t;
        t = tmp;
        tmp = m0;
        m0 = m1;
        m1 = tmp;
    }
    if (m0 < 0) m0 += b / s;
    return {s, m0};
}

// 逆元がなかったら例外をセットする。
unsigned long inv_impl(unsigned long x, unsigned long mod) {
    auto res = inv_gcd(x, mod);
    if (res.first != 1) {
        PyErr_SetString(PyExc_ValueError,
                        "value and mod are not mutually prime.");
    }
    return res.second;
}

unsigned long pow_impl(unsigned long x, long long n, unsigned long mod) {
    bool neg = false;
    if (n < 0) {
        n = -n;
        neg = true;
    }
    unsigned long result = 1;
    while (n) {
        if (n & 1) result = result * x % mod;
        x = x * x % mod;
        n >>= 1;
    }
    if (neg) result = inv_impl(result, mod);

    return result;
}

constexpr long long MOD_MAX = INT32_MAX;

namespace ModInt {
extern PyTypeObject ModIntType;

struct ModIntObject {
    PyObject_HEAD
    unsigned long value, mod;
};

// utility macros
#define ModInt_Check(op) PyObject_TypeCheck(op, &ModIntType)
#define GET_MODINT_VALUE(o) (((ModIntObject*)(o))->value)
#define GET_MODINT_MOD(o) (((ModIntObject*)(o))->mod)
#define CHECK_MOD_EQUALITY(lhs, rhs)                                       \
    if (GET_MODINT_MOD(lhs) != GET_MODINT_MOD(rhs)) {                      \
        return PyErr_Format(PyExc_ValueError,                              \
                            "lhs.mod {=%lu} differs from rhs.mod {=%lu}.", \
                            GET_MODINT_MOD(lhs), GET_MODINT_MOD(rhs));     \
    }

// generate ModInt without taking mod and mod check
PyObject* ModInt_new_raw(unsigned long value, unsigned long mod) {
    PyTypeObject* type = &ModIntType;
    ModIntObject* self = (ModIntObject*)type->tp_alloc(type, 0);

    self->value = value;
    self->mod = mod;
    return (PyObject*)self;
}

int ModInt_init(ModIntObject* self, PyObject* args, PyObject* kwargs) {
    static char* kwlist[] = {(char*)"value", (char*)"mod", NULL};
    long long value = 0, mod = 0;

    if (not PyArg_ParseTupleAndKeywords(args, kwargs, "LL", kwlist, &value,
                                        &mod)) {
        return -1;
    }
    if (mod <= 0) {
        PyErr_SetString(PyExc_ValueError, "mod is not positive.");
        return -1;
    }
    if (mod > MOD_MAX) {
        PyErr_Format(PyExc_ValueError, "mod exceeds %lu (=INT32_MAX).",
                     MOD_MAX);
        return -1;
    }

    self->value = py_mod(value, mod);
    self->mod = static_cast<unsigned long>(mod);
    return 0;
}

PyObject* ModInt_str(PyObject* self) {
    return PyUnicode_FromFormat("%lu", GET_MODINT_VALUE(self));
}

PyMemberDef ModInt_members[] = {
    {"value", T_ULONG, offsetof(ModIntObject, value), READONLY},
    {"mod", T_ULONG, offsetof(ModIntObject, mod), READONLY},
    {NULL} /* Sentinel */
};

PyDoc_STRVAR(ModInt_inv_doc, "TODO");
PyObject* ModInt_inv(ModIntObject* self, PyObject* Py_UNUSED(ignored)) {
    unsigned long value = GET_MODINT_VALUE(self);
    unsigned long mod = GET_MODINT_MOD(self);

    value = inv_impl(value, mod);
    if (PyErr_Occurred()) return NULL;
    return ModInt_new_raw(value, mod);
}

PyMethodDef ModInt_methods[] = {
    {"inv", (PyCFunction)ModInt_inv, METH_NOARGS, ModInt_inv_doc},
    {NULL} /* Sentinel */
};

namespace operators {
constexpr auto typeerr_msg =
    "operator is only available between int and ModInt.";

#define PARSE_LRMOD(lhs, rhs, lvalue, rvalue, mod)        \
    if (ModInt_Check(lhs) and ModInt_Check(rhs)) {        \
        /* ModInt (op) ModInt*/                           \
        CHECK_MOD_EQUALITY(lhs, rhs);                     \
        mod = GET_MODINT_MOD(lhs);                        \
        lvalue = GET_MODINT_VALUE(lhs);                   \
        rvalue = GET_MODINT_VALUE(rhs);                   \
    } else if (ModInt_Check(lhs) and PyLong_Check(rhs)) { \
        /* ModInt (op) int*/                              \
        mod = GET_MODINT_MOD(lhs);                        \
        lvalue = GET_MODINT_VALUE(lhs);                   \
        rvalue = py_mod(PyLong_AsLongLong(rhs), mod);     \
    } else if (PyLong_Check(lhs) and ModInt_Check(rhs)) { \
        /* int (op) ModInt*/                              \
        mod = GET_MODINT_MOD(rhs);                        \
        lvalue = py_mod(PyLong_AsLongLong(lhs), mod);     \
        rvalue = GET_MODINT_VALUE(rhs);                   \
    } else {                                              \
        PyErr_SetString(PyExc_TypeError, typeerr_msg);    \
        return NULL;                                      \
    }

PyObject* ModInt_add(PyObject* lhs, PyObject* rhs) {
    unsigned long lvalue = 0, rvalue = 0, mod = 0;
    PARSE_LRMOD(lhs, rhs, lvalue, rvalue, mod);

    unsigned long value = lvalue + rvalue;
    if (value > mod) value -= mod;
    return ModInt_new_raw(value, mod);
}

PyObject* ModInt_subtract(PyObject* lhs, PyObject* rhs) {
    unsigned long lvalue = 0, rvalue = 0, mod = 0;
    PARSE_LRMOD(lhs, rhs, lvalue, rvalue, mod);

    // value < mod < INT32_MAX で valueがulongなので
    // 0UL - value >= mod が保証される。
    unsigned long value = lvalue - rvalue;
    if (value >= mod) value += mod;
    return ModInt_new_raw(value, mod);
}

PyObject* ModInt_multiply(PyObject* lhs, PyObject* rhs) {
    unsigned long lvalue = 0, rvalue = 0, mod = 0;
    PARSE_LRMOD(lhs, rhs, lvalue, rvalue, mod);

    unsigned long value = rvalue * lvalue % mod;
    return ModInt_new_raw(value, mod);
}

PyObject* ModInt_power(PyObject* lhs, PyObject* rhs, PyObject* _mod) {
    if ((PyObject*)_mod != Py_None) {
        PyErr_SetString(
            PyExc_TypeError,
            "pow() 3rd argument not allowed unless all arguments are integers");
        return NULL;
    }

    unsigned long lvalue = 0, mod = 0;
    long long rvalue = 0;
    if (ModInt_Check(lhs) and PyLong_Check(rhs)) {
        // ModInt ** int
        lvalue = GET_MODINT_VALUE(lhs);
        rvalue = PyLong_AsLongLong(rhs);
        mod = GET_MODINT_MOD(lhs);
    } else {
        PyErr_SetString(PyExc_TypeError, typeerr_msg);
        return NULL;
    }

    unsigned long value = pow_impl(lvalue, rvalue, mod);
    if (PyErr_Occurred()) return NULL;
    return ModInt_new_raw(value, mod);
}

PyObject* ModInt_negative(PyObject* self) {
    unsigned long value = 0;
    unsigned long mod = GET_MODINT_MOD(self);

    value -= GET_MODINT_VALUE(self);
    if (value >= mod) value += mod;
    return ModInt_new_raw(value, mod);
}

PyObject* ModInt_positive(PyObject* self) {
    return ModInt_new_raw(GET_MODINT_VALUE(self), GET_MODINT_MOD(self));
}

int ModInt_bool(PyObject* self) { return GET_MODINT_VALUE(self) == 0 ? 0 : 1; }

PyObject* ModInt_int(PyObject* self) {
    return PyLong_FromLong(GET_MODINT_VALUE(self));
}

PyObject* ModInt_floor_divide(PyObject* lhs, PyObject* rhs) {
    unsigned long lvalue = 0, rvalue = 0, mod = 0;
    PARSE_LRMOD(lhs, rhs, lvalue, rvalue, mod);

    unsigned long value = lvalue * inv_impl(rvalue, mod) % mod;
    return ModInt_new_raw(value, mod);
}

PyNumberMethods ModInt_as_number = {
    .nb_add = ModInt_add,
    .nb_subtract = ModInt_subtract,
    .nb_multiply = ModInt_multiply,
    .nb_power = ModInt_power,
    .nb_negative = ModInt_negative,
    .nb_positive = ModInt_positive,
    .nb_bool = ModInt_bool,
    .nb_int = ModInt_int,
    .nb_floor_divide = ModInt_floor_divide,
};

#undef PARSE_LRMOD

}  // namespace operators

PyDoc_STRVAR(ModInt_doc,
             R"(ModInt(value: int, mod: int)

--
TODO:
)");
PyTypeObject ModIntType = {
    // same as `.ob_base = PyVarObject_HEAD_INIT(NULL, 0)`
    .ob_base = {PyObject_HEAD_INIT(NULL) 0},
    .tp_name = "modint._modint.ModInt",
    .tp_basicsize = sizeof(ModIntObject),
    .tp_itemsize = 0,
    .tp_as_number = &operators::ModInt_as_number,
    .tp_str = (reprfunc)ModInt_str,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = ModInt_doc,
    .tp_methods = ModInt_methods,
    .tp_members = ModInt_members,
    .tp_init = (initproc)ModInt_init,
    .tp_new = PyType_GenericNew,
};

}  // namespace ModInt

namespace UsingModInt {
extern PyTypeObject UsingModIntType;

struct UsingModIntObject {
    PyObject_HEAD
    unsigned long mod;
};

int UsingModInt_init(UsingModIntObject* self,
                     PyObject* args,
                     PyObject* kwargs) {
    static char* kwlist[] = {(char*)"mod", NULL};
    long long mod = 0;

    if (not PyArg_ParseTupleAndKeywords(args, kwargs, "L", kwlist, &mod)) {
        return -1;
    }

    if (mod <= 0) {
        PyErr_SetString(PyExc_ValueError, "mod is not positive.");
        return -1;
    }
    if (mod > MOD_MAX) {
        PyErr_Format(PyExc_ValueError, "mod exceeds %lu (=INT32_MAX).",
                     MOD_MAX);
        return -1;
    }

    self->mod = static_cast<unsigned long>(mod);
    return 0;
}

PyObject* UsingModInt_call(UsingModIntObject* self,
                           PyObject* args,
                           PyObject* kwargs) {
    static char* kwlist[] = {(char*)"value", NULL};
    long long value = 0;
    if (not PyArg_ParseTupleAndKeywords(args, kwargs, "|L", kwlist, &value)) {
        return NULL;
    }
    return ModInt::ModInt_new_raw(py_mod(value, self->mod), self->mod);
}

PyMemberDef UsingModInt_members[] = {
    {"mod", T_ULONG, offsetof(UsingModIntObject, mod), READONLY},
    {NULL} /* Sentinel */
};

PyDoc_STRVAR(UsingModInt_doc,
             R"(UsingModInt(mod: int)

--
TODO: The factory class for ModInt.
)");

PyTypeObject UsingModIntType = {
    // same as `.ob_base = PyVarObject_HEAD_INIT(NULL, 0)`
    .ob_base = {PyObject_HEAD_INIT(NULL) 0},
    .tp_name = "modint._modint.UsingModInt",
    .tp_basicsize = sizeof(UsingModIntObject),
    .tp_itemsize = 0,
    .tp_call = (ternaryfunc)UsingModInt_call,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = UsingModInt_doc,
    .tp_members = UsingModInt_members,
    .tp_init = (initproc)UsingModInt_init,
    .tp_new = PyType_GenericNew,
};
}  // namespace UsingModInt

PyDoc_STRVAR(_modint_doc,
             R"(modint module

--
TODO: document of _modint here.
)");

int _modint_exec(PyObject* m) {
    // Ready types
    if (PyType_Ready(&ModInt::ModIntType) < 0) return -1;
    if (PyType_Ready(&UsingModInt::UsingModIntType) < 0) return -1;

    // Add types
    if (PyModule_AddObject(m, "ModInt", Py_NewRef(&ModInt::ModIntType)) < 0) {
        return -1;
    }
    if (PyModule_AddObject(m, "UsingModInt",
                           Py_NewRef(&UsingModInt::UsingModIntType)) < 0) {
        return -1;
    }
    return 0;
}

PyModuleDef_Slot _modint_slots[] = {
    {Py_mod_exec, (void*)_modint_exec},
    {0, NULL},
};

PyModuleDef _modintmodule = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "_modint",
    .m_doc = _modint_doc,
    .m_size = 0,
    .m_slots = _modint_slots,
};

PyMODINIT_FUNC PyInit__modint(void) { return PyModuleDef_Init(&_modintmodule); }
