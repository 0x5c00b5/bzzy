from flask import Flask, redirect, request
import os, datetime
import sqlite3
import json

bzzy = Flask(__name__)

FN_DATABASE = 'bzzydata.db'
FN_JSONDATA = 'dev-data/data.json'

@bzzy.route('/data',methods=['POST'])
def recvdata(): 
        with sqlite3.connect(FN_DATABASE) as con:
            cur = con.cursor()

            # DO SHIT
            cur_datetime = datetime.datetime.now()
            node_id = int(request.form["id"])
            clients = int(request.form["clients"])//2
            date = str(cur_datetime.year)+'-'+str(cur_datetime.month)+'-'+str(cur_datetime.day)
            time = str(cur_datetime.hour)+':'+str(cur_datetime.minute)+':'+str(cur_datetime.second)
            sql_data = request.form["id"]+",'"+date+"','"+time+"',"+str(clients)
            cur.execute('INSERT INTO data VALUES ('+sql_data+')')

            con.commit()
        max_apt_occupancy = 25
        percent = (clients//max_apt_occupancy)*100
        lowPop = True if percent > 75 else False
        update_JSON(node_id, clients, str(percent), lowPop)
        return ('', 204)

def update_JSON(node_id, quantity, ratio, lowPop):
    with open(FN_JSONDATA, 'r') as jsonfile:
        data = json.load(jsonfile)
        for item in data:
            if item['id'] == node_id:
                item['quantity'] = quantity
                item['ratio'] = ratio
                item['lowPopulation'] = lowPop
                break
            print('[!] Unknown ID:', node_id)
    with open(FN_JSONDATA, 'w') as jsonfile:
        json.dump(data, jsonfile)

def parse_db(node_id):
    with sqlite3.connect(FN_DATABASE) as con:
        cur = con.cursor()
        for row in cur.execute('SELECT * FROM data WHERE ID = '+str(node_id)):
            print(row)

if __name__ == '__main__':
    bzzy.run(debug=True,host='0.0.0.0',port=6969)