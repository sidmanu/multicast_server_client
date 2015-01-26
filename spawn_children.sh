echo "Spawning 1000 processes!!!!!"
for i in {1..1000} 
do
    ( ./client 2000 localhost 1,2 >/dev/null 2>&1 & )
done
