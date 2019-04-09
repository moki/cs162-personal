Homework 2

```
./homework-2.pdf
```

```
make
# static files serving mode
./httpserver.out --files files/ --port 9000 [--num-threads 5]
# proxy mode
./httpserver.out --proxy inst.eecs.berkeley.edu:80 --port 9000 [--num-threads 5]
```
