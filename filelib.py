from ksplitter import *

def help():
    print('filelib <source> --b')
    print('filelib <source> --m')

def main():
    if (len(sys.argv) == 1 or (len(sys.argv) == 2 and (sys.argv[1] == '--h' or sys.argv[1] == '--help'))):
        help()

    if (len(sys.argv) == 4):
        working[0] = sys.argv[1]

        if (sys.argv[2] == '--b'):
            splitFile(int(sys.argv[3]))
        if (sys.argv[2] == '--m'):
            mergeFile(int(sys.argv[3]))
        if (sys.argv[2] == '--bp'):
            splitFileP(int(sys.argv[3]))
        if (sys.argv[2] == '--mp'):
            mergeFileP(int(sys.argv[3]))
        if (sys.argv[2] == '--bo'):
            splitFileO(int(sys.argv[3]))
        if (sys.argv[2] == '--mo'):
            mergeFileO(int(sys.argv[3]))

if __name__ == '__main__':
    main()
    
