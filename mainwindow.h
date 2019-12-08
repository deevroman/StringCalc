#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include "vector.h"
#include <QDebug>
#include <cmath>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    struct Operand{
        int type; // 0 - number, 1 - operator, 2 - function, 3 - variable, 4 - constants
        QString value;
        int cnt_args;
        int priority;
        Operand(){}
        Operand(int type, QString value, int cnt_args, int priority)
            : type(type), value(value), cnt_args(cnt_args), priority(priority){}
        Operand(const Operand &a){
            type = a.type;
            value = a.value;
            cnt_args = a.cnt_args;
            priority = a.priority;
        }
        Operand& operator=(const Operand &a){
            type = a.type;
            value = a.value;
            cnt_args = a.cnt_args;
            priority = a.priority;
            return *this;
        }
        double calc(my_stl::vector<double> args, int angle = 0){
            switch (type){
            case 0:
                return  value.toDouble();
            case 1:
                if (value == "+") {
                    if(cnt_args == 2)
                        return args[0] + args[1];
                    else
                        return args[0];
                } else if(value == "-"){
                    if(cnt_args == 2)
                        return args[0] - args[1];
                    else {
                        return -args[0];
                    }
                } else if(value == "*"){
                    return args[0] * args[1];
                } else if(value == "/"){
                    if (eq(args[1], 0.0)){
                        throw QString(trUtf8("Ошибка: деление на ноль"));
                    }
                    return args[0] / args[1];
                } else if(value == "^"){
                    if(args[0] > 0.0 || eq(args[0], 0) && args[1] > 0.0 || fabs(args[1] - round(args[1])) < 1e-12){
                        return pow(args[0], args[1]);
                    }
                    if(eq(args[0], 0.0)){
                        throw QString(trUtf8("Ошибка: возведение в отрицательную степень"));
                    }
                    if(fabs(args[1] - round(args[1])) >= 1e-12){
                        throw QString(trUtf8("Ошибка: возведение в дробую степень отрицательного числа"));
                    }
                }
            case 2:
                if(value == "cos"){
                    if(angle){
                        return cos(toAngle(args[0]));
                    } else{
                        return cos(args[0]);
                    }
                } else if(value == "sin"){
                    if(angle){
                        return sin(toAngle(args[0]));
                    } else{
                        return sin(args[0]);
                    }
                } else if(value == "tg"){
                    if(!eq(cos(args[0]), 0.0)){
                        return sin(args[0]) / cos(args[0]);
                    } else {
                        throw QString(trUtf8("Ошибка: Аргумент тангенса не в ОДЗ"));
                    }
                } else if(value == "ctg"){
                    if(!eq(sin(args[0]), 0.0)){
                        return cos(args[0]) / sin(args[0]);
                    } else {
                        throw QString(trUtf8("Ошибка: Аргумент котангенса не в ОДЗ"));
                    }
                } else if(value == "log"){
                    if(args[1] <= 0.0){
                        throw QString(trUtf8("Ошибка: Логарифм от неположительного числа"));
                    }
                    if(args[0] <= 0.0){
                        throw QString(trUtf8("Ошибка: Логарифм по неположительному основанию"));
                    }
                    if(eq(args[0], 1.0)){
                        throw QString(trUtf8("Ошибка: Логарифм по основанию 1"));
                    }
                    return log2(args[1]) / log2(args[0]);
                }
                break;
            case 3:
                break;
            case 4:
                if (value == "e") {
                    return M_E;
                } else if(value == "pi"){
                    return M_PI;
                }
                break;
            }
        }
    private:
        double toAngle(double rad){
            return rad * M_PI / 180.0;
        }
        bool eq(double a, double b){
            return fabs(a - b) < 1e-7;
        }
    };
    Operand operands [14] = {
        Operand(1, "+", 2, 1),
        Operand(1, "-", 2, 1),
        Operand(1, "*", 2, 2),
        Operand(1, "/", 2, 2),
        Operand(1, "^", 2, 5),
        Operand(2, "cos", 1, 0),
        Operand(2, "sin", 1, 0),
        Operand(2, "tg", 1, 0),
        Operand(2, "ctg", 1, 0),
        Operand(2, "log", 2, 0),
        Operand(-1, "(", 0, 4),
        Operand(-1, ")", 0, 4),
        Operand(1, "+", 1, 3),
        Operand(1, "-", 1, 3),
    };
private:
    template<typename T1, typename T2>
    struct pair{
        T1 first;
        T2 second;
        pair(){}
        pair(T1 a, T2 b)
            : first(a), second(b){}
    };
    QString clear_space(QString);
    void hideTable();
    QString calc_poliz(my_stl::vector<Operand>&, my_stl::vector<pair<QString, double>>&, bool=false);
    void get_str_poliz(my_stl::vector<Operand>&, QString&);
    void parser(QString, my_stl::vector<Operand>&);
    double readNum(QString&, int&);
    QString readStr(QString&, int&);
    double roundDouble(double x){
        if(fabs(x - round(x)) < 1e-12){
            return round(x);
        } else{
            return x;
        }
    }
    Ui::MainWindow *ui;
    const int P_PLUS = 1;
    const int P_SUB = 1;
    const int P_MUL = 2;
    const int P_DIV = 2;
    const int P_POW = 5;
    int Angle = 0;
    const QString CharBeforeUnar = "([+-*/^,";

private slots:
    void calculate();
    void set_visible_poliz(bool);
};

#endif // MAINWINDOW_H
