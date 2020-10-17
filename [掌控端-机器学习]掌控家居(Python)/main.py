import csv
import lightgbm as lgb
from sklearn.metrics import mean_squared_error
import numpy as np
train_data = []
train_y = []
test_data = []
test_y = []
validation_data = []
validation_y = []
def init(reader):
    reader_ = []
    for i in range(reader.__len__()):
        if (reader[i][0] == ''):
            continue
        # print(reader[i])
        reader_.append(list(map(float, reader[i])))
    reader = np.array(reader_)
    return reader

def load_data(path,one_data_num,rate=[0.6,0.2,0.2],y=0):
    with open(path, 'r') as f:
        reader = init(list(csv.reader(f)))
        lendata = reader.shape[0]
        len_train = int(lendata * rate[0])
        len_test = int(lendata * (rate[0]+rate[1]))
        for i in range(1, len_train):
            train_data.append(reader[i:i + one_data_num].reshape(-1).astype(np.float32()))
            train_y.append(y)
        for i in range(len_train, len_test):
            test_data.append(reader[i:i + one_data_num].reshape(-1))
            test_y.append(y)
        for i in range(len_test, lendata-one_data_num):
            validation_data.append(reader[i:i + one_data_num].reshape(-1))
            validation_y.append(y)

one_data_num = 5
load_data(path=r'C:\\Users\\Administrator\\Desktop\\1.csv',one_data_num=one_data_num,y=1)
load_data(path=r'C:\\Users\\Administrator\\Desktop\\2.csv',one_data_num=one_data_num,y=2)
load_data(path=r'C:\\Users\\Administrator\\Desktop\\3.csv',one_data_num=one_data_num,y=3)

train_data = np.array(train_data)
train_y = np.array(train_y)
test_data = np.array(test_data)
test_y = np.array(test_y)
validation_data = np.array(validation_data)
validation_y = np.array(validation_y)

# 将参数写成字典下形式
params = {
    'objective': 'regression', # 目标函数
    #'metric': {'l2', 'auc'},  # 评估函数
    'num_leaves': 80,   # 叶子节点数
    'learning_rate': 0.02,  # 学习速率
    'feature_fraction': 0.5, # 建树的特征选择比例
    'bagging_fraction': 0.8, # 建树的样本采样比例
    'bagging_freq': 5,  # k 意味着每 k 次迭代执行bagging
    'verbose': 1 # <0 显示致命的, =0 显示错误 (警告), >0 显示信息
}

print(train_data.shape)
print(train_y.shape)
print(test_data.shape)
print(test_y.shape)
print(validation_data.shape)
print(validation_y.shape)
# 训练 cv and train
gbm = lgb.LGBMRegressor(objective="regression", learning_rate=0.02, n_estimators=300)
test = gbm.fit(train_data, train_y, eval_set=[(test_data, test_y)], eval_metric="l1", early_stopping_rounds=20)


print('Start predicting...')
# 预测数据集
score = gbm.score(validation_data, validation_y)
print(score)
predict_y = gbm.predict([validation_data[0]])
print(validation_y)
print(predict_y)