from subprocess import Popen 

def create_client():
    p = Popen(["./client", "2000", "localhost", "1,2"])
    p.terminate()


def main():
    for i in range(0,5):    
        create_client()

if __name__ == "__main__":
    main()
