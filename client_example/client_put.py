import httplib, urllib
import json
import pdb
import time
import sys

def getMsg(key_tmp,value_tmp):
    key = str(key_tmp)
    value = str(value_tmp)
    
    msg = {
            "method": "put",
            "key": key,
            "value": value
          }
    return json.dumps(msg,ensure_ascii=False)

msg_p = getMsg(sys.argv[1],sys.argv[2]) 

params = urllib.urlencode({'data': msg_p.encode('utf-8','ignore')})

headers = {"Content-type": "application/x-www-form-urlencoded", \
	"Accept": "text/plain"}
print int(time.time())
conn = httplib.HTTPConnection("127.0.0.1",8817)
conn.request("POST", "", params, headers)
response = conn.getresponse()
print response.status, response.reason
data = response.read()
print data
print int(time.time())
conn.close()
