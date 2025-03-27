g++ -o mysql_demo1 get_info.cpp -lmysqlcppconn -lspdlog
echo "Compilation finished. Running the program..."
./mysql_demo1
rm mysql_demo1