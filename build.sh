# customized build file for my needs!

cd build

cmake -Dclient=OFF -D Protobuf_PROTOC_EXECUTABLE=/usr/bin/protoc ..

make -j 1

cp ./mumble-server ../production/
cp ./MumbleServer.ice ../production/
