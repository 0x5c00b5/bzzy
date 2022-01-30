from flask import Flask, redirect, request
import os, datetime
import sqlite3

bzzy = Flask(__name__)

FN_DATABASE = 'bzzydata.db'
FN_JSONDATA = 'dev-data/data.json'

@bzzy.route('/data',methods=['POST'])
def recvdata(): 
        with sqlite3.connect(FN_DATABASE) as con:
            cur = con.cursor()

            # DO SHIT
            cur_datetime = datetime.datetime.now()
            date = str(cur_datetime.year)+'-'+str(cur_datetime.month)+'-'+str(cur_datetime.day)
            time = str(cur_datetime.hour)+':'+str(cur_datetime.minute)+':'+str(cur_datetime.second)
            sql_data = "'"+request.form["id"]+"','"+date+"','"+time+"',"+request.form["clients"]
            cur.execute('INSERT INTO data VALUES ('+sql_data+')')

            con.commit()
        return ('', 204)

def update_JSON():
    pass

if __name__ == '__main__':
    bzzy.run(debug=True,host='0.0.0.0',port=8080)