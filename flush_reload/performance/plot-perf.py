# %%

"""
@author: Sarpant
"""

import numpy
import matplotlib.pyplot as plt
import pandas as pd

res = pd.read_csv('result.csv')

x = res.iloc[:,0]


y = res.loc[:, 'total_g_predict'] / res.loc[:, 'total_access'] * 100
plt.figure()
plt.plot(x, y, label='Accuracy')
plt.xlabel('Waiting proc cycles of victime')
plt.ylabel('Accuracy %')
plt.scatter(x, y)
plt.show()

y = res.loc[:, 'faux_p'] / res.loc[:, 'total_predict'] * 100
plt.figure()
plt.plot(x, y, label='False positive')
plt.xlabel('Waiting proc cycles of victime')
plt.ylabel('% of false positive')
plt.scatter(x, y)
plt.show()

y = res.loc[:, 'faux_n'] / res.loc[:, 'total_access'] * 100
plt.figure()
plt.plot(x, y, label='False negative')
plt.xlabel('Waiting proc cycles of victime')
plt.ylabel('% of miss predict')
plt.scatter(x, y)
plt.show()

# %%
