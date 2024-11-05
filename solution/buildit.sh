
docker build -t pet .
id=$(docker create pet)
docker cp $id:/home/pet/a.out .
docker rm -v $id
