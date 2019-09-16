

import matplotlib.pyplot as plt
realTM = '/home/newplan/project/ns/ns3_v3.30/ns-allinone-3.30/ns-3.30/scratch/data/TM.'
predictTM = '/home/gaokaihui/TM_prediction/Mars/TM_pred_LSTM_1/'


def fileloader(root_path, file_index):
    src = []
    dst = []
    val = []
    val = 0
    with open(root_path+str(file_index), 'r') as f:
        text = f.readlines()
        # print(text)
        for each in text:
            if val % 2:
                print(each)
                print(each.split(',')[:12])
                return int(each.split(',')[1])
            val += 1
            #print(each.split('\n')[0].split(' '))
            #src_, dst_, val_ = each.split('\n')[0].split(' ')
            # print(int(src_),int(dst_),int(val_))

            # src.append(int(src_))
            # dst.append(int(dst_))
            # val.append(int(val_))
    # return [src, dst, val]


'''
index=1
pred_=fileloader(predictTM,index)
real_=fileloader(realTM,index)
print(pred_[0][6],'-->',pred_[1][6],'pred/real= ',pred_[2][6]/(real_[2][6]+0.1))
print(pred_[2][6])
print(real_[2][6])
#print(pred_[2][1]/(real_[2][1]+0.0001))
'''
data = []
for index in range(0, 250):
    #pred_ = fileloader(predictTM, index)
    real_ = fileloader(realTM, index)
    print(real_)
    # print(pred_[0][1], '-->', pred_[1][1], 'pred/real= ',
    #      ((real_[2][1]+0.1)-pred_[2][1])/(real_[2][1]+0.1))
    # print(pred_[2][1])
    # print(real_[2][1])
    data.append(real_)

# y1=[10,13,5,40,30,60,70,12,55,25]
# x1=range(0,10)
x2 = range(0, len(data))
y2 = data  # [5,8,0,30,20,40,50,10,40,15]
#plt.plot(x1,y1,label='Frist line',linewidth=3,color='r',marker='o', markerfacecolor='blue',markersize=12)
plt.plot(x2, y2, label='second line')
plt.xlabel('Plot Number')
plt.ylabel('Important var')
plt.title('Interesting Graph\nCheck it out')
plt.legend()
plt.show()
