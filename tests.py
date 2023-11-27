from datetime import datetime

import numpy as np
import pandas as pd

# 007
# 648669
# 648668


# 003
# 417289
# 417289

# 002
# 629672
# 629672

dateparse = lambda x: datetime.strptime(x, '%Y-%m-%d %H:%M:%S')

data1 = pd.read_csv("./data/HR_003.csv", parse_dates=['datetime'], date_parser=dateparse)

dateparse = lambda x: datetime.strptime(x, '%Y-%m-%d %H:%M:%S.%f')

data2 = pd.read_csv("./data/ACC_003.csv", parse_dates=['datetime'], date_parser=dateparse)

data2['ts'] = data2["datetime"].astype(np.int64) // 10 ** 9

data1['ts'] = data1["datetime"].astype(np.int64) // 10 ** 9

print(len(data1["ts"].values))
print(len(set(data2["ts"].drop_duplicates().values) & set(data1["ts"].values)))
