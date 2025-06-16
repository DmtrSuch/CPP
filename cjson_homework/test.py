#!/usr/bin/env python3
import cjson

import json

import ujson

import time
from faker import Faker
import random
import json

fake = Faker()

def generate_json(n=30):
    results = []

    for _ in range(n):
        is_valid = random.choice([True, False])
        corrupt_type = ""
        if is_valid:
            d = {
                fake.word(): random.choice([
                    fake.word(),
                    fake.random_int(min=0, max=100),
                ]) for _ in range(random.randint(1, 4))
            }
        else:
            corrupt_type = random.choice(["Expected dictionary\n",
                                          "Key must be string\n",
                                          "Value must be string or digit\n",
                                        ])
            if corrupt_type == "Expected dictionary\n":
                key = fake.word()
                val = fake.word()
                d = [key, val]
            elif corrupt_type == "Key must be string\n":
                key = fake.random_int(min=0, max=100)
                val = fake.word()
                d = {key : val}
            elif corrupt_type == "Value must be string or digit\n":
                key = fake.word()
                val = fake.word()
                d = {key: (key, val)}
        results.append({"json": d,
                        "valid": is_valid,
                        "error": corrupt_type})
    return results

def generate_json_str(n=30):
    results = []

    for _ in range(n):
        is_valid = random.choice([True, False])
        corrupt_type = ""
        if is_valid:
            d = {
                fake.word(): random.choice([
                    fake.word(),
                    fake.random_int(min=0, max=100),
                ]) for _ in range(random.randint(1, 4))
            }
            json_str = json.dumps(d)
        else:
            corrupt_type = random.choice(["Invalid JSON: key must end with '\"'\n",
					  "Invalid JSON: key must start with '\"'\n",
					  "Invalid JSON: must start with '{'\n",
					  "Invalid JSON: must ended with '}'\n",
					  "Invalid JSON: sep must be with ':'\n",
                                          "Invalid JSON: value started with '\"' must be ended with '\"'\n",
                                          "Invalid JSON: something wrong with digit value\n",
                                        ])
            if corrupt_type == "Invalid JSON: key must end with '\"'\n":
                key = fake.word()
                val = fake.word()
                json_str = f'{{"{key}:"{val}"}}'

            elif corrupt_type == "Invalid JSON: key must start with '\"'\n":
                key = random.randint(1, 100)
                val = fake.word()
                json_str = f'{{{key}:"{val}"}}'

            elif corrupt_type == "Invalid JSON: must ended with '}'\n":
                key = fake.word()
                val = fake.word()
                json_str = "{" + f'"{key}": "{val}"'

            elif corrupt_type == "Invalid JSON: must start with '{'\n":
                key = fake.word()
                val = fake.word()
                json_str = f'"{key}": "{val}"' + "}"

            elif corrupt_type == "Invalid JSON: sep must be with ':'\n":
                key = fake.word()
                val = fake.word()
                json_str = f'{{"{key}"="{val}"}}'

            elif corrupt_type == "Invalid JSON: value started with '\"' must be ended with '\"'\n":
                key = fake.word()
                val = fake.word()
                json_str = f'{{"{key}":"{val}}}'

            elif corrupt_type == "Invalid JSON: something wrong with digit value\n":
                key = fake.word()
                val = fake.word()
                json_str = f'{{"{key}":-}}'


        results.append({
            "json": json_str,
            "valid": is_valid,
	    "error": corrupt_type
        })


    return results

def test_func_loads(js_str, valid, err=None):
    if valid:
        start = time.time()
        our_c_json = cjson.loads(js_str)
        time_out_c_json = (time.time() - start) * 1_000_000
        start = time.time()
        uujson = ujson.loads(js_str)
        time_ujson = (time.time() - start) * 1_000_000
        start = time.time()
        jjson = json.loads(js_str)
        time_json = (time.time() - start) * 1_000_000
        assert jjson == uujson == our_c_json
        print(f'''
Time ourjson: {time_out_c_json} ms
Time jjson: {time_json} ms
Time ujson: {time_ujson} ms''')
    else:
        try:
            cjson.loads(js_str)
        except Exception as e:
            assert str(e) == err

def test_func_dumps(js, valid, err=None):
    if valid:
        start = time.time()
        our_c_json = cjson.dumps(js)
        time_out_c_json = (time.time() - start) * 1_000_000
        start = time.time()
        uujson = ujson.dumps(js)
        time_ujson = (time.time() - start) * 1_000_000
        start = time.time()
        jjson = json.dumps(js, separators=(',', ':'))
        time_json = (time.time() - start) * 1_000_000
        assert jjson == uujson == our_c_json
        print(f'''
Time ourjson: {time_out_c_json} ms
Time jjson: {time_json} ms
Time ujson: {time_ujson} ms''')
    else:
        try:
            cjson.dumps(js)
        except Exception as e:
            assert str(e) == err

if __name__ == '__main__':
     start = time.time()
     print("\n----Test loads----")
     for str_js in generate_json_str(100):
         test_func_loads(str_js["json"], str_js["valid"], str_js["error"])
     print("\n----Test dumps----")
     for js in  generate_json(100):
         test_func_dumps(js["json"], js["valid"], js["error"])
     print(f"\nFull time for tests {(time.time() - start) * 1_000_000} ms")
