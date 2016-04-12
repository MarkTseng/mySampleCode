#!/usr/bin/env python
import urllib2
import json

MOIDATA='http://data.moi.gov.tw/MoiOD/System/DownloadFile.aspx?DATA=1E91AC2B-48B3-47A3-83B4-EB4A8AD930B9'

def fetch():
	response = urllib2.urlopen(MOIDATA)
	result = response.read()
	#print(result)
	json_result = json.loads(result)
	count = json_result['count']
	print(count)
	version = json_result['version']
	print(version)
	for i in xrange(0,count):
		print('count %d')%i
		json_result2 = json.dumps(json_result['result'][i]['areaNm'], ensure_ascii=False)
		print(json_result2)
		json_result2 = json.dumps(json_result['result'][i]['roadtype'], ensure_ascii=False)
		print(json_result2)
		json_result2 = json.dumps(json_result['result'][i]['comment'], ensure_ascii=False)
		print(json_result2)
		json_result2 = json.dumps(json_result['result'][i]['happentime'], ensure_ascii=False)
		print(json_result2)
		json_result2 = json.dumps(json_result['result'][i]['happendate'], ensure_ascii=False)
		print(json_result2)
		json_result2 = json.dumps(json_result['result'][i]['region'], ensure_ascii=False)
		print(json_result2)
		json_result2 = json.dumps(json_result['result'][i]['x1'], ensure_ascii=False)
		print(json_result2)
		json_result2 = json.dumps(json_result['result'][i]['y1'], ensure_ascii=False)
		print(json_result2)
		print("\n")

if __name__ == '__main__':
	print(MOIDATA)
	fetch()
