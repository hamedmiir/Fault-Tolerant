# Fault-Tolerant
A fault tolerant system that get number of sensor from user and by using pipe for data transfer and doing parallel search in database find the most-repeated data in that sensor.  
Implemented by fork and assigning processes to files for each sensor in database.

### Running
Run the below codes in two seperate terminals.
```
g++ loadbalancer-worker.cpp -o  && ./loadbalancer-worker
```
```
g++ presenter.cpp -o presenter && ./presenter
```
Enter inputs in the presenter cli, you'll get the most repeated data in other one.
