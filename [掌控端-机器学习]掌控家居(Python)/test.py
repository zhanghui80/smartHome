import csv
data1=[]
with open(r'C:\\Users\\Administrator\\Desktop\\1.csv', 'r') as f:
    reader = list(csv.reader(f))
    lendata = len(reader)
    print(type(reader))
   
    for i in range(1,lendata-10):
        data1.append(reader[i:i+10])
    print(data1)