#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>

static PyObject *cjson_loads(PyObject *self, PyObject *args) {
  char *json_string;
  if (!PyArg_ParseTuple(args, "s#", &json_string)) {
    return NULL;
  }

  PyObject *py_dict = PyDict_New();

  if (!py_dict) {
    printf("ERROR:Failed to create py_dict\n");
    return NULL;
  }

  const char *c = json_string;
  while (isspace(*c))
    c++;
  if (*c != '{') {
    // printf("ERROR:Invalid JSON: must start with '{'\n");
    PyErr_SetString(PyExc_ValueError, "Invalid JSON: must start with '{'\n");
    return NULL;
  }
  c++;
  while (*c && *c != '}') {
    while (isspace(*c))
      c++;
    if (*c != '"') {
      // printf("ERROR:Invalid JSON: key must start with '\"'\n");
      PyErr_SetString(PyExc_ValueError,
                      "Invalid JSON: key must start with '\"'\n");
      return NULL;
    }
    c++;
    const char *start_key = c;
    while (*c && *c != '"' && *c != ':')
      c++;
    if (*c != '"' || *c == ':') {
      // printf("ERROR:Invalid JSON: key must end with '\"'\n");
      PyErr_SetString(PyExc_ValueError,
                      "Invalid JSON: key must end with '\"'\n");
      return NULL;
    }
    size_t len = c - start_key;
    char *key = (char *)malloc(len + 1);
    strncpy(key, start_key, len);
    key[len] = '\0';
    PyObject *key_py = Py_BuildValue("s", key);
    free(key);
    c++;
    while (isspace(*c))
      c++;
    if (*c != ':') {
      // printf("ERROR:Invalid JSON: sep must be with ':'\n");
      PyErr_SetString(PyExc_ValueError, "Invalid JSON: sep must be with ':'\n");
      return NULL;
    }
    c++;
    while (isspace(*c))
      c++;
    PyObject *value = NULL;
    if (*c == '"') {
      c++;
      const char *val_start = c;
      while (*c && *c != '"')
        c++;
      if (*c != '"') {
        // printf("ERROR:Invalid JSON: value started with '\"' must be ended
        // with '\"'\n");
        PyErr_SetString(
            PyExc_ValueError,
            "Invalid JSON: value started with '\"' must be ended with '\"'\n");
        return NULL;
      }
      size_t val_len = c - val_start;
      char *val = (char *)malloc(val_len + 1);
      strncpy(val, val_start, val_len);
      val[val_len] = '\0';
      value = Py_BuildValue("s", val);
      free(val);
      c++;
    } else if (isdigit(*c) || *c == '-') {
      char *endptr;
      long val = strtol(c, &endptr, 10);
      if (endptr == c) {
        // printf("ERROR:Invalid JSON: something wrong with digit value\n");
        PyErr_SetString(PyExc_ValueError,
                        "Invalid JSON: something wrong with digit value\n");
        return NULL;
      }
      value = Py_BuildValue("i", val);
      c = endptr;
    } else {
      // printf("ERROR:Invalid JSON: value must be string with '\"' or be
      // digit\n");
      PyErr_SetString(
          PyExc_TypeError,
          "Invalid JSON: value must be string with '\"' or be digit\n");
      return NULL;
    }
    if (PyDict_SetItem(py_dict, key_py, value) < 0) {
      PyErr_SetString(PyExc_RuntimeError,
                      "Invalid JSON: something wrong with create dict\n");
      return NULL;
    }
    Py_DECREF(key_py);
    Py_DECREF(value);
    while (isspace(*c))
      c++;
    if (*c == ',') {
      c++;
    } else if (*c != '}') {
      // printf("ERROR:Invalid JSON: must ended with '}'\n");
      PyErr_SetString(PyExc_ValueError, "Invalid JSON: must ended with '}'\n");
      return NULL;
    }
  }
  return py_dict;
}

static PyObject *cjson_dumps(PyObject *self, PyObject *args) {
  PyObject *py_dict;
  if (!PyArg_ParseTuple(args, "O", &py_dict)) {
    return NULL;
  }
  if (!PyDict_Check(py_dict)) {
    PyErr_SetString(PyExc_TypeError, "Expected dictionary\n");
    return NULL;
  }
  PyObject *keys = PyDict_Keys(py_dict);
  if (!keys)
    return NULL;

  Py_ssize_t size = PyList_Size(keys);
  PyObject *result = PyUnicode_FromString("{");
  for (Py_ssize_t i = 0; i < size; i++) {
    PyObject *key = PyList_GetItem(keys, i);
    PyObject *value = PyDict_GetItem(py_dict, key);

    if (!PyUnicode_Check(key)) {
      PyErr_SetString(PyExc_TypeError, "Key must be string\n");
      return NULL;
    }

    const char *key_str = PyUnicode_AsUTF8(key);
    PyObject *value_str_obj = NULL;

    if (PyLong_Check(value)) {
      value_str_obj = PyObject_Str(value);
    } else if (PyUnicode_Check(value)) {
      value_str_obj = PyUnicode_FromFormat("\"%s\"", PyUnicode_AsUTF8(value));
    } else {
      PyErr_SetString(PyExc_TypeError, "Value must be string or digit\n");
      return NULL;
    }

    const char *value_str = PyUnicode_AsUTF8(value_str_obj);
    PyObject *entry = PyUnicode_FromFormat("\"%s\":%s", key_str, value_str);

    if (i > 0) {
      PyUnicode_AppendAndDel(&result, PyUnicode_FromString(","));
    }
    PyUnicode_AppendAndDel(&result, entry);
    Py_DECREF(value_str_obj);
  }
  Py_DECREF(keys);
  PyUnicode_AppendAndDel(&result, PyUnicode_FromString("}"));
  return result;
}

static PyMethodDef cjson_methods[] = {
    {"loads", cjson_loads, METH_VARARGS, "Convert to Python dict."},
    {"dumps", cjson_dumps, METH_VARARGS, "Convert Python dict to string"},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef cjson_module = {
    PyModuleDef_HEAD_INIT, "cjson", "Module to pack/unpack json in python.", -1,
    cjson_methods};

PyMODINIT_FUNC PyInit_cjson(void) { return PyModule_Create(&cjson_module); }
