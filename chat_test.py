# -*- coding: UTF-8 -*-
import http.client
import urllib.parse
import json
import subprocess
import time
import requests
import os

# 启动C++程序
# subprocess.Popen(['chmod', '+x', 'demo'])
subprocess.Popen(['./demo'])

# Chat Configuration
# API_KEY = "ebb785194c713e7b419ca8742277d414.hCBC11QCZvC5N0YK"
API_KEY = "Please change to your own API Key " # https://open.bigmodel.cn/
BASE_URL = "https://open.bigmodel.cn/api/paas/v4/chat/completions"
history = [{"role": "system", "content": "请尽量保持回答的简洁性。"}]
# Aliyun
host = 'nls-gateway-cn-shanghai.aliyuncs.com'


def chat(query, history):
    history += [{"role": "user", "content": query}]
    data = {
        "model": "glm-3-turbo", ##glm-4
        "messages": history,
        "temperature": 0.3,
        "stream": True,
    }

    headers = {
        "Authorization": f"Bearer {API_KEY}",
        "Content-Type": "application/json"
    }

    complete_text = ''
    response_accumulator = []
    response = requests.post(BASE_URL, data=json.dumps(data), headers=headers, stream=True)
    send_to_lvgl(f"[CLEAR]{query}: ")
    try:
        for chunk in response.iter_lines():
            if chunk:
                chunk_str = chunk.decode("utf-8")
                if chunk_str.startswith("data: "):
                    chunk_str = chunk_str[len("data: "):]

                done, result = process_chunk(chunk_str,response_accumulator)
                chunk_str = "data: " + chunk_str
                if result:
                    send_to_lvgl(result)

        if done:
            complete_text = ''.join(response_accumulator)
            return complete_text

    except Exception as e:
        print(f"Error: {str(e)}")



def process_chunk(chunk,response_accumulator):
    if chunk.strip() == "[DONE]":
        return True, None
    try:
        data = json.loads(chunk)
        # print('process_chunk data:', data)
        if 'choices' in data and data['choices']:
            for choice in data['choices']:
                if 'delta' in choice and 'content' in choice['delta'] and choice['delta']['content']:
                    result = choice['delta']['content']
                    response_accumulator.append(result)
                    return False, result
    except Exception as e:
        print(f"处理数据块时出错: {e}")
    return False, None



def send_to_lvgl(text):
    pipe_name = '/tmp/query_pipe'
    try:
        with open(pipe_name, 'w') as pipe:
            pipe.write(text)
            pipe.flush()
    except Exception as e:
        print(f"LVGL send error: {e}")

while True:
    try:
        query = input("请输入你的问题: ")
        answer = chat(query, history)
        print("回答:", answer)
    except Exception as e:
        print(e)