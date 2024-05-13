
#sudo find src/* -exec touch {} +
#sudo find include/* -exec touch {} +
#sudo touch OpNovice.cc

mkdir build1
cd build1 
cmake ..
make
./OpNovice L 20 -t 70 -m run.mac

cd ..
mkdir build2 
cd build2 
cmake ..
make
./OpNovice L 30 -t 70 -m run.mac

cd ..
mkdir build3 
cd build3 
cmake ..
make
./OpNovice L 40 -t 70 -m run.mac

cd ..
mkdir build4 
cd build4 
cmake ..
make
./OpNovice L 50 -t 70 -m run.mac

cd ..
mkdir build5
cd build5
cmake ..
make
./OpNovice L 60 -t 70 -m run.mac
