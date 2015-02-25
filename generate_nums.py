#!/usr/bin/env python
import random

def write_random_nums(file):
    f = open(file, 'w');
    for i in range (0, 5000000):
        x = random.randrange(1,5000+1)
        f.write("%d,"%x);

    f.write(str(5555))
    f.write('\0')
    f.close()


def main():
    write_random_nums("sample.csv")


if __name__ == "__main__":
    main()
