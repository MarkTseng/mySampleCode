# -*- coding: utf8 -*-

json_string = """
{
    "pk": 1, 
    "fa": "cc.ee", 
    "fb": {
        "fc": "中文", 
        "fd_id": "12345"
    }
}"""

import json
data = json.loads(json_string)
if data["fa"] == "cc.ee":
    data["fb"]["new_key"] = "cc.ee was present!"

print json.dumps(data,ensure_ascii=False)

