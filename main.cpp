#include "mainwindow.h"
#include <QApplication>

void test(){
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);
    qDebug() << "before";
    for(vector<int>::iterator iterator = vec.begin(); iterator!=vec.end(); iterator++){
        qDebug() << *iterator;
    }
    srand(static_cast<unsigned int>(clock()));
    std::random_shuffle(std::begin(vec), std::end(vec));
    qDebug() << "after";
    for(vector<int>::iterator iterator = vec.begin(); iterator!=vec.end(); iterator++){
        qDebug() << *iterator;
    }
}

int main(int argc, char *argv[])
{
    FileIO number_file("test_number.txt");
    string test = number_file.readFile();
    cout << test << endl;
    QApplication a(argc, argv);
    MainWindow w(test);
    int test_number = stoi(test);
    test_number++;
    number_file.writeFile(to_string(test_number));
    w.show();
    return a.exec();
//    test();
//    return 0;
}
